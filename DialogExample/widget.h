#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include "progressbardialog.h"

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
    void on_pb_progressBar_clicked();

private:
    Ui::Widget *ui;
    ProgressBarDialog *_progressBar;
    int _progressValue;
};

#endif // WIDGET_H
