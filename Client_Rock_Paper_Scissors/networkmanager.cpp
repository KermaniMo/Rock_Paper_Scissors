#include "networkmanager.h"
#include "gamedata.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedPointer>
#include <QDebug>

// ============================================================
//  Singleton Access (Meyers' Singleton — Thread-safe)
// ============================================================

NetworkManager& NetworkManager::instance()
{
    static NetworkManager inst;   // ساخت در اولین فراخوانی، thread-safe در C++11+
    return inst;
}

// ============================================================
//  Constructor / Destructor
// ============================================================

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent),
      m_socket(new QTcpSocket(this))   // parent = this → حافظه خودکار آزاد می‌شود
{
    // فوروارد سیگنال‌های سوکت به بیرون
    connect(m_socket, &QTcpSocket::connected,
            this,     &NetworkManager::connected);

    connect(m_socket, &QTcpSocket::disconnected,
            this,     &NetworkManager::disconnected);

    // اسلات دریافت داده
    connect(m_socket, &QTcpSocket::readyRead,
            this,     &NetworkManager::onReadyRead);
}

NetworkManager::~NetworkManager()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

// ============================================================
//  مدیریت اتصال
// ============================================================

void NetworkManager::connectToHost(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void NetworkManager::disconnectFromHost()
{
    m_socket->disconnectFromHost();
}

// ============================================================
//  ارسال داده
// ============================================================

void NetworkManager::sendPacket(const IGamePacket &packet)
{
    // ۱. تبدیل پکت به QJsonObject
    QJsonObject jsonObj = packet.toJson();

    // ۲. تبدیل به QJsonDocument و سپس QByteArray (فشرده)
    QJsonDocument doc(jsonObj);
    QByteArray    data = doc.toJson(QJsonDocument::Compact);

    // ۳. اضافه کردن کاراکتر خط جدید برای جدا کردن پیام‌ها
    data.append('\n');

    // ۴. نوشتن روی سوکت
    m_socket->write(data);
    m_socket->flush(); // جهت اطمینان از ارسال سریع
}

// ============================================================
//  دریافت داده (اصلاح شده برای خواندن خط به خط)
// ============================================================

void NetworkManager::onReadyRead()
{
    // تا زمانی که یک خط کامل قابل خواندن باشد، حلقه ادامه دارد
    while (m_socket->canReadLine()) {

        // خواندن یک خط (شامل \n انتهایی)
        QByteArray data = m_socket->readLine();

        // حذف \n از انتهای داده برای پارس کردن
        QByteArray jsonBytes = data.trimmed();

        if (jsonBytes.isEmpty()) continue; // خط خالی نادیده گرفته شود

        // تبدیل به QJsonDocument
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonBytes, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "NetworkManager: JSON parse error:" << parseError.errorString() 
                       << "Data:" << jsonBytes;
            continue; // تلاش برای خط بعدی
        }

        if (!doc.isObject()) {
            qWarning() << "NetworkManager: received data is not a JSON object" 
                       << "Data:" << jsonBytes;
            continue;
        }

        QJsonObject jsonObj = doc.object();
        qDebug() << "NetworkManager: Received JSON:" << QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);

        // ساخت پکت با PacketFactory
        std::unique_ptr<IGamePacket> rawPacket = PacketFactory::createPacket(jsonObj);

        if (!rawPacket) {
            qWarning() << "NetworkManager: unknown packet type received";
            continue;
        }

        // تبدیل به QSharedPointer و ارسال سیگنال
        QSharedPointer<IGamePacket> sharedPacket(rawPacket.release());
        emit packetReceived(sharedPacket);
    }
}
