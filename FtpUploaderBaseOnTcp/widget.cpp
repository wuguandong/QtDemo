#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    //初始化套接字
    this->commandSocket = new QTcpSocket(this);
    this->dataSocket = new QTcpSocket(this);
    connect(this->commandSocket, &QTcpSocket::readyRead, this, &Widget::commandReadyReadSlot);
}

Widget::~Widget()
{
    delete ui;
}

//开始下载按钮槽函数
void Widget::on_btnStart_clicked()
{
    //url.setUrl(ui->leUrl->text());
    url.setHost("192.168.1.104");
    url.setUserName("xgh");
    url.setPassword("1");

    commandSocket->connectToHost(url.host(), url.port()==-1? 21: static_cast<quint16>(url.port()));
    this->expectedReply = "220";
    if(url.userName().isEmpty()){
        url.setUserName("anonymous");
        url.setPassword("chrome@example.com");
    }
    this->nextAction = &Widget::sendUser;

    //初始化成员变量
    this->sendedSize = 0;

    ui->progressBar->setValue(0);
}

//commandSocket的readyRead槽函数
void Widget::commandReadyReadSlot()
{
    this->commandBuffer.clear();
    this->commandBuffer = commandSocket->readAll();
    ui->textEdit->append("↓ 收到服务器回复：" + this->commandBuffer);

    if(!this->expectedReply.isEmpty() && this->commandBuffer.mid(0,3) == this->expectedReply.toLatin1()){
        if(nextAction) (this->*nextAction)();
    }
    else{
        ui->textEdit->append( QString("错误：收到不符合期待(%1)的回应:(%2)\r\n").arg(this->expectedReply).arg(QString("").append(this->commandBuffer.mid(0,3))) );
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
    QString command = "MKD -p AAAAA/BBB/CCCC/DDDD\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "257xx";
    this->nextAction = &Widget::sendType;
}

//发送TYPE
void Widget::sendType()
{
    QString command = "TYPE I\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "200";
    this->nextAction = &Widget::sendPasv;
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
        if(!this->hasTrigger){
            this->hasTrigger = true;

            this->sendCwd();
        }
    });
}

//发送CWD
void Widget::sendCwd()
{
    QString command = QString("CWD /imgfile\r\n");
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "250";
    this->nextAction = &Widget::sendStor;
}

//发送STOR
void Widget::sendStor()
{
    QString command = QString("STOR 123.jpg\r\n");
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "150";
    this->nextAction = &Widget::sendFile;
}

//发送文件
void Widget::sendFile()
{
    //打开文件临时放在这个函数
    file.setFileName("C:/Users/wugua/Desktop/123.jpg");
    file.open(QIODevice::ReadOnly);
    this->fileSize = file.size();

//    qint64 len = 0;
//    do{
//        qDebug()<<"发送文件ing..";
//        char buf[4*1024]={0}; //每次发送数据的大小
//         len = file.read(buf,sizeof(buf)); //往文件中读数据
//        //发送数据，读多少，发多少
//        len = dataSocket->write(buf,len);
//        //发送的数据需要累积
//        this->sendedSize += len;
//    }while( len>0 );

    this->dataBuffer = file.readAll();
    this->dataSocket->write(this->dataBuffer);

    //是否发送文件完毕
    if(this->sendedSize == this->fileSize){
        ui->textEdit->append("文件发送完毕!");
        file.close();

        //把dataSocket断开
        this->dataSocket->disconnectFromHost();
        this->dataSocket->close();
    }

    this->expectedReply = "";
    this->nextAction = &Widget::sendQuit;
}

//发送QUIT
void Widget::sendQuit()
{
    QString command = "QUIT\r\n";
    ui->textEdit->append("↑ 向服务器发送：　" + command);
    commandSocket->write(command.toLatin1());
    this->expectedReply = "226";
    this->nextAction = &Widget::closeCommandConnection;
}

//断开命令连接
void Widget::closeCommandConnection()
{
    commandSocket->disconnectFromHost();
    commandSocket->close();

    QMessageBox::information(this ,"提示", "下载完成！");
}

void Widget::on_btnClear_clicked()
{
    ui->textEdit->clear();
}
