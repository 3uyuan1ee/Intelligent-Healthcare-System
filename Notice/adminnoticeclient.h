#ifndef ADMINNOTICECLIENT_H
#define ADMINNOTICECLIENT_H

#include"../Standard.h"

namespace Ui {
class adminNoticeClient;
}

class adminNoticeClient : public QWidget
{
    Q_OBJECT

public:
    explicit adminNoticeClient(QWidget *parent = nullptr);
    ~adminNoticeClient();

private slots:
    void on_pushButton_clicked();

private:
    Ui::adminNoticeClient *ui;
    TcpClient *tcpclient=TcpClient::instance();
};

#endif // ADMINNOTICECLIENT_H
