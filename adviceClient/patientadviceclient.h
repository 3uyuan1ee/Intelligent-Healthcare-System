#ifndef PATIENTADVICECLIENT_H
#define PATIENTADVICECLIENT_H

#include"../Standard.h"

namespace Ui {
class patientAdviceClient;
}

class patientAdviceClient : public QWidget
{
    Q_OBJECT

public:
    explicit patientAdviceClient(QWidget *parent = nullptr);
    ~patientAdviceClient();

private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽


private:
    Ui::patientAdviceClient *ui;
    TcpClient *tcpClient;
};

#endif // PATIENTADVICECLIENT_H
