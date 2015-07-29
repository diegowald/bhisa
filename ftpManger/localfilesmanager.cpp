#include "localfilesmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QUrl>
#include "file.h"

#include <QDebug>

LocalFilesManager::LocalFilesManager(QObject *parent) : QObject(parent)
{
    _initialized = false;

    connect(&_ftpManager, &FtpManager::fileDownloaded, this, &LocalFilesManager::on_fileDownloaded);
    connect(&_ftpManager, &FtpManager::fileUploaded, this, &LocalFilesManager::on_fileUploaded);
    connect(&_ftpManager, &FtpManager::fileDeleted, this, &LocalFilesManager::on_fileDeleted);

    connect(&_ftpManager, &FtpManager::getDirectoryContentsDownloaded, this, &LocalFilesManager::on_getDirectoryContentsDownloaded);
    connect(&_ftpManager, &FtpManager::directoryCreated, this, &LocalFilesManager::on_directoryCreated);
    connect(&_ftpManager, &FtpManager::directoryDeleted, this, &LocalFilesManager::on_directoryDeleted);
    connect(&_ftpManager, &FtpManager::directoryChanged, this, &LocalFilesManager::on_directoryChanged);

    connect(&_ftpManager, &FtpManager::requestInitialize, this, &LocalFilesManager::on_requestInitialize);

    _localRootFolder  = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    qDebug() << _localRootFolder;
    _ftpSessionFolder = "";
}

LocalFilesManager::~LocalFilesManager()
{
}

void LocalFilesManager::initialize(const QString &url, const QString &user, const QString &password)
{
    _url = url;
    _user = user;
    _password = password;
    _initialized = true;
    _ftpManager.initialize(_url, _user, _password);
    _ftpSessionFolder = QString("%1/%2/").arg(_localRootFolder, _url);
    checkAndCreateFolderIfNotExists(_ftpSessionFolder);
}

void LocalFilesManager::downloadFile(const QString &remoteDir, const QString &filename)
{
    _ftpManager.downloadFile(remoteDir, filename, _ftpSessionFolder, false);
}

void LocalFilesManager::uploadFile(const QString &remoteDir, const QString &filename)
{
    _ftpManager.uploadFile(remoteDir, filename, false);
}

void LocalFilesManager::deleteFile(const QString &remoteDir, const QString &filename)
{
    _ftpManager.deleteFile(remoteDir, filename);
}

QString LocalFilesManager::getCurrentDirectory()
{
    return _ftpManager.getCurrentDirectory();
}

void LocalFilesManager::getDirectoryContents(const QString &remoteDir)
{
    _ftpManager.getDirectoryContents(remoteDir, _ftpSessionFolder);
}

void LocalFilesManager::createDirectory(const QString &remoteDir, const QString &directoryName)
{
    _ftpManager.createDirectory(remoteDir, directoryName);
}

void LocalFilesManager::deleteDirectory(const QString &remoteDir)
{
    _ftpManager.deleteDirectory(remoteDir);
}

void LocalFilesManager::changeDirectory(const QString &remoteDir)
{
    _ftpManager.changeDirectory(remoteDir);
}

void LocalFilesManager::on_fileDownloaded(const QString &remoteDir, const QString &filename)
{
    emit fileDownloaded(remoteDir, filename);
}

void LocalFilesManager::on_fileUploaded(const QString &remoteDir, const QString &filename)
{
    emit fileUploaded(remoteDir, filename);
}

void LocalFilesManager::on_fileDeleted(const QString &remoteDir, const QString &filename)
{
    emit fileDeleted(remoteDir, filename);
}

void LocalFilesManager::on_getDirectoryContentsDownloaded(const QString &remoteDir, FileList dirContents)
{
    foreach (FilePtr file, *dirContents)
    {
        QString folder = (remoteDir == "/") ?
                    QString("%1%2").arg(_ftpSessionFolder).arg(file->filename())
                  :
                    QString("%1/%2/%3")
                    .arg(_ftpSessionFolder).arg(remoteDir).arg(file->filename());
        qDebug() << folder;

        if (file->isFolder())
        {
            checkAndCreateFolderIfNotExists(folder);
        }
        else if (isControlFile(file->filename()))
        {
            _ftpManager.downloadFile(remoteDir, file->filename(), folder, true);
            dirContents->remove(file->filename());
            QString filename = extractFileNameFromControlFile(file->filename());
            QString editor;
            qint64 since;
            processControlFile(remoteDir, file->filename(), editor, since);
            if (dirContents->contains(filename))
            {
                (*dirContents)[filename]->setUnderEdition(editor, since);
            }
            else
            {
                (*dirContents)[filename] = FilePtr::create(filename, editor, since);
            }
        }
    }
    emit getDirectoryContentsDownloaded(remoteDir, dirContents);
}

void LocalFilesManager::on_directoryCreated(const QString &remoteDir, const QString &directoryName)
{
    emit directoryCreated(remoteDir, directoryName);
}

void LocalFilesManager::on_directoryDeleted(const QString &remoteDir)
{
    emit directoryDeleted(remoteDir);
}

void LocalFilesManager::on_directoryChanged(const QString &remoteDir)
{
    emit directoryChanged(remoteDir);
}

void LocalFilesManager::on_requestInitialize()
{
    emit requestInitialize();
}

void LocalFilesManager::checkAndCreateFolderIfNotExists(const QString &foldername)
{
    QDir dir(foldername);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
}

bool LocalFilesManager::isControlFile(const QString &filename)
{
    return filename.startsWith(".ctl.", Qt::CaseInsensitive);
}

QString LocalFilesManager::extractFileNameFromControlFile(const QString &controlFileName)
{
    QString s = controlFileName;
    return s.replace(".ctl.", "");
}

QString LocalFilesManager::getControlFileName(const QString &filename)
{
    return QString(".ctl.%1").arg(filename);
}

void LocalFilesManager::processControlFile(const QString &dir, const QString &filename, QString &editor, qint64 &since)
{
    chequear esto
    QString filePath = dir.endsWith("/") ? dir + filename : dir + "/" + filename;
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    editor = "";
    stream >> editor;
    stream >> since;
    file.close();
}

bool LocalFilesManager::lockFile(const QString &remoteDir, const QString &filename)
{
    chequear esto
    // check if file is remotely locked
    if (_ftpManager.fileExists(remoteDir, getControlFileName(filename)))
    {
        return false;
    }

    QString filePath = _ftpSessionFolder + remoteDir + getControlFileName(filename);
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << _user;
    stream << QDateTime::currentDateTime().toMSecsSinceEpoch();
    file.close();

    _ftpManager.uploadFile(remoteDir, filePath, true);
    return true;
}

QUrl LocalFilesManager::getLocalURL(const QString &remoteDir, const QString &filename)
{
    QString dir = remoteDir;
    dir = dir.remove(0, 1);
    QString sUrl = "file://%1%2%3";
    qDebug() <<sUrl.arg(_ftpSessionFolder).arg(dir).arg(filename);
    return QUrl(sUrl.arg(_ftpSessionFolder).arg(dir).arg(filename));
}
