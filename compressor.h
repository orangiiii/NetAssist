#ifndef COMPRESSOR_H
#define COMPRESSOR_H
#include <QByteArray>

class Compressor
{
public:
    Compressor();

    static QByteArray compressData(const QByteArray &data);
    static QByteArray decompressData(const QByteArray &compressedData);

    static QString compressStrData(const QString &data);
    static QString decompressStrData(const QString &compressedData);
};

#endif // COMPRESSOR_H
