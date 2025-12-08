#ifndef MOCKTCPCLIENT_H
#define MOCKTCPCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QByteArray>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include "../config/test_config.h"

class MockTcpClient : public QObject
{
    Q_OBJECT

public:
    explicit MockTcpClient(QObject *parent = nullptr);
    ~MockTcpClient();

    // 模拟连接管理
    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    bool isConnected() const;

    // 模拟数据发送
    void sendData(const QByteArray &data);
    void sendData(const QJsonObject &data);
    void sendData(const QString &command, const QJsonObject &data);

    // 模拟状态控制
    void setSimulatedLatency(int milliseconds);
    void setSimulatedErrorRate(double rate); // 0.0 - 1.0
    void setSimulatedDisconnection(bool enable);
    void setResponseQueue(const QQueue<QByteArray> &responses);

    // 测试辅助方法
    void simulateServerResponse(const QJsonObject &response);
    void simulateError(const QString &error);
    void simulateConnectionLoss();
    void clearHistory();

    // 测试验证方法
    QStringList getSentDataHistory() const;
    int getConnectCallCount() const;
    int getDisconnectCallCount() const;
    int getSendDataCallCount() const;
    bool isConnectionSimulated() const;

signals:
    void dataReceivedJson(const QJsonObject &jsonData);
    void errorOccurred(const QString &error);
    void loginTimeout();
    void connected();
    void disconnected();

private slots:
    void onSimulatedResponse();
    void onSimulatedLatency();
    void onSimulatedError();

private:
    void processReceivedData(const QByteArray &data);
    QJsonObject parseJsonData(const QByteArray &data);
    void simulateNetworkBehavior();

private:
    // 连接状态
    bool m_isConnected;
    QString m_host;
    int m_port;

    // 模拟网络行为
    int m_simulatedLatency;
    double m_simulatedErrorRate;
    bool m_simulatedDisconnection;
    QTimer *m_latencyTimer;

    // 响应队列
    QQueue<QByteArray> m_responseQueue;
    QTimer *m_responseTimer;

    // 错误模拟
    QTimer *m_errorTimer;
    bool m_hasPendingError;

    // 发送历史记录
    mutable QMutex m_historyMutex;
    QStringList m_sentDataHistory;
    int m_connectCallCount;
    int m_disconnectCallCount;
    int m_sendDataCallCount;

    // 测试配置
    TestConfig::ServerConfig m_config;
};

#endif // MOCKTCPCLIENT_H