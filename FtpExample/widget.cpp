#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
 

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    _downloader = new FtpDownloader(this);
    _uploader = new FtpUploader(this);

    connect(_downloader, &FtpDownloader::finished, [=](bool success){
        if(success){
            QMessageBox::information(this, "提示", "下载成功!", "确定");
        }
        else{
            QMessageBox::critical(this, "提示", "下载失败!", "确定");
        }

        ui->progressBar->setValue(0);
    });

    connect(_uploader, &FtpUploader::finished, [=](bool success){
        if(success){
            QMessageBox::information(this, "提示", "上传成功!", "确定");
        }
        else{
            QMessageBox::critical(this, "提示", "上传失败!", "确定");
        }

        ui->progressBar->setValue(0);
    });

    connect(_downloader, &FtpDownloader::progress, [=](int progress){
        ui->progressBar->setValue(progress);
    });

    connect(_uploader, &FtpUploader::progress, [=](int progress){
        ui->progressBar->setValue(progress);
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btn_download_clicked()
{
    QString url = ui->le_url->text();
    QString path = ui->le_path->text();
    if(!_downloader->startDownload(url, path)){
        qDebug()<<"开始下载失败！";
    };
}

void Widget::on_btn_upload_clicked()
{
    QString url = ui->le_url->text();
    QString path = ui->le_path->text();
    if(!_uploader->startUpload(url, path)){
        qDebug()<<"开始上传失败！";
    }
}
