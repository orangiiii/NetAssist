#include "DrawWidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include "define.h"


DrawWidget::DrawWidget(QMainWindow *parent) :
    QMainWindow(parent), drawing(false),drawingEnable(false), drawCircleEnabled(false) ,drawRectangleEnabled(false) {
    setMouseTracking(true);  // 启用鼠标追踪
    setFixedSize(400, 400);  // 设置固定窗口大小
    setupToolBar();

    fileIO = new FileIO(this);


}
// 初始化工具栏
void DrawWidget::setupToolBar() {
    toolBar = new QToolBar(this);

    QAction *circleAction = new QAction(QIcon(":/icons/circle.jpg"), "圆", this);
    QAction *recAction = new QAction(QIcon(":/icons/rectangle.jpg"), "矩形", this);
    QAction *withdrawAction = new QAction(QIcon(":/icons/withdraw.jpg"), "撤回", this);
    QAction *closeAction = new QAction(QIcon(":/icons/close.jpg"), "关闭", this);
    QAction *saveAction = new QAction(QIcon(":/icons/save.jpg"), "保存", this);
    QAction *sendAction = new QAction(QIcon(":/icons/send.jpg"), "发送", this);


    toolBar->addAction(circleAction);
    toolBar->addAction(recAction);
    toolBar->addAction(withdrawAction);
    toolBar->addAction(closeAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(sendAction);

    connect(circleAction,&QAction::triggered,this,&DrawWidget::enableDrawCircle);
    connect(recAction,&QAction::triggered,this,&DrawWidget::enableDrawRectangle);
    connect(withdrawAction,&QAction::triggered,this,&DrawWidget::handleWithdrawButton);
    connect(closeAction,&QAction::triggered,this,&DrawWidget::handleCloseButton);
    connect(saveAction,&QAction::triggered,this,&DrawWidget::handleSaveButton);
    connect(sendAction,&QAction::triggered,this,&DrawWidget::handleSendButton);

    // 创建一个QWidget用于包装工具栏
    QWidget *toolBarWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(toolBarWidget);
    layout->addWidget(toolBar);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);  // 设置居中和顶端对齐
    layout->setContentsMargins(0, 0, 0, 0);  // 去除边距

    // 设置工具栏为QMainWindow的central widget
    setCentralWidget(toolBarWidget);
}
void DrawWidget::enableDrawCircle() {
    drawCircleEnabled = true;
    drawingEnable = true;
    drawRectangleEnabled = false;
    update();
}

void DrawWidget::enableDrawRectangle() {
    drawRectangleEnabled = true;
    drawingEnable = true;
    drawCircleEnabled = false;
    update();
}


void DrawWidget::handleWithdrawButton()
{
    if(steps.size()==0){
        return;
    }
    int step = steps.takeLast();
    if(STEP_RECTANGLE == step){
        rectangles.removeLast();
        update();
        return;
    }
    if(STEP_CIRCLE == step){
        circleCenters.removeLast();
        circleRadii.removeLast();
        update();
        return;
    }


}

void DrawWidget::handleCloseButton()
{
    drawRectangleEnabled = false;
    drawingEnable = false;
    drawCircleEnabled = false;

    this->close();

}
void DrawWidget::handleSaveButton()
{
    QString content ;
    QPixmap pixmap = getPaintArea();
    fileIO->writeImageFile(pixmap);

}
QPixmap DrawWidget::getPaintArea(){
    // 获取绘图区域的几何信息，排除工具栏
    QRect drawArea = this->rect();  // 获取整个窗口的矩形区域
    int toolbarHeight = this->toolBar->height();  // 获取工具栏的高度
    drawArea.adjust(0, toolbarHeight, 0, 0);  // 调整绘图区域，排除工具栏的高度

    QPixmap pixmap(drawArea.size());  // 根据调整后的区域创建pixmap
    pixmap.fill(Qt::white);  // 填充背景为白色

    QPainter painter(&pixmap);
    this->render(&painter, QPoint(), QRegion(drawArea));  // 只渲染绘图区域
    painter.end();
    return pixmap;
}

void DrawWidget::handleSendButton()
{
    if(!network->getIfConnected()){
        QMessageBox::warning(this, "error", "请先连接!");
        return;
    }
    QPixmap pixmap = getPaintArea();
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG"); // 将图像保存为PNG格式到字节数组中

    emit pictureReady(byteArray);
    QByteArray picData=network->getPicData(&pixmap);
    network->send(true,mode,ip,port,picData);

    this->close();
    rectangles.clear();
    circleCenters.clear();
    circleRadii.clear();
    steps.clear();
    currentImage = QPixmap();
}

void DrawWidget::setNetwork(Network* network){
    this->network = network;
}




void DrawWidget::paintEvent(QPaintEvent *event) {

    QWidget::paintEvent(event);
    QPainter painter(this);


    painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿
    // painter.fillRect(this->rect(), Qt::gray);

    if(!currentImage.isNull()){
        painter.drawPixmap(rect(),currentImage);
    }

    // 绘制已保存的矩形
    painter.setPen(QPen(Qt::red, 2));
    for (const QRect &rect : rectangles) {
        painter.drawRect(rect);
    }
    // 绘制已保存的圆
    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i < circleCenters.size(); ++i) {
        painter.drawEllipse(circleCenters[i], circleRadii[i], circleRadii[i]);
    }
    // 绘制当前正在绘制的图形
    if (drawing) {
        painter.setPen(QPen(Qt::green, 2));
        QRect rect(startPoint, endPoint);

        if (drawCircleEnabled) {
            int radius = qSqrt(qPow(rect.width(), 2) + qPow(rect.height(), 2)) ;
            painter.drawEllipse(startPoint, radius, radius);
        }

        if (drawRectangleEnabled) {
            painter.drawRect(rect);
        }
    }



}

void DrawWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drawing = true;
        startPoint = event->pos();  // 记录起始点
        endPoint = event->pos();  // 设置初始结束点
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event) {
    if (drawing) {
        endPoint = event->pos();  // 更新结束点
        update();  // 触发重新绘制
    }
}

void DrawWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && drawing) {
        endPoint = event->pos();
        QRect rect(startPoint, endPoint);
        if (drawCircleEnabled) {
            // 计算圆的半径并保存
            int radius = qSqrt(qPow(rect.width(), 2) + qPow(rect.height(), 2)) ;
            circleCenters.append(startPoint);
            circleRadii.append(radius);
            steps.append(STEP_CIRCLE);
        }

        if (drawRectangleEnabled) {
            rectangles.append(rect);
            steps.append(STEP_RECTANGLE);
        }

        drawing = false;  // 停止绘制
        update();  // 重新绘制
    }
}
void DrawWidget::initDataFromMainwin(QString ip,quint16 port,int mode){
    this->ip=ip;
    this->port=port;
    this->mode=mode;
}

// 设置文本显示
void DrawWidget::setText(const QString &text) {
    currentText = text;
    update();  // 重新绘制
}

// 获取当前显示的文本
QString DrawWidget::getText() const {
    return currentText;
}

// 设置图片显示
void DrawWidget::setImage(const QPixmap image) {
    currentImage = image;
    update();  // 重新绘制
}

