#include "mainwindow.h"
#include "ui_mainwindow.h"
// #include "ui_clientwin.h"
#include <QWidget>
#include "define.h"
#include <QFiledialog>
// #include "clientwin.h"

#include <QApplication>
#include <QMetaObject>

#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    imageLabel(new QLabel(this)),// 初始化 QLabel
    drawWidget(new DrawWidget(this))
{
    QString ip = IP_TEST;
    quint16 port = PORT_TEST;
    // QString ip = ipLine->text();
    // QString port =portLine->text().toUShort();
    // setupUi(this);

    setupUi();
    retranslateUi();

    connect(openButton, &QPushButton::clicked, this, &MainWindow::handleOpenButton);
    connect(createButton, &QPushButton::clicked, this, &MainWindow::handleCreateButton);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButton);
    connect(readButton, &QPushButton::clicked, this, &MainWindow::handleReadButton);
    connect(writeButton, &QPushButton::clicked, this, &MainWindow::handleWriteButton);
    sendButton->setEnabled(false);

    fileIO = new FileIO(this);
    network = new Network(this);

    connect(network, &Network::readMessage, network, &Network::readData);
    connect(network, &Network::dataReceived, this, &MainWindow::updateTextEdit);
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    label = new QLabel();

    // 布局和初始化 QLabel 大小等
    imageLabel->setFixedSize(100, 75);
    imageLabel->setScaledContents(true);  // 如果需要图片自动适应大小

    connect(drawWidget, &DrawWidget::pictureReady, this, &MainWindow::displaySendPicture);
    connect(network, &Network::pictureReceived, this, &MainWindow::displayReceivedPicture);

    //test
    // handleCreateButton();


}

MainWindow::~MainWindow()
{
    // delete ui;
}

void MainWindow::setupUi()
{
    this->resize(500, 350);

    // 创建控件
    ipLabel = new QLabel(this);
    ipLabel->setGeometry(QRect(30, 110, 69, 19));

    portLabel = new QLabel(this);
    portLabel->setGeometry(QRect(30, 160, 69, 19));

    ipLine = new QLineEdit(this);
    ipLine->setGeometry(QRect(70, 100, 113, 25));

    portLine = new QLineEdit(this);
    portLine->setGeometry(QRect(70, 160, 113, 25));

    openButton = new QPushButton(this);
    openButton->setGeometry(QRect(60, 200, 93, 28));
    openButton->setCheckable(true);

    dataText = new QTextBrowser(this);
    dataText->setGeometry(QRect(190, 30, 250, 150));

    textEdit = new QTextEdit(this);
    textEdit->setGeometry(QRect(190, 200, 250, 75));

    sendButton = new QPushButton(this);
    sendButton->setGeometry(QRect(350, 280, 90, 25));
    sendButton->setCheckable(true);

    chooseBox = new QComboBox(this);
    chooseBox->setGeometry(QRect(40, 60, 121, 25));
    chooseBox->addItem(QString());
    chooseBox->addItem(QString());
    chooseBox->addItem(QString());

    createButton = new QPushButton(this);
    createButton->setGeometry(QRect(10, 250, 93, 28));

    readButton = new QPushButton(this);
    readButton->setGeometry(QRect(190, 180, 90, 20));
    readButton->setCheckable(true);

    writeButton = new QPushButton(this);
    writeButton->setGeometry(QRect(350, 180, 90, 20));
    writeButton->setCheckable(true);

    QMetaObject::connectSlotsByName(this);
}

void MainWindow::retranslateUi()
{
    this->setWindowTitle(QCoreApplication::translate("MainWindow", "Form", nullptr));
    ipLabel->setText(QCoreApplication::translate("MainWindow", "ip", nullptr));
    portLabel->setText(QCoreApplication::translate("MainWindow", "port", nullptr));
    openButton->setText(QCoreApplication::translate("MainWindow", "open", nullptr));
    sendButton->setText(QCoreApplication::translate("MainWindow", "send", nullptr));
    chooseBox->setItemText(0, QCoreApplication::translate("MainWindow", "tcp client", nullptr));
    chooseBox->setItemText(1, QCoreApplication::translate("MainWindow", "tcp server", nullptr));
    chooseBox->setItemText(2, QCoreApplication::translate("MainWindow", "udp", nullptr));
    createButton->setText(QCoreApplication::translate("MainWindow", "创建", nullptr));
    readButton->setText(QCoreApplication::translate("MainWindow", "read", nullptr));
    writeButton->setText(QCoreApplication::translate("MainWindow", "write", nullptr));
}
void MainWindow::setupMenuBar() {
    menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);

    QMenu *fileMenu = new QMenu("文件", this);
    menuBar->addMenu(fileMenu);

    QAction *openAction = new QAction("打开", this);
    QAction *saveAction = new QAction("保存", this);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);

    connect(openAction,&QAction::triggered,this,&MainWindow::handleReadButton);
    connect(saveAction,&QAction::triggered,this,&MainWindow::handleWriteButton);
}


// 初始化工具栏
void MainWindow::setupToolBar() {
    toolBar = new QToolBar(this);
    this->addToolBar(toolBar);

    QAction *openAction = new QAction(QIcon(":/icons/circle.jpg"), "圆", this);
    QAction *saveAction = new QAction(QIcon(":/icons/rectangle.jpg"), "矩形", this);

    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    connect(openAction,&QAction::triggered,this,&MainWindow::handleDrawCircleButton);
    connect(saveAction,&QAction::triggered,this,&MainWindow::handleDrawRectangleButton);
}

// 初始化状态栏
void MainWindow::setupStatusBar() {
    statusBar = new QStatusBar(this);
    this->setStatusBar(statusBar);
    statusBar->showMessage("准备就绪");
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    // 创建一个右键菜单
    QMenu contextMenu(this);

    // 创建“打开”和“保存”两个动作（菜单选项）
    QAction openAction("打开", this);
    QAction saveAction("保存", this);

    // 将动作添加到菜单中
    contextMenu.addAction(&openAction);
    contextMenu.addAction(&saveAction);

    // 将动作与相应的槽函数连接，当用户点击时，执行相应的操作
    connect(&openAction, &QAction::triggered, this, &MainWindow::handleReadButton);
    connect(&saveAction, &QAction::triggered, this, &MainWindow::handleWriteButton);

    // 在鼠标右键点击的位置显示菜单
    contextMenu.exec(event->globalPos());
}


void MainWindow::handleOpenButton()
{

    QString ip = IP_TEST;
    quint16 port = PORT_TEST;
    // QString ip = ipLine->text();
    // QString port =portLine->text().toUShort();
    bool ifSendButton = false;
    network->openConnection(chooseBox->currentIndex(),ip,port,&ifSendButton);
    if(ifSendButton){
        sendButton->setEnabled(true);
        openButton->setText("已连接");
    }else{

        openButton->setText("已连接");
    }




}


void MainWindow::handleCreateButton()
{
    MainWindow *newWin = new MainWindow;
    newWin->show();

}
void MainWindow::handleDrawCircleButton()
{
    drawWidget->show();
    drawWidget->enableDrawCircle();
    drawWidget->setNetwork(this->network);

}
void MainWindow::handleDrawRectangleButton()
{
    drawWidget->show();
    drawWidget->enableDrawRectangle();
    drawWidget->setNetwork(this->network);
    drawWidget->initDataFromMainwin(ip, port,chooseBox->currentIndex());

}


void MainWindow::handleSendButton()

{
    // qDebug()<<sendButton->isEnabled();

    QString oriData(textEdit->toPlainText());
    QString data = "send msg:\n"+ oriData;
    QString ip = IP_TEST;
    quint16 port = PORT_TEST;
    // QString ip = ipLine->text();
    // QString port =portLine->text().toUShort();
    QByteArray dataArray;
    dataArray.append(TYPE_TEXT);
    dataArray.append(oriData.toUtf8());


    network->send(sendButton->isEnabled(),chooseBox->currentIndex(),ip,port,dataArray);

    dataText->append(data);
    textEdit->clear();
}

void MainWindow::timeout(){

}


void MainWindow::handleReadButton()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;Image Files (*.png *.jpg *.bmp);;All Files (*)"));
    if (!filePath.isEmpty()) {

        label->setFixedSize(400, 400);
        int type = fileIO->readFile(filePath,*label);
        if(type==READ_TEXT){
            QString fileContent = label->text();
            if (!fileContent.isEmpty()) {
                textEdit->setText(fileContent);
            }
        }else if(type==READ_IMAGE){
            label->show();
        }else{

            label->setFixedSize(200, 50);
            label->show();
        }
    }

}


void MainWindow::handleWriteButton()
{
    QString content = dataText->toPlainText();
    // QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));

        if( fileIO->writeFile(content)){
            QMessageBox::information(this, tr("Success"), tr("Write file successfully."));
        }else{
            QMessageBox::information(this, tr("Error"), tr("Failed to write file."));
        }

}

void MainWindow::updateTextEdit(const QString& data) {
    dataText->append("Received message:");
    dataText->append(data);  // 将数据添加到 QTextEdit 中
}


void MainWindow::handleCircleButton()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;Image Files (*.png *.jpg *.bmp);;All Files (*)"));
    if (!filePath.isEmpty()) {

        label->setFixedSize(400, 400);
        int type = fileIO->readFile(filePath,*label);
        if(type==READ_TEXT){
            QString fileContent = label->text();
            if (!fileContent.isEmpty()) {
                textEdit->setText(fileContent);  // 将读取到的内容显示在 QtextEdit 中
            }
        }else if(type==READ_IMAGE){
            label->show();
        }else{

            label->setFixedSize(200, 50);
            label->show();
        }
    }

}

void MainWindow::handleRectangleButton()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;Image Files (*.png *.jpg *.bmp);;All Files (*)"));
    if (!filePath.isEmpty()) {

        label->setFixedSize(400, 400);
        int type = fileIO->readFile(filePath,*label);
        if(type==READ_TEXT){
            QString fileContent = label->text();
            if (!fileContent.isEmpty()) {
                textEdit->setText(fileContent);  // 将读取到的内容显示在 QtextEdit 中
            }
        }else if(type==READ_IMAGE){
            label->show();
        }else{

            label->setFixedSize(200, 50);
            label->show();
        }
    }



}
void MainWindow::displayReceivedPicture( QByteArray &byteArray){

    QString base64Image = QString::fromLatin1(byteArray.toBase64().data());

    QTextCursor cursor = dataText->textCursor();
    cursor.movePosition(QTextCursor::End);
    // 插入文本信息
    dataText->append("recevied msg:");
    QString html = "<img src=\"data:image/png;base64," + base64Image + "\" width=\"200\">";
    dataText->insertHtml(html);
}
void MainWindow::displaySendPicture( QByteArray &byteArray){

    QString base64Image = QString::fromLatin1(byteArray.toBase64().data());

    QTextCursor cursor = dataText->textCursor();
    cursor.movePosition(QTextCursor::End);
    // 插入文本信息
    dataText->append("send msg:");
    QString html = "<img src=\"data:image/png;base64," + base64Image + "\" width=\"200\">";
    dataText->insertHtml(html);
}
