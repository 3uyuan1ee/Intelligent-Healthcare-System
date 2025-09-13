#include "doctorinfo.h"
#include "infoClient/ui_doctorinfo.h"

doctorInfo::doctorInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorInfo)
{
    ui->setupUi(this);

    StateManager::instance().setState(WidgetState::waitReciveDoctorInfo);
    qDebug()<<"doctorInfo::doctorInfo:状态设置为:waitReciveDoctorInfo";

    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorInfo::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorInfo::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorInfo::onJudgeReceived);

}

doctorInfo::~doctorInfo()
{
    delete ui;
}

void doctorInfo::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorInfo) {
        qDebug() << "doctorinfo::onDataReceived:当前不是医生信息接收状态，忽略接收到的数据";
        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);

    qDebug()<<"doctorinfo::onDataReceived:正在接收医生信息";

    QJsonObject doctorInfo=data.value("doctorInfo").toObject();

    qDebug()<<"doctorInfo::onDataReceived:医生信息："<<doctorInfo;

    //关闭只读
    ui->costLineEdit->setReadOnly(false);
    ui->departmentLineEdit->setReadOnly(false);
    ui->docIdLineEdit->setReadOnly(false);
    ui->docNameLineEdit->setReadOnly(false);
    ui->docBeginSpinBox->setReadOnly(false);
    ui->docEndSpinBox->setReadOnly(false);
    ui->docLimitSpinBox->setReadOnly(false);


    qDebug()<<"doctorInfo::onDataReceived:正在向ui界面显示信息";

    ui->costLineEdit->setText(doctorInfo["cost"].toString());
    ui->departmentLineEdit->setText(doctorInfo["department"].toString());
    ui->docIdLineEdit->setText(doctorInfo["id"].toString());
    ui->docNameLineEdit->setText(doctorInfo["name"].toString());
    ui->docBeginSpinBox->setValue(doctorInfo["begin"].toString().toInt());
    ui->docEndSpinBox->setValue(doctorInfo["end"].toString().toInt());
    ui->docLimitSpinBox->setValue(doctorInfo["limit"].toString().toInt());

    qDebug()<<"doctorInfo::onDataReceived:向ui界面显示信息完成";

    //开启只读
    ui->costLineEdit->setReadOnly(true);
    ui->departmentLineEdit->setReadOnly(true);
    ui->docIdLineEdit->setReadOnly(true);
    ui->docNameLineEdit->setReadOnly(true);
    ui->docBeginSpinBox->setReadOnly(true);
    ui->docEndSpinBox->setReadOnly(true);
    ui->docLimitSpinBox->setReadOnly(true);


};  // 添加数据接收槽
void doctorInfo::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
};  // 添加错误处理槽

//编辑
void doctorInfo::on_editPushButton_2_clicked()
{

    //关闭只读
    ui->costLineEdit->setReadOnly(false);
    ui->departmentLineEdit->setReadOnly(false);
    ui->docIdLineEdit->setReadOnly(false);
    ui->docNameLineEdit->setReadOnly(false);
    ui->docBeginSpinBox->setReadOnly(false);
    ui->docEndSpinBox->setReadOnly(false);
    ui->docLimitSpinBox->setReadOnly(false);

}

//保存
void doctorInfo::on_savePushButton_2_clicked()
{

    //  [doctorInfo] 医生信息
    // {
    //     [username]
    //         [name]
    //         [id]
    //         [department]
    //         [cost]
    //         [begin] { 0 ~ 24 }
    //             [end] { 0 ~ 24 }
    //                 [limit]
    // }

    JsonMessageBuilder *builder =new JsonMessageBuilder("modifyDoctorInfo");
    builder->addDoctorInfo(ui->docNameLineEdit->text(),ui->docIdLineEdit->text(),
                            ui->departmentLineEdit->text(),ui->costLineEdit->text(),
                           ui->docBeginSpinBox->text(),ui->docEndSpinBox->text(),
                           ui->docLimitSpinBox->text());
    tcpClient->sendData(builder->build());

    StateManager::instance().setState(WidgetState::waitReciveDoctorInfoEdit);
    qDebug()<<"doctorInfo::on_savePushButton_2_clicked状态设置为:waitReciveDoctorInfoEdit";


    delete builder; // 记得释放内存
}

void doctorInfo::onJudgeReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveDoctorInfoEdit) {
        qDebug() << "doctorInfo::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"doctorInfo::onJudgeReceived:状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "编辑失败", "服务器返回数据格式错误");
        qDebug()<<"doctorInfo::onJudgeReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"doctorInfo::onJudgeReceived:正在接收患者信息编辑判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","您的编辑已成功保存");
    }else{
        QMessageBox::information(this,"提示","您的编辑保存失败");
    }

    StateManager::instance().setState(WidgetState::waitReciveDoctorInfo);
    qDebug()<<"doctorInfo::onJudgeReceived:状态设置为:waitReciveDoctorInfo";

    JsonMessageBuilder *builder= new JsonMessageBuilder("querydoctorInfo");
    builder->addAdditionalData("doctorUsername",UserSession::instance().getValue("username"));
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存


}

