#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtGlobal>
#include <QClipboard>
#include <QUdpSocket>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
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
    //剪切板内容改变槽函数
    void dataChangedSlot();
    //套接字槽函数
    void readyReadSlot();
    //系统托盘槽函数
    void activatedSlot(QSystemTrayIcon::ActivationReason reason);

protected:
    //重写closeEvent
    void closeEvent(QCloseEvent *event);

private:
    Ui::Widget *ui;

    QClipboard *clipboard; //剪切板
    QUdpSocket *socket; //套接字
    bool enable; //开关状态
    QString peerIp; //对方IP
    quint16 port; //端口号
    bool isShared; //防止死循环
    char buffer[9999];
    QSystemTrayIcon *trayIcon; //系统托盘图标
    QMenu *trayIconMenu;
    QAction *quitAction;
};

#endif // WIDGET_H
