#include "mainwindow.h"
#include <QApplication>
#include "file.h"
#include <QList>

int main(int argc, char *argv[])
{
    qRegisterMetaType<FileList>("FileList");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
