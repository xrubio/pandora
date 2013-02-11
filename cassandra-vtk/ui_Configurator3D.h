/********************************************************************************
** Form generated from reading UI file 'Configurator3D.ui'
**
** Created: Tue Feb 5 12:54:50 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURATOR3D_H
#define UI_CONFIGURATOR3D_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Configurator3D
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *xSize;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QDoubleSpinBox *ySize;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QDoubleSpinBox *zSize;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QComboBox *DEMChooser;
    QComboBox *colorChooser;

    void setupUi(QDialog *Configurator3D)
    {
        if (Configurator3D->objectName().isEmpty())
            Configurator3D->setObjectName(QString::fromUtf8("Configurator3D"));
        Configurator3D->resize(400, 300);
        buttonBox = new QDialogButtonBox(Configurator3D);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        verticalLayoutWidget_3 = new QWidget(Configurator3D);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(130, 10, 138, 101));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(verticalLayoutWidget_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        xSize = new QDoubleSpinBox(verticalLayoutWidget_3);
        xSize->setObjectName(QString::fromUtf8("xSize"));
        xSize->setDecimals(3);
        xSize->setMinimum(0.001);
        xSize->setMaximum(1e+06);
        xSize->setValue(1);

        horizontalLayout_3->addWidget(xSize);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(verticalLayoutWidget_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        ySize = new QDoubleSpinBox(verticalLayoutWidget_3);
        ySize->setObjectName(QString::fromUtf8("ySize"));
        ySize->setDecimals(3);
        ySize->setMinimum(0.001);
        ySize->setMaximum(1e+06);
        ySize->setValue(1);

        horizontalLayout_5->addWidget(ySize);


        verticalLayout_3->addLayout(horizontalLayout_5);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(verticalLayoutWidget_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        zSize = new QDoubleSpinBox(verticalLayoutWidget_3);
        zSize->setObjectName(QString::fromUtf8("zSize"));
        zSize->setDecimals(3);
        zSize->setMinimum(0.001);
        zSize->setMaximum(1e+06);
        zSize->setValue(1);

        horizontalLayout_4->addWidget(zSize);


        verticalLayout_3->addLayout(horizontalLayout_4);

        label_6 = new QLabel(Configurator3D);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(60, 30, 51, 16));
        label_7 = new QLabel(Configurator3D);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(60, 120, 51, 16));
        label_8 = new QLabel(Configurator3D);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(60, 180, 51, 16));
        DEMChooser = new QComboBox(Configurator3D);
        DEMChooser->setObjectName(QString::fromUtf8("DEMChooser"));
        DEMChooser->setGeometry(QRect(140, 120, 78, 24));
        colorChooser = new QComboBox(Configurator3D);
        colorChooser->setObjectName(QString::fromUtf8("colorChooser"));
        colorChooser->setGeometry(QRect(140, 170, 78, 24));

        retranslateUi(Configurator3D);
        QObject::connect(buttonBox, SIGNAL(accepted()), Configurator3D, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Configurator3D, SLOT(reject()));

        QMetaObject::connectSlotsByName(Configurator3D);
    } // setupUi

    void retranslateUi(QDialog *Configurator3D)
    {
        Configurator3D->setWindowTitle(QApplication::translate("Configurator3D", "Dialog", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Configurator3D", "X", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Configurator3D", "Y", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Configurator3D", "Z", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Configurator3D", "Cell Size", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Configurator3D", "DEM:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Configurator3D", "Color:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Configurator3D: public Ui_Configurator3D {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURATOR3D_H
