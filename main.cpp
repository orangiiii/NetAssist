
#include <QApplication>
#include <QFile>
#include <QtDebug>
#include <mainwindow.h>
#include "DrawWidget.h"

int main(int argc,char *argv[]){

    // QCoreApplication a(argc, argv);
    // QString filePath = "C:\\Users\\JOY\\Desktop\\a.txt";
    // QFile qfs(filePath);
    QApplication a(argc, argv);
    // // 创建自定义绘制窗口
    // DrawWidget window;
    // window.setWindowTitle("Qt 图形绘制示例");
    // window.resize(400, 500);
    // window.show();

    MainWindow w;
    w.show();
    return a.exec();
}
