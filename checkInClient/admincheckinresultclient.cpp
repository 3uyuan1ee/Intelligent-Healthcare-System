#include "admincheckinresultclient.h"
#include "ui_admincheckinresultclient.h"

adminCheckInResultClient::adminCheckInResultClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminCheckInResultClient)
{
    ui->setupUi(this);

     tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &adminCheckInResultClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &adminCheckInResultClient::onErrorOccurred);
}

adminCheckInResultClient::~adminCheckInResultClient()
{
    delete ui;
}

void adminCheckInResultClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitAdminReciveCheckin) {
        qDebug() << "adminCheckInResultClient::onDataReceived:当前不是医生考勤接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"adminCheckInResultClient::onDataReceived:状态设置为Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "考勤查询失败", "服务器返回数据格式错误");
        qDebug()<<"adminCheckInResultClient::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"adminCheckInResultClient::onDataReceived:正在接收考勤信息";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","查询成功");
    }else{
        QMessageBox::warning(this, "失败", "服务器返回数据格式错误");
    }

    ui->resultTextEdit->setText(data["attendance"].toString());

}  // 添加数据接收槽
void adminCheckInResultClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽
