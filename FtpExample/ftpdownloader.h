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
    explicit FtpDownloader(QString url, QString path, QObject *parent = nullptr);

private slots:
    void finishedSlot(QNetworkReply *);
    void readyReadSlot();
    void errorSlot();
    void timeoutSlot();

signals:
    void finished(bool success);

private:
    QNetworkReply *reply;
    QFile file;
    QTimer *timer;
    qint64 lastFileSize;
};

#endif // FTPDOWNLOADER_H
