#include "widget.h"
#include <QApplication>
#include <QList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int downloaderNum = 1;
    QList<Widget *> widgets;
    for(int i = 0; i < downloaderNum; i++){
        widgets.append(new Widget());
        widgets[i]->show();
    }

    return a.exec();
}
