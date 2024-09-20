
#include <QApplication>
#include <QFile>
#include <QtDebug>
#include <mainwindow.h>

int main(int argc,char *argv[]){

    // QCoreApplication a(argc, argv);
    // QString filePath = "C:\\Users\\JOY\\Desktop\\a.txt";
    // QFile qfs(filePath);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
