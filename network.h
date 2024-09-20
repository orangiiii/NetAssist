#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>
#include <QtCore>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>



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

    void readData();
    void sendData(QString ip, quint16 port, QString data);
    // void startClientConnection(QString ip, quint16 port);
    // void startServerConnection(QString ip, quint16 port);
    bool startUdpConnection(QString ip, quint16 port);
    void onNewConnection();
    // void send(bool ifSendButton, int mode);
    void send(bool ifSendButton, int mode, QString ip, quint16 port);
    void send(bool ifSendButton, int mode, QString ip, quint16 port, QString data);
    void openConnection(int mode, QString ip, quint16 port,bool *ifSendButton);
    bool startServerConnection(QString ip, quint16 port);
    bool startClientConnection(QString ip, quint16 port);
    void sendUdpData(QString ip, quint16 port, QString data);
    void sendData(QString ip, quint16 port, QString data, int mode);
    void readTcpData();
    void readUdpData();
private:
    Ui::Network *ui;
    QList<QTcpSocket*> clientSockets;
    QTcpSocket *tcpclient;
    QTcpSocket *clientSocket;
    QTcpServer *tcpServer;  // 作为类的成员变量，声明为指针
    // ClientWin *clientWin;
    QUdpSocket *udpSocket;
    // MainWindow* mainWindow;

signals:
    void dataReceived(const QString& data);

};

#endif // NETWORK_H
