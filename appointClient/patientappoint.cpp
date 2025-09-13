#include "patientappoint.h"
#include "ui_patientappoint.h"

patientAppoint::patientAppoint(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::patientAppoint)
{
    ui->setupUi(this);

    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientAppoint::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &patientAppoint::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientAppoint::onJudgeReceived);
    ui->comboBox->addItem("呼吸内科");
    ui->comboBox->addItem("消化内科");
    ui->comboBox->addItem("心内科");
    ui->comboBox->addItem("神经内科");
    ui->comboBox->addItem("肾内科");
    ui->comboBox->addItem("普外科");
    ui->comboBox->addItem("骨科");
    ui->comboBox->addItem("神经外科");
    ui->comboBox->addItem("心胸外科");
    ui->comboBox->addItem("泌尿外科");
    ui->comboBox->addItem("妇产科");
    ui->comboBox->addItem("儿科");
}

patientAppoint::~patientAppoint()
{
    delete ui;
}

void patientAppoint::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveAvailableDoctortList) {
        qDebug() << "patientAppoint::onDataReceived:当前不是医生列表接收状态，忽略接收到的数据";
        return;
    }

     // 完成后恢复为 Idle 状态
    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"patientAppoint::onDataReceived:状态设置为:Idle";

    qDebug()<<"patientAppoint::onDataReceived:正在接收医生列表列表";



    QList<DataManager::DoctorInfo> doctors=DataManager::instance().extractDoctors(data);

    for (const DataManager::DoctorInfo &doctor : doctors) {
        const QString each_instance=QString("医生：")+QString(doctor.name)
                                      +QString("    医生用户名：")+QString(doctor.username)+QString("    出诊时间：")
                                      +QString(doctor.begin)+QString("-")+QString(doctor.end)
                                      +QString("  费用：")+QString(doctor.cost)
                                      +QString("    科室：")+QString(doctor.department)
                                      +QString("    最大挂号上限")+QString(doctor.limit);
        ui->availableDoctorListWidget->addItem(each_instance);
        ui->comboBox_2->addItem(QString(doctor.username));
    }


};  // 添加数据接收槽
void patientAppoint::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
};  // 添加错误处理槽

void patientAppoint::onJudgeReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientAppointmentJudge) {
        qDebug() << "patientAppoint::onDataReceived:当前不是患者预约判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"patientAppoint::onDataReceived:状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "预约提交失败", "服务器返回数据格式错误");
        qDebug()<<"patientAppoint::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"patientAppoint::onDataReceived:正在接收患者预约判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","您的预约已成功提交");
    }else{
        QMessageBox::warning(this, "预约提交失败", "服务器返回数据格式错误");
    }

    //更新patient_client的预约列表显示
    StateManager::instance().setState(WidgetState::waitRecivePatientAppointmentList);
    qDebug()<<"patientAppoint::onDataReceived:状态设置为:waitRecivePatientAppointmentList";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAppointmentList");
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存

}

void patientAppoint::on_pushButton_clicked()
{
    StateManager::instance().setState(WidgetState::waitReciveAvailableDoctortList);
    qDebug()<<"patientAppoint::on_pushButton_clicked:状态设置为:waitReciveAvailableDoctortList";

    //这里一定要清空，不然多次查询会导致累计
    ui->availableDoctorListWidget->clear();
    ui->comboBox_2->clear();

    JsonMessageBuilder *builder = new JsonMessageBuilder("queryDoctorList");
    builder->addAdditionalData("time",ui->spinBox->text());
    tcpClient->sendData(builder->build());
    delete builder;

}


void patientAppoint::on_commitDoctorPushButton_clicked()
{

    StateManager::instance().setState(WidgetState::waitRecivePatientAppointmentJudge);
    qDebug()<<"patientAppoint::on_commitDoctorPushButton_clicked:状态设置为:waitRecivePatientAppointmentJudge";
    JsonMessageBuilder *builder = new JsonMessageBuilder("modifyAppointment");
    QString date;
    date = QString("%1%2%3")
                      .arg(ui->yearSpinBox->value(), 4, 10, QChar('0'))
                      .arg(ui->monthSpinBox->value(), 2, 10, QChar('0'))
                      .arg(ui->daySpinBox->value(), 2, 10, QChar('0'));
    builder->addAppointment(UserSession::instance().getValue("username"),
                            ui->comboBox_2->currentText(),date,
                            ui->spinBox->text(),"0",QString("waiting"));
    tcpClient->sendData(builder->build());
    delete builder;
}

