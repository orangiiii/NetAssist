#include "fileio.h"
// #include "ui_fileio.h"

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <qDebug>

FileIO::FileIO(QWidget *parent) :
    QWidget(parent){}
//     ui(new Ui::FileIO)
// {
//     // ui->setupUi(this);
// }

FileIO::~FileIO()
{
    // delete ui;
}

QByteArray FileIO::readFile(const QString &filePath){
    if(filePath.endsWith("txt",Qt::CaseInsensitive)){
        QFile file(filePath);
        QByteArray fileContent;
        if(!file.exists()){
            QMessageBox::information(this, tr("Error"), tr("not exist."));
        }
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::information(this, tr("Error"), tr("can not open"));
        }
        QTextStream in(&file);
        fileContent = in.readAll().toLatin1();
        file.close();
        return fileContent;
    }else if(filePath.endsWith(".jpg",Qt::CaseInsensitive)||
               filePath.endsWith(".png",Qt::CaseInsensitive )||
                filePath.endsWith(".bmp",Qt::CaseInsensitive)){
        QByteArray returnType("returnType");
        return returnType;
    }else{
        QMessageBox::information(this,tr("Error"),tr("can not support the type"));
    }
    return READ_ERROR;


}

int FileIO::getFileType(const QString &filePath){
    if(filePath.endsWith("txt",Qt::CaseInsensitive)){
        return READ_TEXT;
    }else if(filePath.endsWith(".jpg",Qt::CaseInsensitive)||
               filePath.endsWith(".png",Qt::CaseInsensitive )||
               filePath.endsWith(".bmp",Qt::CaseInsensitive)){
        return READ_IMAGE;

    }else{
        QMessageBox::information(this, tr("Error"), tr("can not support the type"));
        return READ_ERROR;
    }


}




// 写入内容到文件
bool FileIO::writeFile(const QString &content){

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                    tr("Image Files (*.png *.jpg *.bmp);;All Files (*)"));

    if(filePath.endsWith("txt",Qt::CaseInsensitive)){
        QFile file(filePath);

        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){

            QMessageBox::information(this, tr("Error"), tr("can not open"));
            return false;
        }

        // 写入
        QTextStream out (&file);
        out<<content;
        file.close();
        qDebug()<<("Write file successfully.");
        return true;
    }else{

        qDebug()<<("can not support the type");
        return false;
    }
};

// 写入内容到文件
bool FileIO::writeImageFile(QPixmap  &pixmap){

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                    tr("Image Files (*.png *.jpg *.bmp);;All Files (*)"));

    if(filePath.isEmpty()){
        qDebug()<<("filepath is empty");
        return false;
    }

    if(filePath.endsWith(".jpg",Qt::CaseInsensitive)||
              filePath.endsWith(".png",Qt::CaseInsensitive )||
              filePath.endsWith(".bmp",Qt::CaseInsensitive)){
        if (!pixmap.save(filePath)) {
            QMessageBox::information(this, tr("Error"), tr("Failed to save image"));
            return false;
        }
        QMessageBox::information(this, tr("Success"), tr("Write file successfully."));
        return true;

    }else{
        QMessageBox::information(this, tr("Error"), tr("can not support the type"));
        return false;
    }
};

// 追加内容到文件
bool FileIO::appendToFile(const QString &filePath, const QString &content){
    QFile file(filePath);
    if(!file.open(QIODevice::Append | QIODevice::Text)){
        qDebug()<<"can not open";
        return false;
    }

    // 写入
    QTextStream out (&file);
    out<<content;
    file.close();
    return true;
};
