#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QJsonObject>
#include <QString>
#include <QJsonArray>
#include <memory>
#include "protocol.h"

// ============================================================
//  IGamePacket — اینترفیس پایه (Abstract Base Class)
// ============================================================
class IGamePacket {
public:
    virtual ~IGamePacket() = default;

    // تبدیل پکت به JSON
    virtual QJsonObject toJson() const = 0;

    // نوع پیام (از GameProtocol::MessageType)
    virtual int getType() const = 0;
};

// ============================================================
//  LoginPacket — پکت ورود
// ============================================================
class LoginPacket : public IGamePacket {
public:
    QString username;
    bool    success    = false;   // فقط برای LOGIN_RES
    QString message;              // فقط برای LOGIN_RES
    bool    isResponse = false;   // آیا این پاسخ لاگین است؟

    // ساخت دستی (برای ارسال LOGIN_REQ)
    explicit LoginPacket(const QString &username)
        : username(username), isResponse(false) {}

    // ساخت از JSON (هم REQ و هم RES)
    explicit LoginPacket(const QJsonObject &json)
        : username(json[GameProtocol::KEY_USERNAME].toString()),
          success(json[GameProtocol::KEY_SUCCESS].toBool()),
          message(json[GameProtocol::KEY_MESSAGE].toString()),
          isResponse(json[GameProtocol::KEY_TYPE].toInt() == GameProtocol::LOGIN_RES) {}

    QJsonObject toJson() const override {
        QJsonObject obj;
        obj[GameProtocol::KEY_TYPE]     = isResponse ? GameProtocol::LOGIN_RES
                                                     : GameProtocol::LOGIN_REQ;
        obj[GameProtocol::KEY_USERNAME] = username;
        if (isResponse) {
            obj[GameProtocol::KEY_SUCCESS] = success;
            obj[GameProtocol::KEY_MESSAGE] = message;
        }
        return obj;
    }

    int getType() const override {
        return isResponse ? GameProtocol::LOGIN_RES : GameProtocol::LOGIN_REQ;
    }
};

// ============================================================
//  InvitePacket — پکت دعوت به بازی
// ============================================================
class InvitePacket : public IGamePacket {
public:
    QString sender;
    QString target;
    bool    accepted   = false;  // فقط برای پاسخ (RES)
    bool    isResponse = false;  // آیا این پاسخ دعوت است؟

    // ساخت دستی
    InvitePacket(const QString &sender,
                 const QString &target,
                 bool accepted   = false,
                 bool isResponse = false)
        : sender(sender), target(target),
          accepted(accepted), isResponse(isResponse) {}

    // ساخت از JSON
    explicit InvitePacket(const QJsonObject &json)
        : sender(json[GameProtocol::KEY_USERNAME].toString()),
          target(json[GameProtocol::KEY_TARGET].toString()),
          accepted(json[GameProtocol::KEY_SUCCESS].toBool()),
          isResponse(json[GameProtocol::KEY_TYPE].toInt() == GameProtocol::INVITE_RES) {}

    QJsonObject toJson() const override {
        QJsonObject obj;
        obj[GameProtocol::KEY_TYPE]     = isResponse ? GameProtocol::INVITE_RES
                                                     : GameProtocol::INVITE_REQ;
        obj[GameProtocol::KEY_USERNAME] = sender;
        obj[GameProtocol::KEY_TARGET]   = target;
        if (isResponse)
            obj[GameProtocol::KEY_SUCCESS] = accepted;
        return obj;
    }

    int getType() const override {
        return isResponse ? GameProtocol::INVITE_RES : GameProtocol::INVITE_REQ;
    }
};

// ============================================================
//  GameMovePacket — پکت حرکت (سنگ / کاغذ / قیچی)
// ============================================================
class GameMovePacket : public IGamePacket {
public:
    QString player;
    QString move;  // "Rock", "Paper", "Scissors"

    // ساخت دستی
    GameMovePacket(const QString &player, const QString &move)
        : player(player), move(move) {}

    // ساخت از JSON
    explicit GameMovePacket(const QJsonObject &json)
        : player(json[GameProtocol::KEY_USERNAME].toString()),
          move(json[GameProtocol::KEY_MOVE].toString()) {}

    QJsonObject toJson() const override {
        QJsonObject obj;
        obj[GameProtocol::KEY_TYPE]     = GameProtocol::GAME_MOVE;
        obj[GameProtocol::KEY_USERNAME] = player;
        obj[GameProtocol::KEY_MOVE]     = move;
        return obj;
    }

    int getType() const override {
        return GameProtocol::GAME_MOVE;
    }
};

// ============================================================
//  GameResultPacket — پکت نتیجه بازی
// ============================================================
class GameResultPacket : public IGamePacket {
public:
    QString winner;
    QString message;

    // ساخت دستی
    GameResultPacket(const QString &winner, const QString &message)
        : winner(winner), message(message) {}

    // ساخت از JSON
    explicit GameResultPacket(const QJsonObject &json)
        : winner(json[GameProtocol::KEY_WINNER].toString()),
          message(json[GameProtocol::KEY_MESSAGE].toString()) {}

    QJsonObject toJson() const override {
        QJsonObject obj;
        obj[GameProtocol::KEY_TYPE]    = GameProtocol::GAME_RESULT;
        obj[GameProtocol::KEY_WINNER]  = winner;
        obj[GameProtocol::KEY_MESSAGE] = message;
        return obj;
    }

    int getType() const override {
        return GameProtocol::GAME_RESULT;
    }
};

// ============================================================
//  GameStartPacket — پکت شروع بازی
// ============================================================
class GameStartPacket : public IGamePacket {
public:
    QString opponent;

    // ساخت دستی
    explicit GameStartPacket(const QString &opponent)
        : opponent(opponent) {}

    // ساخت از JSON
    explicit GameStartPacket(const QJsonObject &json)
        : opponent(json[GameProtocol::KEY_TARGET].toString()) {}

    QJsonObject toJson() const override {
        QJsonObject obj;
        obj[GameProtocol::KEY_TYPE]   = GameProtocol::GAME_START;
        obj[GameProtocol::KEY_TARGET] = opponent;
        return obj;
    }

    int getType() const override {
        return GameProtocol::GAME_START;
    }
};

// ============================================================
//  PacketFactory — ساخت پکت بر اساس JSON (Factory Method)
// ============================================================
class PacketFactory {
public:
    static std::unique_ptr<IGamePacket> createPacket(const QJsonObject &json) {
        if (!json.contains(GameProtocol::KEY_TYPE))
            return nullptr;

        const int type = json[GameProtocol::KEY_TYPE].toInt();

        switch (type) {
        case GameProtocol::LOGIN_REQ:
        case GameProtocol::LOGIN_RES:
            return std::make_unique<LoginPacket>(json);

        case GameProtocol::INVITE_REQ:
        case GameProtocol::INVITE_RES:
            return std::make_unique<InvitePacket>(json);

        case GameProtocol::GAME_MOVE:
            return std::make_unique<GameMovePacket>(json);

        case GameProtocol::GAME_START:
            return std::make_unique<GameStartPacket>(json);

        case GameProtocol::GAME_RESULT:
            return std::make_unique<GameResultPacket>(json);

        default:
            return nullptr;
        }
    }
};

#endif // GAMEDATA_H
