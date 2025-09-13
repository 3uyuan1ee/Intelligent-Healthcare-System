#ifndef DOCTORADVICECLIENT_H
#define DOCTORADVICECLIENT_H

#include"../Standard.h"

namespace Ui {
class doctorAdviceClient;
}

class doctorAdviceClient : public QWidget
{
    Q_OBJECT

public:
    explicit doctorAdviceClient(QWidget *parent = nullptr);
    ~doctorAdviceClient();
    QString comboText;

private slots:
    void on_pushButton_clicked();
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽
    void onDataReceivedJudge(const QJsonObject &data);

    void on_pushButton_2_clicked();

private:
    Ui::doctorAdviceClient *ui;
    TcpClient *tcpClient;
    QJsonObject patientInfo;
};

#endif // DOCTORADVICECLIENT_H
