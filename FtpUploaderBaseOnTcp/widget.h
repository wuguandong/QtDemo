#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUrl>
#include <QTcpSocket>
#include <QRegExp>
#include <QFile>
#include <QStandardPaths>
#include <QMessageBox>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    //开始下载按钮槽函数
    void on_btnStart_clicked();
    //commandSocket的readyRead槽函数
    void commandReadyReadSlot();
    //清屏按钮槽函数
    void on_btnClear_clicked();

private:
    //发送用户名
    void sendUser();
    //发送密码
    void sendPass();
    //发送PWD
    void sendPwd();
    //获取当前目录
    void getCurrentDirectory();
    //发送TYPE
    void sendType();
    //发送PASV
    void sendPasv();
    //建立dataSocket连接
    void buildDataConnection();
    //发送CWD
    void sendCwd();
    //发送STOR
    void sendStor();
    //发送文件
    void sendFile();
    //发送QUIT
    void sendQuit();
    //断开命令连接
    void closeCommandConnection();

private:
    Ui::Widget *ui;

    QUrl url;
    QTcpSocket *commandSocket;
    QTcpSocket *dataSocket;
    QByteArray commandBuffer;
    QByteArray dataBuffer;
    QString expectedReply;
    void (Widget::*nextAction)();
    qint64 fileSize;
    qint64 sendedSize;
    QFile file;
    bool hasTrigger; //防止多次触发
};

#endif // WIDGET_H
