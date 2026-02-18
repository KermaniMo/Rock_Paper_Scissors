#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QPixmap>
#include <QLabel>
#include <QKeyEvent>
#include <QRandomGenerator>
#include "cameraworker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &username,QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startCamera();
    void stopCamera();
    void updateFrame(QImage mainImg, QImage debugImg, QString msg, int fingers);

    // --- Invite Slots ---
    //void on_btn_play_clicked();
    void onInviteClicked(); // Renamed to avoid double-connection by auto-connect
    void onInviteReceived(const QString &sender);
    void onInviteResult(bool success);
    void onGameStarted(const QString &opponent);
    void onPlayRoundClicked();
    void onGameResult(QString winner, QString message);

    //void onInviteClicked();



private:
    Ui::MainWindow *ui;
    QThread *m_cameraThread;
    CameraWorker *m_worker;
    bool m_running = false;
    QString m_currentGesture;
};
#endif // MAINWINDOW_H
