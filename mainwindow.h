#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QtCore>
#include <QLineEdit>
#include <QGridLayout>
#include <QtGui>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextEdit>
#include <QComboBox>
#include <QPixmap>
#include <QScrollBar>

#include "clientwin.h"
#include "fileio.h"
#include "network.h"
#include "drawwidget.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // void readData();
    void startClientConnection(QString ip, quint16 port);
    void startServerConnection(QString ip, quint16 port);
    void startUdpConnection(QString, quint16);
    void onNewConnection();
    void sendData();
    void timeout();
    void sendUdpData();
    void updateTextEdit(const QString &data);
    void handleDrawButton();
    void handleDrawCircleButton();
    void handleDrawRectangleButton();
    void handleCloseButton();
    void showCustomContextMenu(const QPoint &pos);
    void clearDataText();
private slots:
    void handleOpenButton();

    void handleCreateButton();

    void handleSendButton();

    void handleReadButton();

    void handleWriteButton();

    void displayReceivedPicture( QByteArray &pixmap);  // 用于显示图片的槽函数
    void displaySendPicture( QByteArray &pixmap);


signals:
    void Connected();

private:
    Ui::MainWindow *ui;
    QLabel *ipLabel,*portLabel;
    QLineEdit *ipLine,*portLine;
    QPushButton *connectButton;
    QGridLayout *mainLayout;
    QPushButton *openButton;
    QTextBrowser *dataText;
    QTextEdit *textEdit;
    QPushButton *sendButton;
    QComboBox *chooseBox;
    QPushButton *createButton;
    QPushButton *readButton;
    QPushButton *writeButton;

    void setupUi();
    void retranslateUi();

    // 私有成员
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QStatusBar *statusBar;

    QLabel *label;
    QLabel *imageLabel;  // 用于显示图片

    FileIO *fileIO;
    Network *network;

    void setupMenuBar();  // 初始化菜单栏的函数
    void setupToolBar();  // 初始化工具栏的函数
    void setupStatusBar();  // 初始化状态栏的函数
    void contextMenuEvent(QContextMenuEvent *event);
    // void setMenuBar(QMenuBar *);

    DrawWidget *drawWidget;
    QString ip;
    qint64 port;
};

#endif // MAINWINDOW_H
