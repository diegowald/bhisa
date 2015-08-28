#include "node.h"

Node::Node(const QString &filename, bool isFolder, QObject *parent) : QObject(parent)
{
    _filename = filename;
    _isFolder = isFolder;
}

QString Node::filename() const
{
    return _filename;
}

bool Node::isFolder() const
{
    return _isFolder;
}

void Node::setPermissions(QPermissionList &permissions)
{
    _permissions = permissions;
}

QPermission::PERMIT Node::myPermission(const QString &userName)
{
    if (_permissions.count() == 0)
        return QPermission::PERMIT::Administrator;

    QString s = userName.toLower();
    foreach (QPermission permission, _permissions)
    {
        if (permission.user().toLower() == s)
        {
            return permission.permission();
        }
    }
    return QPermission::PERMIT::none;
}
