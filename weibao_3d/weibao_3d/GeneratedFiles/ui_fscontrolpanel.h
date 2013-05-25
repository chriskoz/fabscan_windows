/********************************************************************************
** Form generated from reading UI file 'fscontrolpanel.ui'
**
** Created by: Qt User Interface Compiler version 5.0.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FSCONTROLPANEL_H
#define UI_FSCONTROLPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "qcameraviewfinder.h"

QT_BEGIN_NAMESPACE

class Ui_FSControlPanel
{
public:
    QGroupBox *groupBox;
    QPushButton *laserOnButton;
    QPushButton *laserOffButton;
    QPushButton *autoResetButton;
    QCheckBox *laserEnable;
    QPushButton *laserStepRightButton;
    QLabel *laserAngle;
    QLineEdit *laserSwipeMaxEdit;
    QLineEdit *laserSwipeMinEdit;
    QGroupBox *groupBox_2;
    QPushButton *fetchFrameButton;
    QPushButton *binaryImage;
    QPushButton *diffImage;
    QPushButton *pushButton_2;
    QGroupBox *groupBox_3;
    QPushButton *stepLeftButton;
    QPushButton *stepRightButton;
    QCheckBox *checkBox;
    QLineEdit *degreesEdit;
    QLabel *label;
    QCameraViewfinder *viewfinder;
    QLabel *cameraLabel;
    QPushButton *laserStepLeftButton;

    void setupUi(QWidget *FSControlPanel)
    {
        if (FSControlPanel->objectName().isEmpty())
            FSControlPanel->setObjectName(QStringLiteral("FSControlPanel"));
        FSControlPanel->resize(135, 480);
        FSControlPanel->setMinimumSize(QSize(135, 360));
        FSControlPanel->setMaximumSize(QSize(135, 480));
        FSControlPanel->setAutoFillBackground(true);
        groupBox = new QGroupBox(FSControlPanel);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 90, 121, 181));
        laserOnButton = new QPushButton(groupBox);
        laserOnButton->setObjectName(QStringLiteral("laserOnButton"));
        laserOnButton->setGeometry(QRect(0, 20, 61, 32));
        laserOffButton = new QPushButton(groupBox);
        laserOffButton->setObjectName(QStringLiteral("laserOffButton"));
        laserOffButton->setGeometry(QRect(60, 20, 61, 32));
        autoResetButton = new QPushButton(groupBox);
        autoResetButton->setObjectName(QStringLiteral("autoResetButton"));
        autoResetButton->setGeometry(QRect(0, 50, 121, 32));
        laserEnable = new QCheckBox(groupBox);
        laserEnable->setObjectName(QStringLiteral("laserEnable"));
        laserEnable->setGeometry(QRect(20, 130, 70, 20));
        laserStepRightButton = new QPushButton(groupBox);
        laserStepRightButton->setObjectName(QStringLiteral("laserStepRightButton"));
        laserStepRightButton->setGeometry(QRect(60, 80, 50, 32));
        laserAngle = new QLabel(groupBox);
        laserAngle->setObjectName(QStringLiteral("laserAngle"));
        laserAngle->setGeometry(QRect(10, 110, 101, 20));
        laserSwipeMaxEdit = new QLineEdit(groupBox);
        laserSwipeMaxEdit->setObjectName(QStringLiteral("laserSwipeMaxEdit"));
        laserSwipeMaxEdit->setGeometry(QRect(10, 150, 40, 21));
        laserSwipeMinEdit = new QLineEdit(groupBox);
        laserSwipeMinEdit->setObjectName(QStringLiteral("laserSwipeMinEdit"));
        laserSwipeMinEdit->setGeometry(QRect(70, 150, 40, 21));
        groupBox_2 = new QGroupBox(FSControlPanel);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 270, 121, 151));
        fetchFrameButton = new QPushButton(groupBox_2);
        fetchFrameButton->setObjectName(QStringLiteral("fetchFrameButton"));
        fetchFrameButton->setGeometry(QRect(0, 20, 121, 32));
        binaryImage = new QPushButton(groupBox_2);
        binaryImage->setObjectName(QStringLiteral("binaryImage"));
        binaryImage->setGeometry(QRect(0, 50, 121, 32));
        diffImage = new QPushButton(groupBox_2);
        diffImage->setObjectName(QStringLiteral("diffImage"));
        diffImage->setGeometry(QRect(0, 80, 121, 32));
        pushButton_2 = new QPushButton(groupBox_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(0, 110, 121, 32));
        groupBox_3 = new QGroupBox(FSControlPanel);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 0, 121, 91));
        stepLeftButton = new QPushButton(groupBox_3);
        stepLeftButton->setObjectName(QStringLiteral("stepLeftButton"));
        stepLeftButton->setGeometry(QRect(10, 40, 51, 21));
        stepRightButton = new QPushButton(groupBox_3);
        stepRightButton->setObjectName(QStringLiteral("stepRightButton"));
        stepRightButton->setGeometry(QRect(60, 40, 50, 21));
        checkBox = new QCheckBox(groupBox_3);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(20, 60, 70, 20));
        degreesEdit = new QLineEdit(groupBox_3);
        degreesEdit->setObjectName(QStringLiteral("degreesEdit"));
        degreesEdit->setGeometry(QRect(13, 16, 91, 22));
        degreesEdit->setLayoutDirection(Qt::LeftToRight);
        degreesEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label = new QLabel(groupBox_3);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(110, 30, 10, 16));
        viewfinder = new QCameraViewfinder(FSControlPanel);
        viewfinder->setObjectName(QStringLiteral("viewfinder"));
        viewfinder->setGeometry(QRect(130, 20, 211, 451));
        viewfinder->setAutoFillBackground(false);
        viewfinder->setStyleSheet(QStringLiteral(""));
        cameraLabel = new QLabel(viewfinder);
        cameraLabel->setObjectName(QStringLiteral("cameraLabel"));
        cameraLabel->setGeometry(QRect(10, 0, 211, 450));
        cameraLabel->setAutoFillBackground(false);
        cameraLabel->setStyleSheet(QStringLiteral("background-color: black; color: white;"));
        cameraLabel->setFrameShape(QFrame::Box);
        laserStepLeftButton = new QPushButton(FSControlPanel);
        laserStepLeftButton->setObjectName(QStringLiteral("laserStepLeftButton"));
        laserStepLeftButton->setGeometry(QRect(10, 430, 111, 32));

        retranslateUi(FSControlPanel);

        QMetaObject::connectSlotsByName(FSControlPanel);
    } // setupUi

    void retranslateUi(QWidget *FSControlPanel)
    {
        FSControlPanel->setWindowTitle(QApplication::translate("FSControlPanel", "Control Panel", 0));
        groupBox->setTitle(QApplication::translate("FSControlPanel", "Laser", 0));
        laserOnButton->setText(QApplication::translate("FSControlPanel", "On", 0));
        laserOffButton->setText(QApplication::translate("FSControlPanel", "Off", 0));
        autoResetButton->setText(QApplication::translate("FSControlPanel", "Detect Laser", 0));
        laserEnable->setText(QApplication::translate("FSControlPanel", "Enabled", 0));
        laserStepRightButton->setText(QApplication::translate("FSControlPanel", ">", 0));
        laserAngle->setText(QApplication::translate("FSControlPanel", "Angle:", 0));
        groupBox_2->setTitle(QApplication::translate("FSControlPanel", "Camera", 0));
        fetchFrameButton->setText(QApplication::translate("FSControlPanel", "Fetch Frame", 0));
        binaryImage->setText(QApplication::translate("FSControlPanel", "Binary Image", 0));
        diffImage->setText(QApplication::translate("FSControlPanel", "Diff Image", 0));
        pushButton_2->setText(QApplication::translate("FSControlPanel", "Laser Image", 0));
        groupBox_3->setTitle(QApplication::translate("FSControlPanel", "Turntable", 0));
        stepLeftButton->setText(QApplication::translate("FSControlPanel", "<", 0));
        stepRightButton->setText(QApplication::translate("FSControlPanel", ">", 0));
        checkBox->setText(QApplication::translate("FSControlPanel", "Enabled", 0));
        degreesEdit->setText(QApplication::translate("FSControlPanel", "10", 0));
        label->setText(QApplication::translate("FSControlPanel", "                         Select a camera from the menu!", 0));
        cameraLabel->setText(QApplication::translate("FSControlPanel", "                                                Select a camera from the menu!", 0));
        laserStepLeftButton->setText(QApplication::translate("FSControlPanel", "EXIT", 0));
    } // retranslateUi

};

namespace Ui {
    class FSControlPanel: public Ui_FSControlPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FSCONTROLPANEL_H
