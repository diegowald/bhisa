#ifndef LOCALFILESMANAGER_H
#define LOCALFILESMANAGER_H

#include <QObject>
#include "ftpmanager.h"

class LocalFilesManager : public QObject
{
    Q_OBJECT
public:
    explicit LocalFilesManager(QObject *parent = 0);
    virtual ~LocalFilesManager();

    void initialize(const QString &url, const QString &user, const QString &password);
    void downloadFile(const QString &remoteDir, const QString &filename);
    void uploadFile(const QString &remoteDir, const QString &filename);
    void deleteFile(const QString &remoteDir, const QString &filename);

    QString getCurrentDirectory();
    void getDirectoryContents(const QString &remoteDir);
    void createDirectory(const QString &remoteDir, const QString &directoryName);
    void deleteDirectory(const QString &remoteDir);
    void changeDirectory(const QString &remoteDir);

signals:
    void fileDownloaded(const QString &remtoeDir, const QString &filename);
    void fileUploaded(const QString &remoteDir, const QString &filename);
    void fileDeleted(const QString &remoteDir, const QString &filename);

    void getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents);
    void directoryCreated(const QString &remoteDir, const QString &directoryName);
    void directoryDeleted(const QString &remoteDir);
    void directoryChanged(const QString &remoteDir);

    void requestInitialize();

public slots:
    void on_fileDownloaded(const QString &remtoeDir, const QString &filename);
    void on_fileUploaded(const QString &remoteDir, const QString &filename);
    void on_fileDeleted(const QString &remoteDir, const QString &filename);

    void on_getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents);
    void on_directoryCreated(const QString &remoteDir, const QString &directoryName);
    void on_directoryDeleted(const QString &remoteDir);
    void on_directoryChanged(const QString &remoteDir);

    void on_requestInitialize();

private:
    void processControlFile(const QString &dir);

    void checkAndCreateFolderIfNotExists(const QString &foldername);

private:
    bool _initialized;
    QString _localRootFolder;
    QString _ftpSessionFolder;
    QString _url;
    QString _user;
    QString _password;
    bool _isWindows;
    FtpManager _ftpManager;

};

#endif // LOCALFILESMANAGER_H
