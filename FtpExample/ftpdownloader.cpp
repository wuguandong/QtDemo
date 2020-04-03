#pragma execution_character_set("utf-8")
#include "ftpdownloader.h"
#include <QNetworkAccessManager>
#include <QDebug>

FtpDownloader::FtpDownloader(QString url, QString path, QObject *parent) : QObject(parent), lastFileSize(0)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &FtpDownloader::finishedSlot);
    this->reply = manager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::readyRead, this, &FtpDownloader::readyReadSlot);
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &FtpDownloader::errorSlot);

    this->file.setFileName(path);
    file.open(QIODevice::WriteOnly);

    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &FtpDownloader::timeoutSlot);
    timer->start(10 * 1000); //10s文件大小不变判断为超时
}

void FtpDownloader::finishedSlot(QNetworkReply *)
{
    this->timer->stop(); //停止定时器
    file.close();
    this->reply->deleteLater();
    this->deleteLater();

    //通过文件大小判断下载成功失败
    if(file.size() > 0){
        emit finished(true);
    }
    else{
        emit finished(false);
        file.remove();
    }
}

void FtpDownloader::readyReadSlot()
{
    file.write(reply->readAll());

    qDebug()<<file.size() / 1024;
}

void FtpDownloader::errorSlot()
{
    if(file.size() > 0){
        file.resize(0); //下载一半出错则将文件大小置为0
    }
}

void FtpDownloader::timeoutSlot()
{
    qint64 currentFileSize = this->file.size();
    if(currentFileSize == this->lastFileSize){
        this->reply->abort(); //中断下载
    }
    else{
        this->lastFileSize = currentFileSize;
    }
}
