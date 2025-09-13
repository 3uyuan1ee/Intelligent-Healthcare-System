#ifndef ADMINCHECKINRESULTCLIENT_H
#define ADMINCHECKINRESULTCLIENT_H

#include"../Standard.h"

namespace Ui {
class adminCheckInResultClient;
}

class adminCheckInResultClient : public QWidget
{
    Q_OBJECT

public:
    explicit adminCheckInResultClient(QWidget *parent = nullptr);
    ~adminCheckInResultClient();

private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

private:
    Ui::adminCheckInResultClient *ui;
    TcpClient *tcpClient;
};

#endif // ADMINCHECKINRESULTCLIENT_H
