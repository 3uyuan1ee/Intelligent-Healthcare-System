#include "doctornoticeclient.h"
#include "Notice/ui_doctornoticeclient.h"


doctorNoticeClient::doctorNoticeClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::doctorNoticeClient)
{
    ui->setupUi(this);

    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorNoticeClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &doctorNoticeClient::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &doctorNoticeClient::onDataReceivedNotice);
    JsonMessageBuilder *builder = new JsonMessageBuilder("queryNoticeList");

    StateManager::instance().setState(WidgetState::waitdie_9);

    tcpClient->sendData(builder->build());

    delete builder;
}

doctorNoticeClient::~doctorNoticeClient()
{
    delete ui;
    // JsonMessageBuilder *builder = new JsonMessageBuilder("exitChat");
    // tcpClient->sendData(builder->build());
    // delete builder;
}

//StateManager::instance().setState(WidgetState::waitdie_7);

void doctorNoticeClient::onDataReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_7) {
        qDebug() << "patientinfo::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    ui->listWidget->addItem(data["message"].toString());



}
void doctorNoticeClient::onErrorOccurred(const QString &error){

}

void doctorNoticeClient::on_pushButton_clicked()
{
    JsonMessageBuilder *builder=new JsonMessageBuilder("chat");
    builder->addAdditionalData("message",ui->lineEdit->text());
    // ui->listWidget->addItem(ui->lineEdit->text());
    tcpClient->sendData(builder->build());
    delete builder;
}

void doctorNoticeClient::closeEvent(QCloseEvent *event)
{
    JsonMessageBuilder *builder = new JsonMessageBuilder("exitChat");
    tcpClient->sendData(builder->build());
    delete builder;

}

void doctorNoticeClient::onDataReceivedNotice(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_9) {
        qDebug() << "patientinfo::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    QList<DataManager::NoticeInfo> Notices=DataManager::instance().extractNotices(data);

    for (const DataManager::NoticeInfo &Notice : Notices) {
        const QString each_instance=QString("[")+QString(Notice.type)
        +QString("][：")+QString(Notice.username)+QString("]时间：")
            +QString(Notice.time)+QString("::")+QString(Notice.message)
            ;
        ui->listWidget_2->addItem(each_instance);
    }

    JsonMessageBuilder *builder = new JsonMessageBuilder("joinChat");

    StateManager::instance().setState(WidgetState::waitdie_7);

    tcpClient->sendData(builder->build());
    delete builder;
};


