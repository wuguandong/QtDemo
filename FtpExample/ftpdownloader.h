#ifndef FTPDOWNLOADER_H
#define FTPDOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>

class FtpDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FtpDownloader(QObject *parent = nullptr);
    bool startDownload(QString url, QString path);

signals:
    void finished(bool success);
    void progress(int progress); //0~100

private slots:
    void finishedSlot(QNetworkReply *reply);
    void readyReadSlot();
    void downloadProgressSlot(qint64 bytesReceived, qint64 bytesTotal);
    void errorSlot(QNetworkReply::NetworkError error);
    void timeoutSlot();

private:
    QNetworkAccessManager *_manager;
    QNetworkReply *_reply;
    QFile _file;
    QTimer *_timer;
    qint64 _lastFileSize;
    bool _busy = false; //下载器是否正忙
};

#endif // FTPDOWNLOADER_H
