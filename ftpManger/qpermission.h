#ifndef QPERMISSION_H
#define QPERMISSION_H

#include <QObject>

class QPermission : public QObject
{
    Q_OBJECT
public:
    typedef enum class PERMIT
    {
        canRead,
        canWrite,
        Administrator
    };

    explicit QPermission(QObject *parent = 0);
    QPermission( const QPermission& other );
    QPermission( QPermission& other );

signals:

public slots:
private:
    QString _userName;
    PERMIT _permit;
};

typedef QList<QPermission> QPermissionList;

#endif // QPERMISSION_H
