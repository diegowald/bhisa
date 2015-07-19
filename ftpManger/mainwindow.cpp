#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FtpManager ftp("ftp://127.0.0.1", "diego", "mic1492");

}

MainWindow::~MainWindow()
{
    delete ui;
}
