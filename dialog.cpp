#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setModal(false);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
}

Dialog::~Dialog()
{
    delete ui;
}
void Dialog::on_findButton_clicked()
{
    emit send_find(ui->findDialog->text());
}
void Dialog::on_next_clicked()
{
    emit nextone(ui->findDialog->text());
}
void Dialog::on_replaceone_clicked()
{
    emit replace_one(ui -> findDialog -> text(), ui -> replaceDialog -> text());
}
void Dialog::on_replaceall_clicked()
{
     emit replace_all(ui -> findDialog -> text(), ui -> replaceDialog -> text());
}
void Dialog::setString(QString findStr)
{
    ui -> findDialog -> setText(findStr);
    on_findButton_clicked();
}
