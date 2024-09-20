// #include "clientwin.h"
// #include "ui_clientwin.h"

// #include <QWidget>
// // #include <QtDebug>



// ClientWin::ClientWin(QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::ClientWin)
// {
//     // ui->setupUi(this);
//     tcpclient = new QTcpSocket(this);
//     connect(tcpclient,&QTcpSocket::readyRead(),this,&ClientWin::readdata);
//         // connect(tcpclient,&QTcpSocket::readyread,this,&Network::readdata);


// }

// ClientWin::~ClientWin()
// {
//     delete ui;
// }

// void ClientWin::readdata(){
//     QByteArray buffer = tcpclient->readAll();
//     QString recstr = QString(buffer);
//     ui->data->append("getMsg:");
//     ui->data->append(recstr);
//     // qDebug()<<"ClientWin";
//     qDebug()<<"clientwin:"+recstr;

// }

// void ClientWin::handleSendButton()
// {

//     qDebug()<<"send:";
//     if(tcpclient->state()==QAbstractSocket::ConnectedState){
//         QString sendStr = ui->textEdit->toPlainText();
//         // 发送
//         tcpclient->write(sendStr.toLocal8Bit());
//         ui->data->append("client send:\n"+sendStr);
//     }
// }

