#include "doctor_client.h"
#include "ui_doctor_client.h"

Doctor_Client::Doctor_Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Doctor_Client)
{
    ui->setupUi(this);

    StateManager::instance().setState(WidgetState::waitReciveDoctorAppointmentList);
    qDebug()<<"Doctor_Client:构造函数:状态设置为waitReciveDoctorAppointmentList";

    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &Doctor_Client::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &Doctor_Client::onErrorOccurred);

    // 延迟100毫秒后执行
    // QTimer::singleShot(100, this, [this]() {
    //     // 这里写延迟后要执行的代码
    //     qDebug() << "延迟100ms后执行";
    // });

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAppointmentList");
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存
}

Doctor_Client::~Doctor_Client()
{
    delete ui;
}

void Doctor_Client::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前不是医生预约列表状态，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorAppointmentList) {
        qDebug() << "Doctor_Client::onDataReceived:当前不是医生预约列表接收状态，忽略接收到的数据";
        return;
    }

    qDebug()<<"Doctor_Client::onDataReceived:正在接受预约列表";

    appointments=DataManager::instance().extractAppointments(data);

    for (const DataManager::AppointmentInfo &appointment : appointments) {
        const QString each_instance=QString("患者用户名：")+QString(appointment.patientUsername)
                                      +QString("    日期：")+QString(appointment.date)+QString("    就诊时间：")
                                      +QString(appointment.time)+QString("  费用：")+QString(appointment.cost)
                                      +QString("    状态：")+QString(appointment.status);
        ui->docAppointmentListWidget->addItem(each_instance);
        if(appointment.status=="waiting"){
            ui->comboBox->addItem(QString("患者用户名：")+QString(appointment.patientUsername)
                                  +QString("    日期：")+QString(appointment.date)+QString("    就诊时间："));
        }
    }


}  // 添加数据接收槽
void Doctor_Client::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

//---------------------------------------个人信息-------------------------------------------------------

void Doctor_Client::on_docInfoPushButton_clicked()
{
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!doctorInfo_Client) {
        doctorInfo_Client = new doctorInfo/*(this)*/;
        //设置窗口属性
        doctorInfo_Client->setAttribute(Qt::WA_DeleteOnClose, false);
    }


    StateManager::instance().setState(WidgetState::waitReciveDoctorInfo);
    qDebug()<<"Doctor_Client::on_docInfoPushButton_clicked:状态设置为:waitReciveDoctorInfo";

    JsonMessageBuilder *builder= new JsonMessageBuilder("queryDoctorInfo");
    builder->addAdditionalData("doctorUsername",UserSession::instance().getValue("username"));
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存

    doctorInfo_Client->show();

}

//---------------------------------------个人信息-------------------------------------------------------

//---------------------------------------编辑病例-------------------------------------------------------

void Doctor_Client::on_docCasePushButton_clicked()
{
    //waitReciveDoctorCaseList
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    //doctorCaseClient *doctorcaseclient
    if (!doctorcaseclient) {
        doctorcaseclient = new doctorCaseClient/*(this)*/;
        //设置窗口属性
        doctorcaseclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    StateManager::instance().setState(WidgetState::waitReciveDoctorCaseList);
    qDebug()<<"Patient_Client::on_casePushButton_clicked:状态设置为waitReciveDoctorCaseList";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryCaseList");
    tcpClient->sendData(builder->build());
    delete builder;

    doctorcaseclient->show();
}

//---------------------------------------编辑病例-------------------------------------------------------

//---------------------------------------编辑医嘱-------------------------------------------------------

void Doctor_Client::on_docAdvicePushButton_clicked()
{

    //waitReciveDoctorCaseList
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    //doctorCaseClient *doctorcaseclient
    if (!doctoradvicemenuClient) {
        doctoradvicemenuClient = new doctorAdviceMenuClient/*(this)*/;
        //设置窗口属性
        doctoradvicemenuClient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    StateManager::instance().setState(WidgetState::waitReciveDoctorAdviceList);
    qDebug()<<"Doctor_Client::on_docAdvicePushButton_clicked:waitReciveDoctorAdviceList";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAdviceList");
    tcpClient->sendData(builder->build());
    delete builder;

    doctoradvicemenuClient->show();
}

//---------------------------------------编辑医嘱-------------------------------------------------------

//---------------------------------------考勤-------------------------------------------------------

void Doctor_Client::on_checkInPushButton_clicked()
{

    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    if (!doctorcheckinclient) {
        doctorcheckinclient = new doctorCheckInClient/*(this)*/;
        //设置窗口属性
        doctorcheckinclient->setAttribute(Qt::WA_DeleteOnClose, false);
    }

    StateManager::instance().setState(WidgetState::waitReciveDoctorAttendance);
    qDebug()<<"doctorInfo_Client::on_checkInPushButton_clicked:状态设置为waitReciveDoctorAttendance";

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryAttendance");

    builder->addAdditionalData("month","09");

    tcpClient->sendData(builder->build());

    delete builder; // 记得释放内存

    doctorcheckinclient->show();
}




//---------------------------------------考勤-------------------------------------------------------

void Doctor_Client::on_pushButton_2_clicked()
{


    doctornoticeclient = new doctorNoticeClient/*(this)*/;
        //设置窗口属性

    doctornoticeclient->show();
}


void Doctor_Client::on_pushButton_clicked()
{
    for (const DataManager::AppointmentInfo &appointment : appointments) {
        const QString each_instance=QString("患者用户名：")+QString(appointment.patientUsername)
                                      +QString("    日期：")+QString(appointment.date)+QString("    就诊时间：")
                                      +QString(appointment.time)+QString("  费用：")+QString(appointment.cost)
                                      +QString("    状态：")+QString(appointment.status);
        ui->docAppointmentListWidget->addItem(each_instance);
        if(ui->comboBox->currentText()==QString("患者用户名：")+QString(appointment.patientUsername)
                                               +QString("    日期：")+QString(appointment.date)+QString("    就诊时间：")){
            JsonMessageBuilder *builder=new JsonMessageBuilder("modifyAppointment");
            builder->addAppointment(appointment.patientUsername,UserSession::instance().getValue("username"),
                                    QString(appointment.date),QString(appointment.time),QString(appointment.cost),"accept");
            tcpClient->sendData(builder->build());
            delete builder;

            ui->docAppointmentListWidget->clear();

            StateManager::instance().setState(WidgetState::waitReciveDoctorAppointmentList);
            qDebug()<<"Doctor_Client:构造函数:状态设置为waitReciveDoctorAppointmentList";

            builder=new JsonMessageBuilder("queryAppointmentList");
            tcpClient->sendData(builder->build());
            delete builder; // 记得释放内存

            return;
        }
    }
}

