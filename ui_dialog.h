/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLineEdit *findDialog;
    QLineEdit *replaceDialog;
    QLabel *label_3;
    QPushButton *findButton;
    QPushButton *replaceone;
    QPushButton *next;
    QPushButton *replaceall;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(340, 215);
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 50, 72, 21));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(120, 90, 81, 16));
        findDialog = new QLineEdit(Dialog);
        findDialog->setObjectName(QStringLiteral("findDialog"));
        findDialog->setGeometry(QRect(100, 50, 113, 23));
        replaceDialog = new QLineEdit(Dialog);
        replaceDialog->setObjectName(QStringLiteral("replaceDialog"));
        replaceDialog->setGeometry(QRect(100, 120, 113, 23));
        label_3 = new QLabel(Dialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(110, 0, 111, 20));
        findButton = new QPushButton(Dialog);
        findButton->setObjectName(QStringLiteral("findButton"));
        findButton->setGeometry(QRect(230, 50, 89, 24));
        replaceone = new QPushButton(Dialog);
        replaceone->setObjectName(QStringLiteral("replaceone"));
        replaceone->setGeometry(QRect(230, 120, 89, 24));
        next = new QPushButton(Dialog);
        next->setObjectName(QStringLiteral("next"));
        next->setGeometry(QRect(0, 120, 89, 24));
        replaceall = new QPushButton(Dialog);
        replaceall->setObjectName(QStringLiteral("replaceall"));
        replaceall->setGeometry(QRect(110, 160, 89, 24));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
        label->setText(QApplication::translate("Dialog", "\346\237\245\346\211\276\345\255\227\347\254\246\344\270\262", nullptr));
        label_2->setText(QApplication::translate("Dialog", "\346\233\277\346\215\242\345\255\227\347\254\246\344\270\262", nullptr));
        label_3->setText(QApplication::translate("Dialog", "\345\255\227\347\254\246\344\270\262\346\237\245\346\211\276\346\233\277\346\215\242", nullptr));
        findButton->setText(QApplication::translate("Dialog", "\346\237\245\346\211\276", nullptr));
        replaceone->setText(QApplication::translate("Dialog", "\346\233\277\346\215\242", nullptr));
        next->setText(QApplication::translate("Dialog", "\344\270\213\344\270\200\344\270\252", nullptr));
        replaceall->setText(QApplication::translate("Dialog", "\345\205\250\351\203\250\346\233\277\346\215\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
