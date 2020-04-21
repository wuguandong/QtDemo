#pragma execution_character_set("utf-8")
#include "ftpuploader.h"
#include <QNetworkAccessManager>
#include <QMetaEnum>
#include <QDebug>

FtpUploader::FtpUploader(QObject *parent) : QObject(parent)
{
    _manager = new QNetworkAccessManager(this);
    connect(_manager, &QNetworkAccessManager::finished, this, &FtpUploader::finishedSlot);

    connect(this, &FtpUploader::finished, [=]{_busy = false;});
}

bool FtpUploader::startUpload(QString url, QString path)
{
    //检查上传器是否正忙
    if(_busy){
        qDebug()<<"FTP上传器忙碌中!";
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

    //打开文件
    this->_file.setFileName(path);
    if(!_file.open(QIODevice::ReadOnly)){
        qDebug()<<"文件打开失败";
        return false;
    }
    QByteArray data = _file.readAll();
    _file.close();

    this->_reply = _manager->put(QNetworkRequest(QUrl(url)), data);
    connect(_reply, &QNetworkReply::uploadProgress, this, &FtpUploader::uploadProgressSlot);
    connect(_reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &FtpUploader::errorSlot);

    return true;
}

void FtpUploader::finishedSlot()
{
    this->_reply->deleteLater();
    this->_reply = nullptr;

    emit finished(true);
}

void FtpUploader::uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal)
{
    emit progress(static_cast<int>(100 * bytesSent / bytesTotal));
}

void FtpUploader::errorSlot(QNetworkReply::NetworkError error)
{
    //打印错误信息
    QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    const char *errStr = metaEnum.valueToKey(error);
    qDebug()<<"文件下载error: " + QString(errStr);

    _reply->deleteLater();
    _reply = nullptr;

    emit finished(false);
}
