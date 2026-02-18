#ifndef SERVERGAMECONTROLLER_H
#define SERVERGAMECONTROLLER_H

#include <QObject>
#include <QMap>
#include <QString>
#include "gamedata.h"

class ClientHandler;
class IGamePacket;
class GameSession;
#include <QSharedPointer>

class ServerGameController : public QObject
{
    Q_OBJECT

public:
    // --- Singleton Access (Meyers' Singleton) ---
    static ServerGameController& instance();

    // غیرفعال‌سازی کپی
    ServerGameController(const ServerGameController&) = delete;
    void operator=(const ServerGameController&) = delete;

    // --- ثبت / حذف کلاینت ---
    void registerClient(const QString &username, ClientHandler *handler);
    void unregisterClient(const QString &username);

public slots:
    // --- دریافت پکت از ClientHandler ---
    void onPacketReceived(ClientHandler *sender, IGamePacket *packet);

signals:
    void logMessage(const QString &msg);
    void clientRegistered(const QString &username);
    void clientUnregistered(const QString &username);
    void gameStarted(const QString &p1, const QString &p2);
    void gameEnded(const QString &p1, const QString &p2);

private:
    // Constructor خصوصی (Singleton)
    explicit ServerGameController(QObject *parent = nullptr);

    // --- ساختارهای داده ---
    QMap<QString, ClientHandler*> m_clients;       // لیست کل آنلاین‌ها
    QMap<QString, QString>        m_pendingInvites; // کلید: دریافت‌کننده، مقدار: فرستنده
    QMap<QString, QString>        m_activeGames;    // نگاشت دوطرفه بازیکن‌ها به هم
    QMap<QString, QSharedPointer<GameSession>> m_sessions; // سشن‌های فعال


    // --- توابع کمکی ---
    QString findUsername(ClientHandler *handler) const;
    void sendPacketTo(const QString &username, const IGamePacket &packet);

    void handleInviteRequest(ClientHandler *sender, InvitePacket *invite);
    void handleInviteResponse(ClientHandler *sender, InvitePacket *response);
    void handleGameMove(ClientHandler *sender, GameMovePacket *packet);
};

#endif // SERVERGAMECONTROLLER_H
