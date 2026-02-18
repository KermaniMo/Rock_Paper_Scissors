#ifndef GAMESESSION_H
#define GAMESESSION_H

#include <QString>
#include <QMap>

class GameSession
{
public:
    GameSession(const QString &player1, const QString &player2);

    // ثبت حرکت یک بازیکن
    void addMove(const QString &player, const QString &move);

    // آیا بازی تمام شده (هر دو بازیکن حرکت زده‌اند)؟
    bool isFinished() const;

    // تعیین برنده (نام برنده یا "DRAW")
    QString determineWinner() const;

    // دریافت حرکت یک بازیکن خاص (برای ارسال در پایان بازی به حریف)
    QString getMove(const QString &player) const;

    QString player1() const { return m_player1; }
    QString player2() const { return m_player2; }

private:
    QString m_player1;
    QString m_player2;
    QMap<QString, QString> m_moves; // key: username, value: move (Rock/Paper/Scissors)
};

#endif // GAMESESSION_H
