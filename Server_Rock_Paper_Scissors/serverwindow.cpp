#include "serverwindow.h"
#include "./ui_serverwindow.h"
#include "servermanager.h"
#include "servergamecontroller.h" // Added update updates
#include "stylemanager.h" // Added

#include <QHostAddress>

// ============================================================
//  Constructor
// ============================================================
ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
    , m_running(false)
{
    ui->setupUi(this);

    // Apply Dark Theme
    this->setStyleSheet(StyleManager::darkTheme());

    // مقدار پیش‌فرض پورت
    ui->txtPort->setText("12345");

    // اتصال دکمه شروع/توقف
    connect(ui->btnStartStop, &QPushButton::clicked,
            this,             &ServerWindow::onStartStopClicked);

    // اتصال به سیگنال لاگ سرور
    connect(&ServerManager::instance(), &ServerManager::logMessage,
            this,                       &ServerWindow::onLogMessage);

    // --- اتصال به سیگنال‌های GameController ---
    ServerGameController &gc = ServerGameController::instance();
    connect(&gc, &ServerGameController::logMessage,         this, &ServerWindow::onLogMessage);
    connect(&gc, &ServerGameController::clientRegistered,   this, &ServerWindow::onClientRegistered);
    connect(&gc, &ServerGameController::clientUnregistered, this, &ServerWindow::onClientUnregistered);
    connect(&gc, &ServerGameController::gameStarted,        this, &ServerWindow::onGameStarted);
    connect(&gc, &ServerGameController::gameEnded,          this, &ServerWindow::onGameEnded);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

// ============================================================
//  شروع / توقف سرور
// ============================================================
void ServerWindow::onStartStopClicked()
{
    if (!m_running) {
        // --- شروع سرور ---
        QString ipText = ui->txtIP->text().trimmed();
        QHostAddress address(ipText.isEmpty() ? QStringLiteral("0.0.0.0") : ipText);

        quint16 port = ui->txtPort->text().toUShort();
        if (port == 0) port = 12345;

        bool ok = ServerManager::instance().startServer(address, port);
        if (ok) {
            m_running = true;
            ui->btnStartStop->setText(QStringLiteral("Stop Server"));
            ui->txtPort->setEnabled(false);
            ui->txtIP->setEnabled(false);
        }
    } else {
        // --- توقف سرور ---
        ServerManager::instance().stopServer();
        m_running = false;
        ui->btnStartStop->setText(QStringLiteral("Start Server"));
        ui->txtPort->setEnabled(true);
        ui->txtIP->setEnabled(true);
    }
}

// ============================================================
//  نمایش لاگ در لیست
// ============================================================
void ServerWindow::onLogMessage(const QString &msg)
{
    ui->logList->addItem(msg);
    ui->logList->scrollToBottom();
}

// ============================================================
//  بروزرسانی لیست کلاینت‌ها
// ============================================================
void ServerWindow::onClientRegistered(const QString &username)
{
    // جلوگیری از تکرار
    auto items = ui->listClients->findItems(username, Qt::MatchExactly);
    if (items.isEmpty()) {
        ui->listClients->addItem(username);
    }
}

void ServerWindow::onClientUnregistered(const QString &username)
{
    auto items = ui->listClients->findItems(username, Qt::MatchExactly);
    for (auto item : items) {
        delete ui->listClients->takeItem(ui->listClients->row(item));
    }
}

// ============================================================
//  بروزرسانی لیست بازی‌های فعال
// ============================================================
void ServerWindow::onGameStarted(const QString &p1, const QString &p2)
{
    QString sessionName = QString("%1 vs %2").arg(p1, p2);
    ui->listGames->addItem(sessionName);
}

void ServerWindow::onGameEnded(const QString &p1, const QString &p2)
{
    // چون ترتیب نام‌ها ممکن است فرق کند، هر دو حالت را چک می‌کنیم
    QString name1 = QString("%1 vs %2").arg(p1, p2);
    QString name2 = QString("%2 vs %1").arg(p2, p1);

    auto items = ui->listGames->findItems(name1, Qt::MatchExactly);
    if (items.isEmpty()) {
        items = ui->listGames->findItems(name2, Qt::MatchExactly);
    }

    for (auto item : items) {
        delete ui->listGames->takeItem(ui->listGames->row(item));
    }
}
