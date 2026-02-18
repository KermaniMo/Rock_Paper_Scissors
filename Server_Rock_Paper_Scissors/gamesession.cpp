#include "gamesession.h"
#include <QDebug>

GameSession::GameSession(const QString &player1, const QString &player2)
    : m_player1(player1), m_player2(player2)
{
}

void GameSession::addMove(const QString &player, const QString &move)
{
    // فقط اگر بازیکن معتبر باشد و قبلاً حرکت نزده باشد
    if ((player == m_player1 || player == m_player2) && !m_moves.contains(player)) {
        m_moves.insert(player, move);
        qDebug() << "GameSession: Move registered for" << player << ":" << move;
    }
}

bool GameSession::isFinished() const
{
    // بازی زمانی تمام است که 2 حرکت ثبت شده باشد
    return m_moves.size() == 2;
}

QString GameSession::getMove(const QString &player) const
{
    return m_moves.value(player, "");
}

QString GameSession::determineWinner() const
{
    if (!isFinished()) return "";

    const QString move1 = m_moves.value(m_player1);
    const QString move2 = m_moves.value(m_player2);

    if (move1 == move2) {
        return "DRAW";
    }

    // منطق بازی: Rock > Scissors, Scissors > Paper, Paper > Rock
    // حرکات استاندارد: "Rock", "Paper", "Scissors" (Case sensitive as per client implementation)

    if (move1 == "Rock") {
        if (move2 == "Scissors") return m_player1;
        if (move2 == "Paper")    return m_player2;
    }
    else if (move1 == "Paper") {
        if (move2 == "Rock")     return m_player1;
        if (move2 == "Scissors") return m_player2;
    }
    else if (move1 == "Scissors") {
        if (move2 == "Paper")    return m_player1;
        if (move2 == "Rock")     return m_player2;
    }

    // حالت‌های نامعتبر یا عجیب (نباید رخ دهد اگر کلاینت درست بفرستد)
    return "DRAW";
}
