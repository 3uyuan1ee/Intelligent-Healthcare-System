#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTimer>
#include <QThread>

#include "../../NetWork/tcpclient.h"
#include "../../Instance/UserSession.h"
#include "../../Instance/StateManager.h"
#include "../../Fun./JsonMessageBuilder.h"
#include "../../Fun./DataManager.h"
#include "../mocks/MockTcpClient.h"
#include "../config/test_config.h"

class NetworkIntegrationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 完整的网络通信流程测试
    void testCompleteLoginFlow();
    void testCompleteAppointmentFlow();
    void testCompleteCaseFlow();
    void testCompleteAdviceFlow();

    // 错误处理流程测试
    void testNetworkErrorHandling();
    void testServerErrorHandling();
    void testTimeoutHandling();

    // 数据一致性测试
    void testDataConsistencyAcrossComponents();
    void testUserSessionPersistence();
    void testStateManagementIntegration();

    // 并发测试
    void testConcurrentRequests();
    void testRaceConditionHandling();

private:
    TcpClient* m_tcpClient;
    MockTcpClient* m_mockClient;
    QTimer* m_testTimer;

    // 辅助方法
    QJsonObject createLoginRequest(const QString& username, const QString& password);
    QJsonObject createLoginResponse(bool success, const QString& userType = "patient");
    QJsonObject createAppointmentResponse();
    void simulateNetworkResponse(const QJsonObject& response);
    void waitForResponse(int timeoutMs = 5000);
};

void NetworkIntegrationTest::initTestCase()
{
    qDebug() << "网络集成测试开始";

    // 初始化测试环境
    qRegisterMetaType<QJsonObject>("QJsonObject");
}

void NetworkIntegrationTest::cleanupTestCase()
{
    qDebug() << "网络集成测试完成";
}

void NetworkIntegrationTest::init()
{
    m_tcpClient = TcpClient::instance();
    m_mockClient = new MockTcpClient(this);
    m_testTimer = new QTimer(this);

    // 设置测试定时器，避免测试阻塞
    m_testTimer->setSingleShot(true);
    m_testTimer->setInterval(10000); // 10秒超时

    connect(m_testTimer, &QTimer::timeout, [this]() {
        qWarning() << "测试超时";
        QFAIL("测试超时");
    });
}

void NetworkIntegrationTest::cleanup()
{
    if (m_tcpClient) {
        m_tcpClient->disconnectFromServer();
    }

    delete m_mockClient;
    delete m_testTimer;

    // 清理用户会话
    UserSession::instance();
    StateManager::instance().setState(WidgetState::Idle);
}

void NetworkIntegrationTest::testCompleteLoginFlow()
{
    qDebug() << "测试完整登录流程";

    m_testTimer->start();

    // 1. 准备登录请求
    QString testUsername = "test_patient";
    QString testPassword = "password123";
    QJsonObject loginRequest = createLoginRequest(testUsername, testPassword);

    // 2. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);
    QSignalSpy errorSpy(m_tcpClient, &TcpClient::errorOccurred);

    // 3. 发送登录请求
    m_tcpClient->sendData("LOGIN", loginRequest);

    // 4. 模拟服务器响应
    QTimer::singleShot(100, [this, testUsername]() {
        QJsonObject loginResponse = createLoginResponse(true, "patient");
        loginResponse["username"] = testUsername;
        loginResponse["name"] = "测试患者";
        loginResponse["id"] = "P001";
        simulateNetworkResponse(loginResponse);
    });

    // 5. 等待响应
    QTest::qWait(1000);

    // 6. 验证结果
    QVERIFY(dataReceivedSpy.count() > 0);
    QCOMPARE(errorSpy.count(), 0);

    // 验证用户会话
    UserSession& session = UserSession::instance();
    QCOMPARE(session.getValue("username"), testUsername);
    QCOMPARE(session.getValue("type"), QString("patient"));
    QCOMPARE(session.getValue("name"), QString("测试患者"));

    // 验证状态管理
    StateManager& stateManager = StateManager::instance();
    QCOMPARE(stateManager.currentState(), WidgetState::LoggedIn);

    m_testTimer->stop();
    qDebug() << "登录流程测试通过";
}

void NetworkIntegrationTest::testCompleteAppointmentFlow()
{
    qDebug() << "测试完整预约流程";

    m_testTimer->start();

    // 1. 首先设置登录状态
    UserSession& session = UserSession::instance();
    session.setUserInfo(QJsonObject(), "patient", "test_patient");
    StateManager::instance().setState(WidgetState::LoggedIn);

    // 2. 创建预约请求
    JsonMessageBuilder builder("MAKE_APPOINTMENT");
    builder.addAppointment("test_patient", "doctor001", "2024-01-20", "10:00", "50", "pending");

    // 3. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);
    QSignalSpy stateSpy(&StateManager::instance(), &StateManager::stateChanged);

    // 4. 发送预约请求
    m_tcpClient->sendData(builder.build());

    // 5. 模拟服务器响应
    QTimer::singleShot(100, [this]() {
        QJsonObject appointmentResponse = createAppointmentResponse();
        simulateNetworkResponse(appointmentResponse);
    });

    // 6. 等待响应
    waitForResponse();

    // 7. 验证结果
    QVERIFY(dataReceivedSpy.count() > 0);

    // 验证响应数据
    QList<QVariant> arguments = dataReceivedSpy.last();
    QJsonObject response = arguments.at(0).toJsonObject();

    QCOMPARE(response["command"].toString(), QString("APPOINTMENT_RESULT"));
    QCOMPARE(response["status"].toString(), QString("success"));

    m_testTimer->stop();
    qDebug() << "预约流程测试通过";
}

void NetworkIntegrationTest::testCompleteCaseFlow()
{
    qDebug() << "测试完整病例流程";

    m_testTimer->start();

    // 1. 设置医生登录状态
    UserSession& session = UserSession::instance();
    session.setUserInfo(QJsonObject(), "doctor", "test_doctor");
    StateManager::instance().setState(WidgetState::LoggedIn);

    // 2. 创建病例请求
    JsonMessageBuilder builder("ADD_CASE");
    builder.addCase("patient001", "test_doctor", "2024-01-20", "11:00",
                   "头痛发热", "体温38.5℃", "无特殊病史",
                   "血常规检查", "上呼吸道感染");

    // 3. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);

    // 4. 发送病例请求
    m_tcpClient->sendData(builder.build());

    // 5. 模拟服务器响应
    QTimer::singleShot(100, [this]() {
        QJsonObject caseResponse;
        caseResponse["command"] = "CASE_RESULT";
        caseResponse["status"] = "success";
        caseResponse["message"] = "病例添加成功";
        simulateNetworkResponse(caseResponse);
    });

    // 6. 等待响应
    waitForResponse();

    // 7. 验证结果
    QVERIFY(dataReceivedSpy.count() > 0);

    QList<QVariant> arguments = dataReceivedSpy.last();
    QJsonObject response = arguments.at(0).toJsonObject();

    QCOMPARE(response["command"].toString(), QString("CASE_RESULT"));
    QCOMPARE(response["status"].toString(), QString("success"));

    m_testTimer->stop();
    qDebug() << "病例流程测试通过";
}

void NetworkIntegrationTest::testCompleteAdviceFlow()
{
    qDebug() << "测试完整医嘱流程";

    m_testTimer->start();

    // 1. 设置医生登录状态
    UserSession& session = UserSession::instance();
    session.setUserInfo(QJsonObject(), "doctor", "test_doctor");
    StateManager::instance().setState(WidgetState::LoggedIn);

    // 2. 创建医嘱请求
    JsonMessageBuilder builder("ADD_ADVICE");
    builder.addAdvice("patient001", "test_doctor", "2024-01-20", "12:00",
                     "阿司匹林", "血常规检查", "物理降温", "多休息");

    // 3. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);

    // 4. 发送医嘱请求
    m_tcpClient->sendData(builder.build());

    // 5. 模拟服务器响应
    QTimer::singleShot(100, [this]() {
        QJsonObject adviceResponse;
        adviceResponse["command"] = "ADVICE_RESULT";
        adviceResponse["status"] = "success";
        adviceResponse["message"] = "医嘱添加成功";
        simulateNetworkResponse(adviceResponse);
    });

    // 6. 等待响应
    waitForResponse();

    // 7. 验证结果
    QVERIFY(dataReceivedSpy.count() > 0);

    QList<QVariant> arguments = dataReceivedSpy.last();
    QJsonObject response = arguments.at(0).toJsonObject();

    QCOMPARE(response["command"].toString(), QString("ADVICE_RESULT"));
    QCOMPARE(response["status"].toString(), QString("success"));

    m_testTimer->stop();
    qDebug() << "医嘱流程测试通过";
}

void NetworkIntegrationTest::testNetworkErrorHandling()
{
    qDebug() << "测试网络错误处理";

    m_testTimer->start();

    // 1. 设置信号监听
    QSignalSpy errorSpy(m_tcpClient, &TcpClient::errorOccurred);
    QSignalSpy timeoutSpy(m_tcpClient, &TcpClient::loginTimeout);

    // 2. 模拟网络错误
    QTimer::singleShot(100, [this]() {
        emit m_tcpClient->errorOccurred("网络连接失败");
    });

    // 3. 等待错误信号
    QTest::qWait(500);

    // 4. 验证错误处理
    QVERIFY(errorSpy.count() > 0);

    QList<QVariant> arguments = errorSpy.first();
    QString errorMessage = arguments.at(0).toString();
    QCOMPARE(errorMessage, QString("网络连接失败"));

    // 验证状态恢复
    StateManager& stateManager = StateManager::instance();
    QCOMPARE(stateManager.currentState(), WidgetState::Error);

    m_testTimer->stop();
    qDebug() << "网络错误处理测试通过";
}

void NetworkIntegrationTest::testServerErrorHandling()
{
    qDebug() << "测试服务器错误处理";

    m_testTimer->start();

    // 1. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);

    // 2. 发送请求
    QJsonObject request;
    request["command"] = "TEST_ERROR";
    m_tcpClient->sendData(request);

    // 3. 模拟服务器错误响应
    QTimer::singleShot(100, [this]() {
        QJsonObject errorResponse;
        errorResponse["command"] = "ERROR";
        errorResponse["code"] = 500;
        errorResponse["message"] = "服务器内部错误";
        simulateNetworkResponse(errorResponse);
    });

    // 4. 等待响应
    waitForResponse();

    // 5. 验证错误处理
    QVERIFY(dataReceivedSpy.count() > 0);

    QList<QVariant> arguments = dataReceivedSpy.last();
    QJsonObject response = arguments.at(0).toJsonObject();

    QCOMPARE(response["command"].toString(), QString("ERROR"));
    QCOMPARE(response["code"].toInt(), 500);

    m_testTimer->stop();
    qDebug() << "服务器错误处理测试通过";
}

void NetworkIntegrationTest::testTimeoutHandling()
{
    qDebug() << "测试超时处理";

    m_testTimer->start();

    // 1. 设置超时时间
    m_tcpClient->setTimeout(1000); // 1秒超时

    // 2. 设置信号监听
    QSignalSpy timeoutSpy(m_tcpClient, &TcpClient::loginTimeout);
    QSignalSpy errorSpy(m_tcpClient, &TcpClient::errorOccurred);

    // 3. 发送请求但不响应
    QJsonObject request;
    request["command"] = "NO_RESPONSE_TEST";
    m_tcpClient->sendData(request);

    // 4. 等待超时
    QTest::qWait(2000); // 等待2秒，超过1秒超时时间

    // 5. 验证超时处理
    QVERIFY(timeoutSpy.count() > 0);

    // 验证状态恢复
    StateManager& stateManager = StateManager::instance();
    QCOMPARE(stateManager.currentState(), WidgetState::Error);

    m_testTimer->stop();
    qDebug() << "超时处理测试通过";
}

void NetworkIntegrationTest::testDataConsistencyAcrossComponents()
{
    qDebug() << "测试组件间数据一致性";

    // 1. 设置用户信息
    QJsonObject userInfo;
    userInfo["username"] = "consistency_test";
    userInfo["name"] = "一致性测试用户";
    userInfo["id"] = "C001";

    UserSession& session = UserSession::instance();
    session.setUserInfo(userInfo, "patient", "consistency_test");

    // 2. 验证JsonMessageBuilder能正确获取用户信息
    JsonMessageBuilder builder("TEST_COMMAND");
    QJsonObject message = builder.build();

    QCOMPARE(message["username"].toString(), QString("consistency_test"));
    QCOMPARE(message["type"].toString(), QString("patient"));

    // 3. 验证StateManager状态与用户类型一致
    StateManager::instance().setState(WidgetState::LoggedIn);
    QCOMPARE(StateManager::instance().currentState(), WidgetState::LoggedIn);

    qDebug() << "数据一致性测试通过";
}

void NetworkIntegrationTest::testUserSessionPersistence()
{
    qDebug() << "测试用户会话持久化";

    // 1. 设置用户信息
    QJsonObject userInfo;
    userInfo["username"] = "persistence_test";
    userInfo["name"] = "持久化测试用户";
    userInfo["id"] = "P001";
    userInfo["phoneNumber"] = "13800138000";

    UserSession& session = UserSession::instance();
    session.setUserInfo(userInfo, "patient", "persistence_test");

    // 2. 验证信息保存
    QCOMPARE(session.getValue("username"), QString("persistence_test"));
    QCOMPARE(session.getValue("name"), QString("持久化测试用户"));
    QCOMPARE(session.getValue("phoneNumber"), QString("13800138000"));

    // 3. 创建新会话实例测试加载
    UserSession& newSession = UserSession::instance();
    QString cacheFile = newSession.getCacheFilePath("patient", "persistence_test");

    QVERIFY(newSession.loadUserInfoFromLocal(cacheFile));
    QCOMPARE(newSession.getValue("username"), QString("persistence_test"));

    qDebug() << "用户会话持久化测试通过";
}

void NetworkIntegrationTest::testStateManagementIntegration()
{
    qDebug() << "测试状态管理集成";

    StateManager& stateManager = StateManager::instance();

    // 1. 测试状态转换
    stateManager.setState(WidgetState::LoggingIn);
    QCOMPARE(stateManager.currentState(), WidgetState::LoggingIn);

    stateManager.setState(WidgetState::LoggedIn);
    QCOMPARE(stateManager.currentState(), WidgetState::LoggedIn);

    stateManager.setState(WidgetState::waitRecivePatientInfo);
    QCOMPARE(stateManager.currentState(), WidgetState::waitRecivePatientInfo);

    // 2. 测试错误状态恢复
    stateManager.setState(WidgetState::Error);
    QCOMPARE(stateManager.currentState(), WidgetState::Error);

    stateManager.setState(WidgetState::Idle);
    QCOMPARE(stateManager.currentState(), WidgetState::Idle);

    qDebug() << "状态管理集成测试通过";
}

void NetworkIntegrationTest::testConcurrentRequests()
{
    qDebug() << "测试并发请求";

    m_testTimer->start();

    // 1. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);

    // 2. 发送多个并发请求
    for (int i = 0; i < 5; ++i) {
        QJsonObject request;
        request["command"] = QString("CONCURRENT_TEST_%1").arg(i);
        request["requestId"] = i;
        m_tcpClient->sendData(request);
    }

    // 3. 模拟并发响应
    for (int i = 0; i < 5; ++i) {
        QTimer::singleShot(100 + i * 50, [this, i]() {
            QJsonObject response;
            response["command"] = QString("CONCURRENT_RESULT_%1").arg(i);
            response["requestId"] = i;
            response["status"] = "success";
            simulateNetworkResponse(response);
        });
    }

    // 4. 等待所有响应
    QTest::qWait(1000);

    // 5. 验证所有响应都被正确处理
    QCOMPARE(dataReceivedSpy.count(), 5);

    // 验证响应的完整性
    for (int i = 0; i < dataReceivedSpy.count(); ++i) {
        QList<QVariant> arguments = dataReceivedSpy.at(i);
        QJsonObject response = arguments.at(0).toJsonObject();
        QVERIFY(response.contains("requestId"));
    }

    m_testTimer->stop();
    qDebug() << "并发请求测试通过";
}

void NetworkIntegrationTest::testRaceConditionHandling()
{
    qDebug() << "测试竞态条件处理";

    m_testTimer->start();

    // 1. 设置信号监听
    QSignalSpy dataReceivedSpy(m_tcpClient, &TcpClient::dataReceivedJson);

    // 2. 快速连续发送相同请求
    for (int i = 0; i < 3; ++i) {
        JsonMessageBuilder builder("RACE_CONDITION_TEST");
        builder.addAdditionalData("sequence", QString::number(i));
        m_tcpClient->sendData(builder.build());
    }

    // 3. 模拟乱序响应
    QTimer::singleShot(100, [this]() {
        QJsonObject response;
        response["command"] = "RACE_CONDITION_RESULT";
        response["sequence"] = "2"; // 最后一个请求的响应先到
        simulateNetworkResponse(response);
    });

    QTimer::singleShot(150, [this]() {
        QJsonObject response;
        response["command"] = "RACE_CONDITION_RESULT";
        response["sequence"] = "0"; // 第一个请求的响应后到
        simulateNetworkResponse(response);
    });

    // 4. 等待响应
    QTest::qWait(500);

    // 5. 验证系统稳定性
    // 系统应该能够处理乱序响应而不崩溃
    QVERIFY(dataReceivedSpy.count() >= 1);

    m_testTimer->stop();
    qDebug() << "竞态条件处理测试通过";
}

// 辅助方法实现
QJsonObject NetworkIntegrationTest::createLoginRequest(const QString& username, const QString& password)
{
    QJsonObject request;
    request["username"] = username;
    request["password"] = password;
    return request;
}

QJsonObject NetworkIntegrationTest::createLoginResponse(bool success, const QString& userType)
{
    QJsonObject response;
    response["command"] = "LOGIN_RESULT";
    response["success"] = success;
    response["type"] = userType;

    if (success) {
        response["message"] = "登录成功";
    } else {
        response["message"] = "登录失败";
    }

    return response;
}

QJsonObject NetworkIntegrationTest::createAppointmentResponse()
{
    QJsonObject response;
    response["command"] = "APPOINTMENT_RESULT";
    response["status"] = "success";
    response["appointmentKey"] = "A001";
    response["message"] = "预约成功";

    QJsonArray appointmentArray;
    QJsonObject appointment;
    appointment["patientUsername"] = "test_patient";
    appointment["doctorUsername"] = "doctor001";
    appointment["date"] = "2024-01-20";
    appointment["time"] = "10:00";
    appointment["cost"] = "50";
    appointment["status"] = "confirmed";
    appointmentArray.append(appointment);

    response["appointments"] = appointmentArray;
    return response;
}

void NetworkIntegrationTest::simulateNetworkResponse(const QJsonObject& response)
{
    // 模拟网络接收到数据
    emit m_tcpClient->dataReceivedJson(response);
}

void NetworkIntegrationTest::waitForResponse(int timeoutMs)
{
    QEventLoop loop;
    QTimer::singleShot(timeoutMs, &loop, &QEventLoop::quit);

    // 等待响应或超时
    loop.exec();
}

QTEST_MAIN(NetworkIntegrationTest)
#include "NetworkIntegration_test.moc"