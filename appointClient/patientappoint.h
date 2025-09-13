#ifndef PATIENTAPPOINT_H
#define PATIENTAPPOINT_H

#include"../Standard.h"

namespace Ui {
class patientAppoint;
}

class patientAppoint : public QWidget
{
    Q_OBJECT

public:
    explicit patientAppoint(QWidget *parent = nullptr);
    ~patientAppoint();
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onJudgeReceived(const QJsonObject &data);

private slots:
    void on_pushButton_clicked();

    void on_commitDoctorPushButton_clicked();

private:
    Ui::patientAppoint *ui;
    TcpClient *tcpClient = TcpClient::instance();
};

#endif // PATIENTAPPOINT_H
