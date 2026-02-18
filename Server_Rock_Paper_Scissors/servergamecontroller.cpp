#include "servergamecontroller.h"
#include "clienthandler.h"
#include "gamedata.h"
#include "gamesession.h"

#include <QDebug>
#include <QJsonDocument>

// ============================================================
//  Singleton Access (Meyers' Singleton — Thread-safe)
// ============================================================
ServerGameController& ServerGameController::instance()
{
    static ServerGameController inst;
    return inst;
}

// ============================================================
//  Constructor
// ============================================================
ServerGameController::ServerGameController(QObject *parent)
    : QObject(parent)
{
}

// ============================================================
//  ثبت و حذف کلاینت
// ============================================================
void ServerGameController::registerClient(const QString &username, ClientHandler *handler)
{
    m_clients.insert(username, handler);
    emit logMessage(QStringLiteral("[GameCtrl] \"%1\" registered.").arg(username));
    emit clientRegistered(username);
}

void ServerGameController::unregisterClient(const QString &username)
{
    // حذف از لیست آنلاین‌ها
    m_clients.remove(username);

    // حذف از pending invites (اگر این کاربر دعوتی داشته)
    m_pendingInvites.remove(username);                       // اگر خودش دریافت‌کننده بود

    // اگر خودش فرستنده بود، باید از مقادیر هم حذف شود
    QStringList keysToRemove;
    for (auto it = m_pendingInvites.begin(); it != m_pendingInvites.end(); ++it) {
        if (it.value() == username)
            keysToRemove.append(it.key());
    }
    for (const auto &key : keysToRemove)
        m_pendingInvites.remove(key);

    // حذف از بازی فعال (دوطرفه)
    if (m_sessions.contains(username)) {
        // اگر سشنی وجود دارد، یعنی بازی در جریان است
        // باید بازی را کنسل کنیم و به حریف اطلاع دهیم (فعلاً فقط حذف می‌کنیم)
        // بهتر است یک پکت GAME_RESULT با پیام "Opponent disconnected" بفرستیم
        auto session = m_sessions.value(username);
        QString opponent;
        if (session->player1() == username) opponent = session->player2();
        else opponent = session->player1();

        // حذف سشن برای هر دو نفر
        m_sessions.remove(username);
        m_sessions.remove(opponent);

        // حذف از مپ ساده (جهت اطمینان)
        m_activeGames.remove(username);
        m_activeGames.remove(opponent);

        emit logMessage(QStringLiteral("[GameCtrl] Game session aborted: \"%1\" disconnected.")
                            .arg(username));
        // اینجا می‌توانیم به opponent پیام بدهیم
    } else if (m_activeGames.contains(username)) {
        // حالت legacy (اگر هنوز سشن ساخته نشده ولی در مپ هست)
        QString opponent = m_activeGames.value(username);
        m_activeGames.remove(username);
        m_activeGames.remove(opponent);
    }

    emit logMessage(QStringLiteral("[GameCtrl] \"%1\" unregistered.").arg(username));
    emit clientUnregistered(username);
}

// ============================================================
//  پردازش پکت‌های دریافتی
// ============================================================
void ServerGameController::onPacketReceived(ClientHandler *sender, IGamePacket *packet)
{
    if (!packet) return;

    const int type = packet->getType();

    // ──────────────────────────────
    //  INVITE_REQ — درخواست بازی
    // ──────────────────────────────
    if (type == GameProtocol::INVITE_REQ) {
        InvitePacket *invite = dynamic_cast<InvitePacket*>(packet);
        if (invite) {
            handleInviteRequest(sender, invite);
        }
    }
    // ──────────────────────────────
    //  INVITE_RES — پاسخ دعوت
    // ──────────────────────────────
    else if (type == GameProtocol::INVITE_RES) {
        InvitePacket *response = dynamic_cast<InvitePacket*>(packet);
        if (response) {
            handleInviteResponse(sender, response);
        }
    }
    // ──────────────────────────────
    //  GAME_MOVE — حرکت بازی (سنگ/کاغذ/قیچی)
    // ──────────────────────────────
    else if (type == GameProtocol::GAME_MOVE) {
        GameMovePacket *move = dynamic_cast<GameMovePacket*>(packet);
        if (move) {
            handleGameMove(sender, move);
        }
    }
}

// ============================================================
//  توابع کمکی
// ============================================================
QString ServerGameController::findUsername(ClientHandler *handler) const
{
    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); ++it) {
        if (it.value() == handler)
            return it.key();
    }
    return {};
}

void ServerGameController::sendPacketTo(const QString &username, const IGamePacket &packet)
{
    if (!m_clients.contains(username)) return;

    ClientHandler *handler = m_clients.value(username);
    handler->sendPacket(packet);
}

// ============================================================
//  توابع مدیریت پکت‌ها
// ============================================================
void ServerGameController::handleInviteRequest(ClientHandler *sender, InvitePacket *invite)
{
    const QString senderName = invite->sender;
    const QString targetName = invite->target;

    qDebug() << "ServerGameController: Processing INVITE_REQ from" << senderName << "to" << targetName;

    // ۱) آیا هدف آنلاین است؟
    if (!m_clients.contains(targetName)) {
        // هدف آفلاین — ارسال INVITE_RES ناموفق به فرستنده
        InvitePacket fail(targetName, senderName, false, true);
        sendPacketTo(senderName, fail);
        emit logMessage(QStringLiteral("[GameCtrl] Invite %1 → %2 failed: target offline.")
                            .arg(senderName, targetName));
        return;
    }

    // ۲) آیا هدف در بازی فعال نیست؟
    if (m_activeGames.contains(targetName)) {
        InvitePacket fail(targetName, senderName, false, true);
        sendPacketTo(senderName, fail);
        emit logMessage(QStringLiteral("[GameCtrl] Invite %1 → %2 failed: target in game.")
                            .arg(senderName, targetName));
        return;
    }

    // ۳) آیا فرستنده خودش در بازی نیست؟
    if (m_activeGames.contains(senderName)) {
        InvitePacket fail(targetName, senderName, false, true);
        sendPacketTo(senderName, fail);
        emit logMessage(QStringLiteral("[GameCtrl] Invite %1 → %2 failed: sender in game.")
                            .arg(senderName, targetName));
        return;
    }

    // همه چیز اوکیه — ذخیره در Pending و فوروارد به هدف
    m_pendingInvites.insert(targetName, senderName);
    sendPacketTo(targetName, *invite);
    emit logMessage(QStringLiteral("[GameCtrl] Invite forwarded: %1 → %2")
                        .arg(senderName, targetName));
}

void ServerGameController::handleInviteResponse(ClientHandler *sender, InvitePacket *response)
{
    // پاسخ‌دهنده (کسی که دعوت را دریافت کرده بود)
    const QString responder = findUsername(sender);
    if (responder.isEmpty()) return;

    // آیا واقعاً pending invite دارد؟
    if (!m_pendingInvites.contains(responder)) {
        emit logMessage(QStringLiteral("[GameCtrl] No pending invite for \"%1\".")
                            .arg(responder));
        return;
    }

    const QString requester = m_pendingInvites.value(responder);

    // حذف از Pending
    m_pendingInvites.remove(responder);

    if (response->accepted) {
        // ── دعوت پذیرفته شد ──

        // ثبت بازی فعال (دوطرفه)
        m_activeGames.insert(requester, responder);
        m_activeGames.insert(responder, requester);

        // --- ایجاد GameSession ---
        QSharedPointer<GameSession> session = QSharedPointer<GameSession>::create(requester, responder);
        m_sessions.insert(requester, session);
        m_sessions.insert(responder, session);


        // فوروارد پاسخ قبول به درخواست‌کننده
        sendPacketTo(requester, *response);

        // ارسال GAME_START به هر دو
        GameStartPacket startForRequester(responder);
        GameStartPacket startForResponder(requester);
        sendPacketTo(requester, startForRequester);
        sendPacketTo(responder, startForResponder);

        emit logMessage(QStringLiteral("[GameCtrl] Game started: \"%1\" vs \"%2\"")
                            .arg(requester, responder));
        emit gameStarted(requester, responder);
    } else {
        // ── دعوت رد شد ──

        // فوروارد پاسخ رد به درخواست‌کننده
        sendPacketTo(requester, *response);

        emit logMessage(QStringLiteral("[GameCtrl] Invite declined: \"%1\" rejected \"%2\"")
                            .arg(responder, requester));
    }
}

// ============================================================
//  مدیریت حرکت بازی
// ============================================================
void ServerGameController::handleGameMove(ClientHandler *sender, GameMovePacket *packet)
{
    const QString username = packet->player;
    const QString move     = packet->move;

    // پیدا کردن سشن
    if (!m_sessions.contains(username)) {
        qDebug() << "Received move from" << username << "but no active session found.";
        return;
    }

    auto session = m_sessions.value(username);
    
    // ثبت حرکت
    session->addMove(username, move);
    
    emit logMessage(QStringLiteral("[GameCtrl] %1 played %2").arg(username, move));

    // چک کردن پایان بازی
    if (session->isFinished()) {
        QString winner = session->determineWinner();
        QString player1 = session->player1();
        QString player2 = session->player2();
        
        QString move1 = session->getMove(player1);
        QString move2 = session->getMove(player2);

        // ارسال نتیجه به Player 1
        {
            QString resultMsg;
            if (winner == "DRAW") resultMsg = "It's a Draw!";
            else if (winner == player1) resultMsg = "You Won!";
            else resultMsg = "You Lost!";
            
            GameResultPacket res(winner, resultMsg + QString(" (Opponent played: %1)").arg(move2));
            sendPacketTo(player1, res);
        }

        // ارسال نتیجه به Player 2
        {
            QString resultMsg;
            if (winner == "DRAW") resultMsg = "It's a Draw!";
            else if (winner == player2) resultMsg = "You Won!";
            else resultMsg = "You Lost!";

            GameResultPacket res(winner, resultMsg + QString(" (Opponent played: %1)").arg(move1));
            sendPacketTo(player2, res);
        }

        emit logMessage(QStringLiteral("[GameCtrl] Game finished. Winner: %1").arg(winner));
        emit gameEnded(player1, player2);

        // پاکسازی سشن
        m_sessions.remove(player1);
        m_sessions.remove(player2);
        // m_activeGames is also cleared in unregisterClient but we should clear it here too to be safe/consistent if we used it
        m_activeGames.remove(player1);
        m_activeGames.remove(player2);
    }
}
