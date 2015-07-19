#include "file.h"

File::File(QObject *parent):QObject(parent)
{

}

File::~File()
{
}

bool File::isLocked() const
{
}

QString File::filename() const
{
}

QString File::underEditionAuthor() const
{
}

QDateTime File::underEditionDateTime() const
{
}
