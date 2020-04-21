#ifndef FTPUPLOADER_H
#define FTPUPLOADER_H

#include <QObject>
#include <QFile>
#include <QNetworkReply>

class FtpUploader : public QObject
{
    Q_OBJECT
public:
    explicit FtpUploader(QObject *parent = nullptr);
    bool startUpload(QString url, QString path);

signals:
    void finished(bool success);
    void progress(int progress); //0~100

private slots:
    void finishedSlot();
    void uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal);
    void errorSlot(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;
    QFile _file;
    bool _busy = false; //上传器是否正忙
};

#endif // FTPUPLOADER_H
