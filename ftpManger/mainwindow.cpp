#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpmanager.h"
#include "dlglogin.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&_fileManager, &LocalFilesManager::requestInitialize, this, &MainWindow::on_requestInitialize);
    connect(&_fileManager, &LocalFilesManager::fileDownloaded, this, &MainWindow::on_fileDownloaded);
    connect(&_fileManager, &LocalFilesManager::fileUploaded, this, &MainWindow::on_fileUploaded);
    connect(&_fileManager, &LocalFilesManager::fileDeleted, this, &MainWindow::on_fileDeleted);

    connect(&_fileManager, &LocalFilesManager::getDirectoryContentsDownloaded, this, &MainWindow::on_getDirectoryContentsDownloaded);
    connect(&_fileManager, &LocalFilesManager::directoryCreated, this, &MainWindow::on_directoryCreated);
    connect(&_fileManager, &LocalFilesManager::directoryDeleted, this, &MainWindow::on_directoryDeleted);
    connect(&_fileManager, &LocalFilesManager::directoryChanged, this, &MainWindow::on_directoryChanged);

    QString _currentDir = _fileManager.getCurrentDirectory();
    ui->treeWidget->clear();
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << _currentDir);
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item);
    ui->tableWidget->setAlternatingRowColors(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileDownloaded(const QString &remoteDir, const QString &filename)
{
    if (_lockAndOpenAfterDownloadFile == filename)
    {
        if (_fileManager.lockFile(remoteDir, filename))
        {
            QDesktopServices::openUrl(_fileManager.getLocalURL(remoteDir, filename));
        }
    }
}

void MainWindow::on_fileUploaded(const QString &remoteDir, const QString &filename)
{
}

void MainWindow::on_fileDeleted(const QString &remoteDir, const QString &filename)
{
}

void MainWindow::on_getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents)
{
    ui->tableWidget->setRowCount(0);
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    qDeleteAll(item->takeChildren());
    foreach (FilePtr file, *dirContents)
    {
        if (file->isFolder())
        {
            item->addChild(new QTreeWidgetItem(QStringList() << file->filename()));
        }
        else
        {
            int currentRow = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(currentRow);
            ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem(file->filename()));
            ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(QString::number(file->size())));
            ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(file->owner()));
            ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(file->underEdition()));
            //ui->tableWidget->item(currentRow, 0)->setBackgroundColor(file->isLocked() ? Qt::red : Qt::white);
            ui->tableWidget->item(currentRow, 0)->font().setBold(file->isLocked());
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
        _fileManager.initialize(dlg.url(), dlg.user(), dlg.password());
    }
    setWindowTitle("File Manager - " + dlg.url());
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
    _fileManager.getDirectoryContents(path(item));
}

void MainWindow::on_actionActualizar_triggered()
{

}

void MainWindow::on_actionTomar_Para_edicion_triggered()
{
    qDebug() << ui->tableWidget->currentRow();
    if ((ui->treeWidget->currentItem() != NULL) && (ui->tableWidget->currentRow() >= 0))
    {
        QString folder = ui->treeWidget->currentItem()->text(0);
        QString file = ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text();
        _lockAndOpenAfterDownloadFile = file;
        _fileManager.downloadFile(folder, file);
    }
}
