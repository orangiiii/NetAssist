#include "DrawWidget.h"

#include <QDebug>
#include "define.h"
// #include <QtMath>


DrawWidget::DrawWidget(QMainWindow *parent) : QMainWindow(parent), drawing(false),drawingEnable(false), drawCircleEnabled(false) ,drawRectangleEnabled(false) {
    setMouseTracking(true);  // 启用鼠标追踪
    setFixedSize(400, 400);  // 设置固定窗口大小
    setupToolBar();


}
// 初始化工具栏
void DrawWidget::setupToolBar() {
    toolBar = new QToolBar(this);
    this->addToolBar(toolBar);

    QAction *circleAction = new QAction(QIcon(":/icons/circle.jpg"), "圆", this);
    QAction *recAction = new QAction(QIcon(":/icons/rectangle.jpg"), "矩形", this);
    QAction *withdrawAction = new QAction(QIcon(":/icons/withdraw.jpg"), "撤回", this);
    QAction *closeAction = new QAction(QIcon(":/icons/close.jpg"), "关闭", this);

    toolBar->addAction(circleAction);
    toolBar->addAction(recAction);
    toolBar->addAction(withdrawAction);
    toolBar->addAction(closeAction);
    connect(circleAction,&QAction::triggered,this,&DrawWidget::enableDrawCircle);
    connect(recAction,&QAction::triggered,this,&DrawWidget::enableDrawRectangle);
    connect(withdrawAction,&QAction::triggered,this,&DrawWidget::handleWithdrawButton);
    connect(closeAction,&QAction::triggered,this,&DrawWidget::handleCloseButton);
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

}



void DrawWidget::openDraw() {
    /*
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿
    painter.fillRect(this->rect(), Qt::gray);*/

}
void DrawWidget::paintEvent(QPaintEvent *event) {

    QWidget::paintEvent(event);
    QPainter painter(this);

    // if(drawingEnable){
        painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿
        // painter.fillRect(this->rect(), Qt::gray);

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
    // }


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
            int radius = qSqrt(qPow(rect.width(), 2) + qPow(rect.height(), 2)) / 2;
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
