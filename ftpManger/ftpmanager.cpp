#include "ftpmanager.h"
#include <Poco/Net/FTPClientSession.h>
#include <iostream>
#include <fstream>

FtpManager::FtpManager(const QString &url, const QString &user, const QString &password, QObject *parent) : QObject(parent)
{
    try
    {
        Poco::Timespan time(0, 0, 1, 0, 0);
        std::string dir = "/home/diego/tmp/";
        std::string file = "test.txt";
        std::string host = "127.0.0.1";
        std::string uname = "diego";
        std::string password = "mic1492";

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
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what();
    }
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
