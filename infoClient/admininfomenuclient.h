#ifndef ADMININFOMENUCLIENT_H
#define ADMININFOMENUCLIENT_H

#include"../Standard.h"
#include"admininfoclient.h"
#include"admininfodoctorclient.h"

namespace Ui {
class adminInfoMenuClient;
}

class adminInfoMenuClient : public QWidget
{
    Q_OBJECT

public:
    explicit adminInfoMenuClient(QWidget *parent = nullptr);
    ~adminInfoMenuClient();

private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::adminInfoMenuClient *ui;
    TcpClient *tcpClient = TcpClient::instance();
    adminInfoClient *admininfoclient=nullptr;
    adminInfoDoctorClient *admininfodoctorclient=nullptr;
};

#endif // ADMININFOMENUCLIENT_H
