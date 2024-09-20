#include "network.h"
#include "ui_network.h"

Network::Network(QWidget *parent)
    : QWidget(parent){

    tcpclient = new QTcpSocket(this);


    // cpp中实例化对象, .h种添加私有成员对象
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
    // clientSocket  = new QTcpSocket(this);
}
//     , ui(new Ui::Network)
// {
//     ui->setupUi(this);
// }

Network::~Network()
{
    delete ui;
}
void Network::readTcpData(){
    QByteArray buffer;
    if(tcpclient->bytesAvailable()>0){

        buffer = tcpclient->readAll();
    }


    QString recstr= QString::fromUtf8(buffer);
    emit dataReceived(recstr);
    // this->ui->data->append("getMsg:");
    // this->ui->data->append(recstr);


}void Network::readUdpData(){
    QByteArray buffer;
    if (udpSocket->hasPendingDatagrams()) {
        qint64 datagramSize = udpSocket->pendingDatagramSize();
        qDebug() << "datagram size: " << datagramSize;
        buffer.resize(datagramSize);
        udpSocket->readDatagram(buffer.data(), buffer.size());
        qDebug() << "Received data: " << buffer;

        // buffer = udpSocket->readAll();
        QString recstr= QString::fromUtf8(buffer);
        emit dataReceived(recstr);

        //由于udp是无连接协议，数据包没有顺序保证，readAll() 可能会丢失多个数据报之间的边界，导致数据不完整或拼接在一起。
        //readAll主要用于tcp连接,在udp中，readAll() 读取的是当前所有可用的数据，而不是一个完整的udp数据包。
    }


    QString recstr= QString::fromUtf8(buffer);;
    // ui->data->append("getMsg:");
    // ui->data->append(recstr);


}

bool Network::startClientConnection(    QString ip,
                                       quint16 port){
    // // 会不会太晚?
    // if(!tcpclient){
    //     // cpp中实例化对象, .h种添加私有成员对象
    //     tcpclient = new QTcpSocket(this);
    // }
    //未连接状态  去连接
    if(tcpclient->state() != QAbstractSocket::ConnectedState) {
        //连接服务器
        tcpclient->connectToHost(ip,port);
        //等待连接成功
        if(!tcpclient->waitForConnected(30000)) {
            qDebug() << "Connection failed!";
            return false;
        }
        // connect
        connect(tcpclient,&QTcpSocket::readyRead,this,&Network::readTcpData);

        return true;
    }
    //已连接状态 去断开
    else {
        //断开服务器
        tcpclient->disconnectFromHost();
    }
    return false;
}
bool Network::startServerConnection(QString ip,
                                       quint16 port){
    // if(!tcpServer){
    //     tcpServer = new QTcpServer(this);
    // }
    // 检查服务器是否在监听
    if (!tcpServer->isListening()) {
        QHostAddress serverAddress;
        if (!serverAddress.setAddress(ip)) {
            QMessageBox::information(this, tr("错误"), tr("无效的IP地址"));
            return false;
        }

        // 服务器开始监听
        if (tcpServer->listen(serverAddress, port)) {
            qDebug() << "Server is listening on " << ip << ":" << port;

            // 连接信号槽，用于处理新客户端连接
            connect(tcpServer, &QTcpServer::newConnection, this, &Network::onNewConnection);
            return true;

        } else {
            QMessageBox::information(this, tr("错误"), tr("无法启动服务器！"));
            return false;
        }
    } else {
        // 停止服务器监听
        tcpServer->close();
        qDebug() << "Server stopped!";
        return false;
    }
}
bool Network::startUdpConnection(QString ip,
                                    quint16 port){
    if(!udpSocket->isOpen()){
        QHostAddress serverAddr;
        if(!serverAddr.setAddress(ip)){
            QMessageBox::information(this,tr("error"),tr("无效的ip地址"));
        }

        if(udpSocket->bind(serverAddr,port)){
            qDebug()<<"udp socket bind to "
                     <<ip<<":"<<port;
            connect(udpSocket,&QUdpSocket::readyRead,this,&Network::readUdpData);
            return true;
        }else{
            QMessageBox::information(this,tr("error"),tr("无法绑定udpsocket"));
        }

    }else{
        udpSocket->close();
        qDebug()<<"udp connection closed.";
        return false;
    }
    return true;

}
void Network::onNewConnection(){
    QTcpSocket *newClientSocket = tcpServer->nextPendingConnection();
    clientSockets.append(newClientSocket);
    qDebug() << "New client connected!";

    // 当客户端发送数据时，处理数据
    // 可能要改一下
    connect(newClientSocket, &QTcpSocket::readyRead, this, [this,newClientSocket]() {
        QByteArray data = newClientSocket->readAll();
        qDebug() << "data received from client: " << data;
        QString recstr = QString::fromUtf8(data);
        emit dataReceived(recstr);
    });

    // 当客户端断开连接时，清理资源
    connect(newClientSocket, &QTcpSocket::disconnected, newClientSocket, [this,newClientSocket](){
        qDebug() << "Client disconnected!";
        clientSockets.removeOne(newClientSocket);  // 从列表中移除断开连接的客户端
        newClientSocket->deleteLater();  // 清理 socket 对象
    });

}

void Network::send(bool ifSendButton,int mode,QString ip,quint16 port,QString data)

{

    if(!ifSendButton){
        qDebug()<<"没连";
        return;
    }

    QByteArray buffer = data.toUtf8();
    // 当前连接的客户端的 QTcpSocket
    if(mode==TCP_SERVER_MODE){
        for (QTcpSocket *clientSocket:clientSockets) {
            if(clientSocket->isOpen()){
                clientSocket->write(buffer);
                clientSocket->flush();  // 确保数据立即发送
            }else {
                qDebug() << "socket is closed.";
            }

        }
    }else if(mode==TCP_CLIENT_MODE){
        tcpclient->write(buffer);
        tcpclient->flush();
    }else if(mode==UDP_MODE){
        // 发送数据
        // 这里的ip端口 组播,后期再改
        // udpSocket->writeDatagram(buffer,QHostAddress::Broadcast,port);
        udpSocket->writeDatagram(buffer,QHostAddress::Broadcast,port);
        // debug
        qDebug() << "data sent(udp): " << data;
    }


    // debug
    qDebug() << "data sent from server to client: " << data;
}

void Network::openConnection(int mode,QString ip,quint16 port,bool *ifSendButton)

{
    if(mode==UDP_MODE){
        qDebug()<<"UDP_MODE";
        *ifSendButton = startUdpConnection(ip,port);
    }else if(mode==TCP_CLIENT_MODE){
        qDebug()<<"TCP_CLIENT_MODE";
        *ifSendButton =startClientConnection(ip,port);
    }else if(mode==TCP_SERVER_MODE){
        qDebug()<<"TCP_SERVER_MODE";
        *ifSendButton =startServerConnection(ip,port);
    }

}
