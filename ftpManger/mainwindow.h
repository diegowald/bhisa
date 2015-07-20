#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ftpmanager.h"

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
    void on_fileDownloaded(const QString &remtoeDir, const QString &filename);
    void on_fileUploaded(const QString &remoteDir, const QString &filename);
    void on_fileDeleted(const QString &remoteDir, const QString &filename);

    void on_getDirectoryContentsDownloaded(const QString &remoteDir, QList<FilePtr> &dirContents);
    void on_directoryCreated(const QString &remoteDir, const QString &directoryName);
    void on_directoryDeleted(const QString &remoteDir);
    void on_directoryChanged(const QString &remoteDir);

    void on_requestInitialize();

private:
    Ui::MainWindow *ui;
    FtpManager _ftpManager;
};

#endif // MAINWINDOW_H
