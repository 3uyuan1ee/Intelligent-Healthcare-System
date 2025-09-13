#ifndef PATIENTINFO_H
#define PATIENTINFO_H

#include"../Standard.h"

namespace Ui {
class patientInfo;
}

class patientInfo : public QWidget
{
    Q_OBJECT

public:
    explicit patientInfo(QWidget *parent = nullptr);
    ~patientInfo();


private slots:

    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onJudgeReceived(const QJsonObject &data);

    void on_editPushButton_clicked();

    void on_savePushButton_clicked();

private:
    Ui::patientInfo *ui;
    TcpClient *tcpClient = TcpClient::instance();
};

#endif // PATIENTINFO_H
