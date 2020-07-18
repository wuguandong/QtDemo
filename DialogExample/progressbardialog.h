#ifndef PROGRESSBARDIALOG_H
#define PROGRESSBARDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>

class ProgressBarDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProgressBarDialog(QWidget *parent = nullptr);
    void start(QString str);
    void setValue(int value);
    void hide();
private:
    QProgressBar *_progressBar;
    QLabel *_label;
};

#endif // PROGRESSBARDIALOG_H
