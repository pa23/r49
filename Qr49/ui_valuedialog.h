/********************************************************************************
** Form generated from reading UI file 'valuedialog.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VALUEDIALOG_H
#define UI_VALUEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ValueDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label_Value;
    QLineEdit *lineEdit_Value;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *action_OK;
    QPushButton *action_Cancel;

    void setupUi(QDialog *ValueDialog)
    {
        if (ValueDialog->objectName().isEmpty())
            ValueDialog->setObjectName(QString::fromUtf8("ValueDialog"));
        ValueDialog->setWindowModality(Qt::ApplicationModal);
        ValueDialog->resize(196, 95);
        ValueDialog->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        ValueDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(ValueDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_Value = new QLabel(ValueDialog);
        label_Value->setObjectName(QString::fromUtf8("label_Value"));

        gridLayout->addWidget(label_Value, 0, 0, 1, 1);

        lineEdit_Value = new QLineEdit(ValueDialog);
        lineEdit_Value->setObjectName(QString::fromUtf8("lineEdit_Value"));

        gridLayout->addWidget(lineEdit_Value, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        line = new QFrame(ValueDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        action_OK = new QPushButton(ValueDialog);
        action_OK->setObjectName(QString::fromUtf8("action_OK"));

        horizontalLayout_2->addWidget(action_OK);

        action_Cancel = new QPushButton(ValueDialog);
        action_Cancel->setObjectName(QString::fromUtf8("action_Cancel"));

        horizontalLayout_2->addWidget(action_Cancel);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(ValueDialog);
        QObject::connect(action_OK, SIGNAL(clicked()), ValueDialog, SLOT(accept()));
        QObject::connect(action_Cancel, SIGNAL(clicked()), ValueDialog, SLOT(reject()));
        QObject::connect(lineEdit_Value, SIGNAL(returnPressed()), ValueDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(ValueDialog);
    } // setupUi

    void retranslateUi(QDialog *ValueDialog)
    {
        ValueDialog->setWindowTitle(QApplication::translate("ValueDialog", "Qr49: ", 0, QApplication::UnicodeUTF8));
        label_Value->setText(QApplication::translate("ValueDialog", "Value", 0, QApplication::UnicodeUTF8));
        lineEdit_Value->setText(QApplication::translate("ValueDialog", "0", 0, QApplication::UnicodeUTF8));
        action_OK->setText(QApplication::translate("ValueDialog", "OK", 0, QApplication::UnicodeUTF8));
        action_Cancel->setText(QApplication::translate("ValueDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ValueDialog: public Ui_ValueDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VALUEDIALOG_H
