#include "loginform.h"
#include "ui_loginform.h"
#include "gamecontroller.h"
#include "mainwindow.h"
#include "stylemanager.h" // Added

#include <QMessageBox>
#include <QDebug>

// ============================================================
//  Constructor
// ============================================================

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    setWindowTitle("Rock Paper Scissors — Login");

    // Apply Dark Theme
    this->setStyleSheet(StyleManager::darkTheme());

    // --- اتصال دکمه ---
    connect(ui->btnLogin, &QPushButton::clicked,
            this, &LoginForm::onBtnLoginClicked);

    // --- اتصال به سیگنال‌های GameController ---
    GameController &gc = GameController::instance();

    connect(&gc, &GameController::loginSuccess,
            this, &LoginForm::onLoginSuccess);

    connect(&gc, &GameController::loginFailed,
            this, &LoginForm::onLoginFailed);

    connect(&gc, &GameController::connectionFailed,
            this, &LoginForm::onConnectionFailed);
}

LoginForm::~LoginForm()
{
    delete ui;
}

// ============================================================
//  دکمه Login
// ============================================================

void LoginForm::onBtnLoginClicked()
{
    QString username = ui->txtUsername->text().trimmed();
    QString ip       = ui->txtIp->text().trimmed();
    quint16 port     = static_cast<quint16>(ui->txtPort->text().toUInt());

    if (username.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً نام کاربری وارد کنید.");
        return;
    }

    // غیرفعال کردن دکمه تا پاسخ بیاید
    ui->btnLogin->setEnabled(false);

    // ارسال درخواست اتصال و لاگین
    GameController::instance().connectAndLogin(username, ip, port);
}

// ============================================================
//  پاسخ‌ها از GameController
// ============================================================

void LoginForm::onLoginSuccess(const QString &username)
{
    qDebug() << "LoginForm: login success for" << username;

    // ساخت و نمایش MainWindow
    MainWindow *mainWin = new MainWindow(username);
    mainWin->show();

    // بستن فرم لاگین
    this->close();
}

void LoginForm::onLoginFailed(const QString &reason)
{
    QMessageBox::critical(this, "Login Failed", reason);
    ui->btnLogin->setEnabled(true);
}

void LoginForm::onConnectionFailed(const QString &reason)
{
    QMessageBox::critical(this, "Connection Failed", reason);
    ui->btnLogin->setEnabled(true);
}
