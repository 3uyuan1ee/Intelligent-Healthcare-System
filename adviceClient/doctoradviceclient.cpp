#include "doctoradviceclient.h"
#include "adviceClient/ui_doctoradviceclient.h"

doctorAdviceClient::doctorAdviceClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorAdviceClient)
{
    ui->setupUi(this);
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorAdviceClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorAdviceClient::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorAdviceClient::onDataReceivedJudge);

    ui->medicineLineEdit->setReadOnly(true);
    ui->checkLineEdit->setReadOnly(true);
    ui->careLineEdit->setReadOnly(true);
    ui->therapyLineEdit->setReadOnly(true);
}

doctorAdviceClient::~doctorAdviceClient()
{
    delete ui;
}

void doctorAdviceClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorAdvice) {
        qDebug() << "doctorAdviceClient::onDataReceived:当前不是患者医嘱列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"doctorAdviceClient::onDataReceived:状态设置为Idle";


    ui->careLineEdit->setReadOnly(false);
    ui->checkLineEdit->setReadOnly(false);
    ui->medicineLineEdit->setReadOnly(false);
    ui->therapyLineEdit->setReadOnly(false);



    qDebug()<<"doctorAdviceClient::onDataReceived:正在接受医嘱列表";

    QList<DataManager::AdviceInfo> advices=DataManager::instance().extractAdvices(data);

    for (const DataManager::AdviceInfo &advice : advices) {
        const QString each_instance=QString("患者用户名：")+QString(advice.patientUsername)
                                      +QString("    日期：")+QString(advice.date)+QString("    就诊时间：")
                                      +QString(advice.time);
        const QString each_instance_1=QString("    药品：")+QString(advice.medicine);
        const QString each_instance_2=QString("    检查：")+QString(advice.check);
        const QString each_instance_3=QString("    治疗：")+QString(advice.therapy);

        const QString each_instance_4=QString("    护理：")+QString(advice.care);

        QString combo=QString("    患者用户名:")+advice.patientUsername+
                        QString("    日期：")+QString(advice.date)+QString("    就诊时间：")+QString(advice.time);
        if(comboText==combo){
            patientInfo = {{"患者用户名", combo.section("患者用户名:", 1).section(" ", 0, 0).trimmed()},
                           {"日期", combo.section("日期：", 1).section(" ", 0, 0).trimmed()},
                           {"就诊时间", combo.section("就诊时间：", 1).trimmed()}};
            ui->checkLineEdit->setText(QString(advice.check));
            ui->careLineEdit->setText(QString(advice.care));
            ui->therapyLineEdit->setText(QString(advice.therapy));
            ui->medicineLineEdit->setText(QString(advice.medicine));

            ui->medicineLineEdit->setReadOnly(true);
            ui->checkLineEdit->setReadOnly(true);
            ui->careLineEdit->setReadOnly(true);
            ui->therapyLineEdit->setReadOnly(true);

            return;
        }
    }
}
void doctorAdviceClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void doctorAdviceClient::onDataReceivedJudge(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorAdviceJudge) {
        qDebug() << "doctorAdviceClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"doctorAdviceClient::onDataReceived:状态设置为Idle";


    if(data.isEmpty()){
        QMessageBox::warning(this, "医嘱提交失败", "服务器返回数据格式错误");
        qDebug()<<"doctorCheckInClient::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"doctorCheckInClient::onDataReceived:正在接收医嘱修改判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","医嘱编辑已成功提交");
    }else{
        QMessageBox::warning(this, "医嘱编辑提交失败", "服务器返回数据格式错误");
    }
}

void doctorAdviceClient::on_pushButton_clicked()
{
    ui->careLineEdit->setReadOnly(false);
    ui->checkLineEdit->setReadOnly(false);
    ui->medicineLineEdit->setReadOnly(false);
    ui->therapyLineEdit->setReadOnly(false);
}


void doctorAdviceClient::on_pushButton_2_clicked()
{
    StateManager::instance().setState(WidgetState::waitReciveDoctorAdviceJudge);
    qDebug()<<"doctorAdviceClient::on_pushButton_2_clicked:状态设置为:waitReciveDoctorAdviceJudge";

    JsonMessageBuilder *builder=new JsonMessageBuilder("modifyAdvice");
    builder->addAdvice(patientInfo["患者用户名"].toString(),UserSession::instance().getValue("username"),
                     patientInfo["日期"].toString(),patientInfo["就诊时间"].toString()
                       ,ui->medicineLineEdit->text(),ui->checkLineEdit->text()
                       ,ui->therapyLineEdit->text(),ui->careLineEdit->text()
                     );

    tcpClient->sendData(builder->build());

    delete builder;
}

