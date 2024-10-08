#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>
#include <QtCore>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QPixmap>
#include <QNetworkInterface>

#include <compressor.h>
#include "define.h"

namespace Ui {
class Network;
}
// class MainWindow;  // 前向声明

class Network : public QWidget
{
    Q_OBJECT

public:
    explicit Network(QWidget *parent = nullptr);
    ~Network();

    // void readData(QByteArray dataBuffer);
    void sendData(QString ip, quint16 port, QString data);
    // void startClientConnection(QString ip, quint16 port);
    // void startServerConnection(QString ip, quint16 port);
    bool startUdpConnection(QString ip, quint16 port);
    void onNewConnection();
    // void send(bool ifSendButton, int mode);
    void send(bool ifSendButton, int mode, QString ip, quint16 port);
    void send(bool ifSendButton, int mode, QString ip, quint16 port, QByteArray data);
    void openConnection(int mode, QString ip, quint16 port,bool *ifSendButton);
    bool startServerConnection(QString ip, quint16 port);
    bool startClientConnection(QString ip, quint16 port);
    void sendUdpData(QString ip, quint16 port, QString data);
    void sendData(QString ip, quint16 port, QString data, int mode);
    void readTcpData();
    void readUdpData();
    void sendPicture(QPixmap *pixmap);
    void readPicture();
    // void setNetwork(Network* networkInstance);
    QByteArray getPicData(QPixmap *pixmap);

    void setIfConnected(bool ifConnected){
        this->ifConnected=ifConnected;
    };
    bool getIfConnected(){
        return ifConnected;
    };
    bool socketValid(QString ip, quint16 port);
    void startUdpServer(quint16 port);
    void readUdpData2();
    QHostAddress getLocalIPAddress();
    void setUserName(QString username);
    bool addUserName(QString username);
public slots:
    void readData(const QByteArray dataBuffer);
private:
    Ui::Network *ui;
    QList<QTcpSocket*> clientSockets;
    QTcpSocket *tcpclient;
    QTcpSocket *clientSocket;
    QTcpServer *tcpServer;  // 作为类的成员变量，声明为指针
    // ClientWin *clientWin;
    QUdpSocket *udpSocket;
    // MainWindow* mainWindow;

    qint64 storedImageSize = 0;
    // Network* network;
    QByteArray accumulatedData;
    QString type;
    // QTcpSocket* tcpSocket;
    bool ifFirst=true;

    bool ifConnected=false;
    QString username;


signals:
    void readMessage(const QByteArray& data);
    void dataReceived(const QString& data);
    void pictureReceived(QByteArray &pixmap);

};

#endif // NETWORK_H
