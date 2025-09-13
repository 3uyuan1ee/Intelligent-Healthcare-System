#include "patientnoticeclient.h"
#include "Notice/ui_patientnoticeclient.h"


patientNoticeClient::patientNoticeClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::patientNoticeClient)
{
    ui->setupUi(this);
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientNoticeClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &patientNoticeClient::onErrorOccurred);
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientNoticeClient::onDataReceivedNotice);

    JsonMessageBuilder *builder = new JsonMessageBuilder("queryNoticeList");

    StateManager::instance().setState(WidgetState::waitdie_8);

    tcpClient->sendData(builder->build());

    delete builder;
}

patientNoticeClient::~patientNoticeClient()
{
    delete ui;

}

//StateManager::instance().setState(WidgetState::waitdie_6);

void patientNoticeClient::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_6) {
        qDebug() << "patientinfo::onJudgeReceived:当前不是患者信息编辑判断接收状态，忽略接收到的数据";
        return;
    }

    ui->listWidget->addItem(data["message"].toString());


}
void patientNoticeClient::onErrorOccurred(const QString &error){

}

void patientNoticeClient::on_pushButton_clicked()
{
    JsonMessageBuilder *builder=new JsonMessageBuilder("chat");
    builder->addAdditionalData("message",ui->lineEdit->text());
    tcpClient->sendData(builder->build());
    delete builder;
}

void patientNoticeClient::closeEvent(QCloseEvent *event)
{
    JsonMessageBuilder *builder = new JsonMessageBuilder("exitChat");
    tcpClient->sendData(builder->build());

    delete builder;

    StateManager::instance().setState(WidgetState::Idle);

}

void patientNoticeClient::onDataReceivedNotice(const QJsonObject &data){



    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitdie_8) {
        qDebug() << "patientNoticeClient::onJudgeReceived:当前不是患者通知接收状态，忽略接收到的数据";
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

    StateManager::instance().setState(WidgetState::waitdie_6);

    tcpClient->sendData(builder->build());
    delete builder;

}
