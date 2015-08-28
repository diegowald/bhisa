#ifndef FOLDER_H
#define FOLDER_H

#include "node.h"

class Folder;
typedef QSharedPointer<Folder> FolderPtr;

class Folder : public Node
{
    Q_OBJECT
public:
    explicit Folder(const QString &filename, QObject *parent = 0);

    NodeList childNodes();
    void addChild(NodeList child);
signals:

public slots:
private:
    NodeList _childNodes;
};

typedef QSharedPointer<Folder> FolderPtr;
#endif // FOLDER_H
