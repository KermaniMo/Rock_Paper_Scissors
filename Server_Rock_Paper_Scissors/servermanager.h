#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QTcpServer>
#include <QHostAddress>
#include <QMap>
#include <QString>

class ClientHandler;

class ServerManager : public QTcpServer
{
    Q_OBJECT

public:
    // --- Singleton Access (Meyers' Singleton) ---
    static ServerManager& instance();

    // غیرفعال‌سازی کپی
    ServerManager(const ServerManager&) = delete;
    void operator=(const ServerManager&) = delete;

    // --- مدیریت سرور ---
    bool startServer(const QHostAddress &address, quint16 port);
    void stopServer();

signals:
    void logMessage(const QString &msg);

protected:
    // بازنویسی برای ساخت ClientHandler به ازای هر اتصال جدید
    void incomingConnection(qintptr socketDescriptor) override;

private:
    // Constructor خصوصی (Singleton)
    explicit ServerManager(QObject *parent = nullptr);

    QMap<QString, ClientHandler*> m_clients;
};

#endif // SERVERMANAGER_H
