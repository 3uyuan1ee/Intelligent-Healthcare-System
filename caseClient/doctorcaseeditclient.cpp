#include "doctorcaseeditclient.h"
#include "caseClient/ui_doctorcaseeditclient.h"


doctorCaseEditClient::doctorCaseEditClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorCaseEditClient)
{
    ui->setupUi(this);
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorCaseEditClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorCaseEditClient::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorCaseEditClient::onDataReceivedJudge);

    ui->mainLineEdit->setReadOnly(true);
    ui->checkLineEdit->setReadOnly(true);
    ui->diagnoseLineEdit->setReadOnly(true);
    ui->nowLineEdit->setReadOnly(true);
    ui->pastLineEdit->setReadOnly(true);
}

doctorCaseEditClient::~doctorCaseEditClient()
{
    delete ui;
}

void doctorCaseEditClient::on_pushButton_clicked()
{
    ui->mainLineEdit->setReadOnly(false);
    ui->checkLineEdit->setReadOnly(false);
    ui->diagnoseLineEdit->setReadOnly(false);
    ui->nowLineEdit->setReadOnly(false);
    ui->pastLineEdit->setReadOnly(false);

}

void doctorCaseEditClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorCase) {
        qDebug() << "doctorCaseEditClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"doctorCaseEditClient::onDataReceived:状态设置为Idle";





    qDebug()<<"doctorCaseEditClient::onDataReceived:正在接受病例列表";

    QList<DataManager::CaseInfo> cases=DataManager::instance().extractCases(data);

    for (const DataManager::CaseInfo &caseinfo : cases) {
        const QString each_instance=QString("患者用户名：")+QString(caseinfo.patientUsername)
                                      +QString("    日期：")+QString(caseinfo.date)+QString("    就诊时间：")
                                      +QString(caseinfo.time);
        const QString each_instance_1=QString("    主诉：")+QString(caseinfo.main);
        const QString each_instance_2=QString("    现病史：")+QString(caseinfo.now);
        const QString each_instance_3=QString("    既往史：")+QString(caseinfo.past);

        const QString each_instance_4=QString("    体格检查：")+QString(caseinfo.check);
        const QString each_instance_5=QString("    诊断：")+QString(caseinfo.diagnose);

        QString combo=QString("    患者用户名:")+caseinfo.patientUsername+
                        QString("    日期：")+QString(caseinfo.date)+QString("    就诊时间：")+QString(caseinfo.time);
        if(comboText==combo){
            patientInfo = {{"患者用户名", combo.section("患者用户名:", 1).section(" ", 0, 0).trimmed()},
                                    {"日期", combo.section("日期：", 1).section(" ", 0, 0).trimmed()},
                                    {"就诊时间", combo.section("就诊时间：", 1).trimmed()}};
            ui->mainLineEdit->setText(QString(caseinfo.main));
            ui->checkLineEdit->setText(QString(caseinfo.check));
            ui->diagnoseLineEdit->setText(QString(caseinfo.diagnose));
            ui->nowLineEdit->setText(QString(caseinfo.now));
            ui->pastLineEdit->setText(QString(caseinfo.past));

            ui->mainLineEdit->setReadOnly(true);
            ui->checkLineEdit->setReadOnly(true);
            ui->diagnoseLineEdit->setReadOnly(true);
            ui->nowLineEdit->setReadOnly(true);
            ui->pastLineEdit->setReadOnly(true);

            return;
        }
    }

    // //QList<DataManager::CaseInfo> DataManager::extractCases(const QJsonObject &jsonData)
    // {
    //     QList<CaseInfo> cases;
    //     for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
    //         if (it.key().startsWith("case_")) {
    //             QJsonObject caseObj = it.value().toObject();
    //             CaseInfo info;
    //             info.patientUsername = caseObj["patientUsername"].toString();
    //             info.doctorUsername = caseObj["doctorUsername"].toString();
    //             info.date = caseObj["date"].toString();
    //             info.time = caseObj["time"].toString();
    //             info.main = caseObj["main"].toString();
    //             info.now = caseObj["now"].toString();
    //             info.past = caseObj["past"].toString();
    //             info.check = caseObj["check"].toString();
    //             info.diagnose = caseObj["diagnose"].toString();
    //             cases.append(info);
    //         }
    //     }
    //     return cases;
    // }

}  // 添加数据接收槽
void doctorCaseEditClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void doctorCaseEditClient::on_pushButton_2_clicked()
{
    StateManager::instance().setState(WidgetState::waitReciveDoctorCaseJudge);
    qDebug()<<"doctorCaseEditClient::on_pushButton_2_clicked:状态设置为:waitReciveDoctorCaseJudge";

    JsonMessageBuilder *builder=new JsonMessageBuilder("modifyCase");
    builder->addCase(patientInfo["患者用户名"].toString(),UserSession::instance().getValue("username"),
                     patientInfo["日期"].toString(),patientInfo["就诊时间"].toString(),ui->mainLineEdit->text(),
                    ui->nowLineEdit->text(),ui->pastLineEdit->text(),
                     ui->checkLineEdit->text(),ui->diagnoseLineEdit->text());

    tcpClient->sendData(builder->build());

    delete builder;

}
void doctorCaseEditClient::onDataReceivedJudge(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorCaseJudge) {
        qDebug() << "doctorCaseEditClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"doctorCaseEditClient::onDataReceived:状态设置为Idle";


    if(data.isEmpty()){
        QMessageBox::warning(this, "病例提交失败", "服务器返回数据格式错误");
        qDebug()<<"doctorCheckInClient::onDataReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"doctorCheckInClient::onDataReceived:正在接收病例编辑判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","病例编辑已成功提交");
    }else{
        QMessageBox::warning(this, "病例编辑提交失败", "服务器返回数据格式错误");
    }
}

