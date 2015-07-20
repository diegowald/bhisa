#ifndef FTPEXCEPTION_H
#define FTPEXCEPTION_H

#include <exception>

class FtpException : public std::exception
{
public:
    FtpException();
};

#endif // FTPEXCEPTION_H
