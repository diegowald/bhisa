#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include "file.h"


class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(QObject *parent = 0);
    virtual ~FtpManager();

    void initialize(const QString &url, const QString &user, const QString &password);
    void downloadFile(const QString &remoteDir, const QString &filename, const QString &localFolder, bool blockingCall);
    void uploadFile(const QString &remoteDir, const QString &filename, const QString &localFilename, bool blockingCall);
    void deleteFile(const QString &remoteDir, const QString &filename);

    QString getCurrentDirectory();
    void getDirectoryContents(const QString &remoteDir, const QString &localFolder, bool blockingCall);
    void createDirectory(const QString &remoteDir, const QString &directoryName);
    void deleteDirectory(const QString &remoteDir);
    void changeDirectory(const QString &remoteDir);

    bool fileExists(const QString &remoteDir, const QString &filename);

signals:
    void fileDownloaded(const QString &remtoeDir, const QString &filename);
    void fileUploaded(const QString &remoteDir, const QString &filename);
    void fileDeleted(const QString &remoteDir, const QString &filename);

    void getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents, const QString &localFolder);
    void directoryCreated(const QString &remoteDir, const QString &directoryName);
    void directoryDeleted(const QString &remoteDir);
    void directoryChanged(const QString &remoteDir);

    void requestInitialize();

public slots:
private:
    static void internal_downloadFile(FtpManager *ftpManager, const QString &remtoeDir, const QString &filename, const QString &localFolder);
    static void internal_uploadFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename, const QString &localFilename);
    static void internal_deleteFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename);
    static void internal_getDirectoryContents(FtpManager *ftpManager, const QString &remoteDir, const QString &localFolder);
    static bool internal_FileExists(FtpManager *ftpManager, const QString &remoteDir, const QString &filename);

    static void stream_copy_n(std::istream & in, std::size_t count, std::ostream & out);

    static FileList parseDirectoryContents(std::ostringstream &content, bool isWindows);
    static FileList parseDirectoryContentsLinux(std::ostringstream &content);
    static FileList parseDirectoryContentsWindows(std::ostringstream &content);

    void gatherServerType();

private:
    bool _initialized;
    QString _url;
    QString _user;
    QString _password;
    bool _isWindows;
};

#endif // FTPMANAGER_H
