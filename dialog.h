#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_findButton_clicked();

    void on_replaceone_clicked();

    void on_next_clicked();

    void on_replaceall_clicked();

    void setString(QString);
private:
    Ui::Dialog *ui;
signals:
    void nextone(QString);
    void send_find(QString);
    void replace_one(QString, QString);
    void replace_all(QString, QString);
};

#endif // DIALOG_H
