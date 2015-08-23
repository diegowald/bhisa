#include "dlglogin.h"
#include "ui_dlglogin.h"

DlgLogin::DlgLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgLogin)
{
    ui->setupUi(this);
    ui->txtURL->setText("adbhisa.ddns.net");
    ui->lblUrl->setVisible(false);
    ui->txtURL->setVisible(false);
}

DlgLogin::~DlgLogin()
{
    delete ui;
}

QString DlgLogin::url() const
{
    return ui->txtURL->text();
}

QString DlgLogin::user() const
{
    return ui->txtUser->text();
}

QString DlgLogin::password() const
{
    return ui->txtPassword->text();
}
