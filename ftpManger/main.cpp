#include "mainwindow.h"
#include <QApplication>
#include "node.h"
#include <QList>

int main(int argc, char *argv[])
{
    qRegisterMetaType<NodeList>("NodeList");
    qRegisterMetaType<NodePtr>("NodePtr");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
