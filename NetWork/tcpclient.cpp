#include "TcpClient.h"
#include <QDebug>
#include <QByteArray>

// 静态成员变量初始化（放在所有函数之外）
TcpClient* TcpClient::m_instance = nullptr;

TcpClient* TcpClient::instance()
{
    if (!m_instance) {
        m_instance = new TcpClient();
    }
    return m_instance;
}

TcpClient::TcpClient(QObject *parent) : QObject(parent),
    timeoutDuration(10000)  // 默认超时为10秒
{

    // 创建 TcpClient 对象时，初始化 socket 对象并记录日志
    qDebug() << "TcpClient: Socket 创建成功。";

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);

    // 初始化定时器
    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);  // 单次触发
    connect(timeoutTimer, &QTimer::timeout, this, &TcpClient::onTimeout);
    timeoutTimerSend = new QTimer(this);
    timeoutTimerSend->setSingleShot(true);
    connect(timeoutTimerSend, &QTimer::timeout, this, &TcpClient::onTimeout);
}



TcpClient::~TcpClient()
{
    // 销毁 socket 对象时记录日志
    // QMutexLocker locker(&m_mutex);

    qDebug() << "TcpClient: 析构函数调用，正在清理资源...";

    // Disconnect all signals first
    disconnect(socket, nullptr, this, nullptr);
    disconnect(timeoutTimer, nullptr, this, nullptr);
    disconnect(timeoutTimerSend, nullptr, this, nullptr);

    // Stop and delete timers safely
    if (timeoutTimer) {
        if (timeoutTimer->isActive()) {
            timeoutTimer->stop();
        }
        delete timeoutTimer;
        timeoutTimer = nullptr;
    }

    if (timeoutTimerSend) {
        if (timeoutTimerSend->isActive()) {
            timeoutTimerSend->stop();
        }
        delete timeoutTimerSend;
        timeoutTimerSend = nullptr;
    }

    // Delete socket safely
    if (socket) {
        if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->disconnectFromHost();
            socket->waitForDisconnected(1000);
        }
        delete socket;
        socket = nullptr;
    }

    qDebug() << "TcpClient: 资源清理完成。";


}



void TcpClient::connectToServer(const QString &host, int port)
{

     // QMutexLocker locker(&m_mutex);

    // 如果已经连接，先断开
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
        socket->waitForDisconnected(1000);
        qDebug()<<"connectToServer:tcp已有连接，正在断开...";
    }

    // 尝试连接到服务器，并记录目标主机和端口
    qDebug() << "connectToServer:尝试连接到服务器：" << host << "端口：" << port;

    socket->connectToHost(host, port);

    // 检查连接状态，记录连接过程中的状态
    if (socket->state() == QTcpSocket::ConnectingState) {
        qDebug() << "connectToServer:连接中...";
        // 启动连接超时定时器
        if (timeoutTimer) {
            timeoutTimer->start(timeoutDuration);
        }
    } else {
        qDebug() << "connectToServer:socket 状态：未处于连接状态。";
    }
}

void TcpClient::sendData(const QByteArray &data)
{

    // QMutexLocker locker(&m_mutex);

    if (socket->state() == QTcpSocket::ConnectedState) {
        // 连接状态下，发送数据并记录发送的内容
        qDebug() << "sendData:发送数据到服务器：" << data;
        socket->write(data);
    } else {
        // 未连接到服务器时，记录错误信息
        qDebug() << "sendData:未连接到服务器，无法发送数据。";
        emit errorOccurred("Not connected to the server.");
    }
}

void TcpClient::sendData(const QJsonObject &data){

    QMutexLocker locker(&m_mutex);


    // 线程检查
    if (!socket || !timeoutTimerSend) {
        qWarning() << "TcpClient is being destroyed, cannot send data";
        return;
    }

    // 启动超时定时器
    // timeoutTimerSend = new QTimer(this);
    if (timeoutTimerSend) {
        timeoutTimerSend->start(timeoutDuration);
    }


    QJsonDocument doc(data);
    QByteArray byteArray =doc.toJson(QJsonDocument::Compact);  // 使用 Compact 来避免格式化时添加不必要的空格

    // 发送数据
    if (socket && socket->isOpen()) {
        qDebug() << "sendData(2):发送数据到服务器：" << QString(byteArray); // 输出时转换为 QString
        socket->write(byteArray);
        socket->flush();
        qDebug() << "sendData(2):发送的数据: " << byteArray;
    } else {
        qWarning() << "sendData(2):TCP连接未打开，无法发送数据";
    }
}

void TcpClient::sendData(const QString &command, const QJsonObject &data)
{

    QMutexLocker locker(&m_mutex);

    // 线程检查
    if (!socket || !timeoutTimerSend) {
        qWarning() << "TcpClient is being destroyed, cannot send data";
        return;
    }

    // 启动超时定时器
    // timeoutTimerSend = new QTimer(this);
    if (timeoutTimerSend) {
        timeoutTimerSend->start(timeoutDuration);
    }

    // 创建根JSON对象
    QJsonObject jsonRequest;

    // 设置 command 和 data
    jsonRequest["command"] = command;
    // jsonRequest["command"] = "echo";
    jsonRequest["data"] = data;

    // 将 JSON 对象转换为 QByteArray
    QJsonDocument doc(jsonRequest);
    QByteArray byteArray =doc.toJson(QJsonDocument::Compact);  // 使用 Compact 来避免格式化时添加不必要的空格

    // 发送数据
    if (socket && socket->isOpen()) {
        qDebug() << "sendData(2):发送数据到服务器：" << QString(byteArray); // 输出时转换为 QString
        socket->write(byteArray);
        socket->flush();
        qDebug() << "sendData(2):发送的数据: " << byteArray;
    } else {
        qWarning() << "sendData(2):TCP连接未打开，无法发送数据";
    }
}

/*QByteArray TcpClient::receiveData()
{

    // 读取接收到的数据并记录日志
    QByteArray data = socket->readAll();
    qDebug() << "接收到的数据：" << data;

    return socket->readAll();
}*/

/*-------------------------------json处理方法，已封装到OnReadyRead中-----------------------------
    QJsonObject TcpClient::receiveJson()
{
    // 读取接收到的数据
    QByteArray receivedData = socket->readAll();
    QJsonObject dataObject;

    // 解析接收到的数据为JSON文档
    QJsonDocument doc = QJsonDocument::fromJson(receivedData);
    if (dataObject.isEmpty()) {
        qWarning() << "接收到的数据不是一个有效的JSON对象";
        return dataObject;  // 返回空数据表示解析失败
    }

    // 获取JSON对象
    QJsonObject jsonResponse = doc.object();

    // 提取 "reply" 字段

    QString replyStatus = jsonResponse["reply"].toString();
    dataObject["reply"]=replyStatus;
    qDebug() << "服务器返回的状态: " << replyStatus;

    // 提取 "data" 字段
    if (jsonResponse.contains("data") && jsonResponse["data"].isObject()) {
        QJsonObject data = jsonResponse["data"].toObject();

        // 遍历 "data" 中的每个 key-value 对，添加到 dataObject
        QJsonObject::const_iterator it = data.constBegin();
        while (it != data.constEnd()) {
            dataObject[it.key()] = it.value();
            qDebug() << "添加到 dataObject: " << it.key() << " : " << it.value().toString();
            ++it;
        }
    } else {
        qWarning() << "没有找到 'data' 字段，或者 'data' 字段不是一个 JSON 对象";
    }

    // 打印data字段的内容
    qDebug() << "服务器返回的数据: " << dataObject;

    return dataObject; // 返回解析后的数据
}
-------------------------------json处理方法，已封装到OnReadyRead中-----------------------------*/

void TcpClient::disconnectFromServer()
{

     // QMutexLocker locker(&m_mutex);

    // 断开与服务器的连接时记录日志
    qDebug() << "正在断开与服务器的连接...";

    socket->disconnectFromHost();
}

void TcpClient::onReadyRead()
{

    // QMutexLocker locker(&m_mutex);

    QByteArray data = socket->readAll();
    qDebug() << "onReadyRead: 数据可以读取。";

    // // 停止超时定时器
    stopTimeout();
    qDebug()<<"onDataReceived:超时计数器已停止";


    // 将 QByteArray 转换为 QJsonObject
    QJsonDocument doc = QJsonDocument::fromJson(data);

    // 判断是否转换成功
    if (doc.isObject()) {
        QJsonObject jsonObject = doc.object();

        QJsonObject dataObject;

        QJsonObject jsonResponse = doc.object();

        // 提取 "reply" 字段

        QString replyStatus = jsonResponse["reply"].toString();
        dataObject["reply"]=replyStatus;
        qDebug() << "onReadyRead:服务器返回的状态: " << replyStatus;

        // 提取 "data" 字段
        if (jsonResponse.contains("data") && jsonResponse["data"].isObject()) {
            QJsonObject data = jsonResponse["data"].toObject();

            // 遍历 "data" 中的每个 key-value 对，添加到 dataObject
            QJsonObject::const_iterator it = data.constBegin();
            while (it != data.constEnd()) {
                dataObject[it.key()] = it.value();
                qDebug() << "onReadyRead:添加到 dataObject: " << it.key() << " : " << it.value().toString();
                qDebug()<<"2222222222222222222222222222222222"<<dataObject;
                ++it;
            }
        } else {
            qWarning() << "onReadyRead:没有找到 'data' 字段，或者 'data' 字段不是一个 JSON 对象";
        }

        // 打印data字段的内容
        qDebug() << "onReadyRead:服务器返回的数据: " << dataObject<<"(判断传回的字节流是否成功解析为json)";

        // 发射处理后的 QJsonObject
        qDebug() << "onReadyRead:发射 dataReceivedJson 信号：" << dataObject;
        emit dataReceivedJson(dataObject);
        qDebug() << "onReadyRead:jsonObject:" << jsonObject;

    } else {
        // 如果转换失败，打印警告
        qWarning() << "onReadyRead:接收到的数据无法转换为 QJsonObject：" << data;
    }
}

void TcpClient::onConnected()
{
    // 当与服务器连接成功时，记录日志
    qDebug() << "onConnected:成功连接到服务器！";



    if (timeoutTimer && timeoutTimer->isActive()) {
        timeoutTimer->stop();
    }

}

void TcpClient::onDisconnected()
{
    // 当与服务器断开连接时，记录日志
    qDebug() << "onDisconnected:已与服务器断开连接。";

    // 停止所有定时器
    stopTimeout();

}

void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    //详细的错误信息
    qDebug() << "发生 socket 错误，错误字符串：" << socket->errorString();

    // 具体的错误代码
    qDebug() << "错误代码：" << socketError;

    // 停止所有定时器
    stopTimeout();

    // 发出错误信号，通知外部
    emit errorOccurred(socket->errorString());
}

void TcpClient::onTimeout()
{
    // QMutexLocker locker(&m_mutex);
    // 超时处理
    QMessageBox::information(nullptr, "信息", "请求超时！");

    StateManager::instance().setState(WidgetState::Idle);

    // 确定是哪个定时器触发的超时
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if (timer) {
        timer->stop();
    }

    // 断开连接
    if (socket && socket->state() != QAbstractSocket::UnconnectedState) {
        socket->disconnectFromHost();
    }

    emit loginTimeout();  // 触发超时信号
    qDebug() << "请求超时！";
}


void TcpClient::stopTimeout()
{
    timeoutTimer->stop();
    timeoutTimerSend->stop();    // 停止定时器
}

void TcpClient::setTimeout(int timeout)
{
    // QMutexLocker locker(&m_mutex);
    timeoutDuration = timeout;
}
