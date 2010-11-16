/********************************************************************************
** Form generated from reading UI file 'checkoutdatadialog.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHECKOUTDATADIALOG_H
#define UI_CHECKOUTDATADIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CheckoutDataDialogClass
{
public:
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit_CheckoutData;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_SaveAs;
    QPushButton *pushButton_Close;

    void setupUi(QDialog *CheckoutDataDialogClass)
    {
        if (CheckoutDataDialogClass->objectName().isEmpty())
            CheckoutDataDialogClass->setObjectName(QString::fromUtf8("CheckoutDataDialogClass"));
        CheckoutDataDialogClass->setWindowModality(Qt::WindowModal);
        CheckoutDataDialogClass->resize(640, 480);
        CheckoutDataDialogClass->setMinimumSize(QSize(640, 480));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        CheckoutDataDialogClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(CheckoutDataDialogClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        plainTextEdit_CheckoutData = new QPlainTextEdit(CheckoutDataDialogClass);
        plainTextEdit_CheckoutData->setObjectName(QString::fromUtf8("plainTextEdit_CheckoutData"));
        plainTextEdit_CheckoutData->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextEdit_CheckoutData->setReadOnly(true);

        verticalLayout->addWidget(plainTextEdit_CheckoutData);

        line = new QFrame(CheckoutDataDialogClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_SaveAs = new QPushButton(CheckoutDataDialogClass);
        pushButton_SaveAs->setObjectName(QString::fromUtf8("pushButton_SaveAs"));

        horizontalLayout->addWidget(pushButton_SaveAs);

        pushButton_Close = new QPushButton(CheckoutDataDialogClass);
        pushButton_Close->setObjectName(QString::fromUtf8("pushButton_Close"));

        horizontalLayout->addWidget(pushButton_Close);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(pushButton_Close, pushButton_SaveAs);
        QWidget::setTabOrder(pushButton_SaveAs, plainTextEdit_CheckoutData);

        retranslateUi(CheckoutDataDialogClass);
        QObject::connect(pushButton_Close, SIGNAL(clicked()), CheckoutDataDialogClass, SLOT(hide()));

        QMetaObject::connectSlotsByName(CheckoutDataDialogClass);
    } // setupUi

    void retranslateUi(QDialog *CheckoutDataDialogClass)
    {
        CheckoutDataDialogClass->setWindowTitle(QApplication::translate("CheckoutDataDialogClass", "Qr49: Checkout Data", 0, QApplication::UnicodeUTF8));
        pushButton_SaveAs->setText(QApplication::translate("CheckoutDataDialogClass", "Save As...", 0, QApplication::UnicodeUTF8));
        pushButton_Close->setText(QApplication::translate("CheckoutDataDialogClass", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CheckoutDataDialogClass: public Ui_CheckoutDataDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHECKOUTDATADIALOG_H
