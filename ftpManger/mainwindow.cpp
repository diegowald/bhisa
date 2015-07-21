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

    connect(&_ftpManager, &FtpManager::getDirectoryContentsDownloaded, this, &MainWindow::on_getDirectoryContentsDownloaded);
    connect(&_ftpManager, SIGNAL(directoryCreated(QString,QString)), this, SLOT(on_directoryCreated(QString,QString)));
    connect(&_ftpManager, SIGNAL(directoryDeleted(QString)), this, SLOT(on_directoryDeleted(QString)));
    connect(&_ftpManager, SIGNAL(directoryChanged(QString)), this, SLOT(on_directoryChanged(QString)));

    QString _currentDir = _ftpManager.getCurrentDirectory();
    ui->treeWidget->clear();
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << _currentDir);
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item);
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

void MainWindow::on_getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    qDeleteAll(item->takeChildren());
    foreach (FilePtr file, *dirContents)
    {
        if (file->isFolder())
        {
            item->addChild(new QTreeWidgetItem(QStringList() << file->filename()
                                               << file->permissions()
                                               << file->owner()
                                               << file->underEdition()));
        }
        else
        {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(file->filename()));
        }
    }
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

QString MainWindow::path(QTreeWidgetItem *item)
{
    if (item->parent() == NULL)
        return item->text(0);
    return path(item->parent()) + "/" + item->text(0);
}

void MainWindow::on_treeWidget_itemSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidget->selectedItems().at(0);
    _ftpManager.getDirectoryContents(path(item));
}

void MainWindow::on_actionActualizar_triggered()
{

}
