#include "patienthealthquestionclient.h"
#include "QuestionClient/ui_patienthealthquestionclient.h"


patientHealthQuestionClient::patientHealthQuestionClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::patientHealthQuestionClient)
{
    ui->setupUi(this);

    tcpClient=TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &patientHealthQuestionClient::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &patientHealthQuestionClient::onErrorOccurred);
}

patientHealthQuestionClient::~patientHealthQuestionClient()
{
    delete ui;
}

void patientHealthQuestionClient::on_pushButton_clicked()
{


    StateManager::instance().setState(WidgetState::waitRecivePatientQuestionResult);
    qDebug()<<"patientHealthQuestionClient::on_pushButton_clicked:状态设置为:waitRecivePatientQuestionResult";

    JsonMessageBuilder *builder=new JsonMessageBuilder("modifyQuestion");
    builder->addQuestionnaire(ui->nameLineEdit->text(),ui->genderLineEdit->text(),
                                ui->ageLineEdit->text(),ui->heightLineEdit->text(),
                                ui->weightLineEdit->text(),ui->heartLineEdit->text(),
                              ui->pressureLineEdit->text(),ui->lungLineEdit->text());
    tcpClient->sendData(builder->build());


}

void patientHealthQuestionClient::onDataReceived(const QJsonObject &data){

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitRecivePatientQuestionResult) {
        qDebug() << "patientCaseMenuClient::onDataReceived:当前不是患者病例列表接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"patientHealthQuestionClient::onDataReceived:状态设置为Idle";

    qDebug()<<"patientHealthQuestionClient::onDataReceived:正在接收问卷结果";

    QMessageBox::information(this,"问卷结果",data.value("result").toString());


};  // 添加数据接收槽
void patientHealthQuestionClient::onErrorOccurred(const QString &error){

};  // 添加错误处理槽
