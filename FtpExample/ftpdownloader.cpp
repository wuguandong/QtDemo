#pragma execution_character_set("utf-8")
#include "ftpdownloader.h"
#include <QNetworkAccessManager>
#include <QDir>
#include <QMetaEnum>
#include <QDebug>

FtpDownloader::FtpDownloader(QObject *parent): QObject(parent), _lastFileSize(0)
{
    this->_manager = new QNetworkAccessManager(this);
    connect(_manager, &QNetworkAccessManager::finished, this, &FtpDownloader::finishedSlot);

    this->_timer = new QTimer(this);
    connect(this->_timer, &QTimer::timeout, this, &FtpDownloader::timeoutSlot);

    connect(this, &FtpDownloader::finished, [=]{_busy = false;});
}

bool FtpDownloader::startDownload(QString url, QString path)
{
    //检查下载器是否正忙
    if(_busy){
        qDebug()<<"FTP下载器忙碌中!";
        return false;
    }
    else{
        _busy = true;
    }

    //检查Url正确性
    QUrl qurl(url);
    if(!qurl.isValid()) return false;
    if(qurl.scheme() != "ftp") return false;
    if(qurl.path().isEmpty() || qurl.path()=="/") return false;

    //如果下载路径的目录不存在 则创建目录
    QDir dir(path.left(path.lastIndexOf('/'))); //路径去掉最后的文件名
    if(!dir.exists()){
        if(!dir.mkpath(dir.path())) return false;
    }

    //打开文件
    this->_file.setFileName(path);
    if(!_file.open(QIODevice::WriteOnly)){
        qDebug()<<"文件打开失败";
        return false;
    }

    this->_reply = _manager->get(QNetworkRequest(qurl));
    connect(_reply, &QNetworkReply::readyRead, this, &FtpDownloader::readyReadSlot);
    connect(_reply, &QNetworkReply::downloadProgress, this, &FtpDownloader::downloadProgressSlot);
    connect(_reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &FtpDownloader::errorSlot);

    _timer->start(10 * 1000); //10s文件大小不变判断为超时

    return true;
}

void FtpDownloader::finishedSlot(QNetworkReply *)
{
    if(_timer->isActive()) this->_timer->stop(); //停止定时器

    _file.waitForBytesWritten(5 * 1000); //等待文件写入结束（5秒钟）
    _file.close();

    this->_reply->deleteLater();
    this->_reply = nullptr;

    emit finished(true);
}

void FtpDownloader::readyReadSlot()
{
    _file.write(_reply->readAll());
}

void FtpDownloader::downloadProgressSlot(qint64 bytesReceived, qint64 bytesTotal)
{
    emit progress(static_cast<int>(100 * bytesReceived / bytesTotal));
}

void FtpDownloader::errorSlot(QNetworkReply::NetworkError error)
{
    //打印错误信息
    QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    const char *errStr = metaEnum.valueToKey(error);
    qDebug()<<"文件下载error: " + QString(errStr);

    _file.close();
    _reply->deleteLater();
    _reply = nullptr;

    emit finished(false);
}

void FtpDownloader::timeoutSlot()
{
    qint64 currentFileSize = this->_file.size();
    if(currentFileSize == this->_lastFileSize){
        emit _reply->error(QNetworkReply::TimeoutError); //主动发出超时信号
    }
    else{
        this->_lastFileSize = currentFileSize;
    }
}
