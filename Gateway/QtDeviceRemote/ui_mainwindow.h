/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
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
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *page_3;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QLabel *label_3;
    QPlainTextEdit *plainTextEdit_3;
    QWidget *page_2;
    QPushButton *pushButton;
    QPushButton *pushButton_3;
    QLabel *label;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QLabel *label_2;
    QPlainTextEdit *plainTextEdit_2;
    QPushButton *pushButton_15;
    QWidget *page;
    QPushButton *pushButton_2;
    QPushButton *pushButton_10;
    QPlainTextEdit *plainTextEdit_4;
    QPushButton *pushButton_11;
    QPlainTextEdit *plainTextEdit_5;
    QPlainTextEdit *plainTextEdit_6;
    QPlainTextEdit *plainTextEdit_7;
    QPushButton *pushButton_12;
    QPushButton *pushButton_13;
    QPushButton *pushButton_14;
    QPushButton *pushButton_16;
    QPushButton *pushButton_17;
    QPlainTextEdit *plainTextEdit_8;
    QPushButton *pushButton_18;
    QPushButton *pushButton_19;
    QPlainTextEdit *plainTextEdit_9;
    QPushButton *pushButton_20;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 480);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setEnabled(true);
        stackedWidget->setStyleSheet(QString::fromUtf8(""));
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        pushButton_8 = new QPushButton(page_3);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(210, 120, 141, 131));
        pushButton_9 = new QPushButton(page_3);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(370, 120, 141, 131));
        label_3 = new QLabel(page_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(220, 50, 241, 51));
        QFont font;
        font.setPointSize(20);
        font.setItalic(true);
        label_3->setFont(font);
        plainTextEdit_3 = new QPlainTextEdit(page_3);
        plainTextEdit_3->setObjectName(QString::fromUtf8("plainTextEdit_3"));
        plainTextEdit_3->setGeometry(QRect(210, 280, 301, 41));
        stackedWidget->addWidget(page_3);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        pushButton = new QPushButton(page_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(460, 260, 161, 101));
        pushButton_3 = new QPushButton(page_2);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(300, 260, 161, 101));
        label = new QLabel(page_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(60, 100, 111, 61));
        label->setStyleSheet(QString::fromUtf8(""));
        plainTextEdit = new QPlainTextEdit(page_2);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(160, 110, 221, 51));
        plainTextEdit->setStyleSheet(QString::fromUtf8(""));
        pushButton_4 = new QPushButton(page_2);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(160, 160, 111, 91));
        pushButton_5 = new QPushButton(page_2);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(270, 160, 111, 91));
        pushButton_6 = new QPushButton(page_2);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(380, 160, 111, 91));
        pushButton_7 = new QPushButton(page_2);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(490, 160, 111, 91));
        label_2 = new QLabel(page_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(280, 50, 201, 61));
        label_2->setFont(font);
        plainTextEdit_2 = new QPlainTextEdit(page_2);
        plainTextEdit_2->setObjectName(QString::fromUtf8("plainTextEdit_2"));
        plainTextEdit_2->setGeometry(QRect(380, 110, 221, 51));
        pushButton_15 = new QPushButton(page_2);
        pushButton_15->setObjectName(QString::fromUtf8("pushButton_15"));
        pushButton_15->setGeometry(QRect(150, 260, 151, 101));
        stackedWidget->addWidget(page_2);
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        pushButton_2 = new QPushButton(page);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 290, 111, 101));
        pushButton_2->setStyleSheet(QString::fromUtf8(""));
        pushButton_10 = new QPushButton(page);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        pushButton_10->setGeometry(QRect(360, 20, 101, 81));
        plainTextEdit_4 = new QPlainTextEdit(page);
        plainTextEdit_4->setObjectName(QString::fromUtf8("plainTextEdit_4"));
        plainTextEdit_4->setGeometry(QRect(490, 30, 291, 371));
        plainTextEdit_4->setStyleSheet(QString::fromUtf8(""));
        pushButton_11 = new QPushButton(page);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));
        pushButton_11->setGeometry(QRect(360, 100, 101, 81));
        plainTextEdit_5 = new QPlainTextEdit(page);
        plainTextEdit_5->setObjectName(QString::fromUtf8("plainTextEdit_5"));
        plainTextEdit_5->setGeometry(QRect(190, 20, 141, 61));
        plainTextEdit_6 = new QPlainTextEdit(page);
        plainTextEdit_6->setObjectName(QString::fromUtf8("plainTextEdit_6"));
        plainTextEdit_6->setGeometry(QRect(190, 90, 141, 61));
        plainTextEdit_7 = new QPlainTextEdit(page);
        plainTextEdit_7->setObjectName(QString::fromUtf8("plainTextEdit_7"));
        plainTextEdit_7->setGeometry(QRect(190, 160, 141, 71));
        pushButton_12 = new QPushButton(page);
        pushButton_12->setObjectName(QString::fromUtf8("pushButton_12"));
        pushButton_12->setGeometry(QRect(30, 20, 151, 71));
        pushButton_13 = new QPushButton(page);
        pushButton_13->setObjectName(QString::fromUtf8("pushButton_13"));
        pushButton_13->setGeometry(QRect(30, 90, 151, 71));
        pushButton_14 = new QPushButton(page);
        pushButton_14->setObjectName(QString::fromUtf8("pushButton_14"));
        pushButton_14->setGeometry(QRect(30, 160, 151, 71));
        pushButton_16 = new QPushButton(page);
        pushButton_16->setObjectName(QString::fromUtf8("pushButton_16"));
        pushButton_16->setGeometry(QRect(360, 260, 101, 81));
        pushButton_17 = new QPushButton(page);
        pushButton_17->setObjectName(QString::fromUtf8("pushButton_17"));
        pushButton_17->setGeometry(QRect(120, 260, 111, 81));
        plainTextEdit_8 = new QPlainTextEdit(page);
        plainTextEdit_8->setObjectName(QString::fromUtf8("plainTextEdit_8"));
        plainTextEdit_8->setGeometry(QRect(240, 260, 91, 81));
        pushButton_18 = new QPushButton(page);
        pushButton_18->setObjectName(QString::fromUtf8("pushButton_18"));
        pushButton_18->setGeometry(QRect(360, 180, 101, 71));
        pushButton_19 = new QPushButton(page);
        pushButton_19->setObjectName(QString::fromUtf8("pushButton_19"));
        pushButton_19->setGeometry(QRect(120, 340, 111, 81));
        plainTextEdit_9 = new QPlainTextEdit(page);
        plainTextEdit_9->setObjectName(QString::fromUtf8("plainTextEdit_9"));
        plainTextEdit_9->setGeometry(QRect(240, 340, 91, 81));
        pushButton_20 = new QPushButton(page);
        pushButton_20->setObjectName(QString::fromUtf8("pushButton_20"));
        pushButton_20->setGeometry(QRect(360, 340, 101, 71));
        stackedWidget->addWidget(page);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 27));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton_8->setText(QApplication::translate("MainWindow", "\345\257\206\347\240\201\347\231\273\345\275\225", nullptr));
        pushButton_9->setText(QApplication::translate("MainWindow", "\346\214\207\347\272\271\347\231\273\345\275\225", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\202\250\347\232\204\347\231\273\345\275\225\346\226\271\345\274\217", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "\347\231\273\345\275\225", nullptr));
        pushButton_3->setText(QApplication::translate("MainWindow", "\345\233\236\351\200\200", nullptr));
        label->setText(QApplication::translate("MainWindow", "\347\231\273\345\275\225\345\257\206\347\240\201", nullptr));
        pushButton_4->setText(QApplication::translate("MainWindow", "1", nullptr));
        pushButton_5->setText(QApplication::translate("MainWindow", "2", nullptr));
        pushButton_6->setText(QApplication::translate("MainWindow", "3", nullptr));
        pushButton_7->setText(QApplication::translate("MainWindow", "4", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "SmartDevice", nullptr));
        pushButton_15->setText(QApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        pushButton_10->setText(QApplication::translate("MainWindow", "query sensor", nullptr));
        pushButton_11->setText(QApplication::translate("MainWindow", "device data", nullptr));
        pushButton_12->setText(QApplication::translate("MainWindow", "\347\261\273\345\236\213", nullptr));
        pushButton_13->setText(QApplication::translate("MainWindow", "\347\274\226\345\217\267", nullptr));
        pushButton_14->setText(QApplication::translate("MainWindow", "\346\223\215\344\275\234", nullptr));
        pushButton_16->setText(QApplication::translate("MainWindow", "\346\267\273\345\212\240\346\214\207\347\272\271", nullptr));
        pushButton_17->setText(QApplication::translate("MainWindow", "\345\275\225\345\205\245\344\275\215\347\275\256", nullptr));
        pushButton_18->setText(QApplication::translate("MainWindow", "sql data", nullptr));
        pushButton_19->setText(QApplication::translate("MainWindow", "\345\275\225\345\205\245\347\255\211\347\272\247", nullptr));
        pushButton_20->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\346\214\207\347\272\271", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
