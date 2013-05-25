/********************************************************************************
** Form generated from reading UI file 'fsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.0.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FSDIALOG_H
#define UI_FSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_FSDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;

    void setupUi(QDialog *FSDialog)
    {
        if (FSDialog->objectName().isEmpty())
            FSDialog->setObjectName(QStringLiteral("FSDialog"));
        FSDialog->resize(400, 123);
        buttonBox = new QDialogButtonBox(FSDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 80, 341, 40));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(FSDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 351, 51));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        retranslateUi(FSDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FSDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FSDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(FSDialog);
    } // setupUi

    void retranslateUi(QDialog *FSDialog)
    {
        FSDialog->setWindowTitle(QApplication::translate("FSDialog", "Dialog", 0));
        label->setText(QApplication::translate("FSDialog", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class FSDialog: public Ui_FSDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FSDIALOG_H
