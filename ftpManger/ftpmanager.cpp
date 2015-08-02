#include "ftpmanager.h"
#include <Poco/Net/FTPClientSession.h>
#include <Poco/Net/NetException.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "ftpexception.h"

FtpManager::FtpManager(QObject *parent) : QObject(parent), _url(""), _user(""), _password("")

{
    _initialized = false;


}

FtpManager::~FtpManager()
{
}

void FtpManager::stream_copy_n(std::istream & in, std::size_t count, std::ostream & out)
{
    const std::size_t buffer_size = 4096;
    char buffer[buffer_size];
    std::streamsize readCount = 0;
    do
    {
        in.read(buffer, buffer_size);
        readCount = in.gcount();
        if (readCount > 0)
            out.write(buffer, readCount);
    } while(readCount > 0);

/*    in.read(buffer, count);
    out.write(buffer, count);*/
}


void FtpManager::initialize(const QString &url, const QString &user, const QString &password)
{
    _url = url;
    _user = user;
    _password = password;
    gatherServerType();
    _initialized = true;
}

void FtpManager::downloadFile(const QString &remoteDir, const QString &filename, const QString &localFolder, bool blockingCall)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

    QFuture<void> future = QtConcurrent::run(internal_downloadFile, this, remoteDir, filename, localFolder);
    if (blockingCall)
        future.waitForFinished();
}

void FtpManager::uploadFile(const QString &remoteDir, const QString &filename, const QString &localFilename, bool blockingCall)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

    QFuture<void> future = QtConcurrent::run(internal_uploadFile, this, remoteDir, filename, localFilename);
    if (blockingCall)
        future.waitForFinished();
}

void FtpManager::deleteFile(const QString &remoteDir, const QString &filename)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();
}

void FtpManager::getDirectoryContents(const QString &remoteDir, const QString &localFolder, bool blockingCall)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

    QFuture<void> future = QtConcurrent::run(internal_getDirectoryContents, this, remoteDir, localFolder);
    if (blockingCall)
    {
        future.waitForFinished();
    }
}

bool FtpManager::fileExists(const QString &remoteDir, const QString &filename)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

    QFuture<bool> future = QtConcurrent::run(internal_FileExists, this, remoteDir, filename);
    future.waitForFinished();
    return future.result();
}

QString FtpManager::getCurrentDirectory()
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

    QString workDir = "";
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string host = _url.toStdString();
        std::string uname = _user.toStdString();
        std::string password = _password.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        std::string currDir = session.getWorkingDirectory();
        session.close();
        workDir = QString::fromStdString(currDir);
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
    }
    return workDir;
}

void FtpManager::createDirectory(const QString &remoteDir, const QString &directoryName)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

}

void FtpManager::deleteDirectory(const QString &remoteDir)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();
}

void FtpManager::changeDirectory(const QString &remoteDir)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();
}


void FtpManager::internal_downloadFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename, const QString &localFolder)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string dir = remoteDir.toStdString();
        std::string file = filename.toStdString();
        std::string host = ftpManager->_url.toStdString();
        std::string uname = ftpManager->_user.toStdString();
        std::string password = ftpManager->_password.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        session.setWorkingDirectory(dir);
        session.setFileType(Poco::Net::FTPClientSession::TYPE_BINARY);
        std::istream& is = session.beginDownload(file);
        std::ofstream ofs;

        QString completePath;
        if (remoteDir.endsWith("/"))
            completePath = localFolder + remoteDir + filename;
        else
            completePath = localFolder + remoteDir + "/" + filename;
        ofs.open(completePath.toStdString(), std::ofstream::out | std::ofstream::app | std::ofstream::binary);
        stream_copy_n(is, is.gcount(), ofs);
        ofs.close();
        session.endDownload();
        session.close();
        QFile f(completePath);
        f.setPermissions(QFileDevice::Permission::ReadGroup
                         | QFileDevice::Permission::ReadOther
                         | QFileDevice::Permission::ReadOwner
                         | QFileDevice::Permission::ReadUser);
        emit ftpManager->fileDownloaded(remoteDir, filename);
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
    }
}

void FtpManager::internal_uploadFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename, const QString &localFilename)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string dir = remoteDir.toStdString();
        std::string file = filename.toStdString();
        std::string host = ftpManager->_url.toStdString();
        std::string uname = ftpManager->_user.toStdString();
        std::string password = ftpManager->_password.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        session.setWorkingDirectory(dir);
        session.setFileType(Poco::Net::FTPClientSession::TYPE_BINARY);
        std::ostream& os = session.beginUpload(file);
        std::ifstream ifs;

        ifs.open(localFilename.toStdString(), std::ifstream::in | std::ifstream::app | std::ifstream::binary);

        stream_copy_n(ifs, ifs.gcount(), os);
        ifs.close();
        session.endDownload();
        session.close();
        emit ftpManager->fileUploaded(remoteDir, filename);
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
    }
}

void FtpManager::internal_deleteFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string host = ftpManager->_url.toStdString();
        std::string uname = ftpManager->_user.toStdString();
        std::string password = ftpManager->_password.toStdString();

        std::string dir = remoteDir.toStdString();
        std::string file = filename.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);

        session.setWorkingDirectory(dir);
        session.remove(file);
        emit ftpManager->fileDeleted(remoteDir, filename);
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
    }
}

void FtpManager::internal_getDirectoryContents(FtpManager *ftpManager, const QString &remoteDir, const QString &localFolder)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string dir = remoteDir.toStdString();
        std::string host = ftpManager->_url.toStdString();
        std::string uname = ftpManager->_user.toStdString();
        std::string password = ftpManager->_password.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        std::istream &is = session.beginList(remoteDir.toStdString(), true);
        std::ostringstream os;
        stream_copy_n(is, 0, os);
        session.endList();
        session.close();
        std::cerr << os.str();
        FileList dirContents = parseDirectoryContents(os, ftpManager->_isWindows);
        emit ftpManager->getDirectoryContentsDownloaded(remoteDir, dirContents, localFolder);
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
    }
}

bool FtpManager::internal_FileExists(FtpManager *ftpManager, const QString &remoteDir, const QString &filename)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string dir = (remoteDir + filename).toStdString();
        std::string host = ftpManager->_url.toStdString();
        std::string uname = ftpManager->_user.toStdString();
        std::string password = ftpManager->_password.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        std::istream &is = session.beginList(dir, true);
        std::ostringstream os;
        stream_copy_n(is, 0, os);
        session.endList();
        session.close();
        std::cerr << os.str();
        FileList dirContents = parseDirectoryContents(os, ftpManager->_isWindows);
        return dirContents->count() > 0;
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
        std::cerr << ex.message();
        if (ex.code() == 550)
        {
            return false;
        }
        throw ex;
    }
    return false;
}

FileList FtpManager::parseDirectoryContents(std::ostringstream &content, bool isWindows)
{
    if (isWindows)
        return parseDirectoryContentsWindows(content);
    else
        return parseDirectoryContentsLinux(content);
}

FileList FtpManager::parseDirectoryContentsLinux(std::ostringstream &content)
{
    std::stringstream ss;
    ss << content.str();
    FileList res = FileList::create();


    std::string line;
    while (std::getline(ss, line))
    {
        QString permissions;
        QString numberOfLinks;
        QString user;
        QString group;
        QString size;
        QString month;
        QString day;
        QString time;
        QString filename;

        QString l = QString::fromStdString(line);
        QStringList fields = l.split(' ', QString::SplitBehavior::SkipEmptyParts);
        permissions = fields[0];
        l = l.replace(l.indexOf(permissions), permissions.size(), "");
        numberOfLinks = fields[1];
        l = l.replace(l.indexOf(numberOfLinks), numberOfLinks.size(), "");
        user = fields[2];
        l = l.replace(l.indexOf(user), user.size(), "");
        group  = fields[3];
        l = l.replace(l.indexOf(group), group.size(), "");
        size  = fields[4];
        l = l.replace(l.indexOf(size), size.size(), "");
        month  = fields[5];
        l = l.replace(l.indexOf(month), month.size(), "");
        day  = fields[6];
        l = l.replace(l.indexOf(day), day.size(), "");
        time  = fields[7];
        l = l.replace(l.indexOf(time), time.size(), "");
        filename = l.trimmed();

        if (filename.toLower() != ".control.db")
        {
            qDebug() << permissions << ", " << numberOfLinks << ", " << user
                     << ", " <<  group  << ", " <<  size  << ", " <<
                        month  << ", " <<  day  << ", " <<  time << ", " <<  filename;

            (*res)[filename] = FilePtr::create(filename);
            (*res)[filename]->setFileData(permissions, user, size.toUInt(),
                                            month + "-" + day, time);
        }
    }

    return res;
}

FileList FtpManager::parseDirectoryContentsWindows(std::ostringstream &content)
{
    std::stringstream ss;
    ss << content.str();
    FileList res = FileList::create();


    std::string line;
    while (std::getline(ss, line))
    {
        QString permissions = "";
        QString numberOfLinks = "";
        QString user = "";
        QString group = "";
        QString size = "";
        QString date = "";
        QString time = "";
        QString filename = "";

        QString l = QString::fromStdString(line);
        QStringList fields = l.split(' ', QString::SplitBehavior::SkipEmptyParts);

        date = fields[0];
        l = l.replace(l.indexOf(date), date.size(), "");

        time = fields[1];
        l = l.replace(l.indexOf(time), time.size(), "");

        QString word = fields[2];
        if (word == "<DIR>")
            permissions = "d";
        else
            size = word;
        l = l.replace(l.indexOf(word), word.size(), "");

        filename = l.trimmed();
        if (filename.toLower() != ".control.db")
        {
            qDebug() << permissions << ", " << numberOfLinks << ", " << user
                     << ", " <<  group  << ", " <<  size  << ", " <<
                        ", " <<  time << ", " <<  filename;

            (*res)[filename] = FilePtr::create(filename);
            (*res)[filename]->setFileData(permissions, user, size.toUInt(),
                                          date, time);
        }
    }

    return res;
}


void FtpManager::gatherServerType()
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string host = _url.toStdString();
        std::string uname = _user.toStdString();
        std::string password = _password.toStdString();

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        session.setFileType(Poco::Net::FTPClientSession::TYPE_BINARY);

        std::string result = session.systemType();

        _isWindows = QString::fromStdString(result).contains("Win");
        session.close();
    }
    catch (Poco::Net::FTPException &ex)
    {
        std::cerr << ex.displayText();
    }
}
