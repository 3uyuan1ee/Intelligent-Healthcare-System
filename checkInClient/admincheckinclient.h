#ifndef ADMINCHECKINCLIENT_H
#define ADMINCHECKINCLIENT_H

#include"../Standard.h"
#include"admincheckinresultclient.h"

namespace Ui {
class adminCheckInClient;
}

class adminCheckInClient : public QWidget
{
    Q_OBJECT

public:
    explicit adminCheckInClient(QWidget *parent = nullptr);
    ~adminCheckInClient();

private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::adminCheckInClient *ui;
    adminCheckInResultClient *admincheckinresultclient=nullptr;
    TcpClient *tcpClient;
};

#endif // ADMINCHECKINCLIENT_H
