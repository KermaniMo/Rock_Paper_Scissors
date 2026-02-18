#include "servermanager.h"
#include "clienthandler.h"
#include "servergamecontroller.h"

#include <QTcpSocket>
#include <QDebug>

// ============================================================
//  Singleton Access (Meyers' Singleton — Thread-safe)
// ============================================================
ServerManager& ServerManager::instance()
{
    static ServerManager inst;
    return inst;
}

// ============================================================
//  Constructor
// ============================================================
ServerManager::ServerManager(QObject *parent)
    : QTcpServer(parent)
{
}

// ============================================================
//  شروع و توقف سرور
// ============================================================
bool ServerManager::startServer(const QHostAddress &address, quint16 port)
{
    if (isListening()) {
        qWarning() << "ServerManager: Already listening!";
        return false;
    }

    if (!listen(address, port)) {
        emit logMessage(QStringLiteral("Failed to start server: %1").arg(errorString()));
        return false;
    }

    emit logMessage(QStringLiteral("Server started on %1:%2")
                        .arg(serverAddress().toString())
                        .arg(port));
    return true;
}

void ServerManager::stopServer()
{
    // بستن همه کلاینت‌ها
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        it.value()->deleteLater();
    }
    m_clients.clear();

    close(); // توقف listen

    emit logMessage(QStringLiteral("Server stopped."));
}

// ============================================================
//  مدیریت اتصال جدید
// ============================================================
void ServerManager::incomingConnection(qintptr socketDescriptor)
{
    // ساخت سوکت جدید
    QTcpSocket *socket = new QTcpSocket();
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "ServerManager: Failed to set socket descriptor";
        delete socket;
        return;
    }

    // ساخت ClientHandler برای این کلاینت
    ClientHandler *handler = new ClientHandler(socket, this);

    emit logMessage(QStringLiteral("New connection from %1:%2")
                        .arg(socket->peerAddress().toString())
                        .arg(socket->peerPort()));

    // اتصال سیگنال لاگین
    connect(handler, &ClientHandler::userLoggedIn,
            this, [this, handler](const QString &username, ClientHandler *client) {

        m_clients.insert(username, client);

        // ثبت در ServerGameController
        ServerGameController::instance().registerClient(username, client);

        // اتصال پکت‌های غیر لاگین به ServerGameController
        connect(handler, &ClientHandler::packetReceived,
                &ServerGameController::instance(), &ServerGameController::onPacketReceived);

        emit logMessage(QStringLiteral("User \"%1\" connected.").arg(username));
    });

    // اتصال سیگنال قطع اتصال
    connect(socket, &QTcpSocket::disconnected,
            this, [this, handler, socket]() {
        // پیدا کردن و حذف کاربر از مپ
        QString disconnectedUser;
        for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
            if (it.value() == handler) {
                disconnectedUser = it.key();
                m_clients.erase(it);
                break;
            }
        }

        if (!disconnectedUser.isEmpty()) {
            // حذف از ServerGameController
            ServerGameController::instance().unregisterClient(disconnectedUser);
            emit logMessage(QStringLiteral("User \"%1\" disconnected.").arg(disconnectedUser));
        } else {
            emit logMessage(QStringLiteral("Client disconnected (not logged in)."));
        }

        handler->deleteLater();
    });
}
