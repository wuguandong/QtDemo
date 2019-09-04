#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget), overtime(5 * 1000)
{
    ui->setupUi(this);

    //初始化套接字
    this->commandSocket = new QTcpSocket(this);
    this->dataSocket = new QTcpSocket(this);
    connect(this->commandSocket, &QTcpSocket::readyRead, this, &Widget::commandReadyReadSlot);
    connect(this->dataSocket, &QTcpSocket::readyRead, this, &Widget::dataReadyReadSlot);

    //初始化定时器
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &Widget::tryRedownload);
}

Widget::~Widget()
{
    delete ui;
}

//开始下载按钮槽函数
void Widget::on_btnStart_clicked()
{
    url.setUrl(ui->leUrl->text());

    commandSocket->connectToHost(url.host(), url.port()==-1? 21: static_cast<quint16>(url.port()));
    this->expectedReply = "220";
    if(url.userName().isEmpty()){
        url.setUserName("anonymous");
        url.setPassword("chrome@example.com");
    }
    this->nextAction = &Widget::sendUser;

    //初始化成员变量
    this->hasCreateFile = false;
    this->receivedSize = 0;
    this->tryRedownloadCount = 0;

    ui->progressBar->setValue(0);

    this->resetTimer();//启动定时器
}

//commandSocket的readyRead槽函数
void Widget::commandReadyReadSlot()
{
    this->resetTimer();//重启定时器
    this->commandBuffer.clear();
    this->commandBuffer = commandSocket->readAll();
    ui->textEdit->append("↓ 收到服务器回复：" + this->commandBuffer);

    if(!this->expectedReply.isEmpty() && this->commandBuffer.mid(0,3) == this->expectedReply.toLatin1()){
        if(nextAction) (this->*nextAction)();
    }
    else{
        ui->textEdit->append( QString("错误：收到不符合期待(%1)的回应:(%2)\r\n").arg(this->expectedReply).arg(QString("").append(this->commandBuffer.mid(0,3))) );
        this->tryRedownload();
    }
}

//dataSocket的readyRead槽函数
void Widget::dataReadyReadSlot()
{
    this->resetTimer();//重启定时器
    this->dataBuffer.clear();
    this->dataBuffer = dataSocket->readAll();

    //写入文件
    qint64 size = file.write(this->dataBuffer);
    this->receivedSize += size;

    //更新进度条
    ui->progressBar->setValue(static_cast<int>(receivedSize / 1024));

    if(this->receivedSize >= this->fileSize){
        dataSocket->disconnectFromHost();
        dataSocket->close();
        file.close();
    }
}

//尝试重新下载
void Widget::tryRedownload()
{
    //关闭连接
    if(commandSocket->state() == QAbstractSocket::ConnectedState){
        commandSocket->disconnectFromHost();
        commandSocket->close();
    }
    if(dataSocket->state() == QAbstractSocket::ConnectedState){
        dataSocket->disconnectFromHost();
        dataSocket->close();
    }
    //删除文件
    if(file.isOpen()){
        file.close();
        file.remove();
    }

    if(this->tryRedownloadCount++ <3){
        ui->textEdit->append(QString("\r\n********  警告：第%1次重新下载  ********\r\n").arg(tryRedownloadCount));
        commandSocket->connectToHost(url.host(), url.port()==-1? 21: static_cast<quint16>(url.port()));
        this->expectedReply = "220";
        this->nextAction = &Widget::sendUser;
        this->hasCreateFile = false; //初始化成员变量
        this->receivedSize = 0;
        ui->progressBar->setValue(0);
    }
    else{
        if(this->timer->isActive()) this->timer->stop(); //停止定时器
        QMessageBox::information(this, "提示", "下载失败！");
    }
}

//发送用户名
void Widget::sendUser()
{
    QString command = QString("USER %1\r\n").arg(url.userName());
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "331";
    this->nextAction = &Widget::sendPass;
}

//发送密码
void Widget::sendPass()
{
    QString command = QString("PASS %1\r\n").arg(url.password());
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "230";
    this->nextAction = &Widget::sendPwd;
}

//发送PWD
void Widget::sendPwd()
{
    QString command = "PWD\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "257";
    this->nextAction = &Widget::getCurrentDirectory;
}

//获取当前目录
void Widget::getCurrentDirectory()
{
    QRegExp rx(R"(".+")");
    rx.indexIn(this->commandBuffer);
    QString tmp = rx.capturedTexts().at(0);
    this->currentDirectory = tmp.mid(1,tmp.length()-2);

    this->sendType();
}

//发送TYPE
void Widget::sendType()
{
    QString command = "TYPE I\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "200";
    this->nextAction = &Widget::sendSize;
}

//发送SIZE
void Widget::sendSize()
{
    QString command = QString("SIZE %1%2\r\n").arg(this->currentDirectory == "/"? "": this->currentDirectory).arg(url.path());
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "213";
    this->nextAction = &Widget::getFileSize;
}

//获取文件大小
void Widget::getFileSize()
{
    this->fileSize = commandBuffer.mid(4, commandBuffer.length()-6).toInt();

    //初始化进度条
    ui->progressBar->setMaximum(static_cast<int>(this->fileSize / 1024));

    this->sendPasv();
}

//发送PASV
void Widget::sendPasv()
{
    QString command = "PASV\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "227";
    this->nextAction = &Widget::buildDataConnection;
}

//建立dataSocket连接
void Widget::buildDataConnection()
{
    QRegExp rx(R"(\(\d{1,3},\d{1,3},\d{1,3},\d{1,3},\d{1,3},\d{1,3}\))");
    rx.indexIn(this->commandBuffer);
    QString tmp = rx.capturedTexts().at(0);
    QStringList list = tmp.mid(1,tmp.length()-2).split(',');

    quint16 port = static_cast<quint16>(list.at(4).toInt() * 256 + list.at(5).toInt());
    dataSocket->connectToHost(url.host(), port);
    connect(dataSocket, &QTcpSocket::connected, [=]{
        if(!this->hasCreateFile){
            this->hasCreateFile = true;
            //创建文件
            file.setFileName(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/").append(url.fileName()));
            bool isOk = file.open(QIODevice::WriteOnly);
            if(!isOk){
                ui->textEdit->append("错误：文件打开失败");
                return;
            }

            this->sendRetr();
        }
    });
}

//发送RETR
void Widget::sendRetr()
{
    QString command = QString("RETR %1%2\r\n").arg(this->currentDirectory == "/"? "": this->currentDirectory).arg(url.path());
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "150";
    this->nextAction = &Widget::waitTransferComplete;
}

//等待传输完成
void Widget::waitTransferComplete()
{
    this->expectedReply = "226";
    this->nextAction = &Widget::sendQuit;
}

//发送QUIT
void Widget::sendQuit()
{
    QString command = "QUIT\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "221";
    this->nextAction = &Widget::closeCommandConnection;
}

//断开命令连接
void Widget::closeCommandConnection()
{
    commandSocket->disconnectFromHost();
    commandSocket->close();

    if(this->timer->isActive()) this->timer->stop(); //停止定时器

    QMessageBox::information(this ,"提示", "下载完成！");
}

//启动或重启定时器
void Widget::resetTimer()
{
    if(!this->timer->isActive()){
        this->timer->start(overtime);
    }
    else{
        this->timer->stop();
        this->timer->start(overtime);
    }
}



void Widget::on_btnClear_clicked()
{
    ui->textEdit->clear();
}
