#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectdialog.h"
#include <QStatusBar>
#include <QToolBar>
#include <Qmenu>
#include <QAction>
#include <QLineEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <Qtsql/qsql.h>
#include <Qtsql/QSqlDatabase>
#include <QCompleter>
#include <Qlabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QSqlQuery>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    connectDialog *connecter;
    QAction *openAct;
    QAction *connectAct;

    QLineEdit *IpEdit;
    QLineEdit *portEdit;
    QPushButton *connectBtn;
    QPushButton *sendBtn;
    QPushButton *generateBtn;
    QPushButton *centralWidgetBtn;
    QPushButton *importMessageBtn;
    QPushButton *exportMessageBtn;
    QTextEdit *sendEdit;
    QTextEdit *messageEdit;
    QToolBar *mainBar;
    QToolBar *IpBar;

    QLabel *trancodeLabel;
    QLineEdit *trancodeEdit;
    QCompleter *trancodeComPleter;
    QHBoxLayout *trancodeLayout;
    QHBoxLayout *rightBtnLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QHBoxLayout *mainLayout;
    QStringList trancodeList;

    QTableWidget *table;
    QWidget *centralWidget;
    QTableWidgetItem item[150];
    QSplitter *rightSplitter;
    QSplitter *leftSplitter;

    QSplitter *mainSplitter;
    QSqlDatabase db;
    QString mainPath;

    //netWork;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QString ip;
    qint16 port;
    bool isConnect;
    QString message;
    QString tempfilename;


    Ui::MainWindow *ui;\
    //void setTableHead();
    void setUi();
    void setToolBar();
    int setCodeEditData();
    int connectToDb();
    int setTableData();
    //void createXmlMessage();
    void createXmlMessage();
    int getLayer(QString data);
    //Tcp
    void receiveMessage();
    void readMessage();

    void exportTempMessage();

    void setAp();

private slots:
    void setMessageEdit();
    void connectToServer();
    void sendMessage();
    void importMessage();
    void importTempMessage();
    void exportMessage();

    //void createNSPMsg();
    void acceptConnection();
    void disConnection();
    void receiveData();
    void setTableHead();
};

#endif // MAINWINDOW_H
