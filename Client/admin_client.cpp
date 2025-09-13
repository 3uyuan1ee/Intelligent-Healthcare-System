#include "admin_client.h"
#include "ui_admin_client.h"

Admin_Client::Admin_Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Admin_Client)
{
    ui->setupUi(this);

    StateManager::instance().setState(WidgetState::waitReciveAdminChart);
    qDebug()<<"Admin_Client::构造函数:状态设置为waitReciveAdminChart";

    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &Admin_Client::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &Admin_Client::onErrorOccurred);

    // 延迟100毫秒后执行
    // QTimer::singleShot(100, this, [this]() {
    //     // 这里写延迟后要执行的代码
    //     qDebug() << "延迟100ms后执行";
    // });

    JsonMessageBuilder *builder=new JsonMessageBuilder("queryChart");
    tcpClient->sendData(builder->build());
    delete builder; // 记得释放内存
}

Admin_Client::~Admin_Client()
{
    delete ui;
}

void Admin_Client::onDataReceived(const QJsonObject &data){
    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前状态不合，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::waitReciveAdminChart) {
        qDebug() << "Admin_Client::onDataReceived:当前不是Chart接收状态，忽略接收到的数据";

        return;
    }

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"Admin_Client::onDataReceived:状态设置为Idle";


    qDebug()<<"Admin_Client::onDataReceived:正在接受Chart";


    ui->adminNoticeListWidget->addItem(data["chart"].toString());



}  // 添加数据接收槽
void Admin_Client::onErrorOccurred(const QString &error){

    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;

} // 添加错误处理槽

//    adminCheckInClient *admincheckinclient=nullptr;
// adminNoticeClient *adminnoticeclient=nullptr;
// adminInfoMenuClient *admininfomenuclient=nullptr;

void Admin_Client::on_adminCheckInPushButton_clicked()
{

    //waitReciveDoctorCaseList
    //这里一定要写，如果窗口关闭又打开，就会创建多个实例，导致ui信息无法显示
    //如果实例不存在，则创建
    //doctorCaseClient *doctorcaseclient
    if (!admincheckinclient) {
        admincheckinclient = new adminCheckInClient/*(this)*/;
        //设置窗口属性
        admincheckinclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    admincheckinclient->show();
}


void Admin_Client::on_adminInfoPushButton_clicked()
{
    if (!admininfomenuclient) {
        admininfomenuclient = new adminInfoMenuClient/*(this)*/;
        //设置窗口属性
        admininfomenuclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    admininfomenuclient->show();
}


void Admin_Client::on_adminNoticePushButton_clicked()
{

    if (!adminnoticeclient) {
        adminnoticeclient = new adminNoticeClient/*(this)*/;
        //设置窗口属性
        adminnoticeclient->setAttribute(Qt::WA_DeleteOnClose, false);

    }

    adminnoticeclient->show();
}


