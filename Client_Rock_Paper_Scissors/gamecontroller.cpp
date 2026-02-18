#include "gamecontroller.h"
#include "networkmanager.h"
#include "gamedata.h"
#include "protocol.h"

#include <QDebug>

// ============================================================
//  Singleton Access (Meyers' Singleton)
// ============================================================

GameController& GameController::instance()
{
    static GameController inst;
    return inst;
}

// ============================================================
//  Constructor
// ============================================================

GameController::GameController(QObject *parent)
    : QObject(parent)
{
    NetworkManager &net = NetworkManager::instance();

    // وصل شدن سوکت → ارسال خودکار LoginPacket
    connect(&net, &NetworkManager::connected,
            this, &GameController::onSocketConnected);

    // دریافت پکت از شبکه
    connect(&net, &NetworkManager::packetReceived,
            this, &GameController::onPacketReceived);

    // فوروارد سیگنال قطع اتصال (اختیاری — برای آینده)
    connect(&net, &NetworkManager::disconnected,
            this, [this]() {
                qDebug() << "GameController: socket disconnected";
            });
}

// ============================================================
//  توابع ورودی از UI
// ============================================================

void GameController::connectAndLogin(const QString &username,
                                     const QString &ip,
                                     quint16 port)
{
    m_myUsername = username;
    qDebug() << "GameController: connecting to" << ip << ":" << port
             << "as" << username;

    NetworkManager::instance().connectToHost(ip, port);
}

void GameController::onSocketConnected()
{
    // سوکت وصل شد → بلافاصله پکت لاگین ارسال کن
    LoginPacket packet(m_myUsername);
    NetworkManager::instance().sendPacket(packet);
    qDebug() << "GameController: socket connected, sent LOGIN_REQ for"
             << m_myUsername;
}

void GameController::sendInvite(const QString &target)
{
    InvitePacket packet(m_myUsername, target,
                        /*accepted=*/false,
                        /*isResponse=*/false);
    NetworkManager::instance().sendPacket(packet);
    qDebug() << "GameController: sent INVITE_REQ to" << target;
}

void GameController::sendInviteResponse(const QString &sender, bool accepted)
{
    InvitePacket packet(sender, m_myUsername,
                        /*accepted=*/accepted,
                        /*isResponse=*/true);
    NetworkManager::instance().sendPacket(packet);
    qDebug() << "GameController: sent INVITE_RES to" << sender
             << "accepted:" << accepted;
}

void GameController::playMove(const QString &move)
{
    GameMovePacket packet(m_myUsername, move);
    NetworkManager::instance().sendPacket(packet);
    qDebug() << "GameController: sent GAME_MOVE" << move;
}

// ============================================================
//  Routing — پردازش پکت‌های دریافتی
// ============================================================

void GameController::onPacketReceived(QSharedPointer<IGamePacket> packet)
{
    if (!packet) return;

    const int type = packet->getType();

    switch (type) {

    // -------------------- LOGIN_RES --------------------
    case GameProtocol::LOGIN_RES: {
        auto *login = dynamic_cast<LoginPacket*>(packet.data());
        if (!login) break;

        if (login->success) {
            m_myUsername = login->username;
            emit loginSuccess(login->username);
            qDebug() << "GameController: login SUCCESS for" << login->username;
        } else {
            emit loginFailed(login->message);
            qDebug() << "GameController: login FAILED:" << login->message;
        }
        break;
    }

    // -------------------- INVITE_REQ --------------------
    case GameProtocol::INVITE_REQ: {
        auto *invite = dynamic_cast<InvitePacket*>(packet.data());
        if (!invite) break;

        emit inviteReceived(invite->sender);
        qDebug() << "GameController: invite received from" << invite->sender << "Target was:" << invite->target;
        break;
    }

    // -------------------- INVITE_RES --------------------
    case GameProtocol::INVITE_RES: {
        auto *invite = dynamic_cast<InvitePacket*>(packet.data());
        if (!invite) break;

        emit inviteResult(invite->accepted);
        qDebug() << "GameController: invite response from" << invite->sender
                 << "accepted:" << invite->accepted;
        break;
    }

    // -------------------- GAME_START --------------------
    case GameProtocol::GAME_START: {
        auto *start = dynamic_cast<GameStartPacket*>(packet.data());
        if (!start) break;

        m_opponentName = start->opponent;
        emit gameStarted(start->opponent);
        qDebug() << "GameController: game started vs" << start->opponent;
        break;
    }

    // -------------------- GAME_MOVE --------------------
    case GameProtocol::GAME_MOVE: {
        auto *move = dynamic_cast<GameMovePacket*>(packet.data());
        if (!move) break;

        emit opponentMove(move->move);
        qDebug() << "GameController: opponent move:" << move->move;
        break;
    }

    // -------------------- GAME_RESULT --------------------
    case GameProtocol::GAME_RESULT: {
        auto *result = dynamic_cast<GameResultPacket*>(packet.data());
        if (!result) break;

        emit gameResult(result->winner, result->message);
        qDebug() << "GameController: result — winner:" << result->winner
                 << "msg:" << result->message;
        break;
    }

    default:
        qWarning() << "GameController: unhandled packet type" << type;
        break;
    }
}
