#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QtSql/qsql.h>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QFile>
#include <QCompleter>
#include <QFileDialog>

extern QString g_DBDriver;
extern QString g_Server;
extern int g_port;
extern QString g_DBName;
extern QString g_UsrName;
extern QString g_PassWord;

class connectDialog : public QWidget
{
    Q_OBJECT
public:
    explicit connectDialog(QWidget *parent = 0);

private:
    QLabel *driverLabel;
    QComboBox *driverComboBox;
    QLabel *serverLabel;
    QLineEdit *serverEdit;
    QLabel *portLabel;
    QLineEdit *portEdit;
    QLabel *dbNameLabel;
    QLineEdit *dbNameEdit;
    QLabel *usrLabel;
    QLineEdit *usrEdit;
    QLabel *passwordLable;
    QLineEdit *passwordEdit;
    QPushButton *connectBtn;
    QPushButton *cancelBtn;
    QLineEdit *tranCodeEdit;

    QHBoxLayout *driverLayout;
    QHBoxLayout *serverLayout;
    QHBoxLayout *portLayout;
    QHBoxLayout *dbNameLayout;
    QHBoxLayout *usrLayout;
    QHBoxLayout *passwordLayout;
    QHBoxLayout *btnLayout;
    QVBoxLayout *dialogLayout;

    QStringList serverList;
    QStringList portList;
    QStringList dbNameList;
    QStringList usrNameList;

    QCompleter *serverCompleter;
    QCompleter *portCompleter;
    QCompleter *dbNameCompleter;
    QCompleter *usrNameCompleter;

    void setUi();
    int readCfg();
signals:

public slots:
    int connectToDB();
    int writeCfg();

};

#endif // CONNECTDIALOG_H
