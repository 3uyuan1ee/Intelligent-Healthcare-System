#include "patientadviceclient.h"

#include "adviceClient/ui_patientadviceclient.h"

patientAdviceClient::patientAdviceClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::patientAdviceClient)
{
    ui->setupUi(this);
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientAdviceClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &patientAdviceClient::onErrorOccurred);

}

patientAdviceClient::~patientAdviceClient()
{
    delete ui;
}

void patientAdviceClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientAdviceList) {
        qDebug() << "patientCaseMenuClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"patientAdviceClient::onDataReceived:状态设置为:Idle";

    ui->patientAdviceListWidget->clear();

    qDebug()<<"patientCaseMenuClient::onDataReceived:正在接受病例列表";

    QList<DataManager::AdviceInfo> advices=DataManager::instance().extractAdvices(data);

    for (const DataManager::AdviceInfo &advice : advices) {
        const QString each_instance=QString("      医生用户名：")+QString(advice.doctorUsername)
                                      +QString("    日期：")+QString(advice.date)+QString("    就诊时间：")
                                      +QString(advice.time);
        const QString each_instance_1=QString("    药品：")+QString(advice.medicine);
        const QString each_instance_2=QString("    检查：")+QString(advice.check);
        const QString each_instance_3=QString("    治疗：")+QString(advice.therapy);

        const QString each_instance_4=QString("    护理：")+QString(advice.care);

        ui->patientAdviceListWidget->addItem(each_instance);
        ui->patientAdviceListWidget->addItem(each_instance_1);
        ui->patientAdviceListWidget->addItem(each_instance_2);
        ui->patientAdviceListWidget->addItem(each_instance_3);
        ui->patientAdviceListWidget->addItem(each_instance_4);
    }

    // // 医嘱信息结构体
    // struct AdviceInfo {
    //     QString patientUsername;
    //     QString doctorUsername;
    //     QString date;
    //     QString time;
    //     QString medicine;
    //     QString check;
    //     QString therapy;
    //     QString care;
    // };

}  // 添加数据接收槽
void patientAdviceClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽
