#pragma execution_character_set("utf-8")
#include "ftpuploader.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QDebug>

FtpUploader::FtpUploader(QString url, QString path, QObject *parent) : QObject(parent)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) return; //打开文件，如果打开失败则返回
    QByteArray data = file.readAll();
    file.close();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &FtpUploader::finishedSlot);
    this->reply = manager->put(QNetworkRequest(QUrl(url)), data);
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &FtpUploader::errorSlot);
}

void FtpUploader::finishedSlot()
{
    qDebug()<<"结束";
}

void FtpUploader::errorSlot()
{
    qDebug()<<"错误";
}
