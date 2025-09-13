#include "doctorcaseclient.h"
#include "caseClient/ui_doctorcaseclient.h"

doctorCaseClient::doctorCaseClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorCaseClient)

{
    ui->setupUi(this);
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorCaseClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorCaseClient::onErrorOccurred);



}

doctorCaseClient::~doctorCaseClient()
{
    delete ui;
}

void doctorCaseClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorCaseList) {
        qDebug() << "patientCaseMenuClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"patientCaseMenuClient::onDataReceived:状态设置为Idle";

    //这里一定要清空，不然多次查询会导致累计
    ui->listWidget->clear();
    ui->comboBox->clear();


    qDebug()<<"patientCaseMenuClient::onDataReceived:正在接受病例列表";

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
        ui->listWidget->addItem(each_instance);
        ui->listWidget->addItem(each_instance_1);
        ui->listWidget->addItem(each_instance_2);
        ui->listWidget->addItem(each_instance_3);
        ui->listWidget->addItem(each_instance_4);
        ui->listWidget->addItem(each_instance_5);

        QString combo=QString("    患者用户名:")+caseinfo.patientUsername+
                        QString("    日期：")+QString(caseinfo.date)+QString("    就诊时间：")+QString(caseinfo.time);
        ui->comboBox->addItem(combo);
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
void doctorCaseClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void doctorCaseClient::on_pushButton_clicked()
{
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    //doctorCaseClient *doctorcaseclient

    if (!doctorcaseeditclient) { doctorcaseeditclient = new doctorCaseEditClient(nullptr)/*(this)*/;

        //设置窗口属性
        doctorcaseeditclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }
     doctorcaseeditclient->comboText=ui->comboBox->currentText();

    StateManager::instance().setState(WidgetState::waitReciveDoctorCase);
    qDebug()<<"doctorCaseClient::on_pushButton_clicked:状态设置为:waitReciveDoctorCase";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryCaseList");
    tcpClient->sendData(builder->build());
    delete builder;

    doctorcaseeditclient->show();

}

