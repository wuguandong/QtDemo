#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget), enable(false)
{
    ui->setupUi(this);

    //设置应用名称和图标
    this->setWindowTitle("共享剪切板");
    this->setWindowIcon(QIcon(":/images/icon.png"));

    //初始化剪切板
    this->clipboard = QGuiApplication::clipboard();

    //初始化定时器
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &Widget::timeoutSlot);

    //初始化套接字
    this->socket = new QUdpSocket(this);
    connect(this->socket, &QUdpSocket::readyRead, this, &Widget::readyReadSlot);
}

Widget::~Widget()
{
    delete ui;
}

//开关按钮槽函数
void Widget::on_btnSwitch_clicked()
{
    if(!this->enable){ //点击开启
        ui->btnSwitch->setText("关闭");
        //获取编辑框的ip及端口号
        this->peerIp = ui->lePeerIp->text();
        this->port = static_cast<quint16>(ui->lePort->text().toInt());
        //套接字绑定
        this->socket->bind(QHostAddress::AnyIPv4, this->port); //bind自己的ip及端口
        //禁用编辑框
        ui->lePeerIp->setDisabled(true);
        ui->lePort->setDisabled(true);
        //初始化lastContent
        this->lastContent = clipboard->text();
        //启动定时器
        this->timer->start(1000);
    }
    else{ //点击关闭
        ui->btnSwitch->setText("开启");
        //启用编辑框
        ui->lePeerIp->setDisabled(false);
        ui->lePort->setDisabled(false);
        //停止定时器
        this->timer->stop();
    }
    enable = !enable;
}

//定时器槽函数
void Widget::timeoutSlot()
{
    this->currentContent = clipboard->text();
    if(lastContent != currentContent){
        //发送剪切板
        socket->writeDatagram(currentContent.toUtf8(), QHostAddress(this->peerIp), this->port);
        qDebug()<<"UDP发送："<<this->currentContent;
    }
    //更新lastContent
    lastContent = currentContent;
}

//套接字槽函数
void Widget::readyReadSlot()
{
    memset(buffer, 0, sizeof(buffer));
    socket->readDatagram(buffer,sizeof(buffer));
    this->lastContent = QString(buffer);
    qDebug()<<"UDP接收："<<this->lastContent;
    clipboard->setText(this->lastContent);
}
