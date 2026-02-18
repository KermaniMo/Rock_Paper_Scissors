#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QMessageBox>
#include "stylemanager.h"
#include "gamecontroller.h"

MainWindow::MainWindow(const QString &username,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cameraThread(new QThread(this))
    , m_worker(new CameraWorker())
{
    ui->setupUi(this);

    // Set the username in the UI
    ui->label_player_title->setText("🎮 " + username);
    this->setWindowTitle("Rock Paper Scissors - " + username);
    // ==================== QSS Dark Theme ====================
    this->setStyleSheet(StyleManager::darkTheme());

    // ==================== Initial State ====================
    // Debug view hidden by default (exists in layout but invisible)
    ui->label_debug->setVisible(false);

    // Initial text
    ui->label_camera->setText("Ready");
    ui->label_result->setText("Ready?");
    // --- SIMULATION DEFAULT (Offline) ---
    m_currentGesture = "Rock";
    ui->label_fingers->setText("🪨 Rock (Simulated)");

    // Connect Game Button (Defined in UI now)
    connect(ui->btnPlayRound, &QPushButton::clicked, this, &MainWindow::onPlayRoundClicked);


    // ==================== Thread Setup ====================
    m_worker->moveToThread(m_cameraThread);
    connect(m_cameraThread, &QThread::finished, m_worker, &QObject::deleteLater);

    // Button connections
    connect(ui->btn_start, &QPushButton::clicked, this, &MainWindow::startCamera);
    connect(ui->btn_stop,  &QPushButton::clicked, this, &MainWindow::stopCamera);
    connect(ui->btn_start, &QPushButton::clicked, this, &MainWindow::startCamera);
    connect(ui->btn_stop,  &QPushButton::clicked, this, &MainWindow::stopCamera);


    // --- Invite Connections ---
    connect(ui->btnInvite, &QPushButton::clicked, this, &MainWindow::onInviteClicked);

    GameController &ctrl = GameController::instance();
    connect(&ctrl, &GameController::inviteReceived, this, &MainWindow::onInviteReceived);
    connect(&ctrl, &GameController::inviteResult, this, &MainWindow::onInviteResult);
    connect(&ctrl, &GameController::inviteResult, this, &MainWindow::onInviteResult);
    connect(&ctrl, &GameController::gameStarted, this, &MainWindow::onGameStarted);
    connect(&ctrl, &GameController::gameResult, this, &MainWindow::onGameResult);


    // Frame updates from worker thread
    connect(m_worker, &CameraWorker::frameReady, this, &MainWindow::updateFrame);

    // Start the thread
    m_cameraThread->start();
}

MainWindow::~MainWindow()
{
    QMetaObject::invokeMethod(m_worker, "stopWork", Qt::QueuedConnection);
    m_cameraThread->quit();
    m_cameraThread->wait();
    delete ui;
}

// ==================== Camera Control ====================

void MainWindow::startCamera()
{
    m_running = true;

    // Loading feedback
    ui->label_camera->clear();
    ui->label_camera->setText("⏳ Initializing Camera...");
    ui->label_camera->setAlignment(Qt::AlignCenter);
    ui->label_result->setText("Get Ready...");
    ui->label_fingers->setText("Detecting...");

    QMetaObject::invokeMethod(m_worker, "startWork", Qt::QueuedConnection);
}

void MainWindow::stopCamera()
{
    m_running = false;
    QMetaObject::invokeMethod(m_worker, "stopWork", Qt::QueuedConnection);

    ui->label_camera->clear();
    ui->label_camera->setText("Camera Stopped");
    ui->label_debug->clear();
    ui->label_debug->setText("Debug Stopped");
    ui->label_fingers->setText("Waiting for gesture...");
    ui->label_result->setText("Ready?");
    m_currentGesture.clear();
}

// ==================== Frame Update ====================

void MainWindow::updateFrame(QImage mainImg, QImage debugImg, QString msg, int fingers)
{
    Q_UNUSED(fingers);

    if (!m_running) {
        return;
    }

    // Display main camera feed
    ui->label_camera->setPixmap(
        QPixmap::fromImage(mainImg).scaled(
            ui->label_camera->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation
        )
    );

    // Conditional debug view (only update if visible)
    if (ui->label_debug->isVisible() && !debugImg.isNull()) {
        ui->label_debug->setPixmap(
            QPixmap::fromImage(debugImg).scaled(
                ui->label_debug->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation
            )
        );
    }

    // Show gesture name only (Rock/Scissors/Paper)
    ui->label_fingers->setText(msg);

    // Cache current gesture for Play Round
    m_currentGesture = msg;
}

// ==================== Play Round (Game Logic) ====================

// ==================== Play Round Logic Removed (Offline Mode) ====================


// ==================== Debug Toggle (Ctrl+D) ====================

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_D) {
        ui->label_debug->setVisible(!ui->label_debug->isVisible());
    }
    QMainWindow::keyPressEvent(event);
}

// ==================== Invite Logic ====================

void MainWindow::onInviteClicked()
{
    QString target = ui->txtOpponent->text().trimmed();
    if (target.isEmpty()) {
        QMessageBox::warning(this, "Invite", "Please enter an opponent's name.");
        return;
    }

    // Prevent self-invite
    if (target == GameController::instance().myUsername()) {
        QMessageBox::warning(this, "Invite", "You cannot invite yourself!");
        return;
    }

    GameController::instance().sendInvite(target);
    ui->label_result->setText("Inviting " + target + "...");
}

void MainWindow::onInviteReceived(const QString &sender)
{
    // Ask user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Game Invite",
                                  sender + " wants to play with you!\nAccept?",
                                  QMessageBox::Yes | QMessageBox::No);

    bool accepted = (reply == QMessageBox::Yes);
    GameController::instance().sendInviteResponse(sender, accepted);

    if (accepted) {
        ui->label_result->setText("Accepted invite from " + sender);
        // Could auto-start here, but better wait for GAME_START
    } else {
        ui->label_result->setText("Rejected invite from " + sender);
    }
}

void MainWindow::onInviteResult(bool success)
{
    if (success) {
        ui->label_result->setText("Opponent ACCEPTED!");
        // Waiting for GAME_START packet to actually start
    } else {
        ui->label_result->setText("Opponent REJECTED or BUSY.");
        QMessageBox::information(this, "Invite Result", "The opponent declined or is not available.");
    }
}

void MainWindow::onGameStarted(const QString &opponent)
{
    QString msg = QString("Game Started vs %1!").arg(opponent);
    ui->label_result->setText(msg);
    ui->label_player_title->setText("🎮 YOU vs " + opponent);

    // Auto-start camera if not already running
    if (!m_running) {
        startCamera();
    }

    QMessageBox::information(this, "Game Start", msg);

    // Enable network play button
    ui->btnPlayRound->setEnabled(true);
    ui->btnPlayRound->setText("✋ Send Move");
}

void MainWindow::onPlayRoundClicked()
{
    // 1. Check if gesture is detected
    if (m_currentGesture.isEmpty() || m_currentGesture == "None" || m_currentGesture == "No hand detected") {
        QMessageBox::warning(this, "Move", "No gesture detected! Please show your hand to the camera.");
        return;
    }

    // 2. Parse gesture name
    QString move;
    if (m_currentGesture.contains("Rock", Qt::CaseInsensitive)) move = "Rock";
    else if (m_currentGesture.contains("Paper", Qt::CaseInsensitive)) move = "Paper";
    else if (m_currentGesture.contains("Scissors", Qt::CaseInsensitive)) move = "Scissors";
    else {
        QMessageBox::warning(this, "Move", "Gesture not clear. Please hold steady.");
        return;
    }

    // 3. Send to server
    GameController::instance().playMove(move);

    // 4. Update UI
    ui->btnPlayRound->setEnabled(false);
    ui->btnPlayRound->setText("⏳ Waiting...");
    ui->label_result->setText("Move sent! Waiting for opponent...");
}

void MainWindow::onGameResult(QString winner, QString message)
{
    // Show result
    ui->label_result->setText(message);
    
    QString title = "Game Result";
    if (winner == "DRAW") title = "Draw";
    else if (winner == GameController::instance().myUsername()) title = "Victory! 🎉";
    else title = "Defeat 💀";

    QMessageBox::information(this, title, message);

    // Reset button for next round
    ui->btnPlayRound->setEnabled(false);
    ui->btnPlayRound->setText("✋ Send Move");
    
    // Reset title to normal
    ui->label_player_title->setText("🎮 " + GameController::instance().myUsername());
}
