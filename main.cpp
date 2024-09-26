
#include <QApplication>
#include <QFile>
#include <QtDebug>
#include <mainwindow.h>
#include "DrawWidget.h"

int main(int argc,char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
