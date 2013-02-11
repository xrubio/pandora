/********************************************************************************
** Form generated from reading UI file 'ColorInterval.ui'
**
** Created: Tue Feb 5 12:54:50 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORINTERVAL_H
#define UI_COLORINTERVAL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ColorInterval
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QSpinBox *minValue;
    QLabel *label_7;
    QSpinBox *maxValue;
    QPushButton *colorChooserButton;
    QPushButton *splitButton;
    QPushButton *removeButton;

    void setupUi(QWidget *ColorInterval)
    {
        if (ColorInterval->objectName().isEmpty())
            ColorInterval->setObjectName(QString::fromUtf8("ColorInterval"));
        ColorInterval->resize(380, 49);
        horizontalLayoutWidget = new QWidget(ColorInterval);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 10, 357, 26));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        minValue = new QSpinBox(horizontalLayoutWidget);
        minValue->setObjectName(QString::fromUtf8("minValue"));
        minValue->setEnabled(true);
        minValue->setMaximum(1000);
        minValue->setValue(25);

        horizontalLayout->addWidget(minValue);

        label_7 = new QLabel(horizontalLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_7);

        maxValue = new QSpinBox(horizontalLayoutWidget);
        maxValue->setObjectName(QString::fromUtf8("maxValue"));
        maxValue->setEnabled(false);
        maxValue->setMaximum(1000);
        maxValue->setValue(50);

        horizontalLayout->addWidget(maxValue);

        colorChooserButton = new QPushButton(horizontalLayoutWidget);
        colorChooserButton->setObjectName(QString::fromUtf8("colorChooserButton"));

        horizontalLayout->addWidget(colorChooserButton);

        splitButton = new QPushButton(horizontalLayoutWidget);
        splitButton->setObjectName(QString::fromUtf8("splitButton"));

        horizontalLayout->addWidget(splitButton);

        removeButton = new QPushButton(horizontalLayoutWidget);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));

        horizontalLayout->addWidget(removeButton);


        retranslateUi(ColorInterval);

        QMetaObject::connectSlotsByName(ColorInterval);
    } // setupUi

    void retranslateUi(QWidget *ColorInterval)
    {
        ColorInterval->setWindowTitle(QApplication::translate("ColorInterval", "Form", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("ColorInterval", "to", 0, QApplication::UnicodeUTF8));
        colorChooserButton->setText(QString());
        splitButton->setText(QApplication::translate("ColorInterval", "Split", 0, QApplication::UnicodeUTF8));
        removeButton->setText(QApplication::translate("ColorInterval", "Remove", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ColorInterval: public Ui_ColorInterval {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORINTERVAL_H
