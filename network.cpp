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
    // static QByteArray accumulatedData; // 用于累积接收到的分包数据
    if(ifFirst){
        type=buffer.mid(0,1);
        accumulatedData.append(buffer.mid(1));
    }else{
        accumulatedData.append(buffer);
    }

    if(TYPE_TEXT==type){
        QString recstr = QString::fromUtf8(accumulatedData);
        emit dataReceived(recstr);
        accumulatedData.clear();  // 清空累积数据
    }else if(TYPE_IMAGE==type){


        if(ifFirst){
            if(accumulatedData.size()  < sizeof(qint64)){
                ifFirst=false;
                return;
            }
            QByteArray sizeData = accumulatedData.mid(0,sizeof(qint64));
            QDataStream sizeStream(&sizeData, QIODevice::ReadOnly);
            sizeStream.setByteOrder(QDataStream::LittleEndian);  // 显式设置为小端序
            qint64 imageSize ;
            sizeStream >> imageSize;
            qDebug()<< imageSize ;
            storedImageSize = imageSize;  // 保存图像大小
            qDebug() << "Expected image size:" << imageSize;

            if((accumulatedData.size()<sizeof(qint64)+imageSize)){
                qDebug() << "Not enough data received to load the image.";
                ifFirst=false;
                return;
            }
            QByteArray imageData = accumulatedData.mid(sizeof(qint64), imageSize);

            QPixmap pixmap;
            pixmap.loadFromData(imageData, "PNG");

            if (!pixmap.isNull()) {
                emit pictureReceived(imageData);
            } else {
                qDebug() << "Failed to load the image from data.";
            }

            accumulatedData = accumulatedData.mid(imageSize+sizeof(qint64));
            ifFirst = true;
        }else{
            qint64 imageSize=storedImageSize;
            if(accumulatedData.size()<imageSize+sizeof(qint64)){
                qDebug()<<"waiting for data";
                return;
            }
            // 提取图像数据
            QByteArray imageData = accumulatedData.mid(sizeof(qint64), imageSize);
            QPixmap pixmap;
            pixmap.loadFromData(imageData, "PNG");

            if (!pixmap.isNull()) {
                emit pictureReceived(imageData);
            } else {
                qDebug() << "Failed to load the image from data.";
            }

            accumulatedData = accumulatedData.mid(imageSize+sizeof(qint64));
            ifFirst = true;
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

        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        qDebug() << "Received data:" << buffer;

        QByteArray nameSize = buffer.mid(0,sizeof(qint64));
        QDataStream stream(nameSize);
        qint64 userNameSize;
        stream>>userNameSize;

        qDebug() << "User name size:" << userNameSize;
        QByteArray uName = buffer.mid(sizeof(qint64),userNameSize);


        QHostAddress localAddress = getLocalIPAddress();
        quint16 localPort = udpSocket->localPort();

        if (sender == localAddress && senderPort == localPort&&this->username==uName) {
            qDebug() << "Received message from self, ignoring.";
            return;  // 忽略本地发出的消息
        }


        // buffer = udpSocket->readAll();
        QString recstr= QString::fromUtf8(buffer.mid(sizeof(qint64)+userNameSize+1));
        emit dataReceived(recstr);

        //由于udp是无连接协议，数据包没有顺序保证，readAll() 可能会丢失多个数据报之间的边界，导致数据不完整或拼接在一起。
        //readAll主要用于tcp连接,在udp中，readAll() 读取的是当前所有可用的数据，而不是一个完整的udp数据包。
    }


    QString recstr= QString::fromUtf8(buffer);;
    // ui->data->append("getMsg:");
    // ui->data->append(recstr);


}
QHostAddress  Network::getLocalIPAddress() {
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (const QHostAddress &address, list) {
        // 排除 IPv6 地址和回环地址 (127.0.0.1)
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost) {
            return address;
        }
    }
    return QHostAddress::LocalHost;  // 默认返回回环地址
}

bool Network::startClientConnection(    QString ip,
                                       quint16 port){
     //未连接状态  去连接
    if(tcpclient->state() != QAbstractSocket::ConnectedState) {
        //连接服务器
        tcpclient->connectToHost(ip,port);
        //等待连接成功
        if(!tcpclient->waitForConnected(20000)) {
            qDebug() << "Connection failed!";
            return false;
        }
        // 检查连接状态
        if (tcpclient->state() == QAbstractSocket::ConnectedState) {
            connect(tcpclient, &QTcpSocket::readyRead, this, &Network::readTcpData);
            return true;
        } else {
            qDebug() << "Failed to establish connection!";
            return false;
        }

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
bool Network::startUdpConnection(QString userName,
                                    quint16 port){

    if(addUserName(userName)){
        udpSocket = new QUdpSocket(this);
        // 绑定到广播端口，让客户端能够接收来自该端口的消息
        if (udpSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            qDebug() << "UDP socket bound to broadcast port " << port;
            return connect(udpSocket, &QUdpSocket::readyRead, this, &Network::readUdpData);

        } else {
            qDebug() << "Failed to bind UDP socket on port " << port;
            return false;
        }
    }else{
        return false;
    }




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
static QList<QString> userList;
bool Network::addUserName(QString username)
{
    this->username=username;
    if(!userList.contains(username)){
        userList.append(username);
        return true;
    }else{
        QMessageBox::information(this,tr("error"),tr("用户已登录"));
        return false;
    }


}
void Network::send(bool ifSendButton,int mode,QString username,quint16 port,QByteArray data)
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
        // // 发送数据
        // // 这里的ip端口 组播,后期再改
        // udpSocket->writeDatagram(data,QHostAddress::Broadcast,port);
        // // debug
        // qDebug() << "data sent(udp): " << data.toStdString();

        quint16 serverPort = PORT_TEST;
        QByteArray dataArray ;
        qint64 nameSize = username.toUtf8().size();

        QDataStream stream(&dataArray, QIODevice::WriteOnly);
        stream << nameSize;
        dataArray.append(username.toUtf8());
        dataArray.append(data);
        qint64 bytesSent = udpSocket->writeDatagram(dataArray, QHostAddress::Broadcast, serverPort);  // 向广播地址发送数据

        if (bytesSent == -1) {
            qDebug() << "Failed to send broadcast message.";
        } else {
            qDebug() << "Broadcast message sent successfully.";
        }


    }


}

void Network::openConnection(int mode,QString ip,quint16 port,bool *ifSendButton)

{

    if(mode==UDP_MODE){
        qDebug()<<"UDP_MODE";
        if(ip.isEmpty()||ip==""){
            ip="user";
        }

        qDebug() << ip;
        QString uName=ip;
        qDebug() << uName;
        *ifSendButton = startUdpConnection(uName,port);
    }else if(mode==TCP_CLIENT_MODE){
        qDebug()<<"TCP_CLIENT_MODE";
        *ifSendButton =startClientConnection(ip,port);
    }else if(mode==TCP_SERVER_MODE){
        qDebug()<<"TCP_SERVER_MODE";
        *ifSendButton =startServerConnection(ip,port);
    }

}
bool Network::socketValid(QString ip,quint16 port){
    QHostAddress address;
    if(port > 0 && port <= 65535&&address.setAddress(ip)){

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
