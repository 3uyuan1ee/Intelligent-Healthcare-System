#include "adminnoticeclient.h"
#include "ui_adminnoticeclient.h"

adminNoticeClient::adminNoticeClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminNoticeClient)
{
    ui->setupUi(this);
}

adminNoticeClient::~adminNoticeClient()
{
    delete ui;
}

void adminNoticeClient::on_pushButton_clicked()
{
    JsonMessageBuilder *builder=new JsonMessageBuilder("modifyNotice");
    builder->addNotice(UserSession::instance().getValue("username"),"admin",ui->textEdit->toPlainText());
    tcpclient->sendData(builder->build());
    delete builder;

    QMessageBox::information(this,"提示","发送成功");
}

