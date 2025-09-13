#ifndef PATIENTCASEMENUCLIENT_H
#define PATIENTCASEMENUCLIENT_H

#include"../Standard.h"

namespace Ui {
class patientCaseMenuClient;
}

class patientCaseMenuClient : public QWidget
{
    Q_OBJECT

public:
    explicit patientCaseMenuClient(QWidget *parent = nullptr);
    ~patientCaseMenuClient();


private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽



private:
    Ui::patientCaseMenuClient *ui;
    TcpClient *tcpClient;
};

#endif // PATIENTCASEMENUCLIENT_H
