#include "qpermission.h"

QPermission::QPermission(QObject *parent) : QObject(parent)
{

}

QPermission::QPermission(const QPermission &other)
{
    _userName = other._userName;
    _permit = other._permit;
    setParent(other.parent());
}

QPermission::QPermission(QPermission &other)
{
    _userName = other._userName;
    _permit = other._permit;
    setParent(other.parent());
}

void QPermission::setUser(const QString &user)
{
    _userName = user;
}

void QPermission::setPermission(PERMIT permission)
{
    _permit = permission;
}

void QPermission::setPermission(const QString &permission)
{
    QString p = permission.toUpper();
    if (p == "R")
    {
        _permit = PERMIT::canRead;
    }
    else if (p == "W")
    {
        _permit = PERMIT::canWrite;
    }
    else if (p == "X")
    {
        _permit = PERMIT::Administrator;
    }
    else
    {
        _permit = PERMIT::none;
    }
}

QString QPermission::user() const
{
    return _userName;
}

QPermission::PERMIT QPermission::permission() const
{
    return _permit;
}

QString QPermission::permissionString() const
{
    QString s = "";
    switch (_permit)
    {
    case PERMIT::canRead:
        s = "R";
        break;
    case PERMIT::canWrite:
        s = "W";
        break;
    case PERMIT::Administrator:
        s = "X";
        break;
    case PERMIT::none:
    default:
        s = "";
        break;
    }
    return s;
}

QPermission& QPermission::operator=(const QPermission& other)
{
    if (this != &other)
    {
        _userName = other._userName;
        _permit = other._permit;
    }
    return *this;
}
