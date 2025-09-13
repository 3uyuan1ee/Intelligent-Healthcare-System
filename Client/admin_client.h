#ifndef ADMIN_CLIENT_H
#define ADMIN_CLIENT_H

#include"../Standard.h"

#include"../checkInClient/admincheckinclient.h"
#include"../Notice/adminnoticeclient.h"
#include"../infoClient/admininfomenuclient.h"

namespace Ui {
class Admin_Client;
}

class Admin_Client : public QWidget
{
    Q_OBJECT

public:
    explicit Admin_Client(QWidget *parent = nullptr);
    ~Admin_Client();

private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    void on_adminCheckInPushButton_clicked();

    void on_adminInfoPushButton_clicked();

    void on_adminNoticePushButton_clicked();

private:
    Ui::Admin_Client *ui;
    TcpClient *tcpClient;
    adminCheckInClient *admincheckinclient=nullptr;
    adminNoticeClient *adminnoticeclient=nullptr;
    adminInfoMenuClient *admininfomenuclient=nullptr;
};

#endif // ADMIN_CLIENT_H
