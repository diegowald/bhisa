#include "file.h"

File::File(QObject *parent):QObject(parent)
{

}

File::File(const QString &permissions, const QString &numberOfLinks,
           const QString &user, const QString &group,
           const QString &size, const QString &date, const QString &time,
           const QString &filename, QObject *parent) : QObject(parent)
{
    _permissions = permissions;
    _numberOfLinks = numberOfLinks.toInt();
    _user = user;
    _group = group;
    _size = size.toUInt();
    _date = date;
    _time = time;
    _filename = filename;

    _isFolder = _permissions.length() > 0 ? _permissions.at(0) == 'd' : false;
    _underEdition = false;
    _underEditionAuthor = "";
}

File::File(const QString &filename,
           const QString &underEditionAuthor,
           qint64 underEditionDate,
           QObject *parent) : QObject(parent)
{
    _underEdition = true;
    _underEditionAuthor = underEditionAuthor;
    _underEditionDate = QDateTime::fromMSecsSinceEpoch(underEditionDate);
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
