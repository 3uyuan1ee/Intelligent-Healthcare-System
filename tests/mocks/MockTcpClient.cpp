#include "MockTcpClient.h"
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QDebug>

MockTcpClient::MockTcpClient(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
    , m_port(0)
    , m_simulatedLatency(0)
    , m_simulatedErrorRate(0.0)
    , m_simulatedDisconnection(false)
    , m_latencyTimer(nullptr)
    , m_responseTimer(nullptr)
    , m_errorTimer(nullptr)
    , m_hasPendingError(false)
    , m_connectCallCount(0)
    , m_disconnectCallCount(0)
    , m_sendDataCallCount(0)
{
    m_config = TestConfig::getServerConfig();

    // 初始化定时器
    m_latencyTimer = new QTimer(this);
    m_latencyTimer->setSingleShot(true);
    connect(m_latencyTimer, &QTimer::timeout, this, &MockTcpClient::onSimulatedLatency);

    m_responseTimer = new QTimer(this);
    m_responseTimer->setSingleShot(true);
    connect(m_responseTimer, &QTimer::timeout, this, &MockTcpClient::onSimulatedResponse);

    m_errorTimer = new QTimer(this);
    connect(m_errorTimer, &QTimer::timeout, this, &MockTcpClient::onSimulatedError);
}

MockTcpClient::~MockTcpClient()
{
    // Qt会自动清理子对象
}

void MockTcpClient::connectToServer(const QString &host, int port)
{
    QMutexLocker locker(&m_historyMutex);
    m_connectCallCount++;

    m_host = host;
    m_port = port;

    qDebug() << "MockTcpClient: Attempting to connect to" << host << ":" << port;

    // 模拟连接延迟
    int delay = qMax(0, m_simulatedLatency - 100);
    if (delay > 0) {
        QTimer::singleShot(delay, this, [this]() {
            if (!m_simulatedDisconnection) {
                m_isConnected = true;
                emit connected();
                qDebug() << "MockTcpClient: Connected to server";

                // 启动错误模拟定时器
                if (m_simulatedErrorRate > 0.0) {
                    int errorInterval = 5000 + QRandomGenerator::global()->bounded(10000);
                    m_errorTimer->start(errorInterval);
                }
            } else {
                emit errorOccurred("Simulated connection failure");
            }
        });
    } else {
        if (!m_simulatedDisconnection) {
            m_isConnected = true;
            emit connected();
            qDebug() << "MockTcpClient: Connected to server immediately";
        } else {
            emit errorOccurred("Simulated connection failure");
        }
    }
}

void MockTcpClient::disconnectFromServer()
{
    QMutexLocker locker(&m_historyMutex);
    m_disconnectCallCount++;

    m_isConnected = false;
    m_errorTimer->stop();
    m_responseTimer->stop();
    m_latencyTimer->stop();

    emit disconnected();
    qDebug() << "MockTcpClient: Disconnected from server";
}

bool MockTcpClient::isConnected() const
{
    return m_isConnected;
}

void MockTcpClient::sendData(const QByteArray &data)
{
    if (!m_isConnected) {
        emit errorOccurred("Not connected to server");
        return;
    }

    {
        QMutexLocker locker(&m_historyMutex);
        m_sendDataCallCount++;
        m_sentDataHistory.append(QString(data));
    }

    simulateNetworkBehavior();

    // 模拟发送延迟
    if (m_simulatedLatency > 0) {
        QTimer::singleShot(m_simulatedLatency, this, [this, data]() {
            if (m_isConnected) {
                processReceivedData(data);
            }
        });
    } else {
        processReceivedData(data);
    }
}

void MockTcpClient::sendData(const QJsonObject &data)
{
    QJsonDocument doc(data);
    sendData(doc.toJson(QJsonDocument::Compact));
}

void MockTcpClient::sendData(const QString &command, const QJsonObject &data)
{
    QJsonObject request;
    request["command"] = command;
    request["data"] = data;

    sendData(request);
}

void MockTcpClient::setSimulatedLatency(int milliseconds)
{
    m_simulatedLatency = qMax(0, milliseconds);
}

void MockTcpClient::setSimulatedErrorRate(double rate)
{
    m_simulatedErrorRate = qBound(0.0, rate, 1.0);
}

void MockTcpClient::setSimulatedDisconnection(bool enable)
{
    m_simulatedDisconnection = enable;
}

void MockTcpClient::setResponseQueue(const QQueue<QByteArray> &responses)
{
    m_responseQueue = responses;
}

void MockTcpClient::simulateServerResponse(const QJsonObject &response)
{
    QJsonDocument doc(response);
    m_responseQueue.enqueue(doc.toJson(QJsonDocument::Compact));

    if (!m_responseTimer->isActive() && m_isConnected) {
        m_responseTimer->start(m_simulatedLatency);
    }
}

void MockTcpClient::simulateError(const QString &error)
{
    emit errorOccurred(error);
}

void MockTcpClient::simulateConnectionLoss()
{
    if (m_isConnected) {
        m_isConnected = false;
        emit errorOccurred("Connection lost");
        emit disconnected();
    }
}

void MockTcpClient::clearHistory()
{
    QMutexLocker locker(&m_historyMutex);
    m_sentDataHistory.clear();
    m_connectCallCount = 0;
    m_disconnectCallCount = 0;
    m_sendDataCallCount = 0;
}

QStringList MockTcpClient::getSentDataHistory() const
{
    QMutexLocker locker(&m_historyMutex);
    return m_sentDataHistory;
}

int MockTcpClient::getConnectCallCount() const
{
    QMutexLocker locker(&m_historyMutex);
    return m_connectCallCount;
}

int MockTcpClient::getDisconnectCallCount() const
{
    QMutexLocker locker(&m_historyMutex);
    return m_disconnectCallCount;
}

int MockTcpClient::getSendDataCallCount() const
{
    QMutexLocker locker(&m_historyMutex);
    return m_sendDataCallCount;
}

bool MockTcpClient::isConnectionSimulated() const
{
    return m_simulatedDisconnection || m_simulatedLatency > 0 || m_simulatedErrorRate > 0.0;
}

void MockTcpClient::onSimulatedResponse()
{
    if (!m_responseQueue.isEmpty() && m_isConnected) {
        QByteArray response = m_responseQueue.dequeue();
        emit dataReceivedJson(parseJsonData(response));

        // 如果还有响应，继续处理
        if (!m_responseQueue.isEmpty()) {
            m_responseTimer->start(m_simulatedLatency);
        }
    }
}

void MockTcpClient::onSimulatedLatency()
{
    // 延迟处理完成
}

void MockTcpClient::onSimulatedError()
{
    if (m_isConnected && m_simulatedErrorRate > 0.0) {
        double random = QRandomGenerator::global()->bounded(1.0);
        if (random < m_simulatedErrorRate) {
            simulateError("Simulated network error");
        }

        // 重新安排下一个错误检查
        int nextErrorInterval = 5000 + QRandomGenerator::global()->bounded(10000);
        m_errorTimer->start(nextErrorInterval);
    }
}

void MockTcpClient::processReceivedData(const QByteArray &data)
{
    // 在实际应用中，这里可能会触发服务器响应
    // 对于Mock，我们可以根据数据内容生成适当的响应
    QJsonObject jsonData = parseJsonData(data);
    if (!jsonData.isEmpty()) {
        QString command = jsonData["command"].toString();

        // 根据命令生成模拟响应
        if (command == "login") {
            simulateServerResponse(TestConfig::createMockLoginResponse(true));
        } else if (command == "getUserData") {
            simulateServerResponse(TestConfig::createMockUserData());
        } else if (command == "getAppointments") {
            QJsonObject response;
            response["reply"] = "success";
            QJsonArray appointments;
            appointments.append(TestConfig::createMockAppointmentData());
            response["data"] = appointments;
            simulateServerResponse(response);
        }
    }
}

QJsonObject MockTcpClient::parseJsonData(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError && doc.isObject()) {
        return doc.object();
    }

    qWarning() << "MockTcpClient: Failed to parse JSON data:" << error.errorString();
    return QJsonObject();
}

void MockTcpClient::simulateNetworkBehavior()
{
    // 模拟网络丢包、延迟等行为
    if (m_simulatedErrorRate > 0.0) {
        double random = QRandomGenerator::global()->bounded(1.0);
        if (random < m_simulatedErrorRate) {
            simulateError("Simulated network transmission error");
        }
    }
}