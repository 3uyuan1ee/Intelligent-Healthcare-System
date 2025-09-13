#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include<QJsonObject>
#include<QByteArray>
#include <QSharedPointer>
#include<QTimer>
#include<QMessageBox>
#include <QMutexLocker>
#include"../Instance/StateManager.h"


class TcpClient : public QObject
{
    Q_OBJECT

public:

    // 单例模式，返回单例实例
    // 单例模式获取实例
    static TcpClient* instance();

    // 删除拷贝构造函数和赋值运算符（确保单例）
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    void connectToServer(const QString &host, int port);
    void sendData(const QByteArray &data);
    void sendData(const QJsonObject &data);
    void sendData(const QString &command, const QJsonObject &data);
    //QByteArray receiveData();
    // QJsonObject receiveJson();
    void disconnectFromServer();

    void setTimeout(int timeout);  // 设置超时时间
    void stopTimeout();  // 停止超时定时器


signals:
    void dataReceivedJson(const QJsonObject &jsonData); // 转换信号
    //void dataReceived(const QJsonObject &data);

    void errorOccurred(const QString &error);

    void loginTimeout();  // 超时信号

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

    void onTimeout();  // 超时处理槽


private:

    // 私有构造函数（确保只能通过instance()创建）
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    // 静态实例指针 - 必须声明！
    static TcpClient* m_instance;


    QTcpSocket *socket;

    QMutex m_mutex;

    QTimer *timeoutTimer;  // 用于超时的定时器
    QTimer *timeoutTimerSend;
    int timeoutDuration;   // 超时持续时间（单位：毫秒）
};

#endif // TCPCIENT_H
