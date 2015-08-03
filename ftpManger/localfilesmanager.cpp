#include "localfilesmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QUrl>
#include "file.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>


#include <QDebug>

LocalFilesManager::LocalFilesManager(QObject *parent) : QObject(parent)
{
    _initialized = false;

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
    QFuture<void> future = QtConcurrent::run(internal_downloadFile, this, remoteDir, filename);
}

void LocalFilesManager::uploadFile(const QString &remoteDir, const QString &filename)
{
    QFuture<void> future = QtConcurrent::run(internal_uploadFile, this, remoteDir, filename);
}

void LocalFilesManager::deleteFile(const QString &remoteDir, const QString &filename)
{
    QFuture<void> future = QtConcurrent::run(internal_deleteFile, this, remoteDir, filename);
}

QString LocalFilesManager::getCurrentDirectory()
{
    return _ftpManager.getCurrentDirectory();
}

void LocalFilesManager::getDirectoryContents(const QString &remoteDir)
{
    QFuture<FileList> future = QtConcurrent::run(internal_getDirectoryContents, this, remoteDir, _ftpSessionFolder);
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
    // check if file is remotely locked
    QString controlFileName = getControlFileName(filename);
    if (_ftpManager.fileExists(remoteDir, controlFileName))
    {
        return false;
    }

    QString filePath = localPath(remoteDir, controlFileName);
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << _user << "\n" << QDateTime::currentDateTime().toMSecsSinceEpoch();
    file.close();

    _ftpManager.uploadFile(remoteDir, controlFileName, filePath);
    return true;
}

QString LocalFilesManager::localPath(const QString &remotePath, const QString filename)
{
    QString dir = remotePath;
    dir = dir.remove(0, 1);
    QString s = "%1%2%3";
    qDebug() << s.arg(_ftpSessionFolder).arg(dir).arg(filename);
    return s.arg(_ftpSessionFolder).arg(dir).arg(filename);
}

QUrl LocalFilesManager::getLocalURL(const QString &remoteDir, const QString &filename)
{
    QString dir = remoteDir;
    dir = dir.remove(0, 1);
    QString sUrl = "file://%1";
    qDebug() <<sUrl.arg(localPath(remoteDir, filename));
    return QUrl(sUrl.arg(localPath(remoteDir, filename)));
}

QString LocalFilesManager::loggedUser() const
{
    return _user;
}


void LocalFilesManager::exportFolder(const QString &remoteFolder, const QString &localFolder)
{
    _exportFolder = localFolder;
    QFuture<void> future = QtConcurrent::run(internal_exportFolder, this, remoteFolder, localFolder);
}

void LocalFilesManager::internal_exportFolder(LocalFilesManager *localFileManager, const QString &remoteFolder, const QString &localFolder)
{
    FileList list = internal_getDirectoryContents(localFileManager, remoteFolder, localFolder);
    foreach (FilePtr file, *list)
    {
        if (file->isFolder())
        {
            QString rFolder = remoteFolder + "/" + file->filename();
            internal_exportFolder(localFileManager, rFolder, localFolder);
        }
        else if (!localFileManager->isControlFile(file->filename()))
        {
            localFileManager->_ftpManager.downloadFile(remoteFolder, file->filename(), localFolder);
        }
    }
    if (localFolder == localFileManager->_exportFolder)
    {
        // FIN DEL PROCESO
        localFileManager->_exportFolder = "";
    }
}


void LocalFilesManager::internal_downloadFile(LocalFilesManager* localFileManager, const QString &remoteDir, const QString &filename)
{
    if (localFileManager->_ftpManager.downloadFile(remoteDir, filename, localFileManager->_ftpSessionFolder))
    {
    }
    emit localFileManager->fileDownloaded(remoteDir, filename);
}

void LocalFilesManager::internal_uploadFile(LocalFilesManager *localFileManager, const QString &remoteDir, const QString &filename)
{
    if (localFileManager->_ftpManager.uploadFile(remoteDir, filename, localFileManager->localPath(remoteDir, filename)))
    {
        QString controlFile = localFileManager->getControlFileName(filename);
        if (filename != controlFile)
        {
            localFileManager->deleteFile(remoteDir, controlFile);
        }
    }
    emit localFileManager->fileUploaded(remoteDir, filename);
}

void LocalFilesManager::internal_deleteFile(LocalFilesManager *localFileManager, const QString &remoteDir, const QString &filename)
{
    localFileManager->_ftpManager.deleteFile(remoteDir, filename);
    if (!filename.startsWith(".ctl."))
    {
        emit localFileManager->fileDeleted(remoteDir, filename);
    }
}


FileList LocalFilesManager::internal_getDirectoryContents(LocalFilesManager *localFileManager, const QString &remoteDir, const QString &localFolder)
{
    FileList dirContents = localFileManager->_ftpManager.getDirectoryContents(remoteDir, localFileManager->_ftpSessionFolder);
    FileList contentsProcessed = FileList::create();
    foreach (FilePtr file, *dirContents)
    {
        QString folder = (remoteDir == "/") ?
                    QString("%1%2").arg(localFolder).arg(file->filename())
                  :
                    QString("%1/%2/%3")
                    .arg(localFolder).arg(remoteDir).arg(file->filename());

        if (file->isFolder())
        {
            localFileManager->checkAndCreateFolderIfNotExists(folder);
            if (!contentsProcessed->contains(file->filename()))
            {
                (*contentsProcessed)[file->filename()] = FilePtr::create(file->filename());
            }
            (*contentsProcessed)[file->filename()]->setFileData(file->permissions(),
                                                                file->owner(),
                                                                file->size(),
                                                                file->date(),
                                                                file->time());
        }
        else if (localFileManager->isControlFile(file->filename()))
        {
            localFileManager->_ftpManager.downloadFile(remoteDir, file->filename(), folder);
            dirContents->remove(file->filename());
            QString filename = localFileManager->extractFileNameFromControlFile(file->filename());
            QString editor;
            qint64 since;
            localFileManager->processControlFile(remoteDir, file->filename(), editor, since);
            if (!contentsProcessed->contains(filename))
            {
                (*contentsProcessed)[filename] = FilePtr::create(filename);
            }
            (*contentsProcessed)[filename]->setUnderEdition(editor, since);
        }
        else
        {
            if (!contentsProcessed->contains(file->filename()))
            {
                (*contentsProcessed)[file->filename()] = FilePtr::create(file->filename());
            }
            (*contentsProcessed)[file->filename()]->setFileData(file->permissions(),
                                                                file->owner(),
                                                                file->size(),
                                                                file->date(),
                                                                file->time());
        }
    }
    if ((localFileManager->_exportFolder.length() == 0) ||  !localFolder.startsWith(localFileManager->_exportFolder))
    {
        emit localFileManager->getDirectoryContentsDownloaded(remoteDir, contentsProcessed);
    }
    return contentsProcessed;
}

void LocalFilesManager::internal_deleteDirectory(LocalFilesManager *localFileManager, const QString &remoteDir)
{
    localFileManager->_ftpManager.deleteDirectory(remoteDir);
    emit localFileManager->directoryDeleted(remoteDir);
}
