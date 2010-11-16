/********************************************************************************
** Form generated from reading UI file 'elrsmokecalcdialog.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ELRSMOKECALCDIALOG_H
#define UI_ELRSMOKECALCDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ELRsmokeCalcDialogClass
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_fA;
    QLabel *label_2;
    QLineEdit *lineEdit_sA;
    QLabel *label_3;
    QLineEdit *lineEdit_tA;
    QLabel *label_4;
    QLineEdit *lineEdit_fB;
    QLabel *label_5;
    QLineEdit *lineEdit_sB;
    QLabel *label_6;
    QLineEdit *lineEdit_tB;
    QLabel *label_7;
    QLineEdit *lineEdit_fC;
    QLabel *label_8;
    QLineEdit *lineEdit_sC;
    QLabel *label_9;
    QLineEdit *lineEdit_tC;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QGridLayout *gridLayout_2;
    QLabel *label_10;
    QLineEdit *lineEdit_ELRsmoke;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_CalcELRsmoke;
    QPushButton *pushButton_CloseELRDialog;

    void setupUi(QDialog *ELRsmokeCalcDialogClass)
    {
        if (ELRsmokeCalcDialogClass->objectName().isEmpty())
            ELRsmokeCalcDialogClass->setObjectName(QString::fromUtf8("ELRsmokeCalcDialogClass"));
        ELRsmokeCalcDialogClass->setWindowModality(Qt::ApplicationModal);
        ELRsmokeCalcDialogClass->resize(547, 383);
        ELRsmokeCalcDialogClass->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        ELRsmokeCalcDialogClass->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(ELRsmokeCalcDialogClass);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox = new QGroupBox(ELRsmokeCalcDialogClass);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_fA = new QLineEdit(groupBox);
        lineEdit_fA->setObjectName(QString::fromUtf8("lineEdit_fA"));
        lineEdit_fA->setMaxLength(10);

        gridLayout->addWidget(lineEdit_fA, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_sA = new QLineEdit(groupBox);
        lineEdit_sA->setObjectName(QString::fromUtf8("lineEdit_sA"));
        lineEdit_sA->setMaxLength(10);

        gridLayout->addWidget(lineEdit_sA, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        lineEdit_tA = new QLineEdit(groupBox);
        lineEdit_tA->setObjectName(QString::fromUtf8("lineEdit_tA"));
        lineEdit_tA->setMaxLength(10);

        gridLayout->addWidget(lineEdit_tA, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        lineEdit_fB = new QLineEdit(groupBox);
        lineEdit_fB->setObjectName(QString::fromUtf8("lineEdit_fB"));
        lineEdit_fB->setMaxLength(10);

        gridLayout->addWidget(lineEdit_fB, 3, 1, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        lineEdit_sB = new QLineEdit(groupBox);
        lineEdit_sB->setObjectName(QString::fromUtf8("lineEdit_sB"));
        lineEdit_sB->setMaxLength(10);

        gridLayout->addWidget(lineEdit_sB, 4, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        lineEdit_tB = new QLineEdit(groupBox);
        lineEdit_tB->setObjectName(QString::fromUtf8("lineEdit_tB"));
        lineEdit_tB->setMaxLength(10);

        gridLayout->addWidget(lineEdit_tB, 5, 1, 1, 1);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        lineEdit_fC = new QLineEdit(groupBox);
        lineEdit_fC->setObjectName(QString::fromUtf8("lineEdit_fC"));
        lineEdit_fC->setMaxLength(10);

        gridLayout->addWidget(lineEdit_fC, 6, 1, 1, 1);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 7, 0, 1, 1);

        lineEdit_sC = new QLineEdit(groupBox);
        lineEdit_sC->setObjectName(QString::fromUtf8("lineEdit_sC"));
        lineEdit_sC->setMaxLength(10);

        gridLayout->addWidget(lineEdit_sC, 7, 1, 1, 1);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 8, 0, 1, 1);

        lineEdit_tC = new QLineEdit(groupBox);
        lineEdit_tC->setObjectName(QString::fromUtf8("lineEdit_tC"));
        lineEdit_tC->setMaxLength(10);

        gridLayout->addWidget(lineEdit_tC, 8, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 9, 0, 1, 1);


        verticalLayout_3->addLayout(gridLayout);


        horizontalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(ELRsmokeCalcDialogClass);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_2->addWidget(label_10, 0, 0, 1, 1);

        lineEdit_ELRsmoke = new QLineEdit(groupBox_2);
        lineEdit_ELRsmoke->setObjectName(QString::fromUtf8("lineEdit_ELRsmoke"));
        lineEdit_ELRsmoke->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_ELRsmoke, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 1, 0, 1, 1);


        verticalLayout_4->addLayout(gridLayout_2);


        horizontalLayout->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout);

        line = new QFrame(ELRsmokeCalcDialogClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_CalcELRsmoke = new QPushButton(ELRsmokeCalcDialogClass);
        pushButton_CalcELRsmoke->setObjectName(QString::fromUtf8("pushButton_CalcELRsmoke"));

        horizontalLayout_2->addWidget(pushButton_CalcELRsmoke);

        pushButton_CloseELRDialog = new QPushButton(ELRsmokeCalcDialogClass);
        pushButton_CloseELRDialog->setObjectName(QString::fromUtf8("pushButton_CloseELRDialog"));

        horizontalLayout_2->addWidget(pushButton_CloseELRDialog);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout);

        QWidget::setTabOrder(lineEdit_fA, lineEdit_sA);
        QWidget::setTabOrder(lineEdit_sA, lineEdit_tA);
        QWidget::setTabOrder(lineEdit_tA, lineEdit_fB);
        QWidget::setTabOrder(lineEdit_fB, lineEdit_sB);
        QWidget::setTabOrder(lineEdit_sB, lineEdit_tB);
        QWidget::setTabOrder(lineEdit_tB, lineEdit_fC);
        QWidget::setTabOrder(lineEdit_fC, lineEdit_sC);
        QWidget::setTabOrder(lineEdit_sC, lineEdit_tC);
        QWidget::setTabOrder(lineEdit_tC, pushButton_CalcELRsmoke);
        QWidget::setTabOrder(pushButton_CalcELRsmoke, pushButton_CloseELRDialog);
        QWidget::setTabOrder(pushButton_CloseELRDialog, lineEdit_ELRsmoke);

        retranslateUi(ELRsmokeCalcDialogClass);
        QObject::connect(pushButton_CloseELRDialog, SIGNAL(clicked()), ELRsmokeCalcDialogClass, SLOT(hide()));

        QMetaObject::connectSlotsByName(ELRsmokeCalcDialogClass);
    } // setupUi

    void retranslateUi(QDialog *ELRsmokeCalcDialogClass)
    {
        ELRsmokeCalcDialogClass->setWindowTitle(QApplication::translate("ELRsmokeCalcDialogClass", "Qr49: ELR smoke calculation", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ELRsmokeCalcDialogClass", "Source Data", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ELRsmokeCalcDialogClass", "1st peak at A speed", 0, QApplication::UnicodeUTF8));
        lineEdit_fA->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ELRsmokeCalcDialogClass", "2nd peak at A speed", 0, QApplication::UnicodeUTF8));
        lineEdit_sA->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ELRsmokeCalcDialogClass", "3rd peak at A speed", 0, QApplication::UnicodeUTF8));
        lineEdit_tA->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ELRsmokeCalcDialogClass", "1st peak at B speed", 0, QApplication::UnicodeUTF8));
        lineEdit_fB->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ELRsmokeCalcDialogClass", "2nd peak at B speed", 0, QApplication::UnicodeUTF8));
        lineEdit_sB->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ELRsmokeCalcDialogClass", "3rd peak at B speed", 0, QApplication::UnicodeUTF8));
        lineEdit_tB->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("ELRsmokeCalcDialogClass", "1st peak at C speed", 0, QApplication::UnicodeUTF8));
        lineEdit_fC->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("ELRsmokeCalcDialogClass", "2nd peak at C speed", 0, QApplication::UnicodeUTF8));
        lineEdit_sC->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("ELRsmokeCalcDialogClass", "3rd peak at C speed", 0, QApplication::UnicodeUTF8));
        lineEdit_tC->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ELRsmokeCalcDialogClass", "Calculation Results", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("ELRsmokeCalcDialogClass", "ELR smoke", 0, QApplication::UnicodeUTF8));
        lineEdit_ELRsmoke->setText(QApplication::translate("ELRsmokeCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        pushButton_CalcELRsmoke->setText(QApplication::translate("ELRsmokeCalcDialogClass", "Calculate", 0, QApplication::UnicodeUTF8));
        pushButton_CloseELRDialog->setText(QApplication::translate("ELRsmokeCalcDialogClass", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ELRsmokeCalcDialogClass: public Ui_ELRsmokeCalcDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ELRSMOKECALCDIALOG_H
