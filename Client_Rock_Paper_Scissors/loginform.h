#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginForm;
}
QT_END_NAMESPACE

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void onBtnLoginClicked();
    void onLoginSuccess(const QString &username);
    void onLoginFailed(const QString &reason);
    void onConnectionFailed(const QString &reason);

private:
    Ui::LoginForm *ui;
};

#endif // LOGINFORM_H
