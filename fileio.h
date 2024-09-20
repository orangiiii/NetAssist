#ifndef FILEIO_H
#define FILEIO_H

#include <QWidget>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QLabel>
#include "define.h"

namespace Ui {
class FileIO;
}

class FileIO : public QWidget  // 确保 FileIO 继承自 QWidget
{
    Q_OBJECT

public:
    explicit FileIO(QWidget *parent = nullptr);
    ~FileIO();

    explicit FileIO(QObject *parent = nullptr);

    // 打开文件并读取内容
    // QString readFile(const QString &filePath);

    // 写入内容到文件
    bool writeFile(const QString &filePath, const QString &content);

    // 追加内容到文件
    bool appendToFile(const QString &filePath, const QString &content);


    // QString readFile(const QString &filePath, QLabel &label);

    int readFile(const QString &filePath, QLabel &label);
private:
    Ui::FileIO *ui;
};

#endif // FILEIO_H
