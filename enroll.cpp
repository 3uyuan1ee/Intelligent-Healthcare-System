#include "enroll.h"
#include "ui_enroll.h"

Enroll::Enroll(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Enroll)
{
    ui->setupUi(this);

    ui->EnrollWindow_UserType_comboBox->addItem("patient");
    ui->EnrollWindow_UserType_comboBox->addItem("doctor");
    ui->EnrollWindow_UserType_comboBox->addItem("admin");
    // 初始化 TCP 客户端

    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &Enroll::onRegisterFinished);
    connect(tcpClient, &TcpClient::errorOccurred, this, &Enroll::onErrorOccurred);

}

Enroll::~Enroll()
{
    delete ui;
}

void Enroll::on_EnrollWindow_cancel_pushButton_clicked()
{
    qDebug()<<"关闭注册窗口";
    this->close();


}


// 发送注册请求并处理响应
void Enroll::on_EnrollWindow_enroll_pushButton_clicked()
{

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();
    // 设置为注册状态
    stateManager.setState(WidgetState::Registing);
    qDebug()<<"Enroll::on_EnrollWindow_enroll_pushButton_clicked:状态设置为Registing";

    QString username = ui->EnrollWindow_Username_lineEdit->text();
    QString password = ui->EnrollWindow_Passwords_lineEdit->text();
    QString userType = ui->EnrollWindow_UserType_comboBox->currentText(); // 用ComboBox来选择用户类型

    // 组装发送的JSON数据
    QJsonObject jsonObj;
    jsonObj["username"] = username;
    jsonObj["password"] = password;
    jsonObj["type"] = userType;

    // 发送注册请求
    tcpClient->sendData(command,jsonObj);
    qDebug() << "Sending registration data: " << jsonObj;

}

// 处理注册请求的响应
void Enroll::onRegisterFinished(const QJsonObject &data)
{

    // 获取 StateManager 单例
        StateManager& stateManager = StateManager::instance();

    // 如果当前不是注册状态，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::Registing) {
        qDebug() << "当前不是注册状态，忽略接收到的数据";
        return;
    }

    // 注册完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"Enroll::on_EnrollWindow_enroll_pushButton_clicked:状态设置为Idle";

    qDebug()<<"注册响应判断启动";

    // 获取响应数据并解析
    if (data.isEmpty()) {
        QMessageBox::warning(this, "注册失败", "服务器返回数据格式错误");
        return;
    }

    // 获取服务器返回的回复字段
    QString replyStatus = data.value("reply").toString();

    // 根据返回的状态弹出相应的提示框
    if (replyStatus == "successful") {
        QMessageBox::information(this, "注册成功", "注册成功！");
    } else if (replyStatus == "failed") {
        QMessageBox::warning(this, "注册失败", "用户已存在！");
    } else {
        QMessageBox::critical(this, "注册失败", "未知错误！");
    }




    // 关闭窗口
    this->close();

}

void Enroll::onErrorOccurred(const QString &error)
{
    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
}
