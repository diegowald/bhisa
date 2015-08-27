#include "folder.h"

Folder::Folder(const QString &filename, QObject *parent) : Node(filename, true, parent)
{

}

QList<NodePtr> Folder::childNodes()
{
    return _childNodes;
}
