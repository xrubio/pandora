/********************************************************************************
** Form generated from reading UI file 'displayvtk.ui'
**
** Created: Fri Jan 25 18:08:14 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYVTK_H
#define UI_DISPLAYVTK_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_DisplayVTK
{
public:
    QVTKWidget *qvtkWidget;

    void setupUi(QWidget *DisplayVTK)
    {
        if (DisplayVTK->objectName().isEmpty())
            DisplayVTK->setObjectName(QString::fromUtf8("DisplayVTK"));
        DisplayVTK->resize(400, 300);
        qvtkWidget = new QVTKWidget(DisplayVTK);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
        qvtkWidget->setGeometry(QRect(0, 0, 401, 311));
        qvtkWidget->setMaximumSize(QSize(401, 311));

        retranslateUi(DisplayVTK);

        QMetaObject::connectSlotsByName(DisplayVTK);
    } // setupUi

    void retranslateUi(QWidget *DisplayVTK)
    {
        DisplayVTK->setWindowTitle(QApplication::translate("DisplayVTK", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DisplayVTK: public Ui_DisplayVTK {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYVTK_H
