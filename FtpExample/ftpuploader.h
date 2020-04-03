#ifndef FTPUPLOADER_H
#define FTPUPLOADER_H

#include <QObject>
#include <QNetworkReply>

class FtpUploader : public QObject
{
    Q_OBJECT
public:
    explicit FtpUploader(QString url, QString path, QObject *parent = nullptr);

private slots:
    void finishedSlot();
    void errorSlot();

private:
    QNetworkReply *reply;

};

#endif // FTPUPLOADER_H
