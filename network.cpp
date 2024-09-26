#include "network.h"

Network::Network(QWidget *parent)
    : QWidget(parent){

    tcpclient = new QTcpSocket(this);
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
}

Network::~Network()
{
}

void Network::readData(const QByteArray buffer){
    QByteArray type=buffer.mid(0,1);

    QByteArray data;
    data = buffer.mid(1);
    if(TYPE_TEXT==type){

        QString recstr= QString::fromUtf8(data);
        emit dataReceived(recstr);
    }else if(TYPE_IMAGE==type){
        if(data.size()  < sizeof(qint64)){
            return;
        }

        QByteArray sizeData = data.mid(0,sizeof(qint64));
        QDataStream sizeStream(&sizeData, QIODevice::ReadOnly);
        sizeStream.setByteOrder(QDataStream::LittleEndian);  // 显式设置为小端序
        qint64 imageSize ;
        sizeStream >> imageSize;

        qDebug()<<(imageSize);
        qDebug() << "Expected image size:" << imageSize;

        if(data.size()<sizeof(qint64)+imageSize){
            qDebug() << "Not enough data received to load the image.";
            return;
        }

        // 接收图片数据
        QByteArray imageData = data.mid(sizeof(qint64),imageSize);

        // 将字节数组转换为 QPixmap
        QPixmap pixmap;
        pixmap.loadFromData(imageData, "PNG");  // 假设图片是以 PNG 格式发送的

        // 在 QLabel 中显示图片
        if (!pixmap.isNull()) {
            emit pictureReceived(imageData);  // 发送信号，通知图片已接收
        } else {
            qDebug() << "Failed to load the image from data.";
        }

    }

}
void Network::readTcpData(){
    // 首先检查是否有可用的数据
    if (tcpclient->bytesAvailable() < 1) {
        return;
    }
    QByteArray buffer = tcpclient->readAll();
    readData(buffer);
}
void Network::readUdpData(){

    QByteArray buffer;
    if (udpSocket->hasPendingDatagrams()) {
        qint64 datagramSize = udpSocket->pendingDatagramSize();
        qDebug() << "datagram size: " << datagramSize;
        buffer.resize(datagramSize);

        udpSocket->readDatagram(buffer.data(), buffer.size());
        qDebug() << "Received data:" << buffer;

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
    }else {
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


    connect(newClientSocket, &QTcpSocket::readyRead, this, [this,newClientSocket]() {
        // 首先检查是否有可用的数据
        if (newClientSocket->bytesAvailable() > 1) {
            QByteArray data = newClientSocket->readAll();
            qDebug() << "data received from client " ;

            emit readMessage(data);
        }
    });

    // 当客户端断开连接时，清理资源
    connect(newClientSocket, &QTcpSocket::disconnected, newClientSocket, [this,newClientSocket](){
        qDebug() << "Client disconnected!";
        clientSockets.removeOne(newClientSocket);  // 从列表中移除断开连接的客户端
        newClientSocket->deleteLater();  // 清理 socket 对象
    });

}

void Network::send(bool ifSendButton,int mode,QString ip,quint16 port,QByteArray data)

{

    if(!ifSendButton){
        qDebug()<<"没连";
        return;
    }


    // 当前连接的客户端的 QTcpSocket
    if(mode==TCP_SERVER_MODE){
        for (QTcpSocket *clientSocket:clientSockets) {
            if(clientSocket->isOpen()){
                clientSocket->write(data);
                clientSocket->flush();  // 确保数据立即发送

                qDebug() << "data send from server " ;
            }else {
                qDebug() << "socket is closed.";
            }

        }
    }else if(mode==TCP_CLIENT_MODE){
        tcpclient->write(data);
        tcpclient->flush();
        qDebug() << "data send from client " ;
    }else if(mode==UDP_MODE){
        // 发送数据
        // 这里的ip端口 组播,后期再改
        udpSocket->writeDatagram(data,QHostAddress::Broadcast,port);
        // debug
        qDebug() << "data sent(udp): " << data.toStdString();
    }


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

QByteArray Network::getPicData(QPixmap* pixmap){

    // 将Pixmap保存为字节数组（QByteArray）
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap->save(&buffer, "PNG"); // 将图像保存为PNG格式到字节数组中

    // 获取数据大小
    qint64 byteArraySize = byteArray.size();

    qDebug()<<(sizeof(qint64));
    // 发送数据大小，方便接收端知道要接收多少数据
    QByteArray sizeData;
    QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
    sizeStream.setByteOrder(QDataStream::LittleEndian);  // 显式设置为小端序
    sizeStream << byteArraySize;

    qDebug()<<(byteArraySize);
    QByteArray finalData;
    finalData.append(TYPE_IMAGE);
    finalData.append(sizeData);
    finalData.append(byteArray);

    return finalData;
}
