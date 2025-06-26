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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label_6;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_play1;
    QPushButton *pushButton_play2;
    QPushButton *pushButton_play3;
    QPushButton *pushButton_guide;
    QPushButton *pushButton_exit;
    QLabel *label_5;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_3;
    QLabel *label;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_4;
    QLabel *label_3;
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
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 4, 3, 1, 1);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        pushButton_play1 = new QPushButton(centralwidget);
        pushButton_play1->setObjectName("pushButton_play1");
        QFont font;
        font.setFamilies({QString::fromUtf8("Century Gothic")});
        font.setPointSize(12);
        font.setBold(true);
        pushButton_play1->setFont(font);
        pushButton_play1->setMouseTracking(true);

        verticalLayout->addWidget(pushButton_play1);

        pushButton_play2 = new QPushButton(centralwidget);
        pushButton_play2->setObjectName("pushButton_play2");
        pushButton_play2->setFont(font);
        pushButton_play2->setMouseTracking(true);

        verticalLayout->addWidget(pushButton_play2);

        pushButton_play3 = new QPushButton(centralwidget);
        pushButton_play3->setObjectName("pushButton_play3");
        pushButton_play3->setFont(font);
        pushButton_play3->setMouseTracking(true);

        verticalLayout->addWidget(pushButton_play3);

        pushButton_guide = new QPushButton(centralwidget);
        pushButton_guide->setObjectName("pushButton_guide");
        pushButton_guide->setFont(font);
        pushButton_guide->setMouseTracking(true);

        verticalLayout->addWidget(pushButton_guide);

        pushButton_exit = new QPushButton(centralwidget);
        pushButton_exit->setObjectName("pushButton_exit");
        pushButton_exit->setFont(font);
        pushButton_exit->setMouseTracking(true);

        verticalLayout->addWidget(pushButton_exit);


        gridLayout->addLayout(verticalLayout, 4, 1, 1, 1);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 2, 3, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 4, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 5, 1, 1, 1);

        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 1, 3, 1, 1);

        label_8 = new QLabel(centralwidget);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 0, 3, 1, 1);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Century Gothic")});
        font1.setPointSize(50);
        label_4->setFont(font1);

        gridLayout->addWidget(label_4, 3, 1, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setStyleSheet(QString::fromUtf8("image: url(:/new/prefix1/pieces600/wn.png);"));

        gridLayout->addWidget(label_3, 0, 1, 3, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label_2->setText(QString());
        label_6->setText(QString());
        pushButton_play1->setText(QCoreApplication::translate("MainWindow", "\320\230\320\223\320\240\320\220 \320\235\320\220 \320\236\320\224\320\235\320\236\320\231 \320\224\320\236\320\241\320\232\320\225", nullptr));
        pushButton_play2->setText(QCoreApplication::translate("MainWindow", "\320\230\320\223\320\240\320\220 \320\237\320\240\320\236\320\242\320\230\320\222 \320\221\320\236\320\242\320\220", nullptr));
        pushButton_play3->setText(QCoreApplication::translate("MainWindow", "\320\230\320\223\320\240\320\220 \320\237\320\236 \320\241\320\225\320\242\320\230", nullptr));
        pushButton_guide->setText(QCoreApplication::translate("MainWindow", "\320\247\321\202\320\276 \321\202\320\260\320\272\320\276\320\265 \321\210\320\260\321\205\320\274\320\260\321\202\321\213 \320\244\320\270\321\210\320\265\321\200\320\260? ", nullptr));
        pushButton_exit->setText(QCoreApplication::translate("MainWindow", "\320\222\320\253\320\245\320\236\320\224", nullptr));
        label_5->setText(QString());
        label->setText(QString());
        label_7->setText(QString());
        label_8->setText(QString());
        label_4->setText(QCoreApplication::translate("MainWindow", "Chess960", nullptr));
        label_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
