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

int FileIO::readFile(const QString &filePath,QLabel& label){
    if(filePath.endsWith("txt",Qt::CaseInsensitive)){
        QFile file(filePath);
        QString fileContent;
        if(!file.exists()){
            qDebug()<<"not exist.";

            label.setText("not exist.");
        }
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug()<<"can not open";
            label.setText("can not open");
        }

        QTextStream in(&file);
        fileContent = in.readAll().toUtf8();
        file.close();
        label.setText(fileContent);
        return READ_TEXT;
    }else if(filePath.endsWith(".jpg",Qt::CaseInsensitive)||
               filePath.endsWith(".png",Qt::CaseInsensitive )||
                filePath.endsWith(".bmp",Qt::CaseInsensitive)){
        QPixmap pixmap(filePath);
        if(!pixmap.isNull()){
            label.setPixmap(pixmap);
            return READ_IMAGE;
        }else{
            label.setText("can not load the picture");
        }

    }else{
        label.setText("can not support the type");
    }
    return READ_ERROR;


}




// 写入内容到文件
bool FileIO::writeFile(const QString &content){

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                    tr("Image Files (*.png *.jpg *.bmp);;All Files (*)"));

    if(filePath.isEmpty()){
        QMessageBox::information(this, tr("Error"), tr("filepath is empty"));
        return false;
    }
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
        QMessageBox::information(this, tr("Success"), tr("Write file successfully."));
        return true;
    }else{
        QMessageBox::information(this, tr("Error"), tr("can not support the type"));
        return false;
    }
};

// 写入内容到文件
bool FileIO::writeImageFile(QPixmap  &pixmap){

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                    tr("Image Files (*.png *.jpg *.bmp);;All Files (*)"));

    if(filePath.isEmpty()){
        QMessageBox::information(this, tr("Error"), tr("filepath is empty"));
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
