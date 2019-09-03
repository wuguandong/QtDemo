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
    //dataSocket的readyRead槽函数
    void dataReadyReadSlot();

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
    //发送SIZE
    void sendSize();
    //获取文件大小
    void getFileSize();
    //发送PASV
    void sendPasv();
    //建立dataSocket连接
    void buildDataConnection();
    //发送RETR
    void sendRetr();
    //等待传输完成
    void waitTransferComplete();
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
    QString currentDirectory;
    qint64 fileSize;
    qint64 receivedSize;
    QFile file;
    bool hasCreateFile;

};

#endif // WIDGET_H
