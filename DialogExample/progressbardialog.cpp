#pragma execution_character_set("utf-8")
#include "progressbardialog.h"
#include <QVBoxLayout>

ProgressBarDialog::ProgressBarDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedSize(QSize(400, 80));

    this->setModal(true);

    _progressBar = new QProgressBar(this);
    _progressBar->setTextVisible(false);

    _label = new QLabel(this);
    _label->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(_progressBar);
    layout->addWidget(_label);

    this->setLayout(layout);
}

void ProgressBarDialog::start(QString str)
{
    _progressBar->setValue(0);

    _label->setText(str);

    this->show();
}

void ProgressBarDialog::setValue(int value)
{
    _progressBar->setValue(value);
}

void ProgressBarDialog::hide()
{
    this->accept();
}
