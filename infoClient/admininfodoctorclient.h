#ifndef ADMININFODOCTORCLIENT_H
#define ADMININFODOCTORCLIENT_H

#include"../Standard.h"

namespace Ui {
class adminInfoDoctorClient;
}

class adminInfoDoctorClient : public QWidget
{
    Q_OBJECT

public:
    explicit adminInfoDoctorClient(QWidget *parent = nullptr);
    ~adminInfoDoctorClient();
    QString usr;
private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onJudgeReceived(const QJsonObject &data);

    void on_editPushButton_2_clicked();

    void on_savePushButton_2_clicked();

private:
    Ui::adminInfoDoctorClient *ui;
    TcpClient *tcpClient = TcpClient::instance();
};

#endif // ADMININFODOCTORCLIENT_H
