#include "admininfomenuclient.h"
#include "ui_admininfomenuclient.h"

adminInfoMenuClient::adminInfoMenuClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminInfoMenuClient)
{
    ui->setupUi(this);
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &adminInfoMenuClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &adminInfoMenuClient::onErrorOccurred);
    ui->typeComboBox->addItem("Patient");
    ui->typeComboBox->addItem("Doctor");
}

adminInfoMenuClient::~adminInfoMenuClient()
{
    delete ui;

}

void adminInfoMenuClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie) {
        qDebug() << "adminInfoMenuClient::onDataReceived:当前不是个人信息接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"adminInfoMenuClient::onDataReceived:状态设置为Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "个人信息失败", "服务器返回数据格式错误");
        qDebug()<<"adminInfoMenuClient::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"adminInfoMenuClient::onDataReceived:正在接收个人信息列表";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","查询成功");
    }else{
        QMessageBox::warning(this, "失败", "服务器返回数据格式错误");
    }
    if(ui->typeComboBox->currentText()=="Doctor"){
    QList<DataManager::DoctorInfo> doctors=DataManager::instance().extractDoctors(data);

    for (const DataManager::DoctorInfo &doctor : doctors) {
        const QString each_instance=QString("医生用户名：")+QString(doctor.username)
                                      +QString("    医生名：")+QString(doctor.name);
        ui->listWidget->addItem(each_instance);
        ui->comboBox_2->addItem(QString(doctor.username));
    }
    }else{

        QList<DataManager::PatientInfo> patients=DataManager::instance().extractPatients(data);

        for (const DataManager::PatientInfo &patient : patients) {
            const QString each_instance=QString("患者用户名：")+QString(patient.username)
                                          +QString("    患者名：")+QString(patient.name);
            ui->listWidget->addItem(each_instance);
            ui->comboBox_2->addItem(QString(patient.username));
        }
    }

}  // 添加数据接收槽
void adminInfoMenuClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void adminInfoMenuClient::on_pushButton_clicked()
{
    ui->listWidget->clear();
    ui->comboBox_2->clear();

    JsonMessageBuilder *builder =new JsonMessageBuilder("query"+ui->typeComboBox->currentText()+"List");

    if(ui->typeComboBox->currentText()=="Doctor"){
        builder->addAdditionalData("time","12");}

    tcpClient->sendData(builder->build());

    delete builder; // 记得释放内存

    StateManager::instance().setState(WidgetState::waitdie);
    qDebug()<<"adminInfoMenuClient::on_pushButton_clicked:状态设置为:waitdie";


}


void adminInfoMenuClient::on_pushButton_2_clicked()
{

    JsonMessageBuilder *builder =new JsonMessageBuilder("query"+ui->typeComboBox->currentText()+"Info");

    if(ui->typeComboBox->currentText()=="Doctor"){
    builder->addAdditionalData("doctorUsername",ui->comboBox_2->currentText());


        //如果实例不存在，则创建
        if (!admininfodoctorclient) {
            admininfodoctorclient = new adminInfoDoctorClient/*(this)*/;
            //设置窗口属性
            admininfodoctorclient->setAttribute(Qt::WA_DeleteOnClose, false);
        }

        admininfodoctorclient->usr=ui->comboBox_2->currentText();

        StateManager::instance().setState(WidgetState::waitdie_2);
        qDebug()<<"adminInfoMenuClient::on_pushButton_2_clicked:状态设置为:waitdie_2";



        tcpClient->sendData(builder->build());

        delete builder; // 记得释放内存

        admininfodoctorclient->show();

    }else{
        builder->addAdditionalData("patientUsername",ui->comboBox_2->currentText());

        //如果实例不存在，则创建
        if (!admininfoclient) {
            admininfoclient = new adminInfoClient/*(this)*/;
            //设置窗口属性
            admininfoclient->setAttribute(Qt::WA_DeleteOnClose, false);
        }

        admininfoclient->usr=ui->comboBox_2->currentText();

        StateManager::instance().setState(WidgetState::waitdie_3);
        qDebug()<<"adminInfoMenuClient::on_pushButton_2_clicked:状态设置为:waitdie_3";

        tcpClient->sendData(builder->build());

        delete builder; // 记得释放内存

        admininfoclient->show();
    }




}

