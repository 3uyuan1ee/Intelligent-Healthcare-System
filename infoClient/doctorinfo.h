#ifndef DOCTORINFO_H
#define DOCTORINFO_H

#include"../Standard.h"

namespace Ui {
class doctorInfo;
}

class doctorInfo : public QWidget
{
    Q_OBJECT

public:
    explicit doctorInfo(QWidget *parent = nullptr);
    ~doctorInfo();

private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onJudgeReceived(const QJsonObject &data);
    void on_editPushButton_2_clicked();

    void on_savePushButton_2_clicked();

private:
    Ui::doctorInfo *ui;
    TcpClient *tcpClient = TcpClient::instance();
};

#endif // DOCTORINFO_H
