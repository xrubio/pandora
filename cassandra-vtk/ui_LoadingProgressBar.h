/********************************************************************************
** Form generated from reading UI file 'LoadingProgressBar.ui'
**
** Created: Tue Feb 5 12:54:50 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOADINGPROGRESSBAR_H
#define UI_LOADINGPROGRESSBAR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoadingProgressBar
{
public:
    QLabel *state;
    QProgressBar *bar;

    void setupUi(QWidget *LoadingProgressBar)
    {
        if (LoadingProgressBar->objectName().isEmpty())
            LoadingProgressBar->setObjectName(QString::fromUtf8("LoadingProgressBar"));
        LoadingProgressBar->resize(562, 80);
        state = new QLabel(LoadingProgressBar);
        state->setObjectName(QString::fromUtf8("state"));
        state->setGeometry(QRect(30, 10, 501, 21));
        state->setAlignment(Qt::AlignCenter);
        bar = new QProgressBar(LoadingProgressBar);
        bar->setObjectName(QString::fromUtf8("bar"));
        bar->setGeometry(QRect(30, 40, 501, 23));
        bar->setValue(24);

        retranslateUi(LoadingProgressBar);

        QMetaObject::connectSlotsByName(LoadingProgressBar);
    } // setupUi

    void retranslateUi(QWidget *LoadingProgressBar)
    {
        LoadingProgressBar->setWindowTitle(QApplication::translate("LoadingProgressBar", "Cassandra - Loading Simulation", 0, QApplication::UnicodeUTF8));
        state->setText(QApplication::translate("LoadingProgressBar", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LoadingProgressBar: public Ui_LoadingProgressBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOADINGPROGRESSBAR_H
