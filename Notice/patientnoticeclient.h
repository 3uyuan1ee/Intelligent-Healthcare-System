#ifndef PATIENTNOTICECLIENT_H
#define PATIENTNOTICECLIENT_H

#include"../Standard.h"

namespace Ui {
class patientNoticeClient;
}

class patientNoticeClient : public QWidget
{
    Q_OBJECT

public:
    explicit patientNoticeClient(QWidget *parent = nullptr);
    ~patientNoticeClient();
private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onDataReceivedNotice(const QJsonObject &data);
    void on_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent *event) override; // 重写关闭事件


private:
    Ui::patientNoticeClient *ui;
    TcpClient *tcpClient=TcpClient::instance();
};

#endif // PATIENTNOTICECLIENT_H
