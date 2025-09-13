#include "doctorcheckinclient.h"
#include "checkInClient/ui_doctorcheckinclient.h"
#include<QDate>

doctorCheckInClient::doctorCheckInClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorCheckInClient)
{
    ui->setupUi(this);

    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorCheckInClient::onJudgeReceivedAttendence);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorCheckInClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorCheckInClient::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorCheckInClient::onJudgeReceived);



    QDate currentDate = QDate::currentDate();
    QString date= currentDate.toString("yyyy年MM月dd日");
    qDebug() << "doctorCheckInClient::doctorCheckInClient::"<<date;
    useDate= currentDate.toString("yyyyMMdd");
     qDebug() << "doctorCheckInClient::doctorCheckInClient::"<<useDate;

    ui->dateLineEdit->setText(date);
    ui->dateLineEdit->setReadOnly(true);
    ui->lineEdit->setReadOnly(true);

}

doctorCheckInClient::~doctorCheckInClient()
{
    delete ui;
}

void doctorCheckInClient::on_pushButton_clicked()
{

    StateManager::instance().setState(WidgetState::waitReciveDoctorCheckin);
    qDebug()<<"doctorCheckInClient::on_pushButton_clicked:状态设置为waitReciveDoctorCheckin";

    JsonMessageBuilder *builder=new JsonMessageBuilder("clock");
    builder->addAdditionalData("date",useDate);
    tcpClient->sendData(builder->build());
    delete builder;

}


void doctorCheckInClient::on_pushButton_2_clicked()
{

    StateManager::instance().setState(WidgetState::waitReciveDoctorAbcenceJudge);
    qDebug()<<"doctorCheckInClient::on_pushButton_2_clicked:状态设置为waitReciveDoctorAbcenceJudge";

    QString absenceDate;
    absenceDate = QString("%1%2%3")
                              .arg(ui->yearSpinBox->value(), 4, 10, QChar('0'))
                              .arg(ui->monthSpinBox->value(), 2, 10, QChar('0'))
                              .arg(ui->daySpinBox->value(), 2, 10, QChar('0'));


    JsonMessageBuilder *builder=new JsonMessageBuilder("leave");
    builder->addAdditionalData("date",absenceDate);

    tcpClient->sendData(builder->build());
    delete builder;


}

void doctorCheckInClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorCheckin) {
        qDebug() << "doctorCheckInClient::onDataReceived:当前不是医生考勤接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"doctorCheckInClient::onDataReceived:状态设置为Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "考勤提交失败", "服务器返回数据格式错误");
        qDebug()<<"doctorCheckInClient::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"doctorCheckInClient::onDataReceived:正在接收患者预约判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","您的打卡已成功提交");
    }else{
        QMessageBox::warning(this, "打卡提交失败", "服务器返回数据格式错误");
    }



    StateManager::instance().setState(WidgetState::waitReciveDoctorAttendance);
    qDebug()<<"doctorCheckInClient::onJudgeReceived状态设置为:waitReciveDoctorAttendance";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAttendance");

    QString month=QString("%1").arg(ui->monthSpinBox->value(), 2, 10, QChar('0'));
    builder->addAdditionalData("month","09");

    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存

}  // 添加数据接收槽
void doctorCheckInClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void doctorCheckInClient::onJudgeReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorAbcenceJudge) {
        qDebug() << "doctorCheckInClient::onJudgeReceived当前不是医生请假判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"doctorCheckInClient::onJudgeReceived状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "请假提交失败", "服务器返回数据格式错误");
        qDebug()<<"doctorCheckInClient::onJudgeReceived服务器返回数据类型错误";
        return;
    }

    qDebug()<<"doctorCheckInClient::onJudgeReceived正在接收医生请假判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","请假成功");
    }else{
        QMessageBox::warning(this, "请假提交失败", "服务器返回数据格式错误");
    }

    StateManager::instance().setState(WidgetState::waitReciveDoctorAttendance);
    qDebug()<<"doctorCheckInClient::onJudgeReceived状态设置为:waitReciveDoctorAttendance";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAttendance");

    QString month=QString("%1").arg(ui->monthSpinBox->value(), 2, 10, QChar('0'));
    builder->addAdditionalData("month","09");

    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存

}

void doctorCheckInClient::onJudgeReceivedAttendence(const QJsonObject &data){


    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorAttendance) {
        qDebug() << "doctorCheckInClient::onJudgeReceived当前不是医生考勤判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"doctorCheckInClient::onJudgeReceived状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "考勤查询失败", "服务器返回数据格式错误");
        qDebug()<<"doctorCheckInClient::onJudgeReceived服务器返回数据类型错误";
        return;
    }

    //这里一定要清空，不然多次查询会导致累计
    ui->lineEdit->clear();


    qDebug()<<"doctorCheckInClient::onJudgeReceived:正在接收医生考勤";

    qDebug()<<"1111111111111111111111111111111111"<<data;
    QString result=data.value("attendance").toString();
    ui->lineEdit->setReadOnly(false);

    qDebug()<<"doctorCheckInClient::onJudgeReceived:接收到信息"<<result;
    ui->lineEdit->setText(result);

    ui->lineEdit->setReadOnly(true);
}
