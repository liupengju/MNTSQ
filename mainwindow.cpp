#include "mainwindow.h"
#include "ui_mainwindow.h"


QString g_DBDriver = "QOCI";
QString g_Server = "localhost";
int g_port = 1521;
QString g_DBName = "ORCL";
QString g_UsrName = "C##ZZFKJ";
QString g_PassWord = "zzfkj123";

//  MainWindow::MainWindow(QWidget *parent) :
//    QMainWindow(parent),
//    ui(new Ui::MainWindow)
  MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent)
{
    this->setMinimumSize(600,500);
    mainPath = QDir::currentPath();
    qDebug()<<"mainpath"<<mainPath;

//    ui->setupUi(this);
    setUi();
    setTableHead();
//    trancodeEdit = new QLineEdit(this);

    connecter = new connectDialog();
    this->setToolBar();
    tcpSocket = new QTcpSocket(this);
    statusBar()->showMessage("应用程序打开成功");
    //测试该函数用
    //importMessage();
    isConnect = false;
}
void MainWindow::setUi()
{
    centralWidget = new QWidget;
//    mainSplitter = new QSplitter(Qt::Horizontal,centralWidget);
//    leftSplitter = new QSplitter(Qt::Vertical,mainSplitter);
//    rightSplitter = new QSplitter(Qt::Vertical,mainSplitter);
    trancodeLabel = new QLabel("交易码",this);
    trancodeEdit = new QLineEdit(this);
    trancodeEdit->setText("C41003_客户信息查询");
    importMessageBtn = new QPushButton("导入");
    exportMessageBtn = new QPushButton("导出");
    connect(importMessageBtn,SIGNAL(clicked(bool)),this,SLOT(importMessage()));
    connect(trancodeEdit,SIGNAL(returnPressed()),this,SLOT(importTempMessage()));
    connect(exportMessageBtn,SIGNAL(clicked(bool)),this,SLOT(exportMessage()));
    setCodeEditData();
    trancodeLayout = new QHBoxLayout();
    trancodeLayout->addWidget(trancodeLabel);
    trancodeLayout->addWidget(trancodeEdit);
    trancodeLayout->addWidget(importMessageBtn);
    trancodeLayout->addWidget(exportMessageBtn);
    leftLayout = new QVBoxLayout;
    leftLayout->addLayout(trancodeLayout);
    table = new QTableWidget(80,3,this);
    leftLayout->addWidget(table);
    // leftSplitter->addWidget(table);

    rightBtnLayout = new QHBoxLayout();
    sendBtn = new QPushButton("发送",this);
    connect(sendBtn,SIGNAL(clicked(bool)),this,SLOT(sendMessage()));
    generateBtn = new QPushButton("生成报文",this);
    rightBtnLayout->addWidget(generateBtn);
    connect(generateBtn,SIGNAL(clicked(bool)),this,SLOT(setMessageEdit()));
    rightBtnLayout->addWidget(sendBtn);
    rightLayout = new QVBoxLayout;
    sendEdit = new QTextEdit(this);
    sendEdit->setReadOnly(true);
    messageEdit = new QTextEdit(this);



    rightLayout->addWidget(sendEdit);
    rightLayout->addWidget(messageEdit);
    rightLayout->addLayout(rightBtnLayout);

    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
}
int MainWindow::connectToDb()
{
    qDebug()<<"connectDB......";
    db = QSqlDatabase::addDatabase(g_DBDriver);
    db.setHostName(g_Server);
    db.setPort(g_port);
    db.setDatabaseName(g_DBName);
    db.setUserName(g_UsrName);
    db.setPassword(g_PassWord);

    if (db.open()) {
        qDebug()<<"connect!!!!!!!";
        //this->hide();
        return 0;
    }else{
        QMessageBox::information(this,"Connect Error","数据库连接失败",QMessageBox::No|QMessageBox::Yes,QMessageBox::Yes);
        //this->close();
        return -1;
    }
    return 0;
}
int MainWindow::setCodeEditData()
{
    this->connectToDb();
    QSqlQuery query;
    query.exec("select trncode, trnname from m01_trncode");
    qDebug()<<db.userName();
    qDebug()<<db.password();
    while(query.next())
    {
        QString trncode = query.value(0).toString()+"_"+query.value(1).toString();
        trancodeList<<trncode;
    }
    trancodeComPleter = new QCompleter(this->trancodeList,0);
    trancodeComPleter->setCaseSensitivity(Qt::CaseInsensitive);
    trancodeEdit->setCompleter(trancodeComPleter);
    db.close();
    return 0;
}
void MainWindow::setTableHead()
{
    //table = new QTableWidget(80,3,this);
    QStringList headerList;
    headerList<<"节点名称"<<"节    点"<<"数    据";
    table->setHorizontalHeaderLabels(headerList);
    qDebug()<<"setTableData";
    setTableData();
}
int MainWindow::setTableData()
{
    if(!db.isOpen())
        return 0;
    exportTempMessage();//将当前报文保存到临时文件
    message.clear();
    QString PackType_table[]
    {
        "",
        "m03_fmt_xml",           //1,单层节点XML格式报文
        "m03_fmt_nsp",           //2,定长无分隔符报文
        "",
        "",
        "",
        "",
        "",
        "m03_fmt_xml_multi",     //8，XML_ESB报文格式
        ""
    };
    //暂时送定值
   // trancodeEdit->setText("C41003_客户信息查询交易");
    QString trancode = trancodeEdit->text();
    qDebug()<<"trancodeEdit"<<trancodeEdit->text();
    trancode.truncate(trancode.indexOf('_'));
    qDebug()<<"trancode"<<trancode;
    sendEdit->setText(trancode);
    this->connectToDb();
    QSqlQuery query;
    int count;

    //获取接入渠道号
    query.exec(QString("select srcmq FROM m03_trncodeextint "
                       "WHERE trncode = '%1'").arg(trancode));
    query.next();
    int MQ_ID = query.value(0).toInt();

    //获取脚本号。
    query.exec(QString("select fmtsptid from M03_fmt_sel where trncode = '%1' and "
                       "mq_id = %2 and msgclass =11").arg(trancode).arg(MQ_ID));
    query.next();
    int fmtSptid = query.value(0).toInt();

    //获取脚本对应的报文格式类型。
    query.exec(QString("select packtype from m03_chnl WHERE mq_id = %1").arg(MQ_ID));
    query.next();
    int packType = query.value(0).toInt();
    //getMsgType--->获取报文类型，以确定在那个表中找文件以及建立报文的规则
        //从M03_FMT_DEF获取报文类型【SPT_TYPE】根据脚本号【SPTSTAU】和渠道号【MQ_ID】获取
    query.exec(QString("select count(*) from %1 WHERE mq_id = %2 and "
                       "fmtsptid = %3").arg(PackType_table[packType]).arg(MQ_ID).arg(fmtSptid));
    query.next();
    count = query.value(0).toInt();
    qDebug()<<"count"<<count;
    table->setRowCount(count);
    query.exec(QString("select fldname, pathbuf from %1 WHERE mq_id = %2"
                       "and fmtsptid=%3").arg(PackType_table[packType]).arg(MQ_ID).arg(fmtSptid));
    qDebug()<<PackType_table[packType];
    table->clearContents();
    int i = 0;
    QString fldname;
    QString pathbuf;
    while(query.next())
    {
        fldname = query.value(0).toString().trimmed();
        pathbuf = query.value(1).toString().trimmed();
        QTableWidgetItem *nameitem = new QTableWidgetItem(fldname,QTableWidgetItem::Type);
        QTableWidgetItem *bufitem = new QTableWidgetItem(pathbuf,QTableWidgetItem::Type);
        table->setItem(i,0,nameitem);
        table->closePersistentEditor(nameitem);
        table->setItem(i,1,bufitem);
        table->closePersistentEditor(bufitem);
        i++;
      //  table->setItem(0,i++,);
    }
    tempfilename = trancodeEdit->text();
    return 0;
}

void MainWindow::setToolBar()
{
    openAct = new QAction(QIcon(":/icon/open.ico"),"打开",this);
    connectAct = new QAction(QIcon(":/icon/connect.ico"),"连接",this);

    mainBar = new QToolBar("mainBar");
    mainBar->addAction(openAct);
    mainBar->addAction(connectAct);

    connect(connectAct,SIGNAL(triggered(bool)),this->connecter,SLOT(show()));

    IpEdit = new QLineEdit("IP");
    portEdit = new QLineEdit("Port");
    connectBtn = new QPushButton("连接");
    connect(connectBtn,SIGNAL(clicked(bool)),this,SLOT(connectToServer()));
    IpBar = new QToolBar("IpBar");
    IpBar->addWidget(IpEdit);
    IpBar->addWidget(portEdit);
    IpBar->addWidget(connectBtn);

    this->addToolBar(mainBar);
    this->addToolBar(IpBar);
}
int MainWindow::getLayer(QString data)
{
    int i = 0;
    int layer = 0;
    while(data[i] != '\0')
    {
        if(data[i] == '/')
            layer++;
        i++;
    }
    return layer;
}
void MainWindow::createXmlMessage()//生成XML报文<多层节点和单层节点>
{
    QString messagesize;
    int currentLayer = 0;
    int lastLayer = 0;
    int count = table->rowCount();
    int i = 1;
    message = "<?xml version=\"1.0\" encoding=\"GB2312\"?>";
    message += '\n';
    message += "<root>\n";
    QString dataString;
    QStringList headlist;
    while (i < count-1)
    {
        if( table->item(i,1)!=NULL )
            dataString = table->item(i,1)->text();
        QString data = NULL;
        currentLayer = getLayer(dataString);
        while(lastLayer < currentLayer)
        {
            data = dataString.section('/',lastLayer,lastLayer);
            for(int k = 0;k<lastLayer+1;k++)
            {
                message += "    ";
            }
            message += '<';
            message += data;
            message += '>';
            message += '\n';
            lastLayer ++;
            headlist<<data;
        }
        while(lastLayer>currentLayer)
        {
            for(int k = 0;k<lastLayer;k++)
            {
                message += "    ";
            }
            message += "</";
            message += headlist.takeLast();
            message += '>';
            message += '\n';
            if(lastLayer == currentLayer)
                break;
            lastLayer --;
        }
        if(currentLayer == lastLayer)
        {
            data = dataString.section('/',lastLayer,lastLayer);
            for(int k = 0;k<lastLayer+1;k++)
            {
                message += "    ";
            }
            if(table->item(i,2)!=NULL && table->item(i,2)->text()!=NULL)
            {
                message += '<';
                message += data;
                message += '>';
                message += table->item(i,2)->text();
                message += "</";
                message += data;
                message += '>';
                message += '\n';
            }
            else
            {
                message += '<';
                message += data;
                message += "/>";
                message += '\n';
            }
        }
        while( (i == count-2)&& (!headlist.isEmpty()))
        {
            currentLayer = 0;
            for(int k = 0;k<lastLayer;k++)
            {
                message += "    ";
            }
            message += "</";
            message += headlist.takeLast();
            message += '>';
            message += '\n';
            if(lastLayer == currentLayer)
                break;
            lastLayer --;
        }
        i++;
    }
    message += "</root>";
    messagesize.setNum(message.size());
    message.push_front(messagesize.rightJustified(6,'0'));
}
void MainWindow::setMessageEdit()
{
    createXmlMessage();
    messageEdit->setText(message);
}
//定长报文格式：
//左补零
//定长报文的显示中添加字段长度
//要不要在显示中添加字段中文名称
//void MainWindow::createNSPMsg()
//{
//    QStringList headerList;
//    headerList<<"字段名称"<<"长   度"<<"数    据";
//    table->setHorizontalHeaderLabels(headerList);
//    setTableData();



//    QString messagesize;
//    int count = table->rowCount();
//    int i = 1;
//    message = "<?xml version=\"1.0\" encoding=\"GB2312\"?>";
//    message += '\n';
//    message += "<root>\n";
//    QString dataString;
//    while(i < count -1)
//    {
//        if( table->item(i,1)!=NULL )
//                dataString = table->item(i,1)->text();
//        message.append(dataString);
//    }
//    message += "</root>";
//    messagesize.setNum(message.size());
//    message.push_front(messagesize.rightJustified(6,'0'));
//    messageEdit->setText(message);
//}

void MainWindow::connectToServer()
{
    QString sndBtnStatus = "断开";
    if(connectBtn->text() == sndBtnStatus)
    {
        tcpSocket->abort();
        this->statusBar()->showMessage("连接已断开");
        connectBtn->setText("连接");
        return ;
    }
    quint16 blockSize = 0;
    tcpSocket->connectToHost(IpEdit->text(),portEdit->text().toInt());
 //   if(tcpSocke)
    statusBar()->showMessage("正在连接服务器...");
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(acceptConnection()));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(disConnection()));
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(dispathError(QAbstractSocket::SocketError)));
//    tcpSocket->abort();

    qDebug()<<tcpSocket->ConnectedState;
}
void MainWindow::receiveData()
{
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(255,100,0));
    QTextCursor cur = sendEdit->textCursor();
    cur.movePosition(QTextCursor::End);
    qint64 len = 0;
    while((len = tcpSocket->bytesAvailable()) != 0)
    {
        QByteArray recData;
       // recData = tcpSocket->readLine();
        recData = tcpSocket->readAll();
        QString msg = QString::fromLocal8Bit(recData);

        QDateTime curTime = QDateTime::currentDateTime();
        QString showMsg = tr("服务器") + curTime.toString("<<hh:mm:ss>>") + "\n    " + msg;
        sendEdit->setTextColor(QColor(255,69,0));
        sendEdit->append(showMsg);
        cur.movePosition(QTextCursor::End);
    }
}


MainWindow::~MainWindow()
{
    exportTempMessage();
    delete ui;
    this->connecter->close();
    delete connecter;

}

void MainWindow::acceptConnection()
{
    statusBar()->showMessage("成功连接到服务器...");
    connectBtn->setText("断开");
    isConnect = true;
}

void MainWindow::disConnection()
{
    statusBar()->showMessage("TCP连接已经断开");
    this->connectBtn->setText("连接");
    isConnect = false;
}

void MainWindow::sendMessage()
{
    QTextCursor cur = sendEdit->textCursor();
    cur.movePosition(QTextCursor::End);
    if(isConnect)
    {
        QString msg = messageEdit->toPlainText().trimmed();
        if(!msg.isEmpty())
        {
            tcpSocket->write(msg.toLocal8Bit());
            //
            QDateTime curTime = QDateTime::currentDateTime();
            QString showMsg = tr("客户端:") + curTime.toString("<<hh:mm:ss>>") + "\n    " + msg;
            sendEdit->setTextColor(QColor(0,206,209));
            sendEdit->append(showMsg);
            cur.movePosition(QTextCursor::End);
        }
        else
        {
            QMessageBox::warning(NULL,NULL,tr("发送消息不能为空！"));
        }
    }
    else
    {
        QMessageBox::information(NULL,NULL,tr("客户端未连接！"));
    }
}

void MainWindow::exportTempMessage()
{
   if(message.isNull())
   {
        createXmlMessage();
   }
   QString tempMessage = message;
   tempMessage.push_front(">>\n\n");
   tempMessage.push_front(tempfilename);
   tempMessage.push_front(QString("tranCode <<"));
   qDebug()<<"tempFIleName "<<tempfilename;
   qDebug()<<tempMessage;
   QString path = mainPath + "/Message/tmp/";
   qDebug()<<path;
   QString fileFullName = path + tempfilename;
   qDebug()<<"fileFullName "<< fileFullName;
   QDir dir;
   if(!dir.exists(path))
       dir.mkpath(path);

   qDebug()<<dir.currentPath();
   QFile tempMsg(fileFullName);
   if(!tempMsg.open(QIODevice::WriteOnly | QIODevice::Text))
   {
       qDebug()<<"open file tempMsg faild";
   }
   QTextStream out(&tempMsg);
   out.setCodec("utf-8");
   out<<tempMessage;
   tempMsg.close();
 //  exportMessage();
}

void MainWindow::exportMessage()
{
    if(message.isNull())
    {
         createXmlMessage();
    }
    QString tempMessage = message;
    tempMessage.push_front(">>\n\n");
    tempMessage.push_front(tempfilename);
    tempMessage.push_front(QString("tranCode <<"));
    QFileDialog saveDialog;
    QString filename = saveDialog.getSaveFileName(this, tr("Save File"),
                               mainPath+"/Message/tmp",
                               tr("ALL(*)"));
    qDebug()<<"filename  "<<filename;
    QFile messageFile(filename);
    if( !messageFile.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        QMessageBox::warning(NULL,NULL,"保存文件失败");
    }
    QTextStream out(&messageFile);
    out.setCodec("utf-8");
    out<<tempMessage;
    messageFile.close();
}

void MainWindow::importMessage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/readtest.txt", tr("ALL(*)"));
    QFile messageFile(fileName);
    if(!messageFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(NULL,NULL,"打开文件失败");
    }
    QTextStream in(&messageFile);

   // QByteArray str;
    QString str;
    messageFile.seek(0);
     int i = 0;
    while(!messageFile.atEnd())
    {
        str = messageFile.readLine();
         //获取交易码
        if(str.left(8) == "tranCode")
        {
            str.truncate(str.indexOf('>'));
            qDebug()<<"Str 1"<<str;
            qDebug()<<"str 2"<<str.right(str.size() - str.indexOf('<')-2);
            this->trancodeEdit->setText(str.right(str.size() - str.indexOf('<') -2 ));
            this->setTableHead();
        }
        //获取需要在table组件中显示的行
        if((str.indexOf('/') - str.indexOf('<')) > 1)
        {
            //该行中只有字段名称没有字段数据
            if(str.indexOf('>') - str.indexOf('/') != 1)
            {/*
                str = str.right(str.size() - str.indexOf('<') - 1);
                str = str.left(str.indexOf('/'));
                str.push_front("ALL :");
                sendEdit->append(str);
            }
            else
            {*/
                //该行中既包含字段名也包含数据
                QString name = str;
                QString data = str;
                name = name.right(name.size() - name.indexOf('<') - 1);
                name = name.left(name.indexOf('>'));

                data = data.right(data.size() - data.indexOf('>') - 1);
                data = data.left(data.indexOf('<'));
                //获取name在table中所处的行数
                QString tableText;
                while( table->item(i,1)->text() != "END")
                {
                    tableText = table->item(i,1)->text();
                    while(tableText.contains('/'))
                    {
                        tableText = tableText.right(tableText.size() - tableText.indexOf('/') - 1);
                        qDebug()<<"tableText"<<tableText;
                    }
                    if(name == tableText)
                        break;
                    i++;
                }
                QTableWidgetItem *dataitem = new QTableWidgetItem(data,QTableWidgetItem::Type);
                table->setItem(i,2,dataitem);
              //  table->setItem(0,i++,);
            }
        }
    }
    messageFile.close();
}


void MainWindow::importTempMessage()
{
    QString filename = QString("%1").arg(trancodeEdit->text());
    qDebug()<<QDir::currentPath();
    QString path = mainPath + "/Message/tmp/";
    filename = path + filename;
    qDebug()<<"fileFullName:"<<filename;
    QFile tempMsg(filename);

    tempMsg.open(QIODevice::ReadOnly | QIODevice::Text);

    QDir dir;
    dir.setCurrent(path);
    qDebug()<<dir.currentPath();
    if(tempMsg.exists())
    {
        qDebug()<<"file exits!!!";
        qDebug()<<tempMsg.fileName();
        /*重复代码*/

        QTextStream in(&tempMsg);

       // QByteArray str;
        QString str;
        tempMsg.seek(0);
         int i = 0;
        while(!tempMsg.atEnd())
        {
            str = tempMsg.readLine();
             //获取交易码
            if(str.left(8) == "tranCode")
            {
                str.truncate(str.indexOf('>'));
                qDebug()<<"Str 1"<<str;
                qDebug()<<"str 2"<<str.right(str.size() - str.indexOf('<')-2);
                this->trancodeEdit->setText(str.right(str.size() - str.indexOf('<') -2 ));
                this->setTableHead();
            }
            //获取需要在table组件中显示的行
            if((str.indexOf('/') - str.indexOf('<')) > 1)
            {
                //该行中只有字段名称没有字段数据
                if(str.indexOf('>') - str.indexOf('/') != 1)
                {
                    QString name = str;
                    QString data = str;
                    name = name.right(name.size() - name.indexOf('<') - 1);
                    name = name.left(name.indexOf('>'));

                    data = data.right(data.size() - data.indexOf('>') - 1);
                    data = data.left(data.indexOf('<'));
                    //获取name在table中所处的行数
                    QString tableText;
                    while( table->item(i,1)->text() != "END")
                    {
                        tableText = table->item(i,1)->text();
                        while(tableText.contains('/'))
                        {
                            tableText = tableText.right(tableText.size() - tableText.indexOf('/') - 1);
                            qDebug()<<"tableText"<<tableText;
                        }
                        if(name == tableText)
                            break;
                        i++;
                    }
                    QTableWidgetItem *dataitem = new QTableWidgetItem(data,QTableWidgetItem::Type);
                    table->setItem(i,2,dataitem);
                  //  table->setItem(0,i++,);
                }
            }
        }
        tempMsg.close();
    }
    else
    {
        setTableHead();
        qDebug()<<"file not exit";
        qDebug()<<path<<":"<<tempMsg.fileName();
    }
    dir.setCurrent("../..");
}
