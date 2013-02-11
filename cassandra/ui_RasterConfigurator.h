/********************************************************************************
** Form generated from reading UI file 'RasterConfigurator.ui'
**
** Created: Tue Feb 5 12:54:50 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RASTERCONFIGURATOR_H
#define UI_RASTERCONFIGURATOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RasterConfigurator
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *colorRampLayout;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *transparentEnabled;
    QSpinBox *transparentValue;
    QWidget *Display2D;
    QWidget *Display3D;
    QWidget *horizontalLayoutWidget_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label;
    QLineEdit *model3DFilename;
    QPushButton *model3DButton;

    void setupUi(QDialog *RasterConfigurator)
    {
        if (RasterConfigurator->objectName().isEmpty())
            RasterConfigurator->setObjectName(QString::fromUtf8("RasterConfigurator"));
        RasterConfigurator->setEnabled(true);
        RasterConfigurator->resize(435, 478);
        buttonBox = new QDialogButtonBox(RasterConfigurator);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(120, 430, 221, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tabWidget = new QTabWidget(RasterConfigurator);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 20, 381, 411));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayoutWidget = new QWidget(tab);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 0, 351, 331));
        colorRampLayout = new QVBoxLayout(verticalLayoutWidget);
        colorRampLayout->setObjectName(QString::fromUtf8("colorRampLayout"));
        colorRampLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutWidget = new QWidget(tab);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(120, 340, 241, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        transparentEnabled = new QCheckBox(horizontalLayoutWidget);
        transparentEnabled->setObjectName(QString::fromUtf8("transparentEnabled"));

        horizontalLayout->addWidget(transparentEnabled);

        transparentValue = new QSpinBox(horizontalLayoutWidget);
        transparentValue->setObjectName(QString::fromUtf8("transparentValue"));

        horizontalLayout->addWidget(transparentValue);

        tabWidget->addTab(tab, QString());
        Display2D = new QWidget();
        Display2D->setObjectName(QString::fromUtf8("Display2D"));
        tabWidget->addTab(Display2D, QString());
        Display3D = new QWidget();
        Display3D->setObjectName(QString::fromUtf8("Display3D"));
        horizontalLayoutWidget_6 = new QWidget(Display3D);
        horizontalLayoutWidget_6->setObjectName(QString::fromUtf8("horizontalLayoutWidget_6"));
        horizontalLayoutWidget_6->setGeometry(QRect(10, 40, 211, 63));
        horizontalLayout_6 = new QHBoxLayout(horizontalLayoutWidget_6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(horizontalLayoutWidget_6);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_6->addWidget(label);

        model3DFilename = new QLineEdit(horizontalLayoutWidget_6);
        model3DFilename->setObjectName(QString::fromUtf8("model3DFilename"));
        model3DFilename->setEnabled(true);
        model3DFilename->setReadOnly(true);

        horizontalLayout_6->addWidget(model3DFilename);

        model3DButton = new QPushButton(horizontalLayoutWidget_6);
        model3DButton->setObjectName(QString::fromUtf8("model3DButton"));

        horizontalLayout_6->addWidget(model3DButton);

        tabWidget->addTab(Display3D, QString());

        retranslateUi(RasterConfigurator);
        QObject::connect(buttonBox, SIGNAL(accepted()), RasterConfigurator, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RasterConfigurator, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(RasterConfigurator);
    } // setupUi

    void retranslateUi(QDialog *RasterConfigurator)
    {
        RasterConfigurator->setWindowTitle(QApplication::translate("RasterConfigurator", "Dialog", 0, QApplication::UnicodeUTF8));
        transparentEnabled->setText(QApplication::translate("RasterConfigurator", "Transparent Value", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("RasterConfigurator", "Color Ramp", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(Display2D), QApplication::translate("RasterConfigurator", "2D Display", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RasterConfigurator", "Model", 0, QApplication::UnicodeUTF8));
        model3DButton->setText(QApplication::translate("RasterConfigurator", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(Display3D), QApplication::translate("RasterConfigurator", "3D Display", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RasterConfigurator: public Ui_RasterConfigurator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RASTERCONFIGURATOR_H
