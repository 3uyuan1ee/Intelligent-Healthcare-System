#ifndef DOCTOR_CLIENT_H
#define DOCTOR_CLIENT_H
#include "../infoClient/doctorinfo.h"
#include"../checkInClient/doctorcheckinclient.h"
#include"../caseClient/doctorcaseclient.h"
#include"../adviceClient/doctoradvicemenuclient.h"
#include"../Notice/doctornoticeclient.h"

#include"../Standard.h"
namespace Ui {
class Doctor_Client;
}

class Doctor_Client : public QWidget
{
    Q_OBJECT

public:
    explicit Doctor_Client(QWidget *parent = nullptr);
    ~Doctor_Client();


private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    void on_docInfoPushButton_clicked();

    void on_docCasePushButton_clicked();

    void on_docAdvicePushButton_clicked();

    void on_checkInPushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Doctor_Client *ui;
    TcpClient *tcpClient;
    doctorInfo *doctorInfo_Client =nullptr;
    doctorCheckInClient *doctorcheckinclient=nullptr;
    doctorCaseClient *doctorcaseclient=nullptr;
    doctorAdviceMenuClient *doctoradvicemenuClient=nullptr;
    doctorNoticeClient *doctornoticeclient=nullptr;
    QList<DataManager::AppointmentInfo> appointments;
};

#endif // DOCTOR_CLIENT_H
