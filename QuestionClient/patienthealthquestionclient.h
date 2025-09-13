#ifndef PATIENTHEALTHQUESTIONCLIENT_H
#define PATIENTHEALTHQUESTIONCLIENT_H

#include"../Standard.h"

namespace Ui {
class patientHealthQuestionClient;
}

class patientHealthQuestionClient : public QWidget
{
    Q_OBJECT

public:
    explicit patientHealthQuestionClient(QWidget *parent = nullptr);
    ~patientHealthQuestionClient();


private slots:
    void on_pushButton_clicked();
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

private:
    Ui::patientHealthQuestionClient *ui;
    TcpClient *tcpClient;
};

#endif // PATIENTHEALTHQUESTIONCLIENT_H
