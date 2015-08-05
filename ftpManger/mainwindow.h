#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "localfilesmanager.h"
#include <QTreeWidgetItem>
#include <QTableWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_fileDownloaded(const QString &remoteDir, const QString &filename);
    void on_fileUploaded(const QString &remoteDir, const QString &filename);
    void on_fileDeleted(const QString &remoteDir, const QString &filename);

    void on_getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents);
    void on_directoryCreated(const QString &remoteDir, const QString &directoryName);
    void on_directoryDeleted(const QString &remoteDir);
    void on_directoryChanged(const QString &remoteDir);

    void on_requestInitialize();

private slots:
    void on_treeWidget_itemSelectionChanged();
    void on_actionActualizar_triggered();
    void on_actionTomar_Para_edicion_triggered();
    void on_actionFin_Edicion_triggered();

    void on_actionExportar_triggered();

    void on_actionImportar_triggered();

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    QString path(QTreeWidgetItem *item);
    bool fileLockedByMe() const;
    void openFile(const QString &remoteDir, const QString &filename);

private:
    Ui::MainWindow *ui;
    LocalFilesManager _fileManager;
    QString _lockAndOpenAfterDownloadFile;
};

#endif // MAINWINDOW_H
