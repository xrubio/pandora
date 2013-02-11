/********************************************************************************
** Form generated from reading UI file 'AgentConfigurator.ui'
**
** Created: Tue Feb 5 12:54:50 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AGENTCONFIGURATOR_H
#define UI_AGENTCONFIGURATOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AgentConfigurator
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *Display2D;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QRadioButton *colorButton;
    QRadioButton *iconButton;
    QVBoxLayout *verticalLayout_2;
    QPushButton *colorChooserButton;
    QPushButton *iconChooserButton;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *size2D;
    QWidget *Display3D;
    QWidget *horizontalLayoutWidget_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label;
    QLineEdit *model3DFilename;
    QPushButton *model3DButton;
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

    void setupUi(QDialog *AgentConfigurator)
    {
        if (AgentConfigurator->objectName().isEmpty())
            AgentConfigurator->setObjectName(QString::fromUtf8("AgentConfigurator"));
        AgentConfigurator->resize(283, 348);
        buttonBox = new QDialogButtonBox(AgentConfigurator);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 300, 221, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tabWidget = new QTabWidget(AgentConfigurator);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(20, 20, 241, 271));
        Display2D = new QWidget();
        Display2D->setObjectName(QString::fromUtf8("Display2D"));
        horizontalLayoutWidget = new QWidget(Display2D);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(20, 20, 181, 107));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        colorButton = new QRadioButton(horizontalLayoutWidget);
        colorButton->setObjectName(QString::fromUtf8("colorButton"));

        verticalLayout->addWidget(colorButton);

        iconButton = new QRadioButton(horizontalLayoutWidget);
        iconButton->setObjectName(QString::fromUtf8("iconButton"));

        verticalLayout->addWidget(iconButton);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        colorChooserButton = new QPushButton(horizontalLayoutWidget);
        colorChooserButton->setObjectName(QString::fromUtf8("colorChooserButton"));

        verticalLayout_2->addWidget(colorChooserButton);

        iconChooserButton = new QPushButton(horizontalLayoutWidget);
        iconChooserButton->setObjectName(QString::fromUtf8("iconChooserButton"));

        verticalLayout_2->addWidget(iconChooserButton);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalLayoutWidget_2 = new QWidget(Display2D);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(20, 150, 181, 61));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(horizontalLayoutWidget_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        size2D = new QSpinBox(horizontalLayoutWidget_2);
        size2D->setObjectName(QString::fromUtf8("size2D"));
        size2D->setMaximum(1000);

        horizontalLayout_2->addWidget(size2D);

        tabWidget->addTab(Display2D, QString());
        Display3D = new QWidget();
        Display3D->setObjectName(QString::fromUtf8("Display3D"));
        horizontalLayoutWidget_6 = new QWidget(Display3D);
        horizontalLayoutWidget_6->setObjectName(QString::fromUtf8("horizontalLayoutWidget_6"));
        horizontalLayoutWidget_6->setGeometry(QRect(10, 40, 201, 63));
        horizontalLayout_6 = new QHBoxLayout(horizontalLayoutWidget_6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(horizontalLayoutWidget_6);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_6->addWidget(label);

        model3DFilename = new QLineEdit(horizontalLayoutWidget_6);
        model3DFilename->setObjectName(QString::fromUtf8("model3DFilename"));

        horizontalLayout_6->addWidget(model3DFilename);

        model3DButton = new QPushButton(horizontalLayoutWidget_6);
        model3DButton->setObjectName(QString::fromUtf8("model3DButton"));

        horizontalLayout_6->addWidget(model3DButton);

        verticalLayoutWidget_3 = new QWidget(Display3D);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(100, 130, 101, 101));
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
        xSize->setMinimum(0.01);
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
        ySize->setMinimum(0.01);
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
        zSize->setMinimum(0.01);
        zSize->setValue(1);

        horizontalLayout_4->addWidget(zSize);


        verticalLayout_3->addLayout(horizontalLayout_4);

        label_6 = new QLabel(Display3D);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(30, 150, 53, 15));
        tabWidget->addTab(Display3D, QString());

        retranslateUi(AgentConfigurator);
        QObject::connect(buttonBox, SIGNAL(accepted()), AgentConfigurator, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AgentConfigurator, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AgentConfigurator);
    } // setupUi

    void retranslateUi(QDialog *AgentConfigurator)
    {
        AgentConfigurator->setWindowTitle(QApplication::translate("AgentConfigurator", "Dialog", 0, QApplication::UnicodeUTF8));
        colorButton->setText(QApplication::translate("AgentConfigurator", "Color", 0, QApplication::UnicodeUTF8));
        iconButton->setText(QApplication::translate("AgentConfigurator", "Icon", 0, QApplication::UnicodeUTF8));
        colorChooserButton->setText(QString());
        iconChooserButton->setText(QString());
        label_2->setText(QApplication::translate("AgentConfigurator", "Size", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(Display2D), QApplication::translate("AgentConfigurator", "2D Display", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AgentConfigurator", "Model", 0, QApplication::UnicodeUTF8));
        model3DButton->setText(QApplication::translate("AgentConfigurator", "...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("AgentConfigurator", "X", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("AgentConfigurator", "Y", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("AgentConfigurator", "Z", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("AgentConfigurator", "Size", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(Display3D), QApplication::translate("AgentConfigurator", "3D Display", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AgentConfigurator: public Ui_AgentConfigurator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AGENTCONFIGURATOR_H
