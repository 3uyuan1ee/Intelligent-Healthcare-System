#include "admininfoclient.h"
#include "infoClient/ui_admininfoclient.h"

adminInfoClient::adminInfoClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminInfoClient)
{
    ui->setupUi(this);
    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &adminInfoClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &adminInfoClient::onErrorOccurred);

    //开启只读
    ui->birthLineEdit->setReadOnly(true);
    ui->emailLineEdit->setReadOnly(true);
    ui->genderLineEdit->setReadOnly(true);
    ui->idLineEdit->setReadOnly(true);
    ui->nameLineEdit->setReadOnly(true);
    ui->phoneLineEdit->setReadOnly(true);

}

adminInfoClient::~adminInfoClient()
{
    delete ui;
}

void adminInfoClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_3) {
        qDebug() << "adminInfoClient::onDataReceived:当前不是患者信息接收状态，忽略接收到的数据";
        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);

    qDebug()<<"adminInfoClient::onDataReceived:正在接收患者信息";


    QJsonObject adminInfoClient=data.value("patientInfo").toObject();

    qDebug()<<"adminInfoClient::onDataReceived:患者信息："<<adminInfoClient;

    //关闭只读
    ui->birthLineEdit->setReadOnly(false);
    ui->emailLineEdit->setReadOnly(false);
    ui->genderLineEdit->setReadOnly(false);
    ui->idLineEdit->setReadOnly(false);
    ui->nameLineEdit->setReadOnly(false);
    ui->phoneLineEdit->setReadOnly(false);

    qDebug()<<"adminInfoClient::onDataReceived:正在向ui界面显示信息";

    ui->birthLineEdit->setText(adminInfoClient["birthday"].toString());
    ui->emailLineEdit->setText(adminInfoClient["email"].toString());
    ui->genderLineEdit->setText(adminInfoClient["gender"].toString());
    ui->idLineEdit->setText(adminInfoClient["id"].toString());
    ui->nameLineEdit->setText(adminInfoClient["name"].toString());
    ui->phoneLineEdit->setText(adminInfoClient["phoneNumber"].toString());

    qDebug()<<"adminInfoClient::onDataReceived:向ui界面显示信息完成";

    //开启只读
    ui->birthLineEdit->setReadOnly(true);
    ui->emailLineEdit->setReadOnly(true);
    ui->genderLineEdit->setReadOnly(true);
    ui->idLineEdit->setReadOnly(true);
    ui->nameLineEdit->setReadOnly(true);
    ui->phoneLineEdit->setReadOnly(true);



};  // 添加数据接收槽

void adminInfoClient::on_editPushButton_clicked()
{
    //关闭只读
    ui->birthLineEdit->setReadOnly(false);
    ui->emailLineEdit->setReadOnly(false);
    ui->genderLineEdit->setReadOnly(false);
    ui->idLineEdit->setReadOnly(false);
    ui->nameLineEdit->setReadOnly(false);
    ui->phoneLineEdit->setReadOnly(false);
}


void adminInfoClient::on_savePushButton_clicked()
{
    JsonMessageBuilder *builder =new JsonMessageBuilder("modifyadminInfoClient");
    builder->addPatientInfo(ui->nameLineEdit->text(),ui->genderLineEdit->text(),ui->birthLineEdit->text(),
                            ui->idLineEdit->text(),ui->phoneLineEdit->text(),ui->emailLineEdit->text());
    tcpClient->sendData(builder->build());

    StateManager::instance().setState(WidgetState::waitdie_4);
    qDebug()<<"adminInfoClient::on_savePushButton:状态设置为:waitReciveadminInfoClientEdit";


    delete builder; // 记得释放内存
}


void adminInfoClient::onJudgeReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_4) {
        qDebug() << "adminInfoClient::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"adminInfoClient::onJudgeReceived:状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "编辑失败", "服务器返回数据格式错误");
        qDebug()<<"adminInfoClient::onJudgeReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"adminInfoClient::onJudgeReceived:正在接收患者信息编辑判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","您的编辑已成功保存");
    }else{
        QMessageBox::information(this,"提示","您的编辑保存失败");
    }

    StateManager::instance().setState(WidgetState::waitdie_3);
    qDebug()<<"adminInfoClient::onJudgeReceived:状态设置为:waitReciveadminInfoClient";

    JsonMessageBuilder *builder= new JsonMessageBuilder("queryadminInfoClient");
    builder->addAdditionalData("patientUsername",usr);
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存


}



void adminInfoClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
};  // 添加错误处理槽
