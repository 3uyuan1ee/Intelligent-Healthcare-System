#ifndef DOCTORCHECKINCLIENT_H
#define DOCTORCHECKINCLIENT_H

#include"../Standard.h"

namespace Ui {
class doctorCheckInClient;
}

class doctorCheckInClient : public QWidget
{
    Q_OBJECT

public:
    explicit doctorCheckInClient(QWidget *parent = nullptr);
    ~doctorCheckInClient();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onJudgeReceived(const QJsonObject &data);
    void onJudgeReceivedAttendence(const QJsonObject &data);

private:
    QString useDate;
    Ui::doctorCheckInClient *ui;
    TcpClient *tcpClient;

};

#endif // DOCTORCHECKINCLIENT_H
