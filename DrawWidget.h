#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

class DrawWidget : public QWidget {
    Q_OBJECT

public:
    explicit DrawWidget(QWidget *parent = nullptr);
    void enableDrawCircle(bool enable);  // 启用/禁用绘制圆形
    void enableDrawRectangle(bool enable);  // 启用/禁用绘制矩形

protected:
    void paintEvent(QPaintEvent *event) override;  // 重写绘制事件
    void mousePressEvent(QMouseEvent *event) override;  // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event) override;  // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override;  // 鼠标释放事件

private:
    bool drawing;  // 是否正在绘制
    bool drawCircleEnabled;
    bool drawRectangleEnabled;
    QPoint startPoint;  // 起点
    QPoint endPoint;  // 终点
    // 容器保存已绘制的图形
    QList<QRect> rectangles;  // 保存矩形
    QList<QPoint> circleCenters;  // 保存圆心
    QList<int> circleRadii;

};

#endif // DRAWWIDGET_H
