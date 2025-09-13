#ifndef PATIENT_CLIENT_H
#define PATIENT_CLIENT_H
#include "../infoClient/patientinfo.h"
#include"../appointClient/patientappoint.h"
#include"../caseClient/patientcasemenuclient.h"
#include"../adviceClient/patientadviceclient.h"
#include"../QuestionClient/patienthealthquestionclient.h"
#include"../Notice/patientnoticeclient.h"

#include"../Standard.h"

// // 前向声明
// class TcpClient;
// class patientInfo;

namespace Ui {
class Patient_Client;
}

class Patient_Client : public QWidget
{
    Q_OBJECT

public:
    explicit Patient_Client(QWidget *parent = nullptr);
    ~Patient_Client();


private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    void on_infoPushButton_clicked();

    void on_appointPushButton_clicked();

    void on_casePushButton_clicked();

    void on_advicePushButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Patient_Client *ui;
    TcpClient *tcpClient;
    patientInfo *patientInfo_Client = nullptr; // 添加成员变量
    patientAppoint *patientappoint = nullptr;
    patientCaseMenuClient *patientcasemenuclient = nullptr;
    patientAdviceClient *patientadviceclient=nullptr;
    patientHealthQuestionClient *patienthealthquestionclient=nullptr;
    patientNoticeClient *patientnoticeclient=nullptr;
};

#endif // PATIENT_CLIENT_H
