/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;
    QTextEdit *APP_File_TextEdit;
    QTextEdit *EMV_File_TextEdit;
    QPushButton *APP_File_Select;
    QPushButton *EMV_File_Select;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(700, 400);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(700, 400));
        MainWindow->setMaximumSize(QSize(700, 400));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 40, 31, 16));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 100, 31, 16));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(285, 200, 130, 40));
        APP_File_TextEdit = new QTextEdit(centralwidget);
        APP_File_TextEdit->setObjectName(QString::fromUtf8("APP_File_TextEdit"));
        APP_File_TextEdit->setGeometry(QRect(89, 30, 480, 40));
        sizePolicy.setHeightForWidth(APP_File_TextEdit->sizePolicy().hasHeightForWidth());
        APP_File_TextEdit->setSizePolicy(sizePolicy);
        EMV_File_TextEdit = new QTextEdit(centralwidget);
        EMV_File_TextEdit->setObjectName(QString::fromUtf8("EMV_File_TextEdit"));
        EMV_File_TextEdit->setGeometry(QRect(89, 90, 480, 40));
        APP_File_Select = new QPushButton(centralwidget);
        APP_File_Select->setObjectName(QString::fromUtf8("APP_File_Select"));
        APP_File_Select->setGeometry(QRect(590, 30, 88, 40));
        APP_File_Select->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        EMV_File_Select = new QPushButton(centralwidget);
        EMV_File_Select->setObjectName(QString::fromUtf8("EMV_File_Select"));
        EMV_File_Select->setGeometry(QRect(590, 90, 88, 40));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 700, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ota_pack_tool_1.0", nullptr));
#if QT_CONFIG(tooltip)
        label->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p>address of app file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("MainWindow", "APP:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "EMV:", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "pack file", nullptr));
#if QT_CONFIG(tooltip)
        APP_File_TextEdit->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p>File Path</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        EMV_File_TextEdit->setWhatsThis(QCoreApplication::translate("MainWindow", "<html><head/><body><p>File Path</p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        APP_File_Select->setText(QCoreApplication::translate("MainWindow", "Choose", nullptr));
        EMV_File_Select->setText(QCoreApplication::translate("MainWindow", "Choose", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
