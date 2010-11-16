/********************************************************************************
** Form generated from reading UI file 'filtermassdialog.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILTERMASSDIALOG_H
#define UI_FILTERMASSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FilterMassDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_1stFilterWeightClean;
    QLineEdit *lineEdit_1stFilterWeightClean;
    QLabel *label_1stFilterWeightDirty;
    QLineEdit *lineEdit_1stFilterWeightDirty;
    QLabel *label_2ndFilterWeightClean;
    QLineEdit *lineEdit_2ndFilterWeightClean;
    QLabel *label_2ndFilterWeightDirty;
    QLineEdit *lineEdit_2ndFilterWeightDirty;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *action_OK;
    QPushButton *action_Cancel;

    void setupUi(QDialog *FilterMassDialog)
    {
        if (FilterMassDialog->objectName().isEmpty())
            FilterMassDialog->setObjectName(QString::fromUtf8("FilterMassDialog"));
        FilterMassDialog->setWindowModality(Qt::ApplicationModal);
        FilterMassDialog->resize(325, 188);
        FilterMassDialog->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        FilterMassDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(FilterMassDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_1stFilterWeightClean = new QLabel(FilterMassDialog);
        label_1stFilterWeightClean->setObjectName(QString::fromUtf8("label_1stFilterWeightClean"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_1stFilterWeightClean);

        lineEdit_1stFilterWeightClean = new QLineEdit(FilterMassDialog);
        lineEdit_1stFilterWeightClean->setObjectName(QString::fromUtf8("lineEdit_1stFilterWeightClean"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_1stFilterWeightClean);

        label_1stFilterWeightDirty = new QLabel(FilterMassDialog);
        label_1stFilterWeightDirty->setObjectName(QString::fromUtf8("label_1stFilterWeightDirty"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_1stFilterWeightDirty);

        lineEdit_1stFilterWeightDirty = new QLineEdit(FilterMassDialog);
        lineEdit_1stFilterWeightDirty->setObjectName(QString::fromUtf8("lineEdit_1stFilterWeightDirty"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_1stFilterWeightDirty);

        label_2ndFilterWeightClean = new QLabel(FilterMassDialog);
        label_2ndFilterWeightClean->setObjectName(QString::fromUtf8("label_2ndFilterWeightClean"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2ndFilterWeightClean);

        lineEdit_2ndFilterWeightClean = new QLineEdit(FilterMassDialog);
        lineEdit_2ndFilterWeightClean->setObjectName(QString::fromUtf8("lineEdit_2ndFilterWeightClean"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEdit_2ndFilterWeightClean);

        label_2ndFilterWeightDirty = new QLabel(FilterMassDialog);
        label_2ndFilterWeightDirty->setObjectName(QString::fromUtf8("label_2ndFilterWeightDirty"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_2ndFilterWeightDirty);

        lineEdit_2ndFilterWeightDirty = new QLineEdit(FilterMassDialog);
        lineEdit_2ndFilterWeightDirty->setObjectName(QString::fromUtf8("lineEdit_2ndFilterWeightDirty"));

        formLayout->setWidget(3, QFormLayout::FieldRole, lineEdit_2ndFilterWeightDirty);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(4, QFormLayout::LabelRole, verticalSpacer);


        verticalLayout->addLayout(formLayout);

        line = new QFrame(FilterMassDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        action_OK = new QPushButton(FilterMassDialog);
        action_OK->setObjectName(QString::fromUtf8("action_OK"));

        horizontalLayout->addWidget(action_OK);

        action_Cancel = new QPushButton(FilterMassDialog);
        action_Cancel->setObjectName(QString::fromUtf8("action_Cancel"));

        horizontalLayout->addWidget(action_Cancel);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(lineEdit_1stFilterWeightClean, lineEdit_1stFilterWeightDirty);
        QWidget::setTabOrder(lineEdit_1stFilterWeightDirty, lineEdit_2ndFilterWeightClean);
        QWidget::setTabOrder(lineEdit_2ndFilterWeightClean, lineEdit_2ndFilterWeightDirty);
        QWidget::setTabOrder(lineEdit_2ndFilterWeightDirty, action_Cancel);

        retranslateUi(FilterMassDialog);
        QObject::connect(action_OK, SIGNAL(clicked()), FilterMassDialog, SLOT(accept()));
        QObject::connect(action_Cancel, SIGNAL(clicked()), FilterMassDialog, SLOT(reject()));
        QObject::connect(lineEdit_1stFilterWeightClean, SIGNAL(returnPressed()), FilterMassDialog, SLOT(accept()));
        QObject::connect(lineEdit_1stFilterWeightDirty, SIGNAL(returnPressed()), FilterMassDialog, SLOT(accept()));
        QObject::connect(lineEdit_2ndFilterWeightClean, SIGNAL(returnPressed()), FilterMassDialog, SLOT(accept()));
        QObject::connect(lineEdit_2ndFilterWeightDirty, SIGNAL(returnPressed()), FilterMassDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(FilterMassDialog);
    } // setupUi

    void retranslateUi(QDialog *FilterMassDialog)
    {
        FilterMassDialog->setWindowTitle(QApplication::translate("FilterMassDialog", "Qr49: PT Filter Mass", 0, QApplication::UnicodeUTF8));
        label_1stFilterWeightClean->setText(QApplication::translate("FilterMassDialog", "1st Filter Weight (Clean), mg", 0, QApplication::UnicodeUTF8));
        lineEdit_1stFilterWeightClean->setText(QApplication::translate("FilterMassDialog", "0", 0, QApplication::UnicodeUTF8));
        label_1stFilterWeightDirty->setText(QApplication::translate("FilterMassDialog", "1st Filter Weight (Dirty), mg", 0, QApplication::UnicodeUTF8));
        lineEdit_1stFilterWeightDirty->setText(QApplication::translate("FilterMassDialog", "0", 0, QApplication::UnicodeUTF8));
        label_2ndFilterWeightClean->setText(QApplication::translate("FilterMassDialog", "2nd Filter Weight (Clean), mg", 0, QApplication::UnicodeUTF8));
        lineEdit_2ndFilterWeightClean->setText(QApplication::translate("FilterMassDialog", "0", 0, QApplication::UnicodeUTF8));
        label_2ndFilterWeightDirty->setText(QApplication::translate("FilterMassDialog", "2nd Filter Weight (Dirty), mg", 0, QApplication::UnicodeUTF8));
        lineEdit_2ndFilterWeightDirty->setText(QApplication::translate("FilterMassDialog", "0", 0, QApplication::UnicodeUTF8));
        action_OK->setText(QApplication::translate("FilterMassDialog", "OK", 0, QApplication::UnicodeUTF8));
        action_Cancel->setText(QApplication::translate("FilterMassDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FilterMassDialog: public Ui_FilterMassDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILTERMASSDIALOG_H
