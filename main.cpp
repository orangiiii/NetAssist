
#include <QApplication>
#include <QFile>
#include <QtDebug>
#include <mainwindow.h>
#include <compressor.h>

int main(int argc,char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;
    // w.show();
    QString after = Compressor::compressStrData("d栋asddaksjdjwdwd");
    QString before =Compressor::decompressStrData(after);
    return a.exec();
}
