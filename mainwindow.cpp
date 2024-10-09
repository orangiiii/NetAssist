#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include "define.h"
#include <QGroupBox>

#include <QApplication>
#include <QMetaObject>

#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    imageLabel(new QLabel(this)),// 初始化 QLabel
    drawWidget(new DrawWidget(this))
{
    ip = IP_TEST;
    port = PORT_TEST;

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
    // 主窗口的水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout();

    // 左边红色区域的布局（连接设置部分）
    QGroupBox *connectionGroupBox = new QGroupBox("连接设置");
    QGridLayout *connectionLayout = new QGridLayout();

    // 初始化控件
    ipLabel = new QLabel("IP:", this);
    ipLine = new QLineEdit(this);
    portLabel = new QLabel("Port:", this);
    portLine = new QLineEdit(this);
    openButton = new QPushButton("连接", this);
    createButton = new QPushButton("创建", this);

    // 设置按钮的固定宽度，保证按钮大小一致
    openButton->setFixedWidth(200);
    createButton->setFixedWidth(200);

    chooseBox = new QComboBox(this);
    chooseBox->addItem("tcp client");
    chooseBox->addItem("tcp server");
    chooseBox->addItem("udp");


    // 调整控件的对齐方式和布局
    connectionLayout->addWidget(new QLabel("选择模式:"), 0, 0);
    connectionLayout->addWidget(chooseBox, 0, 1);

    connectionLayout->addWidget(ipLabel, 1,0);
    connectionLayout->addWidget(ipLine, 1, 1);

    connectionLayout->setRowMinimumHeight(1, 30);

    connectionLayout->addWidget(portLabel, 2, 0);
    connectionLayout->addWidget(portLine, 2, 1);


    connectionLayout->addWidget(openButton, 3, 0, 1, 2, Qt::AlignCenter);
    connectionLayout->setRowMinimumHeight(3, 30);

    connectionLayout->addWidget(createButton, 4, 0, 1, 2, Qt::AlignCenter);

    connectionLayout->setSpacing(10);
    connectionLayout->setContentsMargins(10, 10, 10, 10);  // 设置布局的外部间距
    connectionGroupBox->setLayout(connectionLayout);
    connectionLayout->setVerticalSpacing(30);  // 控制每行之间的垂直间距

    // 右边红色区域（数据接收和发送部分）
    QGroupBox *dataReceiveGroupBox = new QGroupBox("数据接收");
    QVBoxLayout *dataReceiveLayout = new QVBoxLayout();
    dataText = new QTextBrowser(this);
    textEdit = new QTextEdit(this);
    sendButton = new QPushButton("发送", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    readButton = new QPushButton("read", this);
    writeButton = new QPushButton("write", this);
    buttonLayout->addWidget(readButton);
    buttonLayout->addWidget(writeButton);

    // 将接收和发送部分布局
    dataReceiveLayout->addWidget(dataText);   // 上方的文本接收框
    dataReceiveLayout->addLayout(buttonLayout); // 放置 read 和 write 按钮
    textEdit->setFixedHeight(80); // 调整红色区域的高度
    dataReceiveLayout->addWidget(textEdit);  // 发送文本框
    dataReceiveLayout->addWidget(sendButton); // 发送按钮
    dataReceiveGroupBox->setLayout(dataReceiveLayout);

    // 设置主布局为水平布局
    mainLayout->addWidget(connectionGroupBox);  // 左边连接设置
    mainLayout->addWidget(dataReceiveGroupBox); // 右边数据接收发送

    // 设置中心组件
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 设置右键菜单清空功能
    dataText->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(dataText, &QTextBrowser::customContextMenuRequested, this, &MainWindow::showCustomContextMenu);
    // 设置窗口大小并禁止调整大小
    this->setFixedSize(550, 500);  // 设置窗口固定大小

    // 连接信号与槽
    connect(chooseBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        if (chooseBox->currentText() == "udp") {
            ipLabel->setText("username:");
            portLabel->setText("pwd:");
        } else {
            ipLabel->setText("ip:");
            portLabel->setText("port:");
        }
    });
}

void MainWindow::retranslateUi()
{
    this->setWindowTitle(QCoreApplication::translate("MainWindow", "Form", nullptr));
    ipLabel->setText(QCoreApplication::translate("MainWindow", "ip", nullptr));
    portLabel->setText(QCoreApplication::translate("MainWindow", "port", nullptr));
    openButton->setText(QCoreApplication::translate("MainWindow", "connect", nullptr));
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


    QMenu *drawMenu = new QMenu("绘图", this);
    menuBar->addMenu(drawMenu);
    QAction *circleAction = new QAction("圆", this);
    QAction *rectangleAction = new QAction("矩形", this);
    drawMenu->addAction(circleAction);
    drawMenu->addAction(rectangleAction);

    connect(circleAction,&QAction::triggered,this,&MainWindow::handleDrawCircleButton);
    connect(rectangleAction,&QAction::triggered,this,&MainWindow::handleDrawRectangleButton);

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
void MainWindow::showCustomContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction actionClear("清空", this);
    connect(&actionClear, &QAction::triggered, this, &MainWindow::clearDataText);
    contextMenu.addAction(&actionClear);

    contextMenu.exec(dataText->mapToGlobal(pos));
}
void MainWindow::clearDataText()
{
    dataText->clear();
}


void MainWindow::handleOpenButton()
{
    if(!ipLine->text().isEmpty()){
        ip = ipLine->text();
    }
    if(!portLine->text().isEmpty()){
        port =portLine->text().toUShort();
    }
    bool ifSendButton = false;
    if(!network->socketValid(ip,port)){
        QMessageBox::information(this,tr("Error"),tr("Wrong socket."));
        return;
    }
    network->openConnection(chooseBox->currentIndex(),ip,port,&ifSendButton);
    if(ifSendButton){
        sendButton->setEnabled(true);
        openButton->setText("已连接");
        network->setIfConnected(true);
        chooseBox->setEnabled(false);

    }else{

        sendButton->setEnabled(false);
        openButton->setText("点击连接");
        network->setIfConnected(false);
        chooseBox->setEnabled(true);
    }
}


void MainWindow::handleCreateButton()
{
    MainWindow *newWin = new MainWindow;
    newWin->show();

}
void MainWindow::handleDrawCircleButton()
{
    drawWidget->setNetwork(this->network);
    drawWidget->enableDrawCircle();
    drawWidget->show();


}
void MainWindow::handleDrawRectangleButton()
{
    drawWidget->enableDrawRectangle();
    drawWidget->setNetwork(this->network);
    drawWidget->initDataFromMainwin(ip, port,chooseBox->currentIndex());
    drawWidget->show();

}


void MainWindow::handleSendButton()

{
    // qDebug()<<sendButton->isEnabled();

    QString oriData(textEdit->toPlainText());
    // 检查消息是否为空
    if (oriData.trimmed().isEmpty()) {
        QMessageBox::warning(this, "error", "无法发送空消息！");
        return;
    }
    QString data = "Send msg:\n"+ oriData;
    // QString ip = IP_TEST;
    quint16 port = PORT_TEST;
    QString username;
    // qDebug << username ;
    if(!ipLine->text().isEmpty()){
        username = ipLine->text();
    }else{
        username =ip;
    }
    // QString port =portLine->text().toUShort();
    QByteArray dataArray;
    dataArray.append(TYPE_TEXT);
    dataArray.append(oriData.toUtf8());


    network->send(sendButton->isEnabled(),chooseBox->currentIndex(),username,port,dataArray);

    dataText->append(data);
    textEdit->clear();
    // 自动滚动到最后一行
    dataText->moveCursor(QTextCursor::End);

}

void MainWindow::timeout(){

}


void MainWindow::handleReadButton()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;Image Files (*.png *.jpg *.bmp);;All Files (*)"));
    if(filePath.isEmpty()){
        return;
    }

        drawWidget->setFixedSize(400, 400);
        drawWidget->setNetwork(this->network);
        drawWidget->initDataFromMainwin(ip, port,chooseBox->currentIndex());
        int type = fileIO->getFileType(filePath);
        if(type==READ_TEXT){
            QString fileContent = fileIO->readFile(filePath);
            if (!fileContent.isEmpty()) {
                textEdit->setText(fileContent);
            }
        }else if(type==READ_IMAGE){
            QPixmap pixmap(filePath);
            if(!pixmap.isNull()){
                drawWidget->setImage(pixmap);
                drawWidget->show();

            }else{
                QMessageBox::information(this,tr("Error"),tr("can not load the picture"));
            }

        }else{

            drawWidget->setFixedSize(200, 50);
            drawWidget->show();
        }


}


void MainWindow::handleWriteButton()
{
    QString htmlContent = dataText->toHtml();
    fileIO->writeFile(htmlContent);

}

void MainWindow::updateTextEdit(const QString& data) {
    dataText->append("Received msg:");
    dataText->append(data);  // 将数据添加到 QTextEdit 中

    dataText->moveCursor(QTextCursor::End);
}

void MainWindow::displayReceivedPicture( QByteArray &byteArray){

    QString base64Image = QString::fromLatin1(byteArray.toBase64().data());


    // 插入文本信息
    QString blank = "<br>";
    QString html = "Received msg:<br><img src=\"data:image/png;base64," + base64Image + "\" width=\"200\">";
    if(dataText->toPlainText().trimmed().length()!=0){
        html=blank+html;
    }
    dataText->insertHtml(html);
    dataText->moveCursor(QTextCursor::End);
}
void MainWindow::displaySendPicture( QByteArray &byteArray){

    QString base64Image = QString::fromLatin1(byteArray.toBase64().data());

    // 插入文本信息
    QString blank = "<br>";
    QString html = "Send msg:<br><img src=\"data:image/png;base64," + base64Image + "\" width=\"200\">";
    if(dataText->toPlainText().trimmed().length()!=0){
        html=blank+html;
    }
    dataText->insertHtml(html);
    dataText->moveCursor(QTextCursor::End);
}
