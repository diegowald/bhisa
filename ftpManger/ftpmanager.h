#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include "node.h"


class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(QObject *parent = 0);
    virtual ~FtpManager();

    void initialize(const QString &url, const QString &user, const QString &password);
    bool downloadFile(const QString &remoteDir, const QString &filename, const QString &localFolder);
    bool uploadFile(const QString &remoteDir, const QString &filename, const QString &localFilename);
    bool deleteFile(const QString &remoteDir, const QString &filename);

    QString getCurrentDirectory();
    NodeList getDirectoryContents(const QString &remoteDir, const QString &localFolder);
    bool createDirectory(const QString &remoteDir, const QString &directoryName);
    bool deleteDirectory(const QString &remoteDir);
    bool changeDirectory(const QString &remoteDir);

    bool fileExists(const QString &remoteDir, const QString &filename);

signals:
    void requestInitialize();

public slots:
private:

    static void stream_copy_n(std::istream & in, std::size_t count, std::ostream & out);

    NodeList parseDirectoryContents(std::ostringstream &content, bool isWindows);
    NodeList parseDirectoryContentsLinux(std::ostringstream &content);
    NodeList parseDirectoryContentsWindows(std::ostringstream &content);

    void gatherServerType();

private:
    bool _initialized;
    QString _url;
    QString _user;
    QString _password;
    bool _isWindows;
};

#endif // FTPMANAGER_H
