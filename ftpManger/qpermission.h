#ifndef QPERMISSION_H
#define QPERMISSION_H

#include <QObject>

class QPermission : public QObject
{
    Q_OBJECT
public:
    enum class PERMIT
    {
        none,
        canRead,
        canWrite,
        Administrator
    };

    explicit QPermission(QObject *parent = 0);
    QPermission( const QPermission& other );
    QPermission( QPermission& other );

    void setUser(const QString &user);
    void setPermission(PERMIT permission);
    void setPermission(const QString &permission);

    QString user() const;
    PERMIT permission() const;
    QString permissionString() const;

    QPermission& operator=(const QPermission& other);
signals:

public slots:
private:
    QString _userName;
    PERMIT _permit;
};



typedef QList<QPermission> QPermissionList;

#endif // QPERMISSION_H
