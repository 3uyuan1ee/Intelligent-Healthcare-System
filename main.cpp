#include "login_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login_Widget w;
    w.show();
    return a.exec();
}
