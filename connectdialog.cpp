#include "connectdialog.h"

connectDialog::connectDialog(QWidget *parent) : QWidget(parent)
{  
    readCfg();
    setUi();
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(close()));
}
int connectDialog::readCfg()
{
    QFile config(":/cfg/cfg/db_config.cfg");
    if(!config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"open config file faild!!!";
    }

    QTextStream textInput(&config);
    QString lineStr;
    while (!textInput.atEnd())
    {
        lineStr = textInput.readLine();
        if(lineStr.startsWith("[DBInfo]"))
        {
            if(!serverList.contains(lineStr.section(" ",1,1)))
                serverList.append(lineStr.section(" ",1,1));
            if(!portList.contains(lineStr.section(" ",2,2)))
                portList.append(lineStr.section(" ",2,2));
            if(!dbNameList.contains(lineStr.section(" ",3,3)))
                dbNameList.append(lineStr.section(" ",3,3));
            if(!usrNameList.contains(lineStr.section(" ",4,4)))
                usrNameList.append(lineStr.section(" ",4,4));
        }
        qDebug()<<"serverList: "<<serverList;
    }
//    serverEdit->setText(serverList.value(0));
//    portEdit->setText(portList.value(0));
//    dbNameEdit->setText(dbNameList.value(0));
//    usrEdit->setText(usrNameList.value(0));
    config.close();
    return 0;
}
void connectDialog::setUi()
{
    driverLabel = new QLabel("驱  动");
    driverComboBox = new QComboBox();
    driverComboBox->addItem("Oracle");
    driverComboBox->addItem("DB2");
    serverLabel = new QLabel("服务器");
    serverEdit = new QLineEdit();
    portLabel = new QLabel("端  口");
    portEdit = new QLineEdit();
    dbNameLabel = new QLabel("数据库");
    dbNameEdit = new QLineEdit();
    usrLabel = new QLabel("用  户");
    usrEdit = new QLineEdit();
    passwordLable = new QLabel("密  码");
    passwordEdit = new QLineEdit();
    connectBtn = new QPushButton("连  接");
    connect(connectBtn,SIGNAL(clicked(bool)),this,SLOT(connectToDB()));
    cancelBtn = new QPushButton("取  消");
    serverCompleter = new QCompleter(this->serverList,this);
    serverCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    portCompleter = new QCompleter(this->portList,this);
    portCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    dbNameCompleter = new QCompleter(this->dbNameList,this);
    dbNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    usrNameCompleter = new QCompleter(this->usrNameList,this);
    usrNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);

    serverEdit->setCompleter(serverCompleter);
    portEdit->setCompleter(portCompleter);
    dbNameEdit->setCompleter(dbNameCompleter);
    usrEdit->setCompleter(usrNameCompleter);

    driverLayout = new QHBoxLayout();
    serverLayout = new QHBoxLayout;
    portLayout = new QHBoxLayout;
    dbNameLayout = new QHBoxLayout;
    usrLayout = new QHBoxLayout;
    passwordLayout = new QHBoxLayout;
    btnLayout = new QHBoxLayout;
    dialogLayout = new QVBoxLayout(this);

    driverLayout->addWidget(driverLabel);
    driverLayout->addWidget(driverComboBox);
    serverLayout->addWidget(serverLabel);
    serverLayout->addWidget(serverEdit);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portEdit);
    dbNameLayout->addWidget(dbNameLabel);
    dbNameLayout->addWidget(dbNameEdit);
    usrLayout->addWidget(usrLabel);
    usrLayout->addWidget(usrEdit);
    passwordLayout->addWidget(passwordLable);
    passwordLayout->addWidget(passwordEdit);
    btnLayout->addWidget(connectBtn);
    btnLayout->addWidget(cancelBtn);

    dialogLayout->addLayout(driverLayout);
    dialogLayout->addLayout(serverLayout);
    dialogLayout->addLayout(portLayout);
    dialogLayout->addLayout(dbNameLayout);
    dialogLayout->addLayout(usrLayout);
    dialogLayout->addLayout(passwordLayout);
    dialogLayout->addLayout(btnLayout);
}
int connectDialog::writeCfg()
{
    QFile config(":/cfg/cfg/db_config.cfg");
    QString newConnecter;
    newConnecter += "[DBInfo] ";
    newConnecter += serverEdit->text().append(" ");
    newConnecter += portEdit->text().append(" ");
    newConnecter += dbNameEdit->text().append(" ");
    newConnecter += usrEdit->text();
    if(!config.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<<"open config file faild!!!";
    }
    qDebug()<<"open file successed!!!";

    QTextStream textInput(&config);
    QString lineStr;
    while (!textInput.atEnd())
    {
        lineStr = textInput.readLine();
        if(lineStr.contains(newConnecter))
        {
            qDebug()<<"contains";
            return 0;
        }
    }
    qDebug()<<"not Contains";
    qDebug()<<newConnecter;
    textInput<<newConnecter<<endl;
 //   config.close();
    return 0;
}
int connectDialog::connectToDB()
{
    qDebug()<<"Availiable Drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    qDebug()<<drivers;
    QSqlDatabase db;
    if( driverComboBox->currentText()=="Oracle")
    {
        db = QSqlDatabase::addDatabase("QOCI");
        g_DBDriver = "QOCI";
    }
    else if(driverComboBox->currentText() == "DB2")
    {
        db = QSqlDatabase::addDatabase("QDB2");
        g_DBDriver = "QDB2";
    }
    db.setHostName(this->serverEdit->text());
    db.setPort(this->portEdit->text().toInt());
    db.setDatabaseName(this->dbNameEdit->text());
    db.setUserName(this->usrEdit->text());
    db.setPassword(this->passwordEdit->text());
    g_Server = this->serverEdit->text();
    g_port = this->portEdit->text().toInt();
    g_DBName = this->dbNameEdit->text();
    g_UsrName = this->passwordEdit->text();
    if (db.open()) {
        QMessageBox::information(this,"Connect Success","数据库连接成功",QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
        this->hide();
        return 0;
    }else{
        writeCfg();
        QMessageBox::information(this,"Connect Error","数据库连接失败",QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
        //this->close();
        return -1;
    }
    this->connectBtn->setText("断开");
    return 0;
}
