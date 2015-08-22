#include "file.h"

File::File(QObject *parent):QObject(parent)
{

}

File::File(const QString &filename, QObject *parent) : QObject(parent)
{
    _filename = filename;

    _isFolder = _permissions.length() > 0 ? _permissions.at(0) == 'd' : false;
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

unsigned int File::size() const
{
    return _size;
}

void File::setUnderEdition(const QString &underEditionAuthor,
                           qint64 underEditionDate)
{
    _underEditionAuthor = underEditionAuthor;
    _underEditionDate = QDateTime::fromMSecsSinceEpoch(underEditionDate);
    _underEdition = true;
}

void File::setFileData(const QString &permissions,
                       const QString &user,
                       unsigned int size,
                       const QString &date,
                       const QString &time)
{
    _permissions = permissions;
    _user = user;
    _size = size;
    _date = date;
    _time = time;
    _isFolder = permissions.startsWith("d");
}

QString File::date() const
{
    return _date;
}

QString File::time() const
{
    return _time;
}

void File::setPermissions(QPermissionList &permissions)
{
    _permissionList = permissions;
}
