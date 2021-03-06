#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QDateTime>
#include <QSharedPointer>
#include <qpermission.h>
#include <node.h>
class File: public Node
{
    Q_OBJECT
public:
    File(QObject *parent = 0);
    File(const QString &filename, QObject *parent = 0);

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

    QString date() const;
    QString time() const;

    void setUnderEdition(const QString &underEditionAuthor,
                         qint64 underEditionDate);

    void setFileData(const QString &permissions,
                     const QString &user,
                     unsigned int size, const QString &date, const QString &time);

    void setPermissions(QPermissionList &permissions);
    QPermission::PERMIT myPermission(const QString &userName);
private:
    QString _filename;

    bool _underEdition;
    bool _isFolder;
    QString _underEditionAuthor;
    QDateTime _underEditionDate;

    QString _permissions;
    QString _user;
    unsigned int _size;
    QString _date;
    QString _time;
    QPermissionList _permissionList;
};

typedef QSharedPointer<File> FilePtr;
//typedef QSharedPointer<QMap<QString, FilePtr>> FileList;

#endif // FILE_H
