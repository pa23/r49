/********************************************************************************
** Form generated from reading UI file 'qr49.ui'
**
** Created: Wed Nov 17 00:31:50 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QR49_H
#define UI_QR49_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Quit;
    QAction *action_PrintReport;
    QAction *action_Execute;
    QAction *action_AboutQt;
    QAction *action_AboutQr49;
    QAction *action_AddRow;
    QAction *action_DeleteRow;
    QAction *action_SaveReportAs;
    QAction *action_CopyFromTable;
    QAction *action_PasteToTable;
    QAction *action_DeleteFromTable;
    QAction *action_CutFromTable;
    QAction *action_Multiply;
    QAction *action_Divide;
    QAction *action_Add;
    QAction *action_Equal;
    QAction *action_SaveSourceData;
    QAction *action_OpenReport;
    QAction *action_CloseReport;
    QAction *action_ABCspeeds;
    QAction *action_ELRsmoke;
    QAction *action_ReportToPDF;
    QAction *action_UndoTable;
    QAction *action_RedoTable;
    QAction *action_Preferences;
    QAction *action_CheckoutData;
    QAction *action_StandardsDescription;
    QAction *action_LoadCalculationOptions;
    QAction *action_SaveCalculationOptionsAs;
    QAction *action_LoadSourceData;
    QAction *action_SaveSourceDataAs;
    QAction *action_Toolbar;
    QAction *action_OpenSolution;
    QAction *action_CheckForUpdates;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_5;
    QComboBox *comboBox_task;
    QLineEdit *lineEdit_Vh;
    QComboBox *comboBox_standard;
    QComboBox *comboBox_FuelType;
    QComboBox *comboBox_NOxSample;
    QComboBox *comboBox_PTcalc;
    QLineEdit *lineEdit_PTmass;
    QPushButton *pushButton_EnterPTmass;
    QComboBox *comboBox_AddPointsCalc;
    QFrame *line;
    QCheckBox *checkBox_reports;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QTabWidget *tabWidget_Data;
    QWidget *tab_SrcData;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_5;
    QTableWidget *tableWidget_SrcDataEU0;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tableWidget_SrcDataEU3;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *tableWidget_SrcDataPoints;
    QWidget *tab_Reports;
    QVBoxLayout *verticalLayout;
    QComboBox *comboBox_OpenedReports;
    QPlainTextEdit *plainTextEdit_Report;
    QWidget *tab_RedPower;
    QVBoxLayout *verticalLayout_7;
    QSplitter *splitter;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_8;
    QTableWidget *tableWidget_FullLoadCurve;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuExport;
    QMenu *menuHelp;
    QMenu *menuCalculation;
    QMenu *menuEdit;
    QMenu *menuView;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(1024, 700);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(1024, 700));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/toxic.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/application-exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Quit->setIcon(icon1);
        action_Quit->setIconVisibleInMenu(false);
        action_PrintReport = new QAction(MainWindow);
        action_PrintReport->setObjectName(QString::fromUtf8("action_PrintReport"));
        action_PrintReport->setEnabled(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/document-print.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_PrintReport->setIcon(icon2);
        action_PrintReport->setIconVisibleInMenu(false);
        action_Execute = new QAction(MainWindow);
        action_Execute->setObjectName(QString::fromUtf8("action_Execute"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/execute.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Execute->setIcon(icon3);
        action_Execute->setIconVisibleInMenu(false);
        action_AboutQt = new QAction(MainWindow);
        action_AboutQt->setObjectName(QString::fromUtf8("action_AboutQt"));
        action_AboutQt->setEnabled(true);
        action_AboutQr49 = new QAction(MainWindow);
        action_AboutQr49->setObjectName(QString::fromUtf8("action_AboutQr49"));
        action_AboutQr49->setEnabled(true);
        action_AddRow = new QAction(MainWindow);
        action_AddRow->setObjectName(QString::fromUtf8("action_AddRow"));
        action_DeleteRow = new QAction(MainWindow);
        action_DeleteRow->setObjectName(QString::fromUtf8("action_DeleteRow"));
        action_SaveReportAs = new QAction(MainWindow);
        action_SaveReportAs->setObjectName(QString::fromUtf8("action_SaveReportAs"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/document-save-as.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_SaveReportAs->setIcon(icon4);
        action_SaveReportAs->setIconVisibleInMenu(false);
        action_CopyFromTable = new QAction(MainWindow);
        action_CopyFromTable->setObjectName(QString::fromUtf8("action_CopyFromTable"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/edit-copy.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_CopyFromTable->setIcon(icon5);
        action_CopyFromTable->setIconVisibleInMenu(false);
        action_PasteToTable = new QAction(MainWindow);
        action_PasteToTable->setObjectName(QString::fromUtf8("action_PasteToTable"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/edit-paste.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_PasteToTable->setIcon(icon6);
        action_PasteToTable->setIconVisibleInMenu(false);
        action_DeleteFromTable = new QAction(MainWindow);
        action_DeleteFromTable->setObjectName(QString::fromUtf8("action_DeleteFromTable"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/edit-delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_DeleteFromTable->setIcon(icon7);
        action_DeleteFromTable->setIconVisibleInMenu(false);
        action_CutFromTable = new QAction(MainWindow);
        action_CutFromTable->setObjectName(QString::fromUtf8("action_CutFromTable"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/edit-cut.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_CutFromTable->setIcon(icon8);
        action_CutFromTable->setIconVisibleInMenu(false);
        action_Multiply = new QAction(MainWindow);
        action_Multiply->setObjectName(QString::fromUtf8("action_Multiply"));
        action_Multiply->setEnabled(true);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/mult.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Multiply->setIcon(icon9);
        action_Multiply->setIconVisibleInMenu(false);
        action_Divide = new QAction(MainWindow);
        action_Divide->setObjectName(QString::fromUtf8("action_Divide"));
        action_Divide->setEnabled(true);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/div.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Divide->setIcon(icon10);
        action_Divide->setIconVisibleInMenu(false);
        action_Add = new QAction(MainWindow);
        action_Add->setObjectName(QString::fromUtf8("action_Add"));
        action_Add->setEnabled(true);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/plus.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Add->setIcon(icon11);
        action_Add->setIconVisibleInMenu(false);
        action_Equal = new QAction(MainWindow);
        action_Equal->setObjectName(QString::fromUtf8("action_Equal"));
        action_Equal->setEnabled(true);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/eq.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Equal->setIcon(icon12);
        action_Equal->setIconVisibleInMenu(false);
        action_SaveSourceData = new QAction(MainWindow);
        action_SaveSourceData->setObjectName(QString::fromUtf8("action_SaveSourceData"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icons/document-save.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_SaveSourceData->setIcon(icon13);
        action_SaveSourceData->setIconVisibleInMenu(false);
        action_OpenReport = new QAction(MainWindow);
        action_OpenReport->setObjectName(QString::fromUtf8("action_OpenReport"));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/icons/document-open.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_OpenReport->setIcon(icon14);
        action_OpenReport->setIconVisibleInMenu(false);
        action_CloseReport = new QAction(MainWindow);
        action_CloseReport->setObjectName(QString::fromUtf8("action_CloseReport"));
        action_ABCspeeds = new QAction(MainWindow);
        action_ABCspeeds->setObjectName(QString::fromUtf8("action_ABCspeeds"));
        action_ABCspeeds->setIconVisibleInMenu(false);
        action_ELRsmoke = new QAction(MainWindow);
        action_ELRsmoke->setObjectName(QString::fromUtf8("action_ELRsmoke"));
        action_ReportToPDF = new QAction(MainWindow);
        action_ReportToPDF->setObjectName(QString::fromUtf8("action_ReportToPDF"));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/icons/gnome-mime-application-pdf.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_ReportToPDF->setIcon(icon15);
        action_ReportToPDF->setIconVisibleInMenu(false);
        action_UndoTable = new QAction(MainWindow);
        action_UndoTable->setObjectName(QString::fromUtf8("action_UndoTable"));
        action_UndoTable->setEnabled(false);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/icons/edit-undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_UndoTable->setIcon(icon16);
        action_UndoTable->setIconVisibleInMenu(false);
        action_RedoTable = new QAction(MainWindow);
        action_RedoTable->setObjectName(QString::fromUtf8("action_RedoTable"));
        action_RedoTable->setEnabled(false);
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/icons/edit-redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_RedoTable->setIcon(icon17);
        action_RedoTable->setIconVisibleInMenu(false);
        action_Preferences = new QAction(MainWindow);
        action_Preferences->setObjectName(QString::fromUtf8("action_Preferences"));
        action_Preferences->setIconVisibleInMenu(false);
        action_CheckoutData = new QAction(MainWindow);
        action_CheckoutData->setObjectName(QString::fromUtf8("action_CheckoutData"));
        action_CheckoutData->setIconVisibleInMenu(false);
        action_StandardsDescription = new QAction(MainWindow);
        action_StandardsDescription->setObjectName(QString::fromUtf8("action_StandardsDescription"));
        action_LoadCalculationOptions = new QAction(MainWindow);
        action_LoadCalculationOptions->setObjectName(QString::fromUtf8("action_LoadCalculationOptions"));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/icons/loadCalcSerrings.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_LoadCalculationOptions->setIcon(icon18);
        action_LoadCalculationOptions->setIconVisibleInMenu(false);
        action_SaveCalculationOptionsAs = new QAction(MainWindow);
        action_SaveCalculationOptionsAs->setObjectName(QString::fromUtf8("action_SaveCalculationOptionsAs"));
        action_LoadSourceData = new QAction(MainWindow);
        action_LoadSourceData->setObjectName(QString::fromUtf8("action_LoadSourceData"));
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/icons/changeSourceData.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_LoadSourceData->setIcon(icon19);
        action_LoadSourceData->setIconVisibleInMenu(false);
        action_SaveSourceDataAs = new QAction(MainWindow);
        action_SaveSourceDataAs->setObjectName(QString::fromUtf8("action_SaveSourceDataAs"));
        action_SaveSourceDataAs->setIconVisibleInMenu(false);
        action_Toolbar = new QAction(MainWindow);
        action_Toolbar->setObjectName(QString::fromUtf8("action_Toolbar"));
        action_Toolbar->setCheckable(true);
        action_Toolbar->setChecked(true);
        action_OpenSolution = new QAction(MainWindow);
        action_OpenSolution->setObjectName(QString::fromUtf8("action_OpenSolution"));
        action_CheckForUpdates = new QAction(MainWindow);
        action_CheckForUpdates->setObjectName(QString::fromUtf8("action_CheckForUpdates"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout_9 = new QVBoxLayout(centralWidget);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        comboBox_task = new QComboBox(centralWidget);
        comboBox_task->setObjectName(QString::fromUtf8("comboBox_task"));

        horizontalLayout_5->addWidget(comboBox_task);

        lineEdit_Vh = new QLineEdit(centralWidget);
        lineEdit_Vh->setObjectName(QString::fromUtf8("lineEdit_Vh"));
        lineEdit_Vh->setEnabled(false);
        lineEdit_Vh->setMinimumSize(QSize(50, 0));
        lineEdit_Vh->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_5->addWidget(lineEdit_Vh);

        comboBox_standard = new QComboBox(centralWidget);
        comboBox_standard->setObjectName(QString::fromUtf8("comboBox_standard"));

        horizontalLayout_5->addWidget(comboBox_standard);

        comboBox_FuelType = new QComboBox(centralWidget);
        comboBox_FuelType->setObjectName(QString::fromUtf8("comboBox_FuelType"));
        comboBox_FuelType->setEnabled(false);

        horizontalLayout_5->addWidget(comboBox_FuelType);

        comboBox_NOxSample = new QComboBox(centralWidget);
        comboBox_NOxSample->setObjectName(QString::fromUtf8("comboBox_NOxSample"));

        horizontalLayout_5->addWidget(comboBox_NOxSample);

        comboBox_PTcalc = new QComboBox(centralWidget);
        comboBox_PTcalc->setObjectName(QString::fromUtf8("comboBox_PTcalc"));

        horizontalLayout_5->addWidget(comboBox_PTcalc);

        lineEdit_PTmass = new QLineEdit(centralWidget);
        lineEdit_PTmass->setObjectName(QString::fromUtf8("lineEdit_PTmass"));
        lineEdit_PTmass->setEnabled(false);
        lineEdit_PTmass->setMinimumSize(QSize(50, 0));
        lineEdit_PTmass->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_5->addWidget(lineEdit_PTmass);

        pushButton_EnterPTmass = new QPushButton(centralWidget);
        pushButton_EnterPTmass->setObjectName(QString::fromUtf8("pushButton_EnterPTmass"));
        pushButton_EnterPTmass->setEnabled(false);
        pushButton_EnterPTmass->setMinimumSize(QSize(25, 0));
        pushButton_EnterPTmass->setMaximumSize(QSize(25, 16777215));

        horizontalLayout_5->addWidget(pushButton_EnterPTmass);

        comboBox_AddPointsCalc = new QComboBox(centralWidget);
        comboBox_AddPointsCalc->setObjectName(QString::fromUtf8("comboBox_AddPointsCalc"));

        horizontalLayout_5->addWidget(comboBox_AddPointsCalc);

        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_5->addWidget(line);

        checkBox_reports = new QCheckBox(centralWidget);
        checkBox_reports->setObjectName(QString::fromUtf8("checkBox_reports"));
        checkBox_reports->setChecked(true);

        horizontalLayout_5->addWidget(checkBox_reports);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout_9->addLayout(horizontalLayout_5);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        tabWidget_Data = new QTabWidget(centralWidget);
        tabWidget_Data->setObjectName(QString::fromUtf8("tabWidget_Data"));
        tab_SrcData = new QWidget();
        tab_SrcData->setObjectName(QString::fromUtf8("tab_SrcData"));
        verticalLayout_6 = new QVBoxLayout(tab_SrcData);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox = new QGroupBox(tab_SrcData);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setMinimumSize(QSize(0, 140));
        groupBox->setMaximumSize(QSize(16777215, 140));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        groupBox->setFont(font);
        verticalLayout_5 = new QVBoxLayout(groupBox);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tableWidget_SrcDataEU0 = new QTableWidget(groupBox);
        if (tableWidget_SrcDataEU0->columnCount() < 6)
            tableWidget_SrcDataEU0->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_SrcDataEU0->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_SrcDataEU0->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget_SrcDataEU0->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget_SrcDataEU0->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget_SrcDataEU0->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget_SrcDataEU0->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        tableWidget_SrcDataEU0->setObjectName(QString::fromUtf8("tableWidget_SrcDataEU0"));
        tableWidget_SrcDataEU0->setEnabled(false);
        tableWidget_SrcDataEU0->setMinimumSize(QSize(0, 0));
        tableWidget_SrcDataEU0->setMaximumSize(QSize(16777215, 16777215));
        tableWidget_SrcDataEU0->setContextMenuPolicy(Qt::DefaultContextMenu);
        tableWidget_SrcDataEU0->setSelectionMode(QAbstractItemView::ContiguousSelection);
        tableWidget_SrcDataEU0->horizontalHeader()->setDefaultSectionSize(100);
        tableWidget_SrcDataEU0->verticalHeader()->setVisible(false);

        verticalLayout_5->addWidget(tableWidget_SrcDataEU0);


        horizontalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(tab_SrcData);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setMinimumSize(QSize(0, 140));
        groupBox_2->setMaximumSize(QSize(16777215, 140));
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tableWidget_SrcDataEU3 = new QTableWidget(groupBox_2);
        if (tableWidget_SrcDataEU3->columnCount() < 11)
            tableWidget_SrcDataEU3->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(1, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(2, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(3, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(4, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(5, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(6, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(7, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(8, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(9, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableWidget_SrcDataEU3->setHorizontalHeaderItem(10, __qtablewidgetitem16);
        tableWidget_SrcDataEU3->setObjectName(QString::fromUtf8("tableWidget_SrcDataEU3"));
        tableWidget_SrcDataEU3->setEnabled(false);
        tableWidget_SrcDataEU3->setMaximumSize(QSize(16777215, 16777215));
        tableWidget_SrcDataEU3->setSelectionMode(QAbstractItemView::ContiguousSelection);
        tableWidget_SrcDataEU3->verticalHeader()->setVisible(false);

        verticalLayout_4->addWidget(tableWidget_SrcDataEU3);


        horizontalLayout->addWidget(groupBox_2);


        verticalLayout_2->addLayout(horizontalLayout);

        groupBox_3 = new QGroupBox(tab_SrcData);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy2);
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        tableWidget_SrcDataPoints = new QTableWidget(groupBox_3);
        if (tableWidget_SrcDataPoints->columnCount() < 28)
            tableWidget_SrcDataPoints->setColumnCount(28);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(0, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(1, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(2, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(3, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(4, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(5, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(6, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(7, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(8, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(9, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(10, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(11, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(12, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(13, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(14, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(15, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(16, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(17, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(18, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(19, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(20, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(21, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(22, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(23, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(24, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(25, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(26, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        tableWidget_SrcDataPoints->setHorizontalHeaderItem(27, __qtablewidgetitem44);
        tableWidget_SrcDataPoints->setObjectName(QString::fromUtf8("tableWidget_SrcDataPoints"));
        tableWidget_SrcDataPoints->setSelectionMode(QAbstractItemView::ContiguousSelection);
        tableWidget_SrcDataPoints->horizontalHeader()->setDefaultSectionSize(80);
        tableWidget_SrcDataPoints->verticalHeader()->setVisible(true);

        verticalLayout_3->addWidget(tableWidget_SrcDataPoints);


        verticalLayout_2->addWidget(groupBox_3);


        verticalLayout_6->addLayout(verticalLayout_2);

        tabWidget_Data->addTab(tab_SrcData, QString());
        tab_Reports = new QWidget();
        tab_Reports->setObjectName(QString::fromUtf8("tab_Reports"));
        verticalLayout = new QVBoxLayout(tab_Reports);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        comboBox_OpenedReports = new QComboBox(tab_Reports);
        comboBox_OpenedReports->setObjectName(QString::fromUtf8("comboBox_OpenedReports"));

        verticalLayout->addWidget(comboBox_OpenedReports);

        plainTextEdit_Report = new QPlainTextEdit(tab_Reports);
        plainTextEdit_Report->setObjectName(QString::fromUtf8("plainTextEdit_Report"));
        plainTextEdit_Report->setLineWrapMode(QPlainTextEdit::NoWrap);

        verticalLayout->addWidget(plainTextEdit_Report);

        tabWidget_Data->addTab(tab_Reports, QString());
        tab_RedPower = new QWidget();
        tab_RedPower->setObjectName(QString::fromUtf8("tab_RedPower"));
        tab_RedPower->setEnabled(false);
        verticalLayout_7 = new QVBoxLayout(tab_RedPower);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        splitter = new QSplitter(tab_RedPower);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        groupBox_4 = new QGroupBox(splitter);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_8 = new QVBoxLayout(groupBox_4);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        tableWidget_FullLoadCurve = new QTableWidget(groupBox_4);
        if (tableWidget_FullLoadCurve->columnCount() < 11)
            tableWidget_FullLoadCurve->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(0, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(1, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(2, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(3, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(4, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(5, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(6, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(7, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(8, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(9, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        tableWidget_FullLoadCurve->setHorizontalHeaderItem(10, __qtablewidgetitem55);
        tableWidget_FullLoadCurve->setObjectName(QString::fromUtf8("tableWidget_FullLoadCurve"));
        tableWidget_FullLoadCurve->setSelectionMode(QAbstractItemView::ContiguousSelection);
        tableWidget_FullLoadCurve->verticalHeader()->setVisible(false);

        verticalLayout_8->addWidget(tableWidget_FullLoadCurve);

        splitter->addWidget(groupBox_4);

        verticalLayout_7->addWidget(splitter);

        tabWidget_Data->addTab(tab_RedPower, QString());

        horizontalLayout_7->addWidget(tabWidget_Data);


        verticalLayout_9->addLayout(horizontalLayout_7);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuExport = new QMenu(menuFile);
        menuExport->setObjectName(QString::fromUtf8("menuExport"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuCalculation = new QMenu(menuBar);
        menuCalculation->setObjectName(QString::fromUtf8("menuCalculation"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        MainWindow->setMenuBar(menuBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        QWidget::setTabOrder(tabWidget_Data, tableWidget_SrcDataEU0);
        QWidget::setTabOrder(tableWidget_SrcDataEU0, tableWidget_SrcDataEU3);
        QWidget::setTabOrder(tableWidget_SrcDataEU3, tableWidget_SrcDataPoints);
        QWidget::setTabOrder(tableWidget_SrcDataPoints, comboBox_OpenedReports);
        QWidget::setTabOrder(comboBox_OpenedReports, plainTextEdit_Report);
        QWidget::setTabOrder(plainTextEdit_Report, tableWidget_FullLoadCurve);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuCalculation->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(action_LoadSourceData);
        menuFile->addAction(action_SaveSourceData);
        menuFile->addAction(action_SaveSourceDataAs);
        menuFile->addSeparator();
        menuFile->addAction(action_LoadCalculationOptions);
        menuFile->addAction(action_SaveCalculationOptionsAs);
        menuFile->addSeparator();
        menuFile->addAction(action_OpenReport);
        menuFile->addAction(action_SaveReportAs);
        menuFile->addAction(action_CloseReport);
        menuFile->addSeparator();
        menuFile->addAction(menuExport->menuAction());
        menuFile->addAction(action_PrintReport);
        menuFile->addSeparator();
        menuFile->addAction(action_Preferences);
        menuFile->addSeparator();
        menuFile->addAction(action_Quit);
        menuExport->addAction(action_ReportToPDF);
        menuHelp->addAction(action_StandardsDescription);
        menuHelp->addSeparator();
        menuHelp->addAction(action_AboutQr49);
        menuHelp->addAction(action_AboutQt);
        menuHelp->addSeparator();
        menuHelp->addAction(action_CheckForUpdates);
        menuCalculation->addAction(action_Execute);
        menuCalculation->addSeparator();
        menuCalculation->addAction(action_ABCspeeds);
        menuCalculation->addAction(action_ELRsmoke);
        menuCalculation->addSeparator();
        menuCalculation->addAction(action_CheckoutData);
        menuEdit->addAction(action_UndoTable);
        menuEdit->addAction(action_RedoTable);
        menuEdit->addSeparator();
        menuEdit->addAction(action_CutFromTable);
        menuEdit->addAction(action_CopyFromTable);
        menuEdit->addAction(action_PasteToTable);
        menuEdit->addAction(action_DeleteFromTable);
        menuEdit->addSeparator();
        menuEdit->addAction(action_Add);
        menuEdit->addAction(action_Multiply);
        menuEdit->addAction(action_Divide);
        menuEdit->addAction(action_Equal);
        menuEdit->addSeparator();
        menuEdit->addAction(action_AddRow);
        menuEdit->addAction(action_DeleteRow);
        menuView->addAction(action_Toolbar);
        toolBar->addAction(action_LoadSourceData);
        toolBar->addAction(action_LoadCalculationOptions);
        toolBar->addSeparator();
        toolBar->addAction(action_OpenReport);
        toolBar->addAction(action_SaveReportAs);
        toolBar->addAction(action_ReportToPDF);
        toolBar->addAction(action_PrintReport);
        toolBar->addSeparator();
        toolBar->addAction(action_UndoTable);
        toolBar->addAction(action_RedoTable);
        toolBar->addSeparator();
        toolBar->addAction(action_CutFromTable);
        toolBar->addAction(action_CopyFromTable);
        toolBar->addAction(action_PasteToTable);
        toolBar->addAction(action_DeleteFromTable);
        toolBar->addSeparator();
        toolBar->addAction(action_Add);
        toolBar->addAction(action_Multiply);
        toolBar->addAction(action_Divide);
        toolBar->addAction(action_Equal);
        toolBar->addSeparator();
        toolBar->addAction(action_Execute);

        retranslateUi(MainWindow);
        QObject::connect(action_Quit, SIGNAL(activated()), MainWindow, SLOT(close()));

        comboBox_task->setCurrentIndex(1);
        comboBox_standard->setCurrentIndex(2);
        comboBox_FuelType->setCurrentIndex(0);
        comboBox_NOxSample->setCurrentIndex(0);
        comboBox_PTcalc->setCurrentIndex(0);
        comboBox_AddPointsCalc->setCurrentIndex(1);
        tabWidget_Data->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Qr49", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        action_PrintReport->setText(QApplication::translate("MainWindow", "Print Report...", 0, QApplication::UnicodeUTF8));
        action_PrintReport->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        action_Execute->setText(QApplication::translate("MainWindow", "Execute", 0, QApplication::UnicodeUTF8));
        action_Execute->setShortcut(QApplication::translate("MainWindow", "F9", 0, QApplication::UnicodeUTF8));
        action_AboutQt->setText(QApplication::translate("MainWindow", "About Qt", 0, QApplication::UnicodeUTF8));
        action_AboutQr49->setText(QApplication::translate("MainWindow", "About Qr49", 0, QApplication::UnicodeUTF8));
        action_AddRow->setText(QApplication::translate("MainWindow", "Add Row", 0, QApplication::UnicodeUTF8));
        action_AddRow->setShortcut(QApplication::translate("MainWindow", "F11", 0, QApplication::UnicodeUTF8));
        action_DeleteRow->setText(QApplication::translate("MainWindow", "Delete Row", 0, QApplication::UnicodeUTF8));
        action_DeleteRow->setShortcut(QApplication::translate("MainWindow", "F12", 0, QApplication::UnicodeUTF8));
        action_SaveReportAs->setText(QApplication::translate("MainWindow", "Save Report As...", 0, QApplication::UnicodeUTF8));
        action_SaveReportAs->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        action_CopyFromTable->setText(QApplication::translate("MainWindow", "Copy", 0, QApplication::UnicodeUTF8));
        action_CopyFromTable->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        action_PasteToTable->setText(QApplication::translate("MainWindow", "Paste", 0, QApplication::UnicodeUTF8));
        action_PasteToTable->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0, QApplication::UnicodeUTF8));
        action_DeleteFromTable->setText(QApplication::translate("MainWindow", "Delete", 0, QApplication::UnicodeUTF8));
        action_DeleteFromTable->setShortcut(QApplication::translate("MainWindow", "Del", 0, QApplication::UnicodeUTF8));
        action_CutFromTable->setText(QApplication::translate("MainWindow", "Cut", 0, QApplication::UnicodeUTF8));
        action_CutFromTable->setShortcut(QApplication::translate("MainWindow", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        action_Multiply->setText(QApplication::translate("MainWindow", "Multiply...", 0, QApplication::UnicodeUTF8));
        action_Multiply->setShortcut(QApplication::translate("MainWindow", "*", 0, QApplication::UnicodeUTF8));
        action_Divide->setText(QApplication::translate("MainWindow", "Divide...", 0, QApplication::UnicodeUTF8));
        action_Divide->setShortcut(QApplication::translate("MainWindow", "/", 0, QApplication::UnicodeUTF8));
        action_Add->setText(QApplication::translate("MainWindow", "Add...", 0, QApplication::UnicodeUTF8));
        action_Add->setShortcut(QApplication::translate("MainWindow", "+", 0, QApplication::UnicodeUTF8));
        action_Equal->setText(QApplication::translate("MainWindow", "Equal...", 0, QApplication::UnicodeUTF8));
        action_Equal->setShortcut(QApplication::translate("MainWindow", "=", 0, QApplication::UnicodeUTF8));
        action_SaveSourceData->setText(QApplication::translate("MainWindow", "Save Source Data", 0, QApplication::UnicodeUTF8));
        action_SaveSourceData->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0, QApplication::UnicodeUTF8));
        action_OpenReport->setText(QApplication::translate("MainWindow", "Open Report...", 0, QApplication::UnicodeUTF8));
        action_OpenReport->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        action_CloseReport->setText(QApplication::translate("MainWindow", "Close Report", 0, QApplication::UnicodeUTF8));
        action_CloseReport->setShortcut(QApplication::translate("MainWindow", "Ctrl+U", 0, QApplication::UnicodeUTF8));
        action_ABCspeeds->setText(QApplication::translate("MainWindow", "ABC speeds", 0, QApplication::UnicodeUTF8));
        action_ELRsmoke->setText(QApplication::translate("MainWindow", "ELR smoke", 0, QApplication::UnicodeUTF8));
        action_ReportToPDF->setText(QApplication::translate("MainWindow", "Report To PDF...", 0, QApplication::UnicodeUTF8));
        action_UndoTable->setText(QApplication::translate("MainWindow", "Undo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_UndoTable->setToolTip(QApplication::translate("MainWindow", "Undo (0)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_UndoTable->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
        action_RedoTable->setText(QApplication::translate("MainWindow", "Redo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_RedoTable->setToolTip(QApplication::translate("MainWindow", "Redo (0)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_RedoTable->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+Z", 0, QApplication::UnicodeUTF8));
        action_Preferences->setText(QApplication::translate("MainWindow", "Preferences...", 0, QApplication::UnicodeUTF8));
        action_Preferences->setShortcut(QApplication::translate("MainWindow", "F2", 0, QApplication::UnicodeUTF8));
        action_CheckoutData->setText(QApplication::translate("MainWindow", "Checkout data", 0, QApplication::UnicodeUTF8));
        action_StandardsDescription->setText(QApplication::translate("MainWindow", "Standards Description", 0, QApplication::UnicodeUTF8));
        action_StandardsDescription->setShortcut(QApplication::translate("MainWindow", "F1", 0, QApplication::UnicodeUTF8));
        action_LoadCalculationOptions->setText(QApplication::translate("MainWindow", "Load Calculation Options...", 0, QApplication::UnicodeUTF8));
        action_LoadCalculationOptions->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", 0, QApplication::UnicodeUTF8));
        action_SaveCalculationOptionsAs->setText(QApplication::translate("MainWindow", "Save Calculation Options As...", 0, QApplication::UnicodeUTF8));
        action_LoadSourceData->setText(QApplication::translate("MainWindow", "Load Source Data...", 0, QApplication::UnicodeUTF8));
        action_LoadSourceData->setShortcut(QApplication::translate("MainWindow", "Ctrl+E", 0, QApplication::UnicodeUTF8));
        action_SaveSourceDataAs->setText(QApplication::translate("MainWindow", "Save Source Data As...", 0, QApplication::UnicodeUTF8));
        action_SaveSourceDataAs->setShortcut(QApplication::translate("MainWindow", "Ctrl+B", 0, QApplication::UnicodeUTF8));
        action_Toolbar->setText(QApplication::translate("MainWindow", "Toolbar", 0, QApplication::UnicodeUTF8));
        action_OpenSolution->setText(QApplication::translate("MainWindow", "Open Solution...", 0, QApplication::UnicodeUTF8));
        action_CheckForUpdates->setText(QApplication::translate("MainWindow", "Check For Updates...", 0, QApplication::UnicodeUTF8));
        comboBox_task->clear();
        comboBox_task->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "points", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "emissions", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "ReducedPower", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_task->setToolTip(QApplication::translate("MainWindow", "Calculation task:\n"
"\n"
"points - Cycle Points\n"
"emissions - Cycle Specific Emissions\n"
"ReducedPower - Reduced Power", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        lineEdit_Vh->setToolTip(QApplication::translate("MainWindow", "Capacity, litres", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        lineEdit_Vh->setInputMask(QString());
        lineEdit_Vh->setText(QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8));
        comboBox_standard->clear();
        comboBox_standard->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "EU6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "EU5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "EU4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "EU3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "EU2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "EU1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "EU0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "OST", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "GOST", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96E8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96F8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96G8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96D8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96E5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96F5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96G5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96D5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96H8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96I8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96J8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96K8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96H5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96I5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96J5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "r96K5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "C1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "D1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "D2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "E1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "E2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "E3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "E5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "F", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "G1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "G2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "FreeCalc", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_standard->setToolTip(QApplication::translate("MainWindow", "Standard", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        comboBox_FuelType->clear();
        comboBox_FuelType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "diesel", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "motor", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "mazut", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_FuelType->setToolTip(QApplication::translate("MainWindow", "Fuel type:\n"
"\n"
"diesel\n"
"motor\n"
"mazut", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        comboBox_NOxSample->clear();
        comboBox_NOxSample->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "wet", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "dry", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_NOxSample->setToolTip(QApplication::translate("MainWindow", "NOx sample type:\n"
"\n"
"wet - Wet Sample\n"
"dry - Dry Sample", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        comboBox_PTcalc->clear();
        comboBox_PTcalc->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "ThroughSmoke", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "ThroughPTmass", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "no", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_PTcalc->setToolTip(QApplication::translate("MainWindow", "gPT calculation method:\n"
"\n"
"ThroughSmoke - Through Smoke\n"
"ThroughPTmass - Through PT mass\n"
"no - Don't Calculate PT", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        lineEdit_PTmass->setToolTip(QApplication::translate("MainWindow", "PT mass, mg", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        lineEdit_PTmass->setText(QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_EnterPTmass->setToolTip(QApplication::translate("MainWindow", "PT filter masses", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_EnterPTmass->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        comboBox_AddPointsCalc->clear();
        comboBox_AddPointsCalc->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "yes", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "no", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_AddPointsCalc->setToolTip(QApplication::translate("MainWindow", "Additional points calculation:\n"
"\n"
"yes\n"
"no", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        checkBox_reports->setText(QApplication::translate("MainWindow", "Create reports", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "OST..EU2, r96**, GOST 30574 Cycle Points Calculation", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_SrcDataEU0->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "idle\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_SrcDataEU0->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "n_interim\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget_SrcDataEU0->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "n_rated\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget_SrcDataEU0->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "N_fan_rated\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget_SrcDataEU0->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "Ne_interim\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget_SrcDataEU0->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "Ne_rated\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "EU3..EU6 Cycle Points Calculation", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget_SrcDataEU3->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "n_hi\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidget_SrcDataEU3->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "n_lo\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidget_SrcDataEU3->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QApplication::translate("MainWindow", "idle\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidget_SrcDataEU3->horizontalHeaderItem(3);
        ___qtablewidgetitem9->setText(QApplication::translate("MainWindow", "n_rated\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidget_SrcDataEU3->horizontalHeaderItem(4);
        ___qtablewidgetitem10->setText(QApplication::translate("MainWindow", "N_fan_rated\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidget_SrcDataEU3->horizontalHeaderItem(5);
        ___qtablewidgetitem11->setText(QApplication::translate("MainWindow", "Ne_A\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidget_SrcDataEU3->horizontalHeaderItem(6);
        ___qtablewidgetitem12->setText(QApplication::translate("MainWindow", "Ne_B\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidget_SrcDataEU3->horizontalHeaderItem(7);
        ___qtablewidgetitem13->setText(QApplication::translate("MainWindow", "Ne_C\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tableWidget_SrcDataEU3->horizontalHeaderItem(8);
        ___qtablewidgetitem14->setText(QApplication::translate("MainWindow", "Ne_a1\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = tableWidget_SrcDataEU3->horizontalHeaderItem(9);
        ___qtablewidgetitem15->setText(QApplication::translate("MainWindow", "Ne_a2\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = tableWidget_SrcDataEU3->horizontalHeaderItem(10);
        ___qtablewidgetitem16->setText(QApplication::translate("MainWindow", "Ne_a3\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Specific Emissions Calculation", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = tableWidget_SrcDataPoints->horizontalHeaderItem(0);
        ___qtablewidgetitem17->setText(QApplication::translate("MainWindow", "Point\n"
"[-]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = tableWidget_SrcDataPoints->horizontalHeaderItem(1);
        ___qtablewidgetitem18->setText(QApplication::translate("MainWindow", "n\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = tableWidget_SrcDataPoints->horizontalHeaderItem(2);
        ___qtablewidgetitem19->setText(QApplication::translate("MainWindow", "Me_b\n"
"[Nm]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = tableWidget_SrcDataPoints->horizontalHeaderItem(3);
        ___qtablewidgetitem20->setText(QApplication::translate("MainWindow", "Ne_b\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem21 = tableWidget_SrcDataPoints->horizontalHeaderItem(4);
        ___qtablewidgetitem21->setText(QApplication::translate("MainWindow", "N_fan\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem22 = tableWidget_SrcDataPoints->horizontalHeaderItem(5);
        ___qtablewidgetitem22->setText(QApplication::translate("MainWindow", "w\n"
"[-]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem23 = tableWidget_SrcDataPoints->horizontalHeaderItem(6);
        ___qtablewidgetitem23->setText(QApplication::translate("MainWindow", "t0\n"
"[oC]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem24 = tableWidget_SrcDataPoints->horizontalHeaderItem(7);
        ___qtablewidgetitem24->setText(QApplication::translate("MainWindow", "B0\n"
"[kPa]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem25 = tableWidget_SrcDataPoints->horizontalHeaderItem(8);
        ___qtablewidgetitem25->setText(QApplication::translate("MainWindow", "Ra\n"
"[%]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem26 = tableWidget_SrcDataPoints->horizontalHeaderItem(9);
        ___qtablewidgetitem26->setText(QApplication::translate("MainWindow", "dPn\n"
"[mmH2O]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem27 = tableWidget_SrcDataPoints->horizontalHeaderItem(10);
        ___qtablewidgetitem27->setText(QApplication::translate("MainWindow", "Gair\n"
"[kg/h]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem28 = tableWidget_SrcDataPoints->horizontalHeaderItem(11);
        ___qtablewidgetitem28->setText(QApplication::translate("MainWindow", "Gfuel\n"
"[kg/h]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem29 = tableWidget_SrcDataPoints->horizontalHeaderItem(12);
        ___qtablewidgetitem29->setText(QApplication::translate("MainWindow", "CNOx\n"
"[ppm]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem30 = tableWidget_SrcDataPoints->horizontalHeaderItem(13);
        ___qtablewidgetitem30->setText(QApplication::translate("MainWindow", "gNOx\n"
"[g/kWh]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem31 = tableWidget_SrcDataPoints->horizontalHeaderItem(14);
        ___qtablewidgetitem31->setText(QApplication::translate("MainWindow", "CCO\n"
"[ppm]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem32 = tableWidget_SrcDataPoints->horizontalHeaderItem(15);
        ___qtablewidgetitem32->setText(QApplication::translate("MainWindow", "CCH\n"
"[ppm]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem33 = tableWidget_SrcDataPoints->horizontalHeaderItem(16);
        ___qtablewidgetitem33->setText(QApplication::translate("MainWindow", "CCO2in\n"
"[%]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem34 = tableWidget_SrcDataPoints->horizontalHeaderItem(17);
        ___qtablewidgetitem34->setText(QApplication::translate("MainWindow", "CCO2out\n"
"[%]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem35 = tableWidget_SrcDataPoints->horizontalHeaderItem(18);
        ___qtablewidgetitem35->setText(QApplication::translate("MainWindow", "CO2\n"
"[%]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem36 = tableWidget_SrcDataPoints->horizontalHeaderItem(19);
        ___qtablewidgetitem36->setText(QApplication::translate("MainWindow", "Ka\n"
"[m-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem37 = tableWidget_SrcDataPoints->horizontalHeaderItem(20);
        ___qtablewidgetitem37->setText(QApplication::translate("MainWindow", "Ka\n"
"[%]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem38 = tableWidget_SrcDataPoints->horizontalHeaderItem(21);
        ___qtablewidgetitem38->setText(QApplication::translate("MainWindow", "FSN\n"
"[-]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem39 = tableWidget_SrcDataPoints->horizontalHeaderItem(22);
        ___qtablewidgetitem39->setText(QApplication::translate("MainWindow", "Pr\n"
"[kPa]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem40 = tableWidget_SrcDataPoints->horizontalHeaderItem(23);
        ___qtablewidgetitem40->setText(QApplication::translate("MainWindow", "ts\n"
"[oC]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem41 = tableWidget_SrcDataPoints->horizontalHeaderItem(24);
        ___qtablewidgetitem41->setText(QApplication::translate("MainWindow", "tauf\n"
"[s]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem42 = tableWidget_SrcDataPoints->horizontalHeaderItem(25);
        ___qtablewidgetitem42->setText(QApplication::translate("MainWindow", "qmdw\n"
"[g/s]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem43 = tableWidget_SrcDataPoints->horizontalHeaderItem(26);
        ___qtablewidgetitem43->setText(QApplication::translate("MainWindow", "qmdew\n"
"[g/s]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem44 = tableWidget_SrcDataPoints->horizontalHeaderItem(27);
        ___qtablewidgetitem44->setText(QApplication::translate("MainWindow", "rd\n"
"[-]", 0, QApplication::UnicodeUTF8));
        tabWidget_Data->setTabText(tabWidget_Data->indexOf(tab_SrcData), QApplication::translate("MainWindow", "Source Data", 0, QApplication::UnicodeUTF8));
        tabWidget_Data->setTabText(tabWidget_Data->indexOf(tab_Reports), QApplication::translate("MainWindow", "Reports", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Full Load Curve", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem45 = tableWidget_FullLoadCurve->horizontalHeaderItem(0);
        ___qtablewidgetitem45->setText(QApplication::translate("MainWindow", "Point\n"
"[-]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem46 = tableWidget_FullLoadCurve->horizontalHeaderItem(1);
        ___qtablewidgetitem46->setText(QApplication::translate("MainWindow", "n\n"
"[min-1]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem47 = tableWidget_FullLoadCurve->horizontalHeaderItem(2);
        ___qtablewidgetitem47->setText(QApplication::translate("MainWindow", "Me_b\n"
"[Nm]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem48 = tableWidget_FullLoadCurve->horizontalHeaderItem(3);
        ___qtablewidgetitem48->setText(QApplication::translate("MainWindow", "t0\n"
"[oC]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem49 = tableWidget_FullLoadCurve->horizontalHeaderItem(4);
        ___qtablewidgetitem49->setText(QApplication::translate("MainWindow", "B0\n"
"[kPa]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem50 = tableWidget_FullLoadCurve->horizontalHeaderItem(5);
        ___qtablewidgetitem50->setText(QApplication::translate("MainWindow", "Ra\n"
"[%]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem51 = tableWidget_FullLoadCurve->horizontalHeaderItem(6);
        ___qtablewidgetitem51->setText(QApplication::translate("MainWindow", "S\n"
"[kPa]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem52 = tableWidget_FullLoadCurve->horizontalHeaderItem(7);
        ___qtablewidgetitem52->setText(QApplication::translate("MainWindow", "pk\n"
"[kPa]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem53 = tableWidget_FullLoadCurve->horizontalHeaderItem(8);
        ___qtablewidgetitem53->setText(QApplication::translate("MainWindow", "Gfuel\n"
"[kg/h]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem54 = tableWidget_FullLoadCurve->horizontalHeaderItem(9);
        ___qtablewidgetitem54->setText(QApplication::translate("MainWindow", "N_k\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem55 = tableWidget_FullLoadCurve->horizontalHeaderItem(10);
        ___qtablewidgetitem55->setText(QApplication::translate("MainWindow", "N_fan\n"
"[kW]", 0, QApplication::UnicodeUTF8));
        tabWidget_Data->setTabText(tabWidget_Data->indexOf(tab_RedPower), QApplication::translate("MainWindow", "Reduced Power", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuExport->setTitle(QApplication::translate("MainWindow", "Export", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        menuCalculation->setTitle(QApplication::translate("MainWindow", "Calculation", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QR49_H
