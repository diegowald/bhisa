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

    QString loggedUser() const;

    void initialize(const QString &url, const QString &user, const QString &password);
    void downloadFile(const QString &remoteDir, const QString &filename);
    void uploadFile(const QString &remoteDir, const QString &filename);
    void deleteFile(const QString &remoteDir, const QString &filename);

    QString getCurrentDirectory();
    void getDirectoryContents(const QString &remoteDir);
    void createDirectory(const QString &remoteDir, const QString &directoryName);
    void deleteDirectory(const QString &remoteDir);
    void changeDirectory(const QString &remoteDir);

    bool lockFile(const QString &remoteDir, const QString &filename);
    QUrl getLocalURL(const QString &remoteDir, const QString &filename);

    void exportFolder(const QString &remoteFolder, const QString &localFolder);
    void importFolder(const QString &localFolder, const QString &remoteFolder);

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
    void on_requestInitialize();

private:
    void processControlFile(const QString &dir, const QString &filename, QString &editor, qint64 &since);
    QPermissionList processPermissionsFile(const QString &remoteDir, const QString &filename);

    void checkAndCreateFolderIfNotExists(const QString &foldername);

    bool isControlFile(const QString &filename);
    bool isPermissionsFile(const QString &filename);
    bool isUsersListFile(const QString &filename);

    QString extractFileNameFromControlFile(const QString &controlFileName);
    QString extractFileNameFromPermissionsFile(const QString &filename);
    QString getControlFileName(const QString &filename);
    QString localPath(const QString &remotePath, const QString filename);

private:

    void internal_exportFolder(const QString &remoteFolder, const QString &localFolder);
    void internal_importFolder(const QString &localFolder, const QString &remoteFolder, const QString &localRootFolder = "-");

    void internal_downloadFile(const QString &remoteDir, const QString &filename);
    void internal_uploadFile(const QString &remoteDir, const QString &filename);
    void internal_deleteFile(const QString &remoteDir, const QString &filename);

    FileList internal_getDirectoryContents(const QString &remoteDir, const QString &localFolder);
    void internal_createDirectory(const QString &remoteDir, const QString &directoryName);
    void internal_deleteDirectory(const QString &remoteDir);
    void internal_changeDirectory(const QString &remoteDir);

private:
    bool _initialized;
    QString _localRootFolder;
    QString _ftpSessionFolder;
    QString _url;
    QString _user;
    QString _password;
    bool _isWindows;
    FtpManager _ftpManager;
    QString _exportFolder;
    QString _importFolder;
};

#endif // LOCALFILESMANAGER_H
