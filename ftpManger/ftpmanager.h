#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>

class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(const QString &url, const QString &user, const QString &password, QObject *parent = 0);
    virtual ~FtpManager();
signals:

public slots:
};

#endif // FTPMANAGER_H
