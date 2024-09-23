#include "DrawWidget.h"

#include <QDebug>
// #include <QtMath>

DrawWidget::DrawWidget(QWidget *parent) : QWidget(parent), drawing(false), drawCircleEnabled(false), drawRectangleEnabled(false) {
    setMouseTracking(true);  // 启用鼠标追踪
}

void DrawWidget::enableDrawCircle(bool enable) {
    drawCircleEnabled = enable;
}

void DrawWidget::enableDrawRectangle(bool enable) {
    drawRectangleEnabled = enable;
}
void DrawWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿

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
            int radius = qSqrt(qPow(rect.width(), 2) + qPow(rect.height(), 2)) / 2;
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
        endPoint = event->pos();  // 最后更新结束点
        // drawing = false;  // 停止绘制
        // update();  // 重新绘制
        QRect rect(startPoint, endPoint);
        if (drawCircleEnabled) {
            // 计算圆的半径并保存
            int radius = qSqrt(qPow(rect.width(), 2) + qPow(rect.height(), 2)) / 2;
            circleCenters.append(startPoint);  // 保存圆心
            circleRadii.append(radius);  // 保存半径
        }

        if (drawRectangleEnabled) {
            rectangles.append(rect);  // 保存矩形
        }

        drawing = false;  // 停止绘制
        update();  // 重新绘制
    }
}
