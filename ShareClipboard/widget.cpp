#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget), enable(false), isShared(false)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    //设置应用名称和图标
    this->setWindowTitle("共享剪切板");
    this->setWindowIcon(QIcon(":/images/icon.png"));

    //设置系统托盘
    this->trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/icon.png"));
    trayIcon->setToolTip("共享剪切板");
    connect(this->trayIcon, &QSystemTrayIcon::activated, this, &Widget::activatedSlot);
    this->trayIconMenu = new QMenu(this);
    this->trayIcon->setContextMenu(this->trayIconMenu);
    this->quitAction = new QAction("退出", this);
    connect(this->quitAction, &QAction::triggered, qApp, &QApplication::quit);
    this->trayIconMenu->addAction(this->quitAction);
    trayIcon->show();

    //初始化剪切板
    this->clipboard = QGuiApplication::clipboard();
    connect(this->clipboard, &QClipboard::dataChanged, this, &Widget::dataChangedSlot);

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
    if(!this->enable){ //点击[开启]
        ui->btnSwitch->setText("关闭");
        //获取编辑框的ip及端口号
        this->peerIp = ui->lePeerIp->text();
        this->port = static_cast<quint16>(ui->lePort->text().toInt());
        //套接字绑定
        this->socket->bind(QHostAddress::AnyIPv4, this->port); //bind自己的ip及端口
        //禁用编辑框
        ui->lePeerIp->setDisabled(true);
        ui->lePort->setDisabled(true);
    }
    else{ //点击[关闭]
        ui->btnSwitch->setText("开启");
        //启用编辑框
        ui->lePeerIp->setDisabled(false);
        ui->lePort->setDisabled(false);
    }
    enable = !enable;
}

//剪切板内容改变槽函数
void Widget::dataChangedSlot()
{
    if(enable){
        if(!isShared){
            //发送剪切板
            socket->writeDatagram(this->clipboard->text().toUtf8(), QHostAddress(this->peerIp), this->port);
            qDebug()<<"发送："<<this->clipboard->text();
        }
        else{
            isShared = false;
        }
    }
}

//套接字槽函数
void Widget::readyReadSlot()
{
    memset(buffer, 0, sizeof(buffer));
    socket->readDatagram(buffer,sizeof(buffer));
    this->isShared = true;
    clipboard->setText(QString(buffer), QClipboard::Clipboard);
//    clipboard->setText(QString(buffer), QClipboard::Selection); //[仅在Linux中打开]
    qDebug()<<"接收："<<QString(buffer);
}

//系统托盘槽函数
void Widget::activatedSlot(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger){
        if(this->isHidden()){
            this->show();
            this->showNormal();
        }
    }
}

//重写closeEvent
void Widget::closeEvent(QCloseEvent *event)
{
    this->hide();
    this->trayIcon->showMessage("共享剪切板", "已最小化到系统托盘");
    event->ignore();
}
