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
