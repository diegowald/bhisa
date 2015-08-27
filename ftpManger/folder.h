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

    QList<FolderPtr> childNodes();
signals:

public slots:
private:
    QList<FolderPtr> _childNodes;
};

typedef QSharedPointer<Folder> FolderPtr;
#endif // FOLDER_H
