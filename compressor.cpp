#include "compressor.h"
#include <QByteArray>
#include <QDebug>


Compressor::Compressor() {}


// 压缩数据
QByteArray Compressor::compressData(const QByteArray &data) {
    qDebug()<<"before compress: " + data.size();
    QByteArray compressedData = qCompress(data);

    qDebug()<<"after compress: " + compressedData.length();
    if (!compressedData.isEmpty()) {
        qDebug() << "Data compressed successfully.";
    } else {
        qDebug() << "Failed to compress data.";
    }
    return compressedData;
}

// 解压缩数据
QByteArray Compressor::decompressData(const QByteArray &compressedData) {

    qDebug()<<"before decompress: " + compressedData.size();
    QByteArray decompressedData = qUncompress(compressedData);
    qDebug()<<"after decompress: " + decompressedData.size();
    if (!decompressedData.isEmpty()) {
        qDebug() << "Data decompressed successfully.";
    } else {
        qDebug() << "Failed to decompress data.";
    }
    return decompressedData;
}
// 压缩数据
QString Compressor::compressStrData(const QString &data) {
    QByteArray compressedData = compressData(data.toUtf8());
    return QString::fromUtf8(compressedData);
}

// 解压缩数据
QString Compressor::decompressStrData(const QString &compressedData) {
    QByteArray decompressedData = decompressData(compressedData.toUtf8());
    return QString::fromUtf8(decompressedData);
}

