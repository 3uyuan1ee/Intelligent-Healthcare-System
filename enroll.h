#ifndef ENROLL_H
#define ENROLL_H

#include"Standard.h"
namespace Ui {
class Enroll;
}

class Enroll : public QWidget
{
    Q_OBJECT

public:
    Enroll(QWidget *parent = nullptr);
    ~Enroll();

private slots:
    void on_EnrollWindow_cancel_pushButton_clicked();

    void on_EnrollWindow_enroll_pushButton_clicked();

    void onRegisterFinished(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
private:
    Ui::Enroll *ui;
    TcpClient *tcpClient;  // 添加 TcpClient 对象
    const QString command="register";
};

#endif // ENROLL_H
