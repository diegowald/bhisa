#include "folder.h"

Folder::Folder(const QString &filename, QObject *parent) : Node(filename, true, parent)
{

}

NodeList Folder::childNodes()
{
    return _childNodes;
}

void Folder::addChild(NodeList child)
{
    _childNodes = child;
}
