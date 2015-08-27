#include "localfilesmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QUrl>
#include "file.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include "folder.h"

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
    QFuture<void> future = QtConcurrent::run(this, &LocalFilesManager::internal_downloadFile, remoteDir, filename);
}

void LocalFilesManager::uploadFile(const QString &remoteDir, const QString &filename)
{
    QFuture<void> future = QtConcurrent::run(this, &LocalFilesManager::internal_uploadFile, remoteDir, filename);
}

void LocalFilesManager::deleteFile(const QString &remoteDir, const QString &filename)
{
    QFuture<void> future = QtConcurrent::run(this, &LocalFilesManager::internal_deleteFile, remoteDir, filename);
}

QString LocalFilesManager::getCurrentDirectory()
{
    return _ftpManager.getCurrentDirectory();
}

void LocalFilesManager::getDirectoryContents(const QString &remoteDir)
{
    QFuture<NodeList> future = QtConcurrent::run(this, &LocalFilesManager::internal_getDirectoryContents, remoteDir, _ftpSessionFolder);
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

bool LocalFilesManager::isPermissionsFile(const QString &filename)
{
    return filename.startsWith(".permissions.", Qt::CaseInsensitive);
}

bool LocalFilesManager::isUsersListFile(const QString &filename)
{
    return QString::compare(filename, ".users.list", Qt::CaseInsensitive) == 0;
}

QString LocalFilesManager::extractFileNameFromControlFile(const QString &controlFileName)
{
    QString s = controlFileName;
    return s.replace(".ctl.", "");
}

QString LocalFilesManager::extractFileNameFromPermissionsFile(const QString &filename)
{
    QString s = filename;
    return s.replace(".permissions.", "");
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

QPermissionList LocalFilesManager::processPermissionsFile(const QString &remoteDir, const QString &filename)
{
    QPermissionList permissions;
    QString filePath = remoteDir.endsWith("/") ? remoteDir + filename : remoteDir + "/" + filename;
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly))
    {
        // el formato es Permission:username

        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();

            QPermission permission;
            QStringList data = line.split(":");
            permission.setPermission(data.at(0));
            permission.setUser(data.at(1));
            permissions.append(permission);
        }
        file.close();
    }
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
    QFuture<void> future = QtConcurrent::run(this, &LocalFilesManager::internal_exportFolder, remoteFolder, localFolder);
}

void LocalFilesManager::importFolder(const QString &localFolder, const QString &remoteFolder)
{
    _importFolder = localFolder;
    QFuture<void> future = QtConcurrent::run(this, &LocalFilesManager::internal_importFolder, localFolder, remoteFolder, localFolder);
}

void LocalFilesManager::internal_exportFolder(const QString &remoteFolder, const QString &localFolder)
{
    NodeList list = internal_getDirectoryContents(remoteFolder, localFolder);
    foreach (FilePtr file, *list)
    {
        if (file->isFolder())
        {
            QString rFolder = remoteFolder + "/" + file->filename();
            internal_exportFolder(rFolder, localFolder);
        }
        else if (!isControlFile(file->filename()))
        {
            _ftpManager.downloadFile(remoteFolder, file->filename(), localFolder);
        }
    }
    if (localFolder == _exportFolder)
    {
        // FIN DEL PROCESO
        _exportFolder = "";
    }
}

void LocalFilesManager::internal_importFolder(const QString &localFolder, const QString &remoteFolder, const QString &localRootFolder)
{
    QDir dir(localFolder);
    QString root = localRootFolder;
    if (localFolder == _importFolder)
    {
        root.chop(dir.dirName().length() + 1);
        qDebug() << root;
    }

    {
        QString remFolder = remoteFolder + dir.absolutePath().replace(root, "");
        remFolder.chop(dir.dirName().length());
        _ftpManager.createDirectory(remFolder, dir.dirName());
    }

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString filename = fileInfo.fileName();
        if ((filename != ".") && (filename != ".."))
        {
            if (fileInfo.isDir())
            {
                internal_importFolder(fileInfo.absoluteFilePath(), remoteFolder, root);
            }
            else
            {
                //uploadFile(remoteFolder, fileInfo.fileName());
                QString remFolder = remoteFolder;
                if (remFolder.endsWith("/"))
                {
                    remFolder.chop(1);
                }
                remFolder += fileInfo.absolutePath().replace(root, "");
                if (!isControlFile(fileInfo.fileName()))
                {
                    _ftpManager.uploadFile(remFolder, fileInfo.fileName(), fileInfo.absoluteFilePath());
                }
            }
        }
    }
    if (localFolder == _importFolder)
    {
        _importFolder = "";
    }
}

void LocalFilesManager::internal_downloadFile(const QString &remoteDir, const QString &filename)
{
    if (_ftpManager.downloadFile(remoteDir, filename, _ftpSessionFolder))
    {
    }
    emit fileDownloaded(remoteDir, filename);
}

void LocalFilesManager::internal_uploadFile(const QString &remoteDir, const QString &filename)
{
    if (_ftpManager.uploadFile(remoteDir, filename, localPath(remoteDir, filename)))
    {
        QString controlFile = getControlFileName(filename);
        if (filename != controlFile)
        {
            deleteFile(remoteDir, controlFile);
        }
    }
    emit fileUploaded(remoteDir, filename);
}

void LocalFilesManager::internal_deleteFile(const QString &remoteDir, const QString &filename)
{
    _ftpManager.deleteFile(remoteDir, filename);
    if (!filename.startsWith(".ctl."))
    {
        emit fileDeleted(remoteDir, filename);
    }
}


NodeList LocalFilesManager::internal_getDirectoryContents(const QString &remoteDir, const QString &localFolder)
{
    NodeList dirContents = _ftpManager.getDirectoryContents(remoteDir, _ftpSessionFolder);
    NodeList contentsProcessed = NodeList::create();
    foreach (NodePtr node, *dirContents)
    {
        QString folder = (remoteDir == "/") ?
                    QString("%1%2").arg(localFolder).arg(node->filename())
                  :
                    QString("%1/%2/%3")
                    .arg(localFolder).arg(remoteDir).arg(node->filename());

        if (node->isFolder())
        {
            checkAndCreateFolderIfNotExists(folder);
            if (!contentsProcessed->contains(node->filename()))
            {
                (*contentsProcessed)[node->filename()] = FolderPtr::create(node->filename());
            }
            /*(*contentsProcessed)[file->filename()]->setFileData(file->permissions(),
                                                                file->owner(),
                                                                file->size(),
                                                                file->date(),
                                                                file->time());*/
        }
        else if (isControlFile(node->filename()))
        {
            _ftpManager.downloadFile(remoteDir, node->filename(), folder);
            dirContents->remove(node->filename());
            QString filename = extractFileNameFromControlFile(node->filename());
            QString editor;
            qint64 since;
            processControlFile(remoteDir, node->filename(), editor, since);
            if (!contentsProcessed->contains(filename))
            {
                (*contentsProcessed)[filename] = FilePtr::create(filename);
            }
            qSharedPointerCast<File>((*contentsProcessed)[filename])->setUnderEdition(editor, since);
        }
        else if (isPermissionsFile(node->filename()))
        {
            _ftpManager.downloadFile(remoteDir, node->filename(), folder);
            dirContents->remove(node->filename());
            QString filename = extractFileNameFromPermissionsFile(node->filename());
            QPermissionList permissions = processPermissionsFile(remoteDir, node->filename());
            if (!contentsProcessed->contains(filename))
            {
                (*contentsProcessed)[filename] = FilePtr::create(filename);
            }
            (*contentsProcessed)[filename]->setPermissions(permissions);
        }
        else if (isUsersListFile(node->filename()))
        {
            _ftpManager.downloadFile(remoteDir, node->filename(), folder);
            dirContents->remove(file->filename());
            QString filename = extractFileNameFromPermissionsFile(file->filename());
            QPermissionList permissions = processPermissionsFile(remoteDir, file->filename());
            if (!contentsProcessed->contains(filename))
            {
                (*contentsProcessed)[filename] = FilePtr::create(filename);
            }
            (*contentsProcessed)[filename]->setPermissions(permissions);
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
    if ((_exportFolder.length() == 0) ||  !localFolder.startsWith(_exportFolder))
    {
        emit getDirectoryContentsDownloaded(remoteDir, contentsProcessed);
    }
    return contentsProcessed;
}

void LocalFilesManager::internal_deleteDirectory(const QString &remoteDir)
{
    _ftpManager.deleteDirectory(remoteDir);
    emit directoryDeleted(remoteDir);
}

QString LocalFilesManager::user() const
{
    return _user;
}
