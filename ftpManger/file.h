#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QDateTime>
#include <QSharedPointer>

class File: public QObject
{
    Q_OBJECT
public:
    File(QObject *parent = 0);
    File(const QString &permissions, const QString &numberOfLinks,
         const QString &user, const QString &group,
         const QString &size, const QString &date, const QString &time,
         const QString &filename, QObject *parent = 0);
    virtual ~File();

    bool isLocked() const;
    bool isFolder() const;
    QString filename() const;
    QString underEditionAuthor() const;
    QDateTime underEditionDateTime() const;
    QString permissions() const;
    QString owner() const;
    QString underEdition() const;
    unsigned int size() const;

private:
    QString _filename;

    bool _underEdition;
    bool _isFolder;
    QString _underEditionAuthor;
    QDateTime _underEditionDate;

    QString _permissions;
    int _numberOfLinks;
    QString _user;
    QString _group;
    unsigned int _size;
    QString _date;
    QString _time;

};

typedef QSharedPointer<File> FilePtr;
typedef QSharedPointer<QMap<QString, FilePtr>> FileList;

#endif // FILE_H
