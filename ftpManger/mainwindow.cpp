#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpmanager.h"
#include "dlglogin.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&_ftpManager, SIGNAL(requestInitialize()), this, SLOT(on_requestInitialize()));
    connect(&_ftpManager, SIGNAL(fileDownloaded(QString,QString)), this, SLOT(on_fileDownloaded(QString,QString)));
    connect(&_ftpManager, SIGNAL(fileUploaded(QString,QString)), this, SLOT(on_fileUploaded(QString,QString)));
    connect(&_ftpManager, SIGNAL(fileDeleted(QString,QString)), this, SLOT(on_fileDeleted(QString,QString)));

    connect(&_ftpManager, SIGNAL(getDirectoryContentsDownloaded(QString,QList<FilePtr>&)), this, SLOT(on_getDirectoryContentsDownloaded(QString,QList<FilePtr>&)));
    connect(&_ftpManager, SIGNAL(directoryCreated(QString,QString)), this, SLOT(on_directoryCreated(QString,QString)));
    connect(&_ftpManager, SIGNAL(directoryDeleted(QString)), this, SLOT(on_directoryDeleted(QString)));
    connect(&_ftpManager, SIGNAL(directoryChanged(QString)), this, SLOT(on_directoryChanged(QString)));

//    _ftpManager.initialize("127.0.0.1", "diego", "mic1492");
    this->_ftpManager.downloadFile("~/tmp/", "test.txt");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileDownloaded(const QString &remtoeDir, const QString &filename)
{
}

void MainWindow::on_fileUploaded(const QString &remoteDir, const QString &filename)
{
}

void MainWindow::on_fileDeleted(const QString &remoteDir, const QString &filename)
{
}

void MainWindow::on_getDirectoryContentsDownloaded(const QString &remoteDir, QList<FilePtr> &dirContents)
{
}

void MainWindow::on_directoryCreated(const QString &remoteDir, const QString &directoryName)
{
}

void MainWindow::on_directoryDeleted(const QString &remoteDir)
{
}

void MainWindow::on_directoryChanged(const QString &remoteDir)
{
}

void MainWindow::on_requestInitialize()
{
    DlgLogin dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        _ftpManager.initialize(dlg.url(), dlg.user(), dlg.password());
    }
}
