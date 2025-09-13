#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include"Standard.h"

#include"Fun./function.h"
#include"enroll.h"

// #include<QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Patient_login;
}
QT_END_NAMESPACE

class Login_Widget : public QWidget
{
    Q_OBJECT

public:
    Login_Widget(QWidget *parent = nullptr);
    ~Login_Widget();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();


    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    // void onLoginTimeout();  // 用于处理登录超时的槽函数
private:
    Ui::Patient_login *ui;
    TcpClient *tcpClient;  // 添加 TcpClient 对象
    QString type;
    QString id;
    QString filename;
    QString username;
    QString command="login";
    // QTimer *timeoutTimer;  // 添加 QTimer 对象
};
#endif // LOGIN_WIDGET_H
