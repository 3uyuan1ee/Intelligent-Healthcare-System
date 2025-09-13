#ifndef DOCTORCASECLIENT_H
#define DOCTORCASECLIENT_H

#include"../Standard.h"
#include"doctorcaseeditclient.h"

namespace Ui {
class doctorCaseClient;
}

class doctorCaseClient : public QWidget
{
    Q_OBJECT

public:

    explicit doctorCaseClient(QWidget *parent = nullptr);
    ~doctorCaseClient();


private slots:
    void onDataReceived(const QJsonObject &data);  // 添加数据接收槽
    void onErrorOccurred(const QString &error);  // 添加错误处理槽

    void on_pushButton_clicked();

private:
    Ui::doctorCaseClient *ui;
    TcpClient *tcpClient;
    doctorCaseEditClient *doctorcaseeditclient=nullptr;
};

#endif // DOCTORCASECLIENT_H
