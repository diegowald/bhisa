#ifndef DLGLOGIN_H
#define DLGLOGIN_H

#include <QDialog>

namespace Ui {
class DlgLogin;
}

class DlgLogin : public QDialog
{
    Q_OBJECT

public:
    explicit DlgLogin(QWidget *parent = 0);
    ~DlgLogin();

    QString url() const;
    QString user() const;
    QString password() const;

private:
    Ui::DlgLogin *ui;
};

#endif // DLGLOGIN_H
