#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QSharedPointer>

class IGamePacket;

// ============================================================
//  GameController — لایه منطق / واسط بین UI و شبکه (Singleton)
// ============================================================
class GameController : public QObject
{
    Q_OBJECT

public:
    // --- Singleton ---
    static GameController& instance();

    GameController(const GameController&) = delete;
    void operator=(const GameController&) = delete;

    // --- وضعیت فعلی ---
    QString myUsername()    const { return m_myUsername; }
    QString opponentName() const { return m_opponentName; }

public slots:
    // --- توابع ورودی از UI ---
    void connectAndLogin(const QString &username, const QString &ip, quint16 port);
    void sendInvite(const QString &target);
    void sendInviteResponse(const QString &sender, bool accepted);
    void playMove(const QString &move);

signals:
    // --- سیگنال‌های خروجی به UI ---
    void loginSuccess(const QString &username);
    void loginFailed(const QString &reason);
    void connectionFailed(const QString &reason);
    void inviteReceived(const QString &sender);
    void inviteResult(bool success);
    void gameStarted(const QString &opponent);
    void opponentMove(const QString &move);
    void gameResult(const QString &winner, const QString &message);

private slots:
    void onPacketReceived(QSharedPointer<IGamePacket> packet);
    void onSocketConnected();

private:
    // Constructor خصوصی
    explicit GameController(QObject *parent = nullptr);

    // --- وضعیت ---
    QString m_myUsername;
    QString m_opponentName;
};

#endif // GAMECONTROLLER_H
