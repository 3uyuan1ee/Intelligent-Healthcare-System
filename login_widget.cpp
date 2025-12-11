/*



*/
#include "login_widget.h"
#include "./ui_login_widget.h"



Login_Widget::Login_Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Patient_login)
    // ,timeoutTimer(new QTimer(this))  // 初始化 timeoutTimer
{
    ui->setupUi(this);



    //隐藏密码
    ui->Login_Window_Passwords_lineEdit->setEchoMode(QLineEdit::Password);

    // timeoutTimer->setSingleShot(true);  // 设置为单次触发
    // connect(timeoutTimer, &QTimer::timeout, this, &Login_Widget::onLoginTimeout);  // 连接超时信号到槽函数

//------------------------------------样式---------------------------------------------

    ui->LoginWindow_usertype_comboBox->addItem("患者");
    ui->LoginWindow_usertype_comboBox->addItem("医生");
    ui->LoginWindow_usertype_comboBox->addItem("管理员");

    ui->Login_Window_Username_lineEdit->setStyleSheet(R"(
    QLineEdit {
        background-color: white;
        border: 1px solid #ccc;
        border-radius: 4px;
        padding: 5px;
    }
    QLineEdit[text=""] {
        color: #999;  /* 占位符文本颜色 */
    }
)");
    ui->Login_Window_Username_lineEdit->setPlaceholderText("用户名");

    ui->Login_Window_Passwords_lineEdit->setStyleSheet(R"(
    QLineEdit {
        background-color: white;
        border: 1px solid #ccc;
        border-radius: 4px;
        padding: 5px;
    }
    QLineEdit[text=""] {
        color: #999;  /* 占位符文本颜色 */
    }
)");
    ui->Login_Window_Passwords_lineEdit->setPlaceholderText("密码");

//------------------------------------样式---------------------------------------------

    // 初始化 TCP 客户端
    tcpClient = TcpClient::instance();
    connect(tcpClient, &TcpClient::dataReceivedJson, this, &Login_Widget::onDataReceived);
    connect(tcpClient, &TcpClient::errorOccurred, this, &Login_Widget::onErrorOccurred);



    // 连接到服务器
    // tcpClient->connectToServer("82.156.140.2", 1437);
    // tcpClient->connectToServer("43.143.238.230", 1437);
    tcpClient->connectToServer("localhost", 1437);
    // tcpClient->connectToServer("43.143.238.230", 8080);
    qDebug() << "TCP 客户端初始化完成，正在连接服务器...";
}

Login_Widget::~Login_Widget()
{
    delete ui;
}

//取消-------------------------------------------
void Login_Widget::on_pushButton_2_clicked()
{
    this->close();
    qDebug() << "关闭登录窗口";
}
//取消-------------------------------------------

//登录------------------------------------------------------------------
void Login_Widget::on_pushButton_clicked()
{

    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前正在登录，防止重复登录
    if (stateManager.currentState() == WidgetState::LoggingIn) {
        qDebug() << "Login_Widget::on_pushButton_clicked:正在登录中，请稍后...";
        return;
    }

    // 设置为登录状态
    stateManager.setState(WidgetState::LoggingIn);
    qDebug()<<"Login_Widget::on_pushButton_clicked:状态设置为LoggingIn";

    QLineEdit *username_lineEdit = ui->Login_Window_Username_lineEdit;
    username = username_lineEdit->text();
    QLineEdit *passwords_lineEdit = ui->Login_Window_Passwords_lineEdit;
    QString passwords = passwords_lineEdit->text();
    type=Function::switchChinese(ui->LoginWindow_usertype_comboBox->currentText());

    qDebug() << "用户名: " << username;  // 记录输入的用户名
    qDebug() << "密码: " << passwords;  // 记录输入的密码
    qDebug() << "用户类型: " << type;   // 记录用户类型

    // 创建 JSON 对象
    QJsonObject jsonRequest;
    jsonRequest["username"] = username;
    jsonRequest["type"] = type;
    jsonRequest["password"] = passwords;

    // 发送数据到服务器
    tcpClient->sendData(command,jsonRequest);
    qDebug() << "发送请求数据: " << jsonRequest;

    UserSession::instance().setUserInfo(jsonRequest,type,username);


/*
    演示用--------------------------------------------------------------
     this->hide();
     function_choose_widget *function_window =new function_choose_widget; //不能在栈上创建对象，一定要在堆空间上创建对象
     function_window->show();
    演示用--------------------------------------------------------------
*/

    // 启动超时定时器
    // timeoutTimer->start(10000);  // 设置为 10 秒超时

}

// void Login_Widget::onLoginTimeout()
// {
//     // 如果在 10 秒内未收到响应，显示超时提示
//     QMessageBox::warning(this, "登录失败", "请求超时，请稍后再试！");
//     qDebug()<<"onLoginTimeoout:请求超时";

//     // 恢复为 Idle 状态，表示登录已结束
//     StateManager::instance().setState(WidgetState::Idle);

//     // 关闭当前窗口或可以执行其他恢复操作
//     // 例如，关闭登录窗口或重置输入框等
//     // this->close();  // 如果需要在超时后关闭窗口
// }

// 处理登录响应
void Login_Widget::onDataReceived(const QJsonObject &data)
{



    // 获取 StateManager 单例
    StateManager& stateManager = StateManager::instance();

    // 如果当前不是登录状态，忽略接收到的数据
    if (stateManager.currentState() != WidgetState::LoggingIn) {
        qDebug() << "当前不是登录状态，忽略接收到的数据";
        return;
    }
    //状态一定要用完之后马上恢复，不然就会导致下面的function调用创建窗口实例之后，状态又被设置成默认的了

    // 完成后恢复为 Idle 状态
    stateManager.setState(WidgetState::Idle);
    qDebug()<<"Login_Widget::onDataReceived:状态设置为Idle";

    qDebug()<<"登录响应判断启动";

    qDebug() << "接收到的数据: " << data;

    // QJsonObject data1=tcpClient->receiveJson();

    if(data.isEmpty()){
        QMessageBox::warning(this, "登录失败", "服务器返回数据格式错误");
        qDebug()<<"服务器返回数据类型错误";
        return;
    }

    // QJsonDocument doc = QJsonDocument::fromJson(data);

    // if (!doc.isObject()) {
    //     QMessageBox::warning(this, "登录失败", "服务器返回数据格式错误");
    //     qDebug() << "服务器返回数据格式错误";
    //     return;
    // }



    // QJsonObject jsonResponse = doc.object();
    // QString replyStatus = jsonResponse["reply"].toString();

    QString replyStatus = data["reply"].toString();

    qDebug() << "登录响应状态: " << replyStatus;

    // if (replyStatus == "successful_connection") {
    //     qDebug() << "连接成功，等待用户登录";
    //     return; // 只是连接成功，等待用户输入登录信息
    // }
    //已经优化成状态判断


    if (replyStatus == "successful") {

        qDebug() << "登录成功，处理缓存和跳转";

        // 登录成功
//--------------------------------------------------------------------------------------------------
    // Function::handleUserCache(type, username, data);
        //这一部分是优化，先查找本地是否有文件，有文件的话直接读取文件进行缓存
//----------------------------------------------------------------------------------------------------

//---------------------------------跳转到下一个窗口------------------------------------
        Function::navigateToClientWindow(type, this);
//----------------------------------------------------------------------------------

        qDebug()<<"窗口跳转完成";
//------------------------------发送获取预约列表请求------------------------------------------------------
    // if(UserSession::instance().getValue("type")!="admin"){
    //     JsonMessageBuilder *builder=new JsonMessageBuilder("queryAppointmentList");
    //     tcpClient->sendData(builder->build());
    //      delete builder; // 记得释放内存
    // }
//--------------------------------------------------------------------------------------------------



    } else if (replyStatus == "usernameWrong") {
        // 用户名错误
        QMessageBox::warning(this, "登录失败", "用户名不存在");

        qDebug() << "用户名错误";

    } else if (replyStatus == "passwordWrong") {
        // 密码错误
        QMessageBox::warning(this, "登录失败", "密码错误");

        qDebug() << "密码错误";

    } else {
        // 其他错误
        QMessageBox::warning(this, "登录失败", "未知错误");

        qDebug() << "未知错误";
    }


}
//登录------------------------------------------------------------------


//注册---------------------------------------------------
void Login_Widget::on_pushButton_3_clicked()
{
    Enroll *enroll_window = new Enroll;
    enroll_window->show();

    qDebug() << "打开注册窗口";

}
//注册---------------------------------------------------
//错误处理
void Login_Widget::onErrorOccurred(const QString &error)
{
    QMessageBox::warning(this, "网络错误", error);

    qDebug() << "网络错误: " << error;
}

