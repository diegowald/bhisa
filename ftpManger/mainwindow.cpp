#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpmanager.h"
#include "dlglogin.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QFileDialog>
#include "file.h"
#include "folder.h"

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
    item->setData(0, Qt::UserRole, QVariant::fromValue(QPermission::PERMIT::Administrator));
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
    statusBar()->showMessage(tr("File %1 downloaded.").arg(filename), 3000);
    if (_lockAndOpenAfterDownloadFile == filename)
    {
        if (_fileManager.lockFile(remoteDir, filename))
        {
            openFile(remoteDir, filename);
        }
    }
}

void MainWindow::openFile(const QString &remoteDir, const QString &filename)
{
    QDesktopServices::openUrl(_fileManager.getLocalURL(remoteDir, filename));
}

void MainWindow::on_fileUploaded(const QString &remoteDir, const QString &filename)
{
    (void) remoteDir;
    statusBar()->showMessage(tr("File %1 uploaded.").arg(filename), 3000);
}

void MainWindow::on_fileDeleted(const QString &remoteDir, const QString &filename)
{
    (void) remoteDir;
    statusBar()->showMessage(tr("File %1 deleted.").arg(filename), 3000);
}

void MainWindow::on_getDirectoryContentsDownloaded(const QString &remoteDir, NodePtr dirContents)
{
    statusBar()->showMessage(tr("Folder %1 retrieved.").arg(remoteDir), 3000);
    ui->tableWidget->setRowCount(0);
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    qDeleteAll(item->takeChildren());
    FolderPtr parentFolder = qSharedPointerCast<Folder>(dirContents);
    foreach (NodePtr node, *parentFolder->childNodes())
    {
        if (node->isFolder())
        {
            FolderPtr folder = qSharedPointerCast<Folder>(node);
            bool addIt = false;
            QTreeWidgetItem *folderItem = new QTreeWidgetItem(QStringList() << folder->filename());
            folderItem->setData(0, Qt::UserRole, QVariant::fromValue(folder->myPermission(_fileManager.user())));
            switch (folder->myPermission(_fileManager.user()))
            {
            case QPermission::PERMIT::Administrator:
                folderItem->setTextColor(0, Qt::red);
                addIt = true;
                break;
            case QPermission::PERMIT::canWrite:
                folderItem->setTextColor(0, Qt::black);
                addIt = true;
                break;
            case QPermission::PERMIT::canRead:
                folderItem->setTextColor(0, Qt::darkGray);
                addIt = true;
                break;
            case QPermission::PERMIT::none:
            default:
                folderItem->setTextColor(0, Qt::lightGray);
                break;
            }
            if (addIt)
            {
                item->addChild(folderItem);
            }
//            _folderInfo[remoteDir] =
        }
        else
        {
            FilePtr file = qSharedPointerCast<File>(node);
            int currentRow = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(currentRow);
            ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem(file->filename()));
            ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(QString::number(file->size())));
            ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(file->underEditionAuthor()));
            ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(file->underEdition()));
            //ui->tableWidget->item(currentRow, 0)->setBackgroundColor(file->isLocked() ? Qt::red : Qt::white);
            if (file->isLocked())
            {
                if (file->underEditionAuthor() == _fileManager.loggedUser())
                {
                    QFont font = ui->tableWidget->item(currentRow, 0)->font();
                    font.setBold(true);
                    ui->tableWidget->item(currentRow, 0)->setFont(font);
                }
                else
                {
                    ui->tableWidget->item(currentRow, 0)->setForeground(Qt::darkGray);
                }
            }
        }
    }
}

void MainWindow::on_directoryCreated(const QString &remoteDir, const QString &directoryName)
{
    statusBar()->showMessage(tr("Folder %1 created on %2.").arg(directoryName).arg(remoteDir), 3000);
}

void MainWindow::on_directoryDeleted(const QString &remoteDir)
{
    statusBar()->showMessage(tr("Folder %1 deleted.").arg(remoteDir), 3000);
}

void MainWindow::on_directoryChanged(const QString &remoteDir)
{
    statusBar()->showMessage(tr("Folder %1 changed.").arg(remoteDir), 3000);
}

void MainWindow::on_requestInitialize()
{
    statusBar()->showMessage(tr("Request initialization."), 3000);
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
    QPermission::PERMIT permission = item->data(0, Qt::UserRole).value<QPermission::PERMIT>();
    if (permission != QPermission::PERMIT::none)
    {
        // chequear los permisos que tengo
        _fileManager.getDirectoryContents(path(item));
        statusBar()->showMessage(tr("Getting contents of %1 folder").arg(path(item)), 3000);
    }
    else
    {
        statusBar()->showMessage(tr("Forbidden."));
    }
}

void MainWindow::on_actionActualizar_triggered()
{
    statusBar()->showMessage(tr("Refreshing"), 3000);
}

void MainWindow::on_actionTomar_Para_edicion_triggered()
{
    if ((ui->treeWidget->currentItem() != NULL) && (ui->tableWidget->currentRow() >= 0))
    {
        QString folder = path(ui->treeWidget->currentItem());
        QString file = ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text();
        if (!fileLockedByMe())
        {
            _lockAndOpenAfterDownloadFile = file;
            _fileManager.downloadFile(folder, file);
            statusBar()->showMessage(tr("Downloading and locking %1.").arg(file), 3000);
        }
        else
        {
            openFile(folder, file);
        }
    }
}

void MainWindow::on_actionFin_Edicion_triggered()
{
    qDebug() << ui->tableWidget->currentRow();
    if ((ui->treeWidget->currentItem() != NULL) && (ui->tableWidget->currentRow() >= 0))
    {
        QString folder = path(ui->treeWidget->currentItem());
        QString file = ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text();
        if (fileLockedByMe())
        {
            _fileManager.uploadFile(folder, file);
        }
    }
}

bool MainWindow::fileLockedByMe() const
{
    qDebug() << ui->tableWidget->currentRow();
    int row = ui->tableWidget->currentRow();
    bool isLocked =  ui->tableWidget->item(row, 3)->text() == tr("Yes");
    bool author = ui->tableWidget->item(row, 2)->text().toLower() == _fileManager.loggedUser().toLower();
    return isLocked && author;
}

void MainWindow::on_actionExportar_triggered()
{
    if (ui->treeWidget->currentItem() != NULL)
    {
        qDebug() << ui->treeWidget->currentItem()->text(0);
        QString remoteFolder = path(ui->treeWidget->currentItem());
        QString localFolder = QFileDialog::getExistingDirectory(this, tr("Select folder"));
        qDebug() << localFolder;
        if (localFolder.length() > 0)
        {
            _fileManager.exportFolder(remoteFolder, localFolder);
        }
    }
}

void MainWindow::on_actionImportar_triggered()
{
    if (ui->treeWidget->currentItem() != NULL)
    {
        qDebug() << ui->treeWidget->currentItem()->text(0);
        QString remoteFolder = path(ui->treeWidget->currentItem());
        QString localFolder = QFileDialog::getExistingDirectory(this, tr("Select folder"));
        qDebug() << localFolder;
        if (localFolder.length() > 0)
        {
            _fileManager.importFolder(localFolder, remoteFolder);
        }
    }
}

void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    (void) item;
    on_actionTomar_Para_edicion_triggered();
}
