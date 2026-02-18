#include "clienthandler.h"
#include "gamedata.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

// ============================================================
//  Constructor / Destructor
// ============================================================
ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
    // سوکت را فرزند این آبجکت قرار می‌دهیم (مدیریت حافظه)
    m_socket->setParent(this);

    connect(m_socket, &QTcpSocket::readyRead,
            this,     &ClientHandler::onReadyRead);
}

ClientHandler::~ClientHandler()
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

QTcpSocket* ClientHandler::socket() const
{
    return m_socket;
}

QString ClientHandler::username() const
{
    return m_username;
}

// ============================================================
//  ارسال پکت به کلاینت
// ============================================================
void ClientHandler::sendPacket(const IGamePacket &packet)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QJsonDocument doc(packet.toJson());
    // ارسال به صورت فشرده (Compact) + کاراکتر خط جدید
    m_socket->write(doc.toJson(QJsonDocument::Compact));
    m_socket->write("\n");
    m_socket->flush();
}

// ============================================================
//  خواندن داده (اصلاح شده برای Newline Delimited JSON)
// ============================================================
void ClientHandler::onReadyRead()
{
    // بررسی اینکه آیا دیتا آمده اما خط کامل نیست
    if (m_socket->bytesAvailable() > 0 && !m_socket->canReadLine()) {
        QByteArray peekData = m_socket->peek(m_socket->bytesAvailable());
        
        // اگر دیتا با { شروع و با } تمام شود، شاید کلاینت قدیمی است و \n نفرستاده
        if (peekData.trimmed().startsWith('{') && peekData.trimmed().endsWith('}')) {
            qWarning() << "ClientHandler: Detected potential JSON without newline. Attempting to parse...";
            
             // خواندن کل دیتا به عنوان یک پکت
             QByteArray data = m_socket->readAll();
             QJsonParseError parseError;
             QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
             
             if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                 // پردازش موفق! کد کلاینت قدیمی است اما سرور هندل کرد.
                 auto packet = PacketFactory::createPacket(doc.object());
                 if (packet) {
                     // پردازش پکت (کپی از پایین)
                      if (packet->getType() == GameProtocol::LOGIN_REQ) {
                            LoginPacket *loginPkt = dynamic_cast<LoginPacket*>(packet.get());
                            if (loginPkt) {
                                m_username = loginPkt->username;
                                QJsonObject resObj;
                                resObj[GameProtocol::KEY_TYPE] = GameProtocol::LOGIN_RES;
                                resObj[GameProtocol::KEY_USERNAME] = m_username;
                                resObj[GameProtocol::KEY_SUCCESS] = true;
                                resObj[GameProtocol::KEY_MESSAGE] = QStringLiteral("Welcome!");
                                QJsonDocument resDoc(resObj);
                                m_socket->write(resDoc.toJson(QJsonDocument::Compact));
                                m_socket->write("\n");
                                m_socket->flush();
                                emit userLoggedIn(m_username, this);
                                return; // خروج از این تابع
                            }
                      } else {
                          emit packetReceived(this, packet.get());
                          return;
                      }
                 }
             }
        }
        
        qDebug() << "ClientHandler: Waiting for newline... Buffer:" << peekData;
    }

    // تا زمانی که یک خط کامل در بافر باشد
    while (m_socket->canReadLine()) {

        QByteArray data = m_socket->readLine();
        QByteArray jsonBytes = data.trimmed(); // حذف \n

        if (jsonBytes.isEmpty()) continue;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonBytes, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            qWarning() << "ClientHandler: JSON parse error:" << parseError.errorString() 
                       << "Data:" << jsonBytes;
            continue;
        }

        // ساخت پکت از JSON با استفاده از PacketFactory
        auto packet = PacketFactory::createPacket(doc.object());
        if (!packet) {
            qWarning() << "ClientHandler: Unknown packet type" << jsonBytes;
            continue;
        }

        // --- بررسی نوع پکت ---
        if (packet->getType() == GameProtocol::LOGIN_REQ) {

            // نام کاربری را از پکت استخراج می‌کنیم
            LoginPacket *loginPkt = dynamic_cast<LoginPacket*>(packet.get());
            if (!loginPkt) continue;

            m_username = loginPkt->username;

            // ساخت پاسخ LOGIN_RES با success = true
            QJsonObject resObj;
            resObj[GameProtocol::KEY_TYPE]     = GameProtocol::LOGIN_RES;
            resObj[GameProtocol::KEY_USERNAME] = m_username;
            resObj[GameProtocol::KEY_SUCCESS]  = true;
            resObj[GameProtocol::KEY_MESSAGE]  = QStringLiteral("Welcome!");

            QJsonDocument resDoc(resObj);
            m_socket->write(resDoc.toJson(QJsonDocument::Compact));
            m_socket->write("\n"); // پایان خط
            m_socket->flush();

            // شلیک سیگنال برای اطلاع ServerManager
            emit userLoggedIn(m_username, this);

        } else {
            // --- پکت‌های غیر لاگین → فوروارد به ServerGameController ---
            emit packetReceived(this, packet.get());
        }
    }
}
