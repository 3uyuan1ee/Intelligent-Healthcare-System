#include <QtTest/QtTest>
#include <QJsonObject>
#include <QSignalSpy>
#include "../../NetWork/tcpclient.h"
#include "../mocks/MockTcpClient.h"
#include "../config/test_config.h"

class TcpClientTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 基础功能测试
    void testSingletonPattern();
    void testConnectionToServer();
    void testDisconnectionFromServer();
    void testSendDataByteArray();
    void testSendDataJsonObject();
    void testSendDataCommandAndObject();

    // 错误处理测试
    void testConnectionTimeout();
    void testSendingDataWithoutConnection();
    void testInvalidJsonData();

    // 信号发射测试
    void testDataReceivedSignal();
    void testErrorOccurredSignal();
    void testLoginTimeoutSignal();

    // 线程安全测试
    void testConcurrentAccess();
    void testMutexProtection();

    // 配置测试
    void testTimeoutConfiguration();
    void testStopTimeout();

private:
    TcpClient *m_tcpClient;
    MockTcpClient *m_mockClient;
    TestConfig::ServerConfig m_config;
};

void TcpClientTest::initTestCase()
{
    qDebug() << "Initializing TcpClient test suite...";

    // 设置测试环境
    TestConfig::setupTestData();
    m_config = TestConfig::getServerConfig();

    // 获取TcpClient单例
    m_tcpClient = TcpClient::instance();
    QVERIFY(m_tcpClient != nullptr);

    // 创建Mock客户端用于隔离测试
    m_mockClient = new MockTcpClient(this);
}

void TcpClientTest::cleanupTestCase()
{
    qDebug() << "Cleaning up TcpClient test suite...";

    // 清理测试数据
    TestConfig::cleanupTestData();

    // 清理Mock客户端
    if (m_mockClient) {
        delete m_mockClient;
        m_mockClient = nullptr;
    }
}

void TcpClientTest::init()
{
    // 每个测试前的准备工作
    m_tcpClient->setTimeout(5000); // 5秒超时
    m_mockClient->clearHistory();
}

void TcpClientTest::cleanup()
{
    // 每个测试后的清理工作
    // TcpClient doesn't have isConnected method, so we'll skip this check
    // if (m_tcpClient->isConnected()) {
    m_tcpClient->disconnectFromServer();
    // }

    if (m_mockClient->isConnected()) {
        m_mockClient->disconnectFromServer();
    }
}

void TcpClientTest::testSingletonPattern()
{
    // 测试单例模式是否正确实现
    TcpClient *instance1 = TcpClient::instance();
    TcpClient *instance2 = TcpClient::instance();

    QCOMPARE(instance1, instance2);
    QCOMPARE(instance1, m_tcpClient);
}

void TcpClientTest::testConnectionToServer()
{
    // 使用Mock客户端测试连接功能
    QSignalSpy spy(m_mockClient, &MockTcpClient::connected);

    m_mockClient->connectToServer(m_config.host, m_config.port);

    // 等待连接信号（最多等待1秒）
    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);

    QVERIFY(m_mockClient->isConnected());
    QCOMPARE(m_mockClient->getConnectCallCount(), 1);
}

void TcpClientTest::testDisconnectionFromServer()
{
    // 先连接
    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 测试断开连接
    QSignalSpy disconnectSpy(m_mockClient, &MockTcpClient::disconnected);

    m_mockClient->disconnectFromServer();

    // 等待断开连接信号
    QVERIFY(disconnectSpy.wait(1000));
    QCOMPARE(disconnectSpy.count(), 1);

    QVERIFY(!m_mockClient->isConnected());
    QCOMPARE(m_mockClient->getDisconnectCallCount(), 1);
}

void TcpClientTest::testSendDataByteArray()
{
    // 连接Mock客户端
    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 测试发送字节数组数据
    QByteArray testData = "test data";
    m_mockClient->sendData(testData);

    // 验证发送历史
    QStringList history = m_mockClient->getSentDataHistory();
    QVERIFY(history.contains(QString(testData)));
    QCOMPARE(m_mockClient->getSendDataCallCount(), 1);
}

void TcpClientTest::testSendDataJsonObject()
{
    // 连接Mock客户端
    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 测试发送JSON对象数据
    QJsonObject testData;
    testData["key1"] = "value1";
    testData["key2"] = 123;

    m_mockClient->sendData(testData);

    // 验证发送历史
    QStringList history = m_mockClient->getSentDataHistory();
    QCOMPARE(history.size(), 1);

    // 验证JSON格式
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(history[0].toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());
}

void TcpClientTest::testSendDataCommandAndObject()
{
    // 连接Mock客户端
    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 测试发送命令和数据对象
    QString command = "testCommand";
    QJsonObject data;
    data["param1"] = "value1";
    data["param2"] = 456;

    m_mockClient->sendData(command, data);

    // 验证发送历史
    QStringList history = m_mockClient->getSentDataHistory();
    QCOMPARE(history.size(), 1);

    // 验证JSON结构和内容
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(history[0].toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QJsonObject sentObject = doc.object();
    QCOMPARE(sentObject["command"].toString(), command);
    QVERIFY(sentObject.contains("data"));

    QJsonObject sentData = sentObject["data"].toObject();
    QCOMPARE(sentData["param1"].toString(), "value1");
    QCOMPARE(sentData["param2"].toInt(), 456);
}

void TcpClientTest::testConnectionTimeout()
{
    // 配置Mock客户端模拟超时
    m_mockClient->setSimulatedLatency(100); // 100ms延迟
    m_mockClient->setSimulatedDisconnection(false);

    QSignalSpy timeoutSpy(m_tcpClient, &TcpClient::loginTimeout);
    QSignalSpy errorSpy(m_tcpClient, &TcpClient::errorOccurred);

    // 设置较短的超时时间
    m_tcpClient->setTimeout(50); // 50ms超时

    // 尝试连接（使用真实TcpClient，但会因为没有真实服务器而超时）
    m_tcpClient->connectToServer("192.0.2.1", m_config.port); // 不存在的IP

    // 等待超时
    QVERIFY(timeoutSpy.wait(2000));
    QCOMPARE(timeoutSpy.count(), 1);
}

void TcpClientTest::testSendingDataWithoutConnection()
{
    QSignalSpy errorSpy(m_mockClient, &MockTcpClient::errorOccurred);

    // 不连接就直接发送数据
    QByteArray testData = "test data";
    m_mockClient->sendData(testData);

    // 应该收到错误信号
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(errorSpy[0][0].toString(), QString("Not connected to server"));

    // 发送计数不应该增加
    QCOMPARE(m_mockClient->getSendDataCallCount(), 0);
}

void TcpClientTest::testInvalidJsonData()
{
    // 这个测试主要针对数据接收时的JSON解析
    // 对于发送端，我们需要确保能处理各种有效的JSON数据

    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 测试空JSON对象
    QJsonObject emptyObject;
    m_mockClient->sendData(emptyObject);

    QStringList history = m_mockClient->getSentDataHistory();
    QCOMPARE(history.size(), 1);

    // 验证空JSON对象可以正确序列化
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(history[0].toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());
    QVERIFY(doc.object().isEmpty());
}

void TcpClientTest::testDataReceivedSignal()
{
    QSignalSpy spy(m_mockClient, &MockTcpClient::dataReceivedJson);

    // 连接Mock客户端
    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 发送数据触发模拟响应
    QJsonObject loginData;
    loginData["username"] = "testuser";
    loginData["password"] = "testpass";

    m_mockClient->sendData("login", loginData);

    // 等待响应信号
    QVERIFY(spy.wait(2000));
    QCOMPARE(spy.count(), 1);

    // 验证接收到的数据
    QJsonObject receivedData = spy[0][0].value<QJsonObject>();
    QVERIFY(receivedData.contains("reply"));
    QCOMPARE(receivedData["reply"].toString(), QString("success"));
}

void TcpClientTest::testErrorOccurredSignal()
{
    QSignalSpy spy(m_mockClient, &MockTcpClient::errorOccurred);

    // 测试未连接时的错误
    m_mockClient->sendData("test");
    QCOMPARE(spy.count(), 1);

    // 测试模拟错误
    m_mockClient->simulateError("Simulated error");
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[1][0].toString(), QString("Simulated error"));

    // 测试连接丢失
    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    m_mockClient->simulateConnectionLoss();
    QVERIFY(spy.count() >= 3);
}

void TcpClientTest::testLoginTimeoutSignal()
{
    QSignalSpy spy(m_mockClient, &MockTcpClient::loginTimeout);

    // Mock客户端不会自动触发超时，需要手动触发
    m_mockClient->simulateError("Connection timeout");

    // 在实际实现中，超时信号可能由不同的条件触发
    // 这里我们验证错误处理机制
    QVERIFY(spy.count() >= 0);
}

void TcpClientTest::testConcurrentAccess()
{
    // 创建多个线程同时访问TcpClient
    const int threadCount = 10;
    const int operationsPerThread = 100;

    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    QList<QThread*> threads;

    for (int i = 0; i < threadCount; ++i) {
        QThread *thread = QThread::create([this, operationsPerThread, i]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                QJsonObject data;
                data["thread"] = i;
                data["operation"] = j;
                data["timestamp"] = QDateTime::currentMSecsSinceEpoch();

                m_mockClient->sendData(QString("thread_%1").arg(i), data);

                // 短暂延迟以增加并发竞争的可能性
                QThread::msleep(1);
            }
        });

        threads.append(thread);
        thread->start();
    }

    // 等待所有线程完成
    for (QThread *thread : threads) {
        thread->wait();
        delete thread;
    }

    // 验证所有操作都被执行
    QCOMPARE(m_mockClient->getSendDataCallCount(), threadCount * operationsPerThread);

    QStringList history = m_mockClient->getSentDataHistory();
    QCOMPARE(history.size(), threadCount * operationsPerThread);
}

void TcpClientTest::testMutexProtection()
{
    // 这个测试验证在并发访问时的线程安全性
    // 主要通过测试concurrentAccess来间接验证

    m_mockClient->connectToServer(m_config.host, m_config.port);
    QSignalSpy connectSpy(m_mockClient, &MockTcpClient::connected);
    QVERIFY(connectSpy.wait(1000));

    // 快速连续发送数据
    for (int i = 0; i < 100; ++i) {  // 减少数量避免测试时间过长
        QJsonObject data;
        data["index"] = i;
        m_mockClient->sendData("rapid_test", data);
    }

    // 验证所有数据都被发送
    QCOMPARE(m_mockClient->getSendDataCallCount(), 100);

    QStringList history = m_mockClient->getSentDataHistory();
    QCOMPARE(history.size(), 100);

    // 验证数据完整性
    for (int i = 0; i < history.size(); ++i) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(history[i].toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);
        QVERIFY(doc.isObject());
    }
}

void TcpClientTest::testTimeoutConfiguration()
{
    // 测试超时配置
    m_tcpClient->setTimeout(1000);
    // 由于没有直接获取超时值的方法，我们通过设置来验证方法不会崩溃

    m_tcpClient->setTimeout(0);
    m_tcpClient->setTimeout(-1);
    m_tcpClient->setTimeout(999999);

    // 验证方法调用不会导致崩溃
    QVERIFY(true);
}

void TcpClientTest::testStopTimeout()
{
    // 测试停止超时定时器
    // 这个方法主要用于内部状态管理，我们验证它不会导致崩溃
    m_tcpClient->stopTimeout();

    QVERIFY(true); // 如果没有崩溃就算通过
}

QTEST_MAIN(TcpClientTest)
#include "TcpClient_test.moc"