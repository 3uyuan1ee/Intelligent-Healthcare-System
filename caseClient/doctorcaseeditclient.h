#ifndef DOCTORCASEEDITCLIENT_H
#define DOCTORCASEEDITCLIENT_H

#include"../Standard.h"

namespace Ui {
class doctorCaseEditClient;
}

class doctorCaseEditClient : public QWidget
{
    Q_OBJECT

public:
    explicit doctorCaseEditClient(QWidget *parent = nullptr);
    ~doctorCaseEditClient();
    QString comboText;

private slots:
    void on_pushButton_clicked();
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onDataReceivedJudge(const QJsonObject &data);

    void on_pushButton_2_clicked();

private:
    Ui::doctorCaseEditClient *ui;
    TcpClient *tcpClient;
    QJsonObject patientInfo;
};

#endif // DOCTORCASEEDITCLIENT_H
