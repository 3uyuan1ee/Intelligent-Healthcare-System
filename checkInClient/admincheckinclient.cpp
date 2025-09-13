#include "admincheckinclient.h"
#include "ui_admincheckinclient.h"

adminCheckInClient::adminCheckInClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminCheckInClient)
{
    ui->setupUi(this);

 tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &adminCheckInClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &adminCheckInClient::onErrorOccurred);
}

adminCheckInClient::~adminCheckInClient()
{
    delete ui;
}

void adminCheckInClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitAdminReciveDoctorList) {
        qDebug() << "adminCheckInClient::onDataReceived:当前不是医生接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"adminCheckInClient::onDataReceived:状态设置为Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "考勤提交失败", "服务器返回数据格式错误");
        qDebug()<<"adminCheckInClient::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"adminCheckInClient::onDataReceived:正在接收医生列表";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","查询成功");
    }else{
        QMessageBox::warning(this, "失败", "服务器返回数据格式错误");
    }

    QList<DataManager::DoctorInfo> doctors=DataManager::instance().extractDoctors(data);

    for (const DataManager::DoctorInfo &doctor : doctors) {
        const QString each_instance=QString("医生用户名：")+QString(doctor.username)
                                      +QString("    医生名：")+QString(doctor.name);
        ui->listWidget->addItem(each_instance);
        ui->comboBox_2->addItem(QString(doctor.username));
    }

}  // 添加数据接收槽
void adminCheckInClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void adminCheckInClient::on_pushButton_clicked()
{
    StateManager::instance().setState(WidgetState::waitAdminReciveDoctorList);
    qDebug()<<"adminCheckInClient::on_pushButton_clicked:waitAdminReciveDoctorList";


    JsonMessageBuilder *builder=new JsonMessageBuilder("queryDoctorList");
    builder->addAdditionalData("time","12");
    tcpClient->sendData(builder->build());
    delete builder;



}


void adminCheckInClient::on_pushButton_2_clicked()
{

    StateManager::instance().setState(WidgetState::waitAdminReciveCheckin);
    qDebug()<<"adminCheckInClient::on_pushButton_clicked:waitAdminReciveCheckin";
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!admincheckinresultclient) {
        admincheckinresultclient = new adminCheckInResultClient/*(this)*/;
        //设置窗口属性
        admincheckinresultclient->setAttribute(Qt::WA_DeleteOnClose, false);
    }

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAttendance");
    builder->addAdditionalData("doctorUsername",ui->comboBox_2->currentText());
    builder->addAdditionalData("month",ui->spinBox->text());
    tcpClient->sendData(builder->build());
    delete builder;

     admincheckinresultclient->show();
}

