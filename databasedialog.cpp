#include "databasedialog.h"
#include "ui_databasedialog.h"

DatabaseDialog::DatabaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseDialog)
{
    ui->setupUi(this);
}

DatabaseDialog::~DatabaseDialog()
{
    delete ui;
}


QString DatabaseDialog::gethost(){
    return ui->hosttxtbox->text();
}

QString DatabaseDialog::getdbname(){
    return ui->dbtxtbox->text();
}

QString DatabaseDialog::getuser(){
    return  ui->usertxtbox->text();
}

QString DatabaseDialog::getpass(){
    return ui->passtxtbox->text();
}

void DatabaseDialog::setuser(QString user){
    ui->usertxtbox->setText(user);
}

void DatabaseDialog::setpass(QString pass){
    ui->passtxtbox->setText(pass);
}

void DatabaseDialog::setdb(QString db){
    ui->dbtxtbox->setText(db);
}

void DatabaseDialog::sethost(QString host){
    ui->hosttxtbox->setText(host);
}
