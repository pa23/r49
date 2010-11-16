/********************************************************************************
** Form generated from reading UI file 'abcspeedscalcdialog.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABCSPEEDSCALCDIALOG_H
#define UI_ABCSPEEDSCALCDIALOG_H

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

class Ui_ABCspeedsCalcDialogClass
{
public:
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_n_hi;
    QLabel *label_2;
    QLineEdit *lineEdit_n_lo;
    QSpacerItem *verticalSpacer;
    QLabel *label_10;
    QLabel *label_11;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLineEdit *lineEdit_A;
    QLabel *label_4;
    QLineEdit *lineEdit_B;
    QLabel *label_5;
    QLineEdit *lineEdit_C;
    QLabel *label_6;
    QLineEdit *lineEdit_a1;
    QLabel *label_7;
    QLineEdit *lineEdit_a2;
    QLabel *label_8;
    QLineEdit *lineEdit_a3;
    QLabel *label_9;
    QLineEdit *lineEdit_n_ref;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QSpacerItem *verticalSpacer_2;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_CalcABCspeeds;
    QPushButton *pushButton_CloseABCDialog;

    void setupUi(QDialog *ABCspeedsCalcDialogClass)
    {
        if (ABCspeedsCalcDialogClass->objectName().isEmpty())
            ABCspeedsCalcDialogClass->setObjectName(QString::fromUtf8("ABCspeedsCalcDialogClass"));
        ABCspeedsCalcDialogClass->setWindowModality(Qt::WindowModal);
        ABCspeedsCalcDialogClass->resize(420, 321);
        ABCspeedsCalcDialogClass->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        ABCspeedsCalcDialogClass->setWindowIcon(icon);
        verticalLayout_4 = new QVBoxLayout(ABCspeedsCalcDialogClass);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox = new QGroupBox(ABCspeedsCalcDialogClass);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_n_hi = new QLineEdit(groupBox);
        lineEdit_n_hi->setObjectName(QString::fromUtf8("lineEdit_n_hi"));
        lineEdit_n_hi->setMaxLength(10);

        gridLayout->addWidget(lineEdit_n_hi, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_n_lo = new QLineEdit(groupBox);
        lineEdit_n_lo->setObjectName(QString::fromUtf8("lineEdit_n_lo"));
        lineEdit_n_lo->setMaxLength(10);

        gridLayout->addWidget(lineEdit_n_lo, 1, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 1, 1, 1);

        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 0, 2, 1, 1);

        label_11 = new QLabel(groupBox);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 1, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout);


        horizontalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(ABCspeedsCalcDialogClass);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

        lineEdit_A = new QLineEdit(groupBox_2);
        lineEdit_A->setObjectName(QString::fromUtf8("lineEdit_A"));
        lineEdit_A->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_A, 0, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 1, 0, 1, 1);

        lineEdit_B = new QLineEdit(groupBox_2);
        lineEdit_B->setObjectName(QString::fromUtf8("lineEdit_B"));
        lineEdit_B->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_B, 1, 1, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 2, 0, 1, 1);

        lineEdit_C = new QLineEdit(groupBox_2);
        lineEdit_C->setObjectName(QString::fromUtf8("lineEdit_C"));
        lineEdit_C->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_C, 2, 1, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 3, 0, 1, 1);

        lineEdit_a1 = new QLineEdit(groupBox_2);
        lineEdit_a1->setObjectName(QString::fromUtf8("lineEdit_a1"));
        lineEdit_a1->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_a1, 3, 1, 1, 1);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 4, 0, 1, 1);

        lineEdit_a2 = new QLineEdit(groupBox_2);
        lineEdit_a2->setObjectName(QString::fromUtf8("lineEdit_a2"));
        lineEdit_a2->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_a2, 4, 1, 1, 1);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_2->addWidget(label_8, 5, 0, 1, 1);

        lineEdit_a3 = new QLineEdit(groupBox_2);
        lineEdit_a3->setObjectName(QString::fromUtf8("lineEdit_a3"));
        lineEdit_a3->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_a3, 5, 1, 1, 1);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_2->addWidget(label_9, 6, 0, 1, 1);

        lineEdit_n_ref = new QLineEdit(groupBox_2);
        lineEdit_n_ref->setObjectName(QString::fromUtf8("lineEdit_n_ref"));
        lineEdit_n_ref->setReadOnly(true);

        gridLayout_2->addWidget(lineEdit_n_ref, 6, 1, 1, 1);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_2->addWidget(label_12, 0, 2, 1, 1);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_2->addWidget(label_13, 1, 2, 1, 1);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_2->addWidget(label_14, 2, 2, 1, 1);

        label_15 = new QLabel(groupBox_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_2->addWidget(label_15, 3, 2, 1, 1);

        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_2->addWidget(label_16, 4, 2, 1, 1);

        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_2->addWidget(label_17, 5, 2, 1, 1);

        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout_2->addWidget(label_18, 6, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 7, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);


        horizontalLayout_2->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        line = new QFrame(ABCspeedsCalcDialogClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_CalcABCspeeds = new QPushButton(ABCspeedsCalcDialogClass);
        pushButton_CalcABCspeeds->setObjectName(QString::fromUtf8("pushButton_CalcABCspeeds"));

        horizontalLayout->addWidget(pushButton_CalcABCspeeds);

        pushButton_CloseABCDialog = new QPushButton(ABCspeedsCalcDialogClass);
        pushButton_CloseABCDialog->setObjectName(QString::fromUtf8("pushButton_CloseABCDialog"));

        horizontalLayout->addWidget(pushButton_CloseABCDialog);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_4->addLayout(verticalLayout);

        QWidget::setTabOrder(lineEdit_n_hi, lineEdit_n_lo);
        QWidget::setTabOrder(lineEdit_n_lo, pushButton_CalcABCspeeds);
        QWidget::setTabOrder(pushButton_CalcABCspeeds, pushButton_CloseABCDialog);
        QWidget::setTabOrder(pushButton_CloseABCDialog, lineEdit_A);
        QWidget::setTabOrder(lineEdit_A, lineEdit_B);
        QWidget::setTabOrder(lineEdit_B, lineEdit_C);
        QWidget::setTabOrder(lineEdit_C, lineEdit_a1);
        QWidget::setTabOrder(lineEdit_a1, lineEdit_a2);
        QWidget::setTabOrder(lineEdit_a2, lineEdit_a3);
        QWidget::setTabOrder(lineEdit_a3, lineEdit_n_ref);

        retranslateUi(ABCspeedsCalcDialogClass);
        QObject::connect(pushButton_CloseABCDialog, SIGNAL(clicked()), ABCspeedsCalcDialogClass, SLOT(hide()));

        QMetaObject::connectSlotsByName(ABCspeedsCalcDialogClass);
    } // setupUi

    void retranslateUi(QDialog *ABCspeedsCalcDialogClass)
    {
        ABCspeedsCalcDialogClass->setWindowTitle(QApplication::translate("ABCspeedsCalcDialogClass", "Qr49: A, B, C speeds calculation", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ABCspeedsCalcDialogClass", "Source Data", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ABCspeedsCalcDialogClass", "n_hi", 0, QApplication::UnicodeUTF8));
        lineEdit_n_hi->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ABCspeedsCalcDialogClass", "n_lo", 0, QApplication::UnicodeUTF8));
        lineEdit_n_lo->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ABCspeedsCalcDialogClass", "Calculation Results", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ABCspeedsCalcDialogClass", "A", 0, QApplication::UnicodeUTF8));
        lineEdit_A->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ABCspeedsCalcDialogClass", "B", 0, QApplication::UnicodeUTF8));
        lineEdit_B->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ABCspeedsCalcDialogClass", "C", 0, QApplication::UnicodeUTF8));
        lineEdit_C->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ABCspeedsCalcDialogClass", "a1", 0, QApplication::UnicodeUTF8));
        lineEdit_a1->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("ABCspeedsCalcDialogClass", "a2", 0, QApplication::UnicodeUTF8));
        lineEdit_a2->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("ABCspeedsCalcDialogClass", "a3", 0, QApplication::UnicodeUTF8));
        lineEdit_a3->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("ABCspeedsCalcDialogClass", "n_ref", 0, QApplication::UnicodeUTF8));
        lineEdit_n_ref->setText(QApplication::translate("ABCspeedsCalcDialogClass", "0", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("ABCspeedsCalcDialogClass", "min-1", 0, QApplication::UnicodeUTF8));
        pushButton_CalcABCspeeds->setText(QApplication::translate("ABCspeedsCalcDialogClass", "Calculate", 0, QApplication::UnicodeUTF8));
        pushButton_CloseABCDialog->setText(QApplication::translate("ABCspeedsCalcDialogClass", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ABCspeedsCalcDialogClass: public Ui_ABCspeedsCalcDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABCSPEEDSCALCDIALOG_H
