#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QSharedPointer>
#include "qpermission.h"

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(const QString &filename, bool isFolder, QObject *parent = 0);

    QString filename() const;

    bool isFolder() const;

    void setPermissions(QPermissionList &permissions);
    QPermission::PERMIT myPermission(const QString &userName);

signals:

public slots:

private:
    QString _filename;
    bool _isFolder;
    QPermissionList _permissions;
};

typedef QSharedPointer<Node> NodePtr;
typedef QSharedPointer<QMap<QString, NodePtr>> NodeList;

#endif // NODE_H
