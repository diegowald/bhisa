#include "ftpmanager.h"
#include <Poco/Net/FTPClientSession.h>
#include <iostream>
#include <fstream>
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
        out.write(buffer, buffer_size);
        readCount = in.gcount();
        count -= buffer_size;
    } while(readCount > 0);

    in.read(buffer, count);
    out.write(buffer, count);
}


void FtpManager::initialize(const QString &url, const QString &user, const QString &password)
{
    _url = url;
    _user = user;
    _password = password;
    _initialized = true;
}

void FtpManager::downloadFile(const QString &remoteDir, const QString &filename)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();

    QFuture<void> future = QtConcurrent::run(internal_downloadFile, this, remoteDir, filename);
}

void FtpManager::uploadFile(const QString &remoteDir, const QString &filename)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();
}

void FtpManager::deleteFile(const QString &remoteDir, const QString &filename)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();
}

void FtpManager::getDirectoryContents(const QString &remoteDir)
{
    if (!_initialized)
        emit requestInitialize();
    if (!_initialized)
        throw FtpException();
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


void FtpManager::internal_downloadFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string dir = remoteDir.toStdString();// "/home/diego/tmp/";
        std::string file = filename.toStdString(); //"test.txt";
        std::string host = ftpManager->_url.toStdString(); // "127.0.0.1";
        std::string uname = ftpManager->_user.toStdString(); // "diego";
        std::string password = ftpManager->_password.toStdString(); // "mic1492";

        Poco::Net::FTPClientSession session(host);
        session.setTimeout(time);
        session.login(uname, password);
        session.setWorkingDirectory(dir);
        session.setFileType(Poco::Net::FTPClientSession::TYPE_BINARY);
        std::istream& is = session.beginDownload(file);
        std::ofstream ofs;
        ofs.open(file.c_str(), std::ofstream::out | std::ofstream::app | std::ofstream::binary);
        stream_copy_n(is, is.gcount(), ofs);
        session.endDownload();
        session.close();
        emit ftpManager->fileDownloaded(remoteDir, filename);
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what();
    }
}

void FtpManager::internal_uploadFile(FtpManager *ftpManager, const QString &remoteDir, const QString &filename)
{
}

void FtpManager::internal_getDirectoryContents(FtpManager *ftpManager, const QString &remoteDir)
{
}
