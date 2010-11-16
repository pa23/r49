/********************************************************************************
** Form generated from reading UI file 'preferencesdialog.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

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

class Ui_PreferencesDialogClass
{
public:
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit_Preferences;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_OK;
    QPushButton *pushButton_Cancel;

    void setupUi(QDialog *PreferencesDialogClass)
    {
        if (PreferencesDialogClass->objectName().isEmpty())
            PreferencesDialogClass->setObjectName(QString::fromUtf8("PreferencesDialogClass"));
        PreferencesDialogClass->setWindowModality(Qt::ApplicationModal);
        PreferencesDialogClass->resize(640, 480);
        PreferencesDialogClass->setMinimumSize(QSize(640, 480));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesDialogClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(PreferencesDialogClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        plainTextEdit_Preferences = new QPlainTextEdit(PreferencesDialogClass);
        plainTextEdit_Preferences->setObjectName(QString::fromUtf8("plainTextEdit_Preferences"));
        plainTextEdit_Preferences->setLineWrapMode(QPlainTextEdit::NoWrap);

        verticalLayout->addWidget(plainTextEdit_Preferences);

        line = new QFrame(PreferencesDialogClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_OK = new QPushButton(PreferencesDialogClass);
        pushButton_OK->setObjectName(QString::fromUtf8("pushButton_OK"));

        horizontalLayout->addWidget(pushButton_OK);

        pushButton_Cancel = new QPushButton(PreferencesDialogClass);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));

        horizontalLayout->addWidget(pushButton_Cancel);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(pushButton_Cancel, pushButton_OK);
        QWidget::setTabOrder(pushButton_OK, plainTextEdit_Preferences);

        retranslateUi(PreferencesDialogClass);
        QObject::connect(pushButton_Cancel, SIGNAL(clicked()), PreferencesDialogClass, SLOT(hide()));
        QObject::connect(pushButton_OK, SIGNAL(clicked()), PreferencesDialogClass, SLOT(accept()));

        QMetaObject::connectSlotsByName(PreferencesDialogClass);
    } // setupUi

    void retranslateUi(QDialog *PreferencesDialogClass)
    {
        PreferencesDialogClass->setWindowTitle(QApplication::translate("PreferencesDialogClass", "Qr49: Preferences", 0, QApplication::UnicodeUTF8));
        pushButton_OK->setText(QApplication::translate("PreferencesDialogClass", "OK", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel->setText(QApplication::translate("PreferencesDialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PreferencesDialogClass: public Ui_PreferencesDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
