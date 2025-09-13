#ifndef ADMININFOCLIENT_H
#define ADMININFOCLIENT_H

#include"../Standard.h"

namespace Ui {
class adminInfoClient;
}

class adminInfoClient : public QWidget
{
    Q_OBJECT

public:
    explicit adminInfoClient(QWidget *parent = nullptr);
    ~adminInfoClient();
    QString usr;
private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onJudgeReceived(const QJsonObject &data);

    void on_editPushButton_clicked();

    void on_savePushButton_clicked();

private:
    Ui::adminInfoClient *ui;
    TcpClient *tcpClient = TcpClient::instance();
};

#endif // ADMININFOCLIENT_H
