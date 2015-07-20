#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QDateTime>

class File: public QObject
{
    Q_OBJECT
public:
    File(QObject *parent = 0);
    virtual ~File();

    bool isLocked() const;
    QString filename() const;
    QString underEditionAuthor() const;
    QDateTime underEditionDateTime() const;

private:
    QString _filename;

    unsigned int size;

    bool _underEdition;
    QString _underEditionAuthor;
    QDateTime _underEditionDate;

};

typedef QSharedPointer<File> FilePtr;

#endif // FILE_H
