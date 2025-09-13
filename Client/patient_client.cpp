#include "patient_client.h"
#include "ui_patient_client.h"


Patient_Client::Patient_Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Patient_Client)
{
    ui->setupUi(this);


    StateManager::instance().setState(WidgetState::waitRecivePatientAppointmentList);
    qDebug()<<"Patient_Client::构造函数:状态设置为waitRecivePatientAppointmentList";

    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &Patient_Client::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &Patient_Client::onErrorOccurred);

    // 延迟100毫秒后执行
    // QTimer::singleShot(100, this, [this]() {
    //     // 这里写延迟后要执行的代码
    //     qDebug() << "延迟100ms后执行";
    // });

        JsonMessageBuilder *builder=new JsonMessageBuilder("queryAppointmentList");
        tcpClient->sendData(builder->build());
        delete builder; // 记得释放内存

}

Patient_Client::~Patient_Client()
{
    delete ui;
}

void Patient_Client::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientAppointmentList) {
        qDebug() << "Patient_Client::onDataReceived:当前不是病人预约列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"Patient_Client::onDataReceived:状态设置为Idle";


    qDebug()<<"Patient_Client::onDataReceived:正在接受预约列表";

    QList<DataManager::AppointmentInfo> appointments=DataManager::instance().extractAppointments(data);

    for (const DataManager::AppointmentInfo &appointment : appointments) {
        const QString each_instance=QString("医生用户名：")+QString(appointment.doctorUsername)
                                      +QString("    日期：")+QString(appointment.date)+QString("    就诊时间：")
                                      +QString(appointment.time)+QString("  费用：")+QString(appointment.cost)
                                      +QString("    状态：")+QString(appointment.status);
        ui->appointmentListWidget->addItem(each_instance);
    }


}  // 添加数据接收槽
void Patient_Client::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

//---------------------------------------个人信息-------------------------------------------------------

void Patient_Client::on_infoPushButton_clicked()
{

    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!patientInfo_Client) {
        patientInfo_Client = new patientInfo/*(this)*/;
        //设置窗口属性
        patientInfo_Client->setAttribute(Qt::WA_DeleteOnClose, false);
    }

    // patientInfo_Client = new patientInfo;

    StateManager::instance().setState(WidgetState::waitRecivePatientInfo);
    qDebug()<<"patientInfo::patientInfo:状态设置为:waitRecivePatientInfo";

    JsonMessageBuilder *builder= new JsonMessageBuilder("queryPatientInfo");
    builder->addAdditionalData("patientUsername",UserSession::instance().getValue("username"));
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存

    patientInfo_Client->show();
}

//---------------------------------------个人信息-------------------------------------------------------


//---------------------------------------预约挂号-----------------------------------------------------

void Patient_Client::on_appointPushButton_clicked()
{

    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!patientappoint) {
        patientappoint = new patientAppoint/*(this)*/;
        //设置窗口属性
        patientappoint->setAttribute(Qt::WA_DeleteOnClose, false);

    }
    patientappoint->show();
}

//---------------------------------------预约挂号-----------------------------------------------------


//---------------------------------------查看病例-----------------------------------------------------
void Patient_Client::on_casePushButton_clicked()
{
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!patientcasemenuclient) {
        patientcasemenuclient = new patientCaseMenuClient/*(this)*/;
        //设置窗口属性
        patientcasemenuclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    StateManager::instance().setState(WidgetState::waitRecivePatientCaseList);
    qDebug()<<"Patient_Client::on_casePushButton_clicked:状态设置为waitRecivePatientAppointmentList";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryCaseList");
    tcpClient->sendData(builder->build());
    delete builder;

    patientcasemenuclient->show();

}
//---------------------------------------查看病例-----------------------------------------------------

//---------------------------------------查看医嘱-----------------------------------------------------

void Patient_Client::on_advicePushButton_clicked()
{
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!patientadviceclient) {
        patientadviceclient = new patientAdviceClient/*(this)*/;
        //设置窗口属性
        patientadviceclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    StateManager::instance().setState(WidgetState::waitRecivePatientAdviceList);
    qDebug()<<"Patient_Client::on_advicePushButton_clicked:状态设置为waitRecivePatientAdviceList";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAdviceList");
    tcpClient->sendData(builder->build());
    delete builder;

    patientadviceclient->show();
}

//---------------------------------------查看医嘱-----------------------------------------------------

//---------------------------------------健康评估-----------------------------------------------------

void Patient_Client::on_pushButton_clicked()
{

    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
        //如果实例不存在，则创建
        if (!patienthealthquestionclient) {
        patienthealthquestionclient = new patientHealthQuestionClient/*(this)*/;
        //设置窗口属性
        patienthealthquestionclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    patienthealthquestionclient->show();
}

//---------------------------------------健康评估-----------------------------------------------------

void Patient_Client::on_pushButton_2_clicked()
{
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    patientnoticeclient = new patientNoticeClient/*(this)*/;

    patientnoticeclient->show();
}

