#include "patientinfo.h"
#include "infoClient/ui_patientinfo.h"

patientInfo::patientInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::patientInfo)
{
    ui->setupUi(this);


    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientInfo::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &patientInfo::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientInfo::onJudgeReceived);




}

patientInfo::~patientInfo()
{

    delete ui;
}

void patientInfo::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientInfo) {
        qDebug() << "patientinfo::onDataReceived:当前不是患者信息接收状态，忽略接收到的数据";
        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);

    qDebug()<<"patientinfo::onDataReceived:正在接收患者信息";


    QJsonObject patientinfo=data.value("patientInfo").toObject();

    qDebug()<<"patientinfo::onDataReceived:患者信息："<<patientinfo;

    //关闭只读
    ui->birthLineEdit->setReadOnly(false);
    ui->emailLineEdit->setReadOnly(false);
    ui->genderLineEdit->setReadOnly(false);
    ui->idLineEdit->setReadOnly(false);
    ui->nameLineEdit->setReadOnly(false);
    ui->phoneLineEdit->setReadOnly(false);

    qDebug()<<"patientinfo::onDataReceived:正在向ui界面显示信息";

    ui->birthLineEdit->setText(patientinfo["birthday"].toString());
    ui->emailLineEdit->setText(patientinfo["email"].toString());
    ui->genderLineEdit->setText(patientinfo["gender"].toString());
    ui->idLineEdit->setText(patientinfo["id"].toString());
    ui->nameLineEdit->setText(patientinfo["name"].toString());
    ui->phoneLineEdit->setText(patientinfo["phoneNumber"].toString());

    qDebug()<<"patientinfo::onDataReceived:向ui界面显示信息完成";

    //开启只读
    ui->birthLineEdit->setReadOnly(true);
    ui->emailLineEdit->setReadOnly(true);
    ui->genderLineEdit->setReadOnly(true);
    ui->idLineEdit->setReadOnly(true);
    ui->nameLineEdit->setReadOnly(true);
    ui->phoneLineEdit->setReadOnly(true);



};  // 添加数据接收槽

void patientInfo::onJudgeReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientInfoEdit) {
        qDebug() << "patientinfo::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"patientinfo::onJudgeReceived:状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "编辑失败", "服务器返回数据格式错误");
        qDebug()<<"patientinfo::onJudgeReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"patientinfo::onJudgeReceived:正在接收患者信息编辑判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","您的编辑已成功保存");
    }else{
        QMessageBox::information(this,"提示","您的编辑保存失败");
    }

    StateManager::instance().setState(WidgetState::waitRecivePatientInfo);
        qDebug()<<"patientinfo::onJudgeReceived:状态设置为:waitRecivePatientInfo";

    JsonMessageBuilder *builder= new JsonMessageBuilder("queryPatientInfo");
    builder->addAdditionalData("patientUsername",UserSession::instance().getValue("username"));
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存


}



void patientInfo::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
};  // 添加错误处理槽

void patientInfo::on_editPushButton_clicked()
{

    //关闭只读
    ui->birthLineEdit->setReadOnly(false);
    ui->emailLineEdit->setReadOnly(false);
    ui->genderLineEdit->setReadOnly(false);
    ui->idLineEdit->setReadOnly(false);
    ui->nameLineEdit->setReadOnly(false);
    ui->phoneLineEdit->setReadOnly(false);

}


void patientInfo::on_savePushButton_clicked()
{

    JsonMessageBuilder *builder =new JsonMessageBuilder("modifyPatientInfo");
    builder->addPatientInfo(ui->nameLineEdit->text(),ui->genderLineEdit->text(),ui->birthLineEdit->text(),
                            ui->idLineEdit->text(),ui->phoneLineEdit->text(),ui->emailLineEdit->text());
    tcpClient->sendData(builder->build());

    StateManager::instance().setState(WidgetState::waitRecivePatientInfoEdit);
    qDebug()<<"patientinfo::on_savePushButton:状态设置为:waitRecivePatientInfoEdit";


    delete builder; // 记得释放内存
}

