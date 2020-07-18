#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    _progressBar = new ProgressBarDialog(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pb_progressBar_clicked()
{
    _progressBar->start("正在下载");

    _progressValue = 0;

    QTimer *timer = new QTimer;
    connect(timer, &QTimer::timeout, [=]{
        _progressBar->setValue(++_progressValue);
        if(_progressValue >= 100){
            QMessageBox::information(this, "提示", "下载成功", "确定");
            _progressBar->hide();
            timer->stop();
            timer->deleteLater();
        }
    });
    timer->start(20);
}
