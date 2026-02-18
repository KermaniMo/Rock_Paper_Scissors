#include "loginform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ابتدا فرم لاگین نمایش داده می‌شود
    // بعد از لاگین موفق، MainWindow از داخل LoginForm ساخته می‌شود
    LoginForm login;
    login.show();

    return a.exec();
}
