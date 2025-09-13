#include "patientcasemenuclient.h"
#include "ui_patientcasemenuclient.h"

patientCaseMenuClient::patientCaseMenuClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::patientCaseMenuClient)
{
    ui->setupUi(this);

    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientCaseMenuClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &patientCaseMenuClient::onErrorOccurred);



}



patientCaseMenuClient::~patientCaseMenuClient()
{
    delete ui;
}


void patientCaseMenuClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientCaseList) {
        qDebug() << "patientCaseMenuClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"patientCaseMenuClient::onDataReceived:状态设置为Idle";

    //这里一定要清空，不然多次查询会导致累计
    ui->patientCaseListWidget->clear();


    qDebug()<<"patientCaseMenuClient::onDataReceived:正在接受病例列表";

    QList<DataManager::CaseInfo> cases=DataManager::instance().extractCases(data);

    for (const DataManager::CaseInfo &caseinfo : cases) {
        const QString each_instance=QString("医生用户名：")+QString(caseinfo.doctorUsername)
                                      +QString("    日期：")+QString(caseinfo.date)+QString("    就诊时间：")
                                      +QString(caseinfo.time);
        const QString each_instance_1=QString("    主诉：")+QString(caseinfo.main);
        const QString each_instance_2=QString("    现病史：")+QString(caseinfo.now);
        const QString each_instance_3=QString("    既往史：")+QString(caseinfo.past);

        const QString each_instance_4=QString("    体格检查：")+QString(caseinfo.check);
        const QString each_instance_5=QString("    诊断：")+QString(caseinfo.diagnose);
        ui->patientCaseListWidget->addItem(each_instance);
        ui->patientCaseListWidget->addItem(each_instance_1);
        ui->patientCaseListWidget->addItem(each_instance_2);
        ui->patientCaseListWidget->addItem(each_instance_3);
        ui->patientCaseListWidget->addItem(each_instance_4);
        ui->patientCaseListWidget->addItem(each_instance_5);
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
void patientCaseMenuClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽


