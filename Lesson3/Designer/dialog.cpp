#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

QString Dialog::getHostName() const
{
      return ui->hostNameEdit->text();
}

QString Dialog::getDatabaseName() const
{
    return ui->databaseNameEdit->text();
}

QString Dialog::getLogin() const
{
    return ui->loginEdit->text();
}

QString Dialog::getPassword() const
{
    return ui->passwordEdit->text();
}

uint Dialog::getPort() const
{
    return ui->portSpinBox->value();
}
