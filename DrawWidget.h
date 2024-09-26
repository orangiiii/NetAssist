#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <fileio.h>
#include "network.h"

class DrawWidget : public QMainWindow {
    Q_OBJECT

public:
    explicit DrawWidget(QMainWindow *parent = nullptr);

    void setBackgroundEnabled(bool enable);
    void setupToolBar();
    void handleCloseButton();
    void enableDrawCircle();
    void enableDrawRectangle();
    void handleWithdrawButton();
    void handleSaveButton();
    void handleSendButton();
    QPixmap getPaintArea();
    void setNetwork(Network *network);
    void initDataFromMainwin(QString ip,quint16 port,int mode);
protected:
    void paintEvent(QPaintEvent *event) override;  // 重写绘制事件
    void mousePressEvent(QMouseEvent *event) override;  // 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event) override;  // 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override;  // 鼠标释放事件

signals:
    void pictureReady( QByteArray &pixmap);

private:
    bool drawing;  // 是否正在绘制
    bool drawingEnable;  // 绘图板
    bool drawCircleEnabled;
    bool drawRectangleEnabled;
    QPoint startPoint;
    QPoint endPoint;
    // 容器保存已绘制的图形
    QList<QRect> rectangles;
    QList<QPoint> circleCenters;
    QList<int> circleRadii;
    QList<int> steps;
    QPainter painter;
    QToolBar *toolBar;

    FileIO *fileIO;
    Network *network;

    QString ip;
    quint16 port;
    int mode;


};

#endif // DRAWWIDGET_H
