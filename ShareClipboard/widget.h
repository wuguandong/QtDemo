#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QClipboard>
#include <QTimer>
#include <QUdpSocket>
#include <QDebug>

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
    //开关按钮槽函数
    void on_btnSwitch_clicked();
    //定时器槽函数
    void timeoutSlot();
    //套接字槽函数
    void readyReadSlot();

private:
    Ui::Widget *ui;

    QClipboard *clipboard; //剪切板
    QTimer *timer; //定时器
    QUdpSocket *socket; //套接字
    bool enable; //开关状态
    QString peerIp; //对方IP
    quint16 port; //端口号
    QString lastContent; //上一次的剪切板内容
    QString currentContent; //当前的剪切板内容
    char buffer[9999];
};

#endif // WIDGET_H
