#include "admininfodoctorclient.h"
#include "infoClient/ui_admininfodoctorclient.h"


adminInfoDoctorClient::adminInfoDoctorClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminInfoDoctorClient)
{
    ui->setupUi(this);
    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &adminInfoDoctorClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &adminInfoDoctorClient::onErrorOccurred);

    connect(tcpClient, &TcpClient::dataReceivedJson, this, &adminInfoDoctorClient::onJudgeReceived);
}

adminInfoDoctorClient::~adminInfoDoctorClient()
{
    delete ui;
}

void adminInfoDoctorClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_2) {
        qDebug() << "adminInfoDoctorClient::onDataReceived:当前不是医生信息接收状态，忽略接收到的数据";
        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);

    qDebug()<<"adminInfoDoctorClient::onDataReceived:正在接收医生信息";

    QJsonObject adminInfoDoctorClient=data.value("doctorInfo").toObject();

    qDebug()<<"adminInfoDoctorClient::onDataReceived:医生信息："<<adminInfoDoctorClient;

    //关闭只读
    ui->costLineEdit->setReadOnly(false);
    ui->departmentLineEdit->setReadOnly(false);
    ui->docIdLineEdit->setReadOnly(false);
    ui->docNameLineEdit->setReadOnly(false);
    ui->docBeginSpinBox->setReadOnly(false);
    ui->docEndSpinBox->setReadOnly(false);
    ui->docLimitSpinBox->setReadOnly(false);


    qDebug()<<"adminInfoDoctorClient::onDataReceived:正在向ui界面显示信息";

    ui->costLineEdit->setText(adminInfoDoctorClient["cost"].toString());
    ui->departmentLineEdit->setText(adminInfoDoctorClient["department"].toString());
    ui->docIdLineEdit->setText(adminInfoDoctorClient["id"].toString());
    ui->docNameLineEdit->setText(adminInfoDoctorClient["name"].toString());
    ui->docBeginSpinBox->setValue(adminInfoDoctorClient["begin"].toString().toInt());
    ui->docEndSpinBox->setValue(adminInfoDoctorClient["end"].toString().toInt());
    ui->docLimitSpinBox->setValue(adminInfoDoctorClient["limit"].toString().toInt());

    qDebug()<<"adminInfoDoctorClient::onDataReceived:向ui界面显示信息完成";

    //开启只读
    ui->costLineEdit->setReadOnly(true);
    ui->departmentLineEdit->setReadOnly(true);
    ui->docIdLineEdit->setReadOnly(true);
    ui->docNameLineEdit->setReadOnly(true);
    ui->docBeginSpinBox->setReadOnly(true);
    ui->docEndSpinBox->setReadOnly(true);
    ui->docLimitSpinBox->setReadOnly(true);


};  // 添加数据接收槽
void adminInfoDoctorClient::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
};  // 添加错误处理槽

//编辑
void adminInfoDoctorClient::on_editPushButton_2_clicked()
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
void adminInfoDoctorClient::on_savePushButton_2_clicked()
{

    //  [adminInfoDoctorClient] 医生信息
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

    JsonMessageBuilder *builder =new JsonMessageBuilder("modifyadminInfoDoctorClient");
    builder->addDoctorInfo(ui->docNameLineEdit->text(),ui->docIdLineEdit->text(),
                           ui->departmentLineEdit->text(),ui->costLineEdit->text(),
                           ui->docBeginSpinBox->text(),ui->docEndSpinBox->text(),
                           ui->docLimitSpinBox->text());
    tcpClient->sendData(builder->build());

    StateManager::instance().setState(WidgetState::waitdie_5);
    qDebug()<<"adminInfoDoctorClient::on_savePushButton_2_clicked状态设置为:waitReciveadminInfoDoctorClientEdit";

    QMessageBox::information(this,"提示","保存成功");

    delete builder; // 记得释放内存
}

void adminInfoDoctorClient::onJudgeReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_5) {
        qDebug() << "adminInfoDoctorClient::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    StateManager::instance().setState(WidgetState::Idle);
    qDebug()<<"adminInfoDoctorClient::onJudgeReceived:状态设置为:Idle";

    if(data.isEmpty()){
        QMessageBox::warning(this, "编辑失败", "服务器返回数据格式错误");
        qDebug()<<"adminInfoDoctorClient::onJudgeReceived:服务器返回数据类型错误";
        return;
    }

    qDebug()<<"adminInfoDoctorClient::onJudgeReceived:正在接收患者信息编辑判断";

    QString replyStatus = data["reply"].toString();

    if(replyStatus=="successful"){
        QMessageBox::information(this,"提示","您的编辑已成功保存");
    }else{
        QMessageBox::information(this,"提示","您的编辑保存失败");
    }

    StateManager::instance().setState(WidgetState::waitdie_2);
    qDebug()<<"adminInfoDoctorClient::onJudgeReceived:状态设置为:waitReciveadminInfoDoctorClient";

    JsonMessageBuilder *builder= new JsonMessageBuilder("queryadminInfoDoctorClient");
    builder->addAdditionalData("doctorUsername",usr);
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存


}
