#ifndef DOCTORADVICEMENUCLIENT_H
#define DOCTORADVICEMENUCLIENT_H

#include"doctoradviceclient.h"

#include"../Standard.h"

namespace Ui {
class doctorAdviceMenuClient;
}

class doctorAdviceMenuClient : public QWidget
{
    Q_OBJECT

public:
    explicit doctorAdviceMenuClient(QWidget *parent = nullptr);
    ~doctorAdviceMenuClient();
private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽


    void on_pushButton_clicked();

private:
    Ui::doctorAdviceMenuClient *ui;
    TcpClient *tcpClient;
    doctorAdviceClient *doctoradviceclient;
};

#endif // DOCTORADVICEMENUCLIENT_H
