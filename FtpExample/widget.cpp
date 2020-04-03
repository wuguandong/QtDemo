#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"
#include "ftpdownloader.h"
#include "ftpuploader.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    //下载
    FtpDownloader *downloader = new FtpDownloader("ftp://192.168.31.90/slam/map.pgm", "C:/Users/wugua/AppData/Local/WGD/GridmapEditor/cache/map.pgm");
    connect(downloader, &FtpDownloader::finished, [=](bool success){
        qDebug()<<"下载成功？"<<success;
    });

    //上传
//    FtpUploader *uploader = new FtpUploader("ftp://192.168.31.90/栅格地图编辑器v0.1.exe", "./栅格地图编辑器v0.1.exe");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    //下载
    FtpDownloader *downloader = new FtpDownloader("ftp://192.168.31.90/slam/map.pgm", "C:/Users/wugua/AppData/Local/WGD/GridmapEditor/cache/map.pgm");
    connect(downloader, &FtpDownloader::finished, [=](bool success){
        qDebug()<<"下载成功？"<<success;
    });
}
