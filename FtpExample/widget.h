#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "ftpdownloader.h"
#include "ftpuploader.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btn_download_clicked();

    void on_btn_upload_clicked();

private:
    Ui::Widget *ui;
    FtpDownloader *_downloader;
    FtpUploader *_uploader;
};

#endif // WIDGET_H
