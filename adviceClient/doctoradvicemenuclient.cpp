#include "doctoradvicemenuclient.h"
#include "adviceClient/ui_doctoradvicemenuclient.h"

doctorAdviceMenuClient::doctorAdviceMenuClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorAdviceMenuClient)
{
    ui->setupUi(this);

    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorAdviceMenuClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorAdviceMenuClient::onErrorOccurred);

}

doctorAdviceMenuClient::~doctorAdviceMenuClient()
{
    delete ui;
}

void doctorAdviceMenuClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorAdviceList) {
        qDebug() << "doctorAdviceMenuClient::onDataReceived:当前不是患者医嘱列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"doctorAdviceMenuClient::onDataReceived:状态设置为Idle";

    //这里一定要清空，不然多次查询会导致累计
    ui->listWidget->clear();
    ui->comboBox->clear();


    qDebug()<<"doctorAdviceMenuClient::onDataReceived:正在接受医嘱列表";

    QList<DataManager::AdviceInfo> advices=DataManager::instance().extractAdvices(data);

    for (const DataManager::AdviceInfo &advice : advices) {
        const QString each_instance=QString("患者用户名：")+QString(advice.patientUsername)
                                      +QString("    日期：")+QString(advice.date)+QString("    就诊时间：")
                                      +QString(advice.time);
        const QString each_instance_1=QString("    药品：")+QString(advice.medicine);
        const QString each_instance_2=QString("    检查：")+QString(advice.check);
        const QString each_instance_3=QString("    治疗：")+QString(advice.therapy);

        const QString each_instance_4=QString("    护理：")+QString(advice.care);
        ui->listWidget->addItem(each_instance);
        ui->listWidget->addItem(each_instance_1);
        ui->listWidget->addItem(each_instance_2);
        ui->listWidget->addItem(each_instance_3);
        ui->listWidget->addItem(each_instance_4);

        QString combo=QString("    患者用户名:")+advice.patientUsername+
                        QString("    日期：")+QString(advice.date)+QString("    就诊时间：")+QString(advice.time);
        ui->comboBox->addItem(combo);
    }

    // //QList<DataManager::advice> DataManager::extractCases(const QJsonObject &jsonData)
    // {
    //     QList<advice> cases;
    //     for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
    //         if (it.key().startsWith("case_")) {
    //             QJsonObject caseObj = it.value().toObject();
    //             advice info;
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
void doctorAdviceMenuClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

void doctorAdviceMenuClient::on_pushButton_clicked()
{
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    //doctorCaseClient *doctorcaseclient

    if (!doctoradviceclient) { doctoradviceclient = new doctorAdviceClient(nullptr)/*(this)*/;

        //设置窗口属性
        doctoradviceclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }
    doctoradviceclient->comboText=ui->comboBox->currentText();

    StateManager::instance().setState(WidgetState::waitReciveDoctorAdvice);
    qDebug()<<"doctorCaseClient::on_pushButton_clicked:状态设置为:waitReciveDoctorAdvice";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAdviceList");
    tcpClient->sendData(builder->build());
    delete builder;

    doctoradviceclient->show();
}

