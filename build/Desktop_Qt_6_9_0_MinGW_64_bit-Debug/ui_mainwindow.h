/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_play1;
    QPushButton *pushButton_play2;
    QPushButton *pushButton_play3;
    QPushButton *pushButton_guide;
    QPushButton *pushButton_exit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        MainWindow->setMinimumSize(QSize(800, 600));
        MainWindow->setMaximumSize(QSize(800, 600));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setEnabled(true);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        pushButton_play1 = new QPushButton(centralwidget);
        pushButton_play1->setObjectName("pushButton_play1");

        verticalLayout->addWidget(pushButton_play1);

        pushButton_play2 = new QPushButton(centralwidget);
        pushButton_play2->setObjectName("pushButton_play2");

        verticalLayout->addWidget(pushButton_play2);

        pushButton_play3 = new QPushButton(centralwidget);
        pushButton_play3->setObjectName("pushButton_play3");

        verticalLayout->addWidget(pushButton_play3);

        pushButton_guide = new QPushButton(centralwidget);
        pushButton_guide->setObjectName("pushButton_guide");

        verticalLayout->addWidget(pushButton_guide);

        pushButton_exit = new QPushButton(centralwidget);
        pushButton_exit->setObjectName("pushButton_exit");

        verticalLayout->addWidget(pushButton_exit);


        gridLayout->addLayout(verticalLayout, 0, 1, 2, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);
        QObject::connect(pushButton_exit, &QPushButton::clicked, MainWindow, qOverload<>(&QMainWindow::close));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Chess960", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButton_play1->setText(QCoreApplication::translate("MainWindow", "\320\230\320\223\320\240\320\220 \320\235\320\220 \320\236\320\224\320\235\320\236\320\231 \320\224\320\236\320\241\320\232\320\225", nullptr));
        pushButton_play2->setText(QCoreApplication::translate("MainWindow", "\320\230\320\223\320\240\320\220 \320\237\320\240\320\236\320\242\320\230\320\222 \320\221\320\236\320\242\320\220", nullptr));
        pushButton_play3->setText(QCoreApplication::translate("MainWindow", "\320\230\320\223\320\240\320\220 \320\237\320\236 \320\241\320\225\320\242\320\230", nullptr));
        pushButton_guide->setText(QCoreApplication::translate("MainWindow", "\320\223\320\220\320\231\320\224", nullptr));
        pushButton_exit->setText(QCoreApplication::translate("MainWindow", "\320\222\320\253\320\245\320\236\320\224", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
