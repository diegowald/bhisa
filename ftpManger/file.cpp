#include "file.h"

File::File(QObject *parent):QObject(parent)
{

}

File::File(const QString &permissions, const QString &numberOfLinks,
           const QString &user, const QString &group,
           const QString &size, const QString &month,
           const QString &day,const QString &time,
           const QString &filename, QObject *parent) : QObject(parent)
{
    _permissions = permissions;
    _numberOfLinks = numberOfLinks.toInt();
    _user = user;
    _group = group;
    _size = size.toUInt();
    _day = day;
    _month = month;
    _time = time;
    _filename = filename;

    _isFolder = _permissions.at(0) == 'd';
    _underEdition = false;
    _underEditionAuthor = "";
}

File::~File()
{
}

bool File::isLocked() const
{
    return _underEdition;
}

bool File::isFolder() const
{
    return _isFolder;
}

QString File::filename() const
{
    return _filename;
}

QString File::underEditionAuthor() const
{
    return _underEditionAuthor;
}

QDateTime File::underEditionDateTime() const
{
    return _underEditionDate;
}

QString File::permissions() const
{
    return _permissions;
}

QString File::owner() const
{
    return _user;
}

QString File::underEdition() const
{
    return _underEdition ? tr("Yes") : tr("No");
}
