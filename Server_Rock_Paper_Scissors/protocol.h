#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>

namespace GameProtocol {

// --- انواع پیام‌ها (Message Types) ---
enum MessageType {
    LOGIN_REQ       = 1,   // ورود
    LOGIN_RES       = 2,   // پاسخ ورود
    USER_LIST_REQ   = 3,   // درخواست لیست
    USER_LIST_RES   = 4,   // ارسال لیست
    INVITE_REQ      = 5,   // درخواست بازی
    INVITE_RES      = 6,   // پاسخ دعوت
    GAME_START      = 7,   // شروع بازی
    GAME_MOVE       = 8,   // حرکت (سنگ/کاغذ)
    GAME_RESULT     = 9,   // نتیجه
    ERROR_MSG       = 99   // خطا
};

// --- کلیدهای JSON (برای جلوگیری از غلط املایی) ---
static const QString KEY_TYPE      = "type";
static const QString KEY_USERNAME  = "username";
static const QString KEY_TARGET    = "target";
static const QString KEY_PAYLOAD   = "payload"; // لیست یوزرها
static const QString KEY_MOVE      = "move";
static const QString KEY_WINNER    = "winner";
static const QString KEY_SUCCESS   = "success";
static const QString KEY_MESSAGE   = "message";

}

#endif // PROTOCOL_H
