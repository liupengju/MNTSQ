#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
