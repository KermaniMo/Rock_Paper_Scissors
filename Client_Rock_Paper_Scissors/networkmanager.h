#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QSharedPointer>

class IGamePacket; // Forward declaration

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    // --- Singleton ---
    static NetworkManager& instance();

    // حذف کپی و انتساب
    NetworkManager(const NetworkManager&) = delete;
    void operator=(const NetworkManager&) = delete;

    ~NetworkManager() override;

    // --- مدیریت اتصال ---
    void connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();

    // --- ارسال داده ---
    void sendPacket(const IGamePacket &packet);

signals:
    // فوروارد سیگنال‌های سوکت
    void connected();
    void disconnected();

    // سیگنال دریافت پکت پردازش‌شده
    void packetReceived(QSharedPointer<IGamePacket> packet);

private slots:
    void onReadyRead();

private:
    // Constructor خصوصی — فقط از طریق instance() قابل دسترسی
    explicit NetworkManager(QObject *parent = nullptr);

    QTcpSocket *m_socket = nullptr;
};

#endif // NETWORKMANAGER_H
