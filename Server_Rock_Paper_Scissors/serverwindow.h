#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ServerWindow;
}
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow() override;

private slots:
    void onStartStopClicked();
    void onLogMessage(const QString &msg);

    // --- New Slots for UI Updates ---
    void onClientRegistered(const QString &username);
    void onClientUnregistered(const QString &username);
    void onGameStarted(const QString &p1, const QString &p2);
    void onGameEnded(const QString &p1, const QString &p2);

private:
    Ui::ServerWindow *ui;
    bool m_running = false;
};

#endif // SERVERWINDOW_H
