/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: qr49.cpp

    Copyright (C) 2009-2017 Artem Petrov <pa2311@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "qr49.h"
#include "ui_qr49.h"
#include "filtermassdialog.h"
#include "valuedialog.h"
#include "preferencesdialog.h"
#include "checkoutdatadialog.h"
#include "undoredotable.h"
#include "dataimportdialog.h"
#include "tablewidgetfunctions.h"
#include "constants.h"
#include "reportsettingsdialog.h"

#include "txCalculationOptions.h"
#include "txConstants.h"
#include "txIdentification.h"
#include "txDataReader.h"
#include "txPointsOfCycle.h"
#include "txEmissionsOnCycle.h"
#include "txReducedPower.h"
#include "txCommonParameters.h"
#include "txAuxiliaryFunctions.h"
#include "txError.h"

#include <QSharedPointer>
#include <QVector>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QTextDocument>
#include <QPrintDialog>
#include <QPrinter>
#include <QFileDialog>
#include <QTextStream>
#include <QClipboard>
#include <QDoubleValidator>
#include <QRegExp>
#include <QObject>
#include <QEvent>
#include <QSettings>
#include <QFontDatabase>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QTextStream>
#include <QUrl>
#include <QDesktopServices>
#include <QDateTime>
#include <QTranslator>
#include <QDirIterator>

#include <regex>

MainWindow::MainWindow(QWidget *parent) :

    QMainWindow(parent),
    ui(new Ui::MainWindow),

    m_qr49settings("pa23software", QR49NAME),

    m_contextMenu(new QMenu()),

    m_filterMassDialog(new FilterMassDialog(this)),
    m_valueDialog(new ValueDialog(this)),
    m_preferencesDialog(new PreferencesDialog(this)),
    m_checkoutDataDialog(new CheckoutDataDialog(0)),
    m_dataImportDialog(new DataImportDialog(this)),
    m_reportsetdialog(new ReportSettingsDialog(this)),

    m_regExp("[-+]?[0-9]*[.,]?[0-9]+([eE][-+]?[0-9]+)?"),

    m_undoCount(0),
    m_redoCount(0) {

    ui->setupUi(this);

    //

    this->setWindowTitle(
                QR49NAME
                + " v"
                + QR49VERSION
                + " (uses "
                + toxic::toxicIdentification{}.name()
                + " v"
                + toxic::toxicIdentification{}.version()
                + ")"
                );

    m_contextMenu->addMenu(ui->menuFile);
    m_contextMenu->addMenu(ui->menuEdit);
    m_contextMenu->addMenu(ui->menuCalculation);

    //

    connect(ui->comboBox_task,
            SIGNAL(activated(int)),
            this,
            SLOT(taskChanged(const int)));

    connect(ui->comboBox_standard,
            SIGNAL(activated(int)),
            this,
            SLOT(standardChanged(const int)));

    connect(ui->comboBox_PTcalc,
            SIGNAL(activated(int)),
            this,
            SLOT(PTcalcChanged(const int)));

    connect(ui->comboBox_OpenedReports,
            SIGNAL(activated(QString)),
            this,
            SLOT(reportChanged(const QString &)));

    connect(ui->plainTextEdit_Report,
            SIGNAL(undoAvailable(bool)),
            this,
            SLOT(reportTextChanged(bool)));

    connect(ui->tabWidget_Data,
            SIGNAL(currentChanged(int)),
            this,
            SLOT(tabChanged(const int)));

    tableCellChangedConnect(true);

    //

    ui->tableWidget_SrcDataEU0->installEventFilter(this);
    ui->tableWidget_SrcDataEU3->installEventFilter(this);
    ui->tableWidget_SrcDataPoints->installEventFilter(this);
    ui->tableWidget_FullLoadCurve->installEventFilter(this);

    m_table = ui->tableWidget_SrcDataPoints;

    //

    m_calculationOptions = QSharedPointer<toxic::txCalculationOptions>
            (new toxic::txCalculationOptions());
    m_commonParameters = QSharedPointer<toxic::txCommonParameters>
            (new toxic::txCommonParameters());

    readPreferences();

    //

    m_regExpValidator = new QRegExpValidator(m_regExp, 0);

    //

    loadAllSourceData();

    //

    m_undoRedo_TableEU0 =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_SrcDataEU0));
    m_undoRedo_TableEU3 =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_SrcDataEU3));
    m_undoRedo_TablePoints =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_SrcDataPoints));
    m_undoRedo_TableFullLoadCurve =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_FullLoadCurve));

    saveStateForAllTables();

    //

    readProgramSettings();

    PTcalcChanged(ui->comboBox_PTcalc->currentIndex());
    standardChanged(ui->comboBox_standard->currentIndex());
    taskChanged(ui->comboBox_task->currentIndex());

    //

    setFonts();

    //

    connect(ui->doubleSpinBox_nhi,
            SIGNAL(editingFinished()),
            this,
            SLOT(abcCalculation()));

    connect(ui->doubleSpinBox_nlo,
            SIGNAL(editingFinished()),
            this,
            SLOT(abcCalculation()));

    connect(ui->doubleSpinBox_Dn,
            SIGNAL(editingFinished()),
            this,
            SLOT(gairCalculation()));

    connect(ui->doubleSpinBox_B0,
            SIGNAL(editingFinished()),
            this,
            SLOT(gairCalculation()));

    connect(ui->doubleSpinBox_t0,
            SIGNAL(editingFinished()),
            this,
            SLOT(gairCalculation()));

    connect(ui->doubleSpinBox_dPn,
            SIGNAL(editingFinished()),
            this,
            SLOT(gairCalculation()));

    connect(ui->doubleSpinBox_nFanRated,
            SIGNAL(editingFinished()),
            this,
            SLOT(nfanCalculation()));

    connect(ui->doubleSpinBox_nRated,
            SIGNAL(editingFinished()),
            this,
            SLOT(nfanCalculation()));

    connect(ui->doubleSpinBox_n,
            SIGNAL(editingFinished()),
            this,
            SLOT(nfanCalculation()));

    appPath = QDir::currentPath() + "/";
}

MainWindow::~MainWindow() {

    m_table = ui->tableWidget_SrcDataEU0;
    on_action_SaveSourceData_triggered();

    m_table = ui->tableWidget_SrcDataEU3;
    on_action_SaveSourceData_triggered();

    m_table = ui->tableWidget_SrcDataPoints;
    on_action_SaveSourceData_triggered();

    m_table = ui->tableWidget_FullLoadCurve;
    on_action_SaveSourceData_triggered();

    writeProgramSettings();

    delete ui;
    delete m_contextMenu;
    delete m_filterMassDialog;
    delete m_valueDialog;
    delete m_preferencesDialog;
    delete m_checkoutDataDialog;
    delete m_dataImportDialog;
    delete m_reportsetdialog;
    delete m_regExpValidator;
}

void MainWindow::closeEvent(QCloseEvent *event) {

    if ( m_savingReportNeeded ) {

        if ( QMessageBox::question(this,
                                   QR49NAME
                                   , tr("Report was changed. Save it?"),
                                   QMessageBox::Yes,
                                   QMessageBox::No) == QMessageBox::Yes ) {

            saveReport(m_changedReportFileName);
        }
    }

    event->accept();
}

void MainWindow::setFonts() {

    QFile excalibFont(":/fonts/fonts/excalib.ttf");
    excalibFont.open(QIODevice::ReadOnly);

    const ptrdiff_t fontid1 =
            QFontDatabase::addApplicationFontFromData(excalibFont.readAll());

    excalibFont.close();

    if ( fontid1 == -1 ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Can not load monospaced font excalib.ttf "
                       "from program resources!")
                    );
    }
    else {

        m_monospacedFont_8.setFamily(
                    QFontDatabase::applicationFontFamilies(fontid1).first()
                    );

        m_monospacedFont_8.setPointSize(8);

        m_monospacedFont_10.setFamily(
                    QFontDatabase::applicationFontFamilies(fontid1).first()
                    );

        m_monospacedFont_10.setPointSize(10);

        ui->plainTextEdit_Report->setFont(m_monospacedFont_10);

        QPlainTextEdit *myPlainTextEdit_CheckoutData =
                m_checkoutDataDialog->findChild<QPlainTextEdit *>
                ("plainTextEdit_CheckoutData");

        if ( !myPlainTextEdit_CheckoutData ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        QString::fromLatin1(Q_FUNC_INFO)
                        + ":::"
                        + tr("Child object not found!")
                        );

            return;
        }

        myPlainTextEdit_CheckoutData->setFont(m_monospacedFont_10);
    }

    QFile dejavuFont(":/fonts/fonts/DejaVuSansMono.ttf");
    dejavuFont.open(QIODevice::ReadOnly);

    const ptrdiff_t fontid2 =
            QFontDatabase::addApplicationFontFromData(dejavuFont.readAll());

    dejavuFont.close();

    if ( fontid2 == -1 ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Can not load monospaced font DejaVuSansMono.ttf "
                       "from program resources!")
                    );
    }
    else {

        m_dejavusansmonoFont_10.setFamily(
                    QFontDatabase::applicationFontFamilies(fontid2).first()
                    );

        m_dejavusansmonoFont_10.setPointSize(10);
    }
}

void MainWindow::writeProgramSettings() {

    m_qr49settings.beginGroup("/Settings");
    m_qr49settings.setValue("/window_geometry", geometry());
    m_qr49settings.setValue("/toolbars_state", QMainWindow::saveState());
    m_qr49settings.setValue("/action_toolbar_checked", ui->action_Toolbar->isChecked());
    m_qr49settings.setValue("/active_tab", ui->tabWidget_Data->currentIndex());
    m_qr49settings.setValue("/task_index", ui->comboBox_task->currentIndex());
    m_qr49settings.setValue("/Vh_value", ui->doubleSpinBox_Vh->value());
    m_qr49settings.setValue("/standard_index", ui->comboBox_standard->currentIndex());
    m_qr49settings.setValue("/chargingType_index", ui->comboBox_chargingType->currentIndex());
    m_qr49settings.setValue("/fuelType_index", ui->comboBox_FuelType->currentIndex());
    m_qr49settings.setValue("/NOxSample_index", ui->comboBox_NOxSample->currentIndex());
    m_qr49settings.setValue("/PTcalc_index", ui->comboBox_PTcalc->currentIndex());
    m_qr49settings.setValue("/PTmass_value", ui->doubleSpinBox_PTmass->value());
    m_qr49settings.setValue("/addPointsCalc_index", ui->comboBox_AddPointsCalc->currentIndex());
    m_qr49settings.setValue("/createReports", ui->checkBox_reports->isChecked());
    m_qr49settings.setValue("/lastReportsDir", m_lastReportsDir.absolutePath());
    m_qr49settings.setValue("/lastCheckoutDataFileName", m_lastCheckoutDataFileName);
    m_qr49settings.setValue("/lastReportFileName", m_lastReportFileName);
    m_qr49settings.setValue("/action_english_checked", ui->action_English->isChecked());
    m_qr49settings.setValue("/action_russian_checked", ui->action_Russian->isChecked());
    m_qr49settings.endGroup();
}

void MainWindow::readProgramSettings() {

    m_qr49settings.beginGroup("/Settings");

    setGeometry(m_qr49settings.value("/window_geometry", QRect(20, 40, 0, 0)).toRect());
    restoreState(m_qr49settings.value("/toolbars_state").toByteArray());
    ui->action_Toolbar->setChecked(m_qr49settings.value("/action_toolbar_checked", true).toBool());
    ui->tabWidget_Data->setCurrentIndex(m_qr49settings.value("active_tab", ui->tabWidget_Data->currentIndex()).toInt());
    ui->comboBox_task->setCurrentIndex(m_qr49settings.value("/task_index", ui->comboBox_task->currentIndex()).toInt());
    ui->doubleSpinBox_Vh->setValue(m_qr49settings.value("/Vh_value", 0).toDouble());
    ui->comboBox_standard->setCurrentIndex(m_qr49settings.value("/standard_index", ui->comboBox_standard->currentIndex()).toInt());
    ui->comboBox_chargingType->setCurrentIndex(m_qr49settings.value("/chargingType_index", ui->comboBox_chargingType->currentIndex()).toInt());
    ui->comboBox_FuelType->setCurrentIndex(m_qr49settings.value("/fuelType_index", ui->comboBox_FuelType->currentIndex()).toInt());
    ui->comboBox_NOxSample->setCurrentIndex(m_qr49settings.value("/NOxSample_index", ui->comboBox_NOxSample->currentIndex()).toInt());
    ui->comboBox_PTcalc->setCurrentIndex(m_qr49settings.value("/PTcalc_index", ui->comboBox_PTcalc->currentIndex()).toInt());
    ui->doubleSpinBox_PTmass->setValue(m_qr49settings.value("/PTmass_value", 0).toDouble());
    ui->comboBox_AddPointsCalc->setCurrentIndex(m_qr49settings.value("/addPointsCalc_index", ui->comboBox_AddPointsCalc->currentIndex()).toInt());
    ui->checkBox_reports->setChecked(m_qr49settings.value("/createReports", ui->checkBox_reports->isChecked()).toBool());
    m_lastReportsDir.setPath(m_qr49settings.value("/lastReportsDir", "").toString());
    m_lastCheckoutDataFileName = m_qr49settings.value("/lastCheckoutDataFileName", "").toString();
    m_lastReportFileName = m_qr49settings.value("/lastReportFileName", "").toString();
    ui->action_English->setChecked(m_qr49settings.value("/action_english_checked", true).toBool());
    ui->action_Russian->setChecked(m_qr49settings.value("/action_russian_checked", false).toBool());

    m_qr49settings.endGroup();

    if ( ui->action_Toolbar->isChecked() ) {
        ui->toolBar->setVisible(true);
    }
    else {
        ui->toolBar->setVisible(false);
    }

    if ( launching ) {

        launching = false;

        if ( ui->action_English->isChecked() ) {
            on_action_English_triggered();
        }
        else {
            on_action_Russian_triggered();
        }
    }

    ui->dateEdit->setDate(QDate::currentDate());
}

void MainWindow::tableCellChangedConnect(const bool b) {

    if ( b ) {

        connect(ui->tableWidget_SrcDataEU0,
                SIGNAL(cellChanged(int, int)),
                this,
                SLOT(tableCellChanged(const int, const int))
                );
        connect(ui->tableWidget_SrcDataEU3,
                SIGNAL(cellChanged(int, int)),
                this,
                SLOT(tableCellChanged(const int, const int))
                );
        connect(ui->tableWidget_SrcDataPoints,
                SIGNAL(cellChanged(int, int)),
                this,
                SLOT(tableCellChanged(const int, const int))
                );
        connect(ui->tableWidget_FullLoadCurve,
                SIGNAL(cellChanged(int, int)),
                this,
                SLOT(tableCellChanged(const int, const int))
                );
    }
    else {

        disconnect(ui->tableWidget_SrcDataEU0,
                   SIGNAL(cellChanged(int, int)),
                   this,
                   SLOT(tableCellChanged(const int, const int))
                   );
        disconnect(ui->tableWidget_SrcDataEU3,
                   SIGNAL(cellChanged(int, int)),
                   this,
                   SLOT(tableCellChanged(const int, const int))
                   );
        disconnect(ui->tableWidget_SrcDataPoints,
                   SIGNAL(cellChanged(int, int)),
                   this,
                   SLOT(tableCellChanged(const int, const int))
                   );
        disconnect(ui->tableWidget_FullLoadCurve,
                   SIGNAL(cellChanged(int, int)),
                   this,
                   SLOT(tableCellChanged(const int, const int))
                   );
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

    if ( ( object == ui->tableWidget_SrcDataEU0 ||
           object == ui->tableWidget_SrcDataEU3 ||
           object == ui->tableWidget_SrcDataPoints ||
           object == ui->tableWidget_FullLoadCurve ) &&
         ( event->type() == QEvent::FocusIn ) ) {

        m_table = (QTableWidget*)object;
    }

    return false;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *cme) {

    m_contextMenu->exec(cme->globalPos());
}

void MainWindow::readPreferences() {

    try {
        m_commonParameters->readConfigFile(CONFIGFILENAME);
    }
    catch(const toxic::txError &toxerr) {
        QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
        return;
    }
}

void MainWindow::loadAllSourceData() {

    const QString filenameSourceEU0 = m_commonParameters->val_srcFileNameEU0();

    if ( QFile::exists(filenameSourceEU0) ) {

        tableCellChangedConnect(false);

        if ( !fillTableEU0(filenameSourceEU0) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        tr("Can not fill table!")
                        );
        }

        tableCellChangedConnect(true);
    }

    //

    const QString filenameSourceEU3 = m_commonParameters->val_srcFileNameEU3();

    if ( QFile::exists(filenameSourceEU3) ) {

        tableCellChangedConnect(false);

        if ( !fillTableEU3(filenameSourceEU3) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        tr("Can not fill table!")
                        );
        }

        tableCellChangedConnect(true);
    }

    //

    const QString filenamePoints = m_commonParameters->val_srcFileNamePoints();

    if ( QFile::exists(filenamePoints) ) {

        tableCellChangedConnect(false);

        if ( !fillTablePoints(filenamePoints) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        tr("Can not fill table!")
                        );
        }

        tableCellChangedConnect(true);
    }

    //

    const QString filenamePowers = m_commonParameters->val_srcFileNameRedPwr();

    if ( QFile::exists(filenamePowers) ) {

        tableCellChangedConnect(false);

        if ( !fillTableFullLoadCurve(filenamePowers) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        tr("Can not fill table!")
                        );
        }

        tableCellChangedConnect(true);
    }
}

bool MainWindow::fillTableEU0(const QString &filename) {

    ui->tableWidget_SrcDataEU0->setRowCount(1);
    ui->tableWidget_SrcDataEU0->setRowHeight(0, tableRowHeight);

    QSharedPointer<toxic::txDataReader>
            readerSourceDataEU0(new toxic::txDataReader());

    try{
        readerSourceDataEU0->readFile(filename, " ");
    }
    catch(const toxic::txError &toxerr) {
        QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
        return false;
    }

    QVector< QVector<double> > arraySourceDataEU0 =
            readerSourceDataEU0->val_data();

    if ( arraySourceDataEU0.isEmpty() ) {
        return false;
    }

    if ( arraySourceDataEU0[0].size() != ui->tableWidget_SrcDataEU0->columnCount() ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Incorrect source data! Check number of columns in file.")
                    );
        return false;
    }

    for ( ptrdiff_t j=0; j<arraySourceDataEU0[0].size(); j++ ) {

        ui->tableWidget_SrcDataEU0->
                setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU0[0][j], 'f', 3)));
        ui->tableWidget_SrcDataEU0->
                item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    return true;
}

bool MainWindow::fillTableEU3(const QString &filename) {

    ui->tableWidget_SrcDataEU3->setRowCount(1);
    ui->tableWidget_SrcDataEU3->setRowHeight(0, tableRowHeight);

    QSharedPointer<toxic::txDataReader>
            readerSourceDataEU3(new toxic::txDataReader());

    try{
        readerSourceDataEU3->readFile(filename, " ");
    }
    catch(const toxic::txError &toxerr) {
        QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
        return false;
    }

    QVector< QVector<double> > arraySourceDataEU3 = readerSourceDataEU3->val_data();

    if ( arraySourceDataEU3.isEmpty() ) {
        return false;
    }

    if ( arraySourceDataEU3[0].size() != ui->tableWidget_SrcDataEU3->columnCount() ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Incorrect source data! Check number of columns in file.")
                    );
        return false;
    }

    for ( ptrdiff_t j=0; j<arraySourceDataEU3[0].size(); j++ ) {

        ui->tableWidget_SrcDataEU3->
                setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU3[0][j], 'f', 3)));
        ui->tableWidget_SrcDataEU3->
                item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    return true;
}

bool MainWindow::fillTablePoints(const QString &filename) {

    QSharedPointer<toxic::txDataReader>
            readerSourceDataPoints(new toxic::txDataReader());

    try{
        readerSourceDataPoints->readFile(filename, " ");
    }
    catch(const toxic::txError &toxerr) {
        QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
        return false;
    }

    QVector< QVector<double> > arraySourceDataPoints =
            readerSourceDataPoints->val_data();

    if ( arraySourceDataPoints.isEmpty() ) {
        return false;
    }

    if ( arraySourceDataPoints[0].size() != ui->tableWidget_SrcDataPoints->columnCount() ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Incorrect source data! Check number of columns in file.")
                    );
        return false;
    }

    for ( ptrdiff_t i=0; i<arraySourceDataPoints.size(); i++ ) {

        ui->tableWidget_SrcDataPoints->setRowCount(i+1);
        ui->tableWidget_SrcDataPoints->setRowHeight(i, tableRowHeight);
        ui->tableWidget_SrcDataPoints->
                verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_SrcDataPoints->
                setItem(i, 0, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][0], 'f', 0)));
        ui->tableWidget_SrcDataPoints->
                item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_SrcDataPoints->
                setItem(i, 1, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][1], 'f', 0)));
        ui->tableWidget_SrcDataPoints->
                item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for ( ptrdiff_t j=2; j<arraySourceDataPoints[i].size(); j++ ) {

            ui->tableWidget_SrcDataPoints->
                    setItem(i, j, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][j], 'f', 3)));
            ui->tableWidget_SrcDataPoints->
                    item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return true;
}

bool MainWindow::fillTableFullLoadCurve(const QString &filename) {

    QSharedPointer<toxic::txDataReader>
            readerFullLoadCurve(new toxic::txDataReader());

    try{
        readerFullLoadCurve->readFile(filename, " ");
    }
    catch(const toxic::txError &toxerr) {
        QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
        return false;
    }

    QVector< QVector<double> > arrayFullLoadCurve =
            readerFullLoadCurve->val_data();

    if ( arrayFullLoadCurve.isEmpty() ) {
        return false;
    }

    if ( arrayFullLoadCurve[0].size() != ui->tableWidget_FullLoadCurve->columnCount() ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Incorrect source data! Check number of columns in file.")
                    );
        return false;
    }

    for ( ptrdiff_t i=0; i<arrayFullLoadCurve.size(); i++ ) {

        ui->tableWidget_FullLoadCurve->setRowCount(i+1);
        ui->tableWidget_FullLoadCurve->setRowHeight(i, tableRowHeight);

        ui->tableWidget_FullLoadCurve->
                setItem(i, 0, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][0], 'f', 0)));
        ui->tableWidget_FullLoadCurve->
                item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_FullLoadCurve->
                setItem(i, 1, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][1], 'f', 0)));
        ui->tableWidget_FullLoadCurve->
                item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for ( ptrdiff_t j=2; j<arrayFullLoadCurve[i].size(); j++ ) {

            ui->tableWidget_FullLoadCurve->
                    setItem(i, j, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][j], 'f', 3)));
            ui->tableWidget_FullLoadCurve->
                    item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return true;
}

bool MainWindow::fillParameters() {

    if ( ui->comboBox_task->currentIndex() == toxic::TASK_EMISSIONS &&
         ui->comboBox_PTcalc->currentIndex() == toxic::PTCALC_THROUGHPTMASS &&
         ui->doubleSpinBox_PTmass->value() == 0 ) {

        on_pushButton_EnterPTmass_clicked();
    }

    m_calculationOptions->setTask(ui->comboBox_task->currentIndex());
    m_calculationOptions->setVh(ui->doubleSpinBox_Vh->value());
    m_calculationOptions->setStandard(ui->comboBox_standard->currentIndex());
    m_calculationOptions->setChargingType(ui->comboBox_chargingType->currentIndex());
    m_calculationOptions->setFuelType(ui->comboBox_FuelType->currentIndex());
    m_calculationOptions->setNOxSample(ui->comboBox_NOxSample->currentIndex());
    m_calculationOptions->setPTcalc(ui->comboBox_PTcalc->currentIndex());
    m_calculationOptions->setPTmass(ui->doubleSpinBox_PTmass->value());
    m_calculationOptions->setAddPointsCalc(ui->comboBox_AddPointsCalc->currentIndex());
    m_calculationOptions->setTestDate(ui->dateEdit->text());

    return true;
}

bool MainWindow::arithmeticOperation(const QString &operation) {

    QDoubleSpinBox *value =
            m_valueDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Value");

    if ( !value ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    QString::fromLatin1(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child object not found!")
                    );
        return false;
    }

    value->setFocus();
    value->selectAll();

    if ( operation == "add" ) {
        m_valueDialog->setWindowTitle(tr("Addition"));
    }
    else if ( operation == "multiply" ) {
        m_valueDialog->setWindowTitle(tr("Multiplication"));
    }
    else if ( operation == "divide" ) {
        m_valueDialog->setWindowTitle(tr("Division"));
    }
    else if ( operation == "equal" ) {
        m_valueDialog->setWindowTitle(tr("Equality"));
    }
    else if ( operation == "randomize" ) {
        m_valueDialog->setWindowTitle(tr("Randomization"));
    }
    else {
        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Unknown arithmetic operation!")
                    );
        return false;
    }

    if ( m_valueDialog->exec() == QDialog::Rejected ) {
        return true;
    }

    if ( m_table->selectedRanges().isEmpty() ) {
        QMessageBox::warning(this, QR49NAME, tr("No selected cells!"));
        return false;
    }

    QTableWidgetSelectionRange selectedRange;

    double x = 0.0;
    double y = value->value();

    if ( (y == 0.0) && (operation == "divide") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Illegal operation \"Divide by zero\"!")
                    );
        return false;
    }

    tableCellChangedConnect(false);

    for ( ptrdiff_t n=0; n<m_table->selectedRanges().size(); n++ ) {

        selectedRange = m_table->selectedRanges()[n];

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                x = m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                        text().toDouble();

                if ( operation == "add" ) {
                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(x+y, 'f', 3));
                }
                else if ( operation == "multiply" ) {
                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(x*y, 'f', 3));
                }
                else if ( operation == "divide" ) {
                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(x/y, 'f', 3));
                }
                else if ( operation == "equal" ) {
                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(y, 'f', 3));
                }
                else if ( operation == "randomize" ) {
                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number((x-y)+2*y*static_cast<double>(qrand())/RAND_MAX, 'f', 3));
                }
            }
        }

    }

    tableCellChangedConnect(true);
    saveTableState();

    return true;
}

void MainWindow::on_action_DataImport_triggered() {

    if ( m_table == ui->tableWidget_SrcDataPoints ) {
        m_dataImportDialog->init(2, m_table);
    }
    else if ( m_table == ui->tableWidget_FullLoadCurve ) {
        m_dataImportDialog->init(3, m_table);
    }
    else {
        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Data import is not available for the current table!")
                    );
        return;
    }

    tableCellChangedConnect(false);

    if ( m_dataImportDialog->exec() == QDialog::Accepted ) {
        tableCellChangedConnect(true);
        saveTableState();
    }
    else {
        tableCellChangedConnect(true);
    }
}

void MainWindow::on_action_LoadSourceData_triggered() {

    const QString dir(m_commonParameters->val_reportsDirName());

    const QString anotherSourceFile(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open source data file..."),
                    dir,
                    QString::fromLatin1("Data files (*.dat);;All files (*)"),
                    0,
                    0)
                );

    if ( m_table == ui->tableWidget_SrcDataEU0 ) {

        ui->tabWidget_Data->setCurrentIndex(0);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTableEU0(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            QR49NAME,
                            tr("Can not fill table!")
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
    else if ( m_table == ui->tableWidget_SrcDataEU3 ) {

        ui->tabWidget_Data->setCurrentIndex(0);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTableEU3(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            QR49NAME,
                            tr("Can not fill table!")
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
    else if ( m_table == ui->tableWidget_SrcDataPoints ) {

        ui->tabWidget_Data->setCurrentIndex(1);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTablePoints(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            QR49NAME,
                            tr("Can not fill table!")
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
    else if ( m_table == ui->tableWidget_FullLoadCurve ) {

        ui->tabWidget_Data->setCurrentIndex(2);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTableFullLoadCurve(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            QR49NAME,
                            tr("Can not fill table!")
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
}

void MainWindow::on_action_SaveSourceData_triggered() {

    if ( m_table == ui->tableWidget_SrcDataEU0 ) {

        const QString filenameSourceEU0 = m_commonParameters->val_srcFileNameEU0();

        QFile SrcDataEU0File(filenameSourceEU0);

        if ( !SrcDataEU0File.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        filenameSourceEU0
                        + tr(" could not be opened!")
                        );
            return;
        }

        SrcDataEU0File.write(toxic::SRCDATACAPTIONS_6.join(" ").toUtf8());
        SrcDataEU0File.write("\n");

        for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

            SrcDataEU0File.write(m_table->item(0, j)->text().toUtf8());
            SrcDataEU0File.write(" ");
        }

        SrcDataEU0File.write("\n");

        SrcDataEU0File.close();
    }
    else if ( m_table == ui->tableWidget_SrcDataEU3 ) {

        const QString filenameSourceEU3 = m_commonParameters->val_srcFileNameEU3();

        QFile SrcDataEU3File(filenameSourceEU3);

        if ( !SrcDataEU3File.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        filenameSourceEU3
                        + tr(" could not be opened!")
                        );
            return;
        }

        SrcDataEU3File.write(toxic::SRCDATACAPTIONS_11.join(" ").toUtf8());
        SrcDataEU3File.write("\n");

        for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

            SrcDataEU3File.write(m_table->item(0, j)->text().toUtf8());
            SrcDataEU3File.write(" ");
        }

        SrcDataEU3File.write("\n");

        SrcDataEU3File.close();
    }
    else if ( m_table == ui->tableWidget_SrcDataPoints ) {

        if ( m_table->rowCount() == 0 ) {
            on_action_InsertRowBelowCurrent_triggered();
        }

        const QString filenamePoints = m_commonParameters->val_srcFileNamePoints();

        QFile SrcDataPointsFile(filenamePoints);

        if ( !SrcDataPointsFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        filenamePoints
                        + tr(" could not be opened!")
                        );
            return;
        }

        SrcDataPointsFile.write(toxic::SRCDATACAPTIONS_EMISSIONS.join(" ").toUtf8());
        SrcDataPointsFile.write("\n");

        for ( ptrdiff_t i=0; i<m_table->rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

                SrcDataPointsFile.
                        write(m_table->item(i, j)->text().toUtf8());
                SrcDataPointsFile.
                        write(" ");
            }

            SrcDataPointsFile.write("\n");
        }

        SrcDataPointsFile.close();
    }
    else if ( m_table == ui->tableWidget_FullLoadCurve ) {

        if ( m_table->rowCount() == 0 ) {
            on_action_InsertRowBelowCurrent_triggered();
        }

        const QString filenamePowers = m_commonParameters->val_srcFileNameRedPwr();

        QFile SrcDataPowersFile(filenamePowers);

        if ( !SrcDataPowersFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        filenamePowers
                        + tr(" could not be opened!")
                        );
            return;
        }

        SrcDataPowersFile.write(toxic::SRCDATACAPTIONS_REDPOWER.join(" ").toUtf8());
        SrcDataPowersFile.write("\n");

        for ( ptrdiff_t i=0; i<m_table->rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

                SrcDataPowersFile.
                        write(m_table->item(i, j)->text().toUtf8());
                SrcDataPowersFile.
                        write(" ");
            }

            SrcDataPowersFile.write("\n");
        }

        SrcDataPowersFile.close();
    }
}

void MainWindow::on_action_SaveSourceDataAs_triggered() {

    const QString newSourceDataFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save source data file as..."),
                    "noname.dat",
                    QString::fromLatin1("Data files (*.dat);;All files (*)"),
                    0,
                    0)
                );

    if ( !newSourceDataFileName.isEmpty() ) {

        QFile SrcDataFile(newSourceDataFileName);

        if ( !SrcDataFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        newSourceDataFileName
                        + tr(" could not be opened!")
                        );
            return;
        }

        if ( m_table == ui->tableWidget_SrcDataEU0 ) {

            SrcDataFile.write(toxic::SRCDATACAPTIONS_6.join(" ").toUtf8());
            SrcDataFile.write("\n");
        }
        else if ( m_table == ui->tableWidget_SrcDataEU3 ) {

            SrcDataFile.write(toxic::SRCDATACAPTIONS_11.join(" ").toUtf8());
            SrcDataFile.write("\n");
        }
        else if ( m_table == ui->tableWidget_SrcDataPoints ) {

            SrcDataFile.write(toxic::SRCDATACAPTIONS_EMISSIONS.join(" ").toUtf8());
            SrcDataFile.write("\n");
        }
        else if ( m_table == ui->tableWidget_FullLoadCurve ) {

            SrcDataFile.write(toxic::SRCDATACAPTIONS_REDPOWER.join(" ").toUtf8());
            SrcDataFile.write("\n");
        }

        for ( ptrdiff_t i=0; i<m_table->rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

                SrcDataFile.write(m_table->item(i, j)->text().toUtf8());
                SrcDataFile.write(" ");
            }

            SrcDataFile.write("\n");
        }

        SrcDataFile.close();
    }
}

void MainWindow::on_action_LoadCalculationOptions_triggered() {

    const QString dir(m_commonParameters->val_reportsDirName());

    const QString anotherOptions(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open calculation options file..."),
                    dir,
                    QString::fromLatin1("Config files (*.conf);;All files (*)"),
                    0,
                    0)
                );

    if ( !anotherOptions.isEmpty() ) {

        try {
            m_calculationOptions->readCalcConfigFile(anotherOptions);
            m_calculationOptions->setCalcConfigFile("");
        }
        catch(const toxic::txError &toxerr) {
            QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
            return;
        }

        ui->comboBox_task->setCurrentIndex(m_calculationOptions->val_task());
        ui->doubleSpinBox_Vh->setValue(m_calculationOptions->val_Vh());
        ui->comboBox_standard->setCurrentIndex(m_calculationOptions->val_standard());
        ui->comboBox_chargingType->setCurrentIndex(m_calculationOptions->val_chargingType());
        ui->comboBox_FuelType->setCurrentIndex(m_calculationOptions->val_fuelType());
        ui->comboBox_NOxSample->setCurrentIndex(m_calculationOptions->val_NOxSample());
        ui->comboBox_PTcalc->setCurrentIndex(m_calculationOptions->val_PTcalc());
        ui->doubleSpinBox_PTmass->setValue(m_calculationOptions->val_PTmass());
        ui->comboBox_AddPointsCalc->setCurrentIndex(m_calculationOptions->val_addPointsCalc());

        QStringList tstdate = m_calculationOptions->val_testDate().split("-");
        if ( tstdate.size() == 3 ) {
            ui->dateEdit->setDate(QDate(tstdate[2].toInt(), tstdate[1].toInt(), tstdate[0].toInt()));
        }

        taskChanged(ui->comboBox_task->currentIndex());
        standardChanged(ui->comboBox_standard->currentIndex());
        PTcalcChanged(ui->comboBox_PTcalc->currentIndex());
    }
}

void MainWindow::on_action_SaveCalculationOptionsAs_triggered() {

    const QString optionsFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save options..."),
                    "noname.conf",
                    QString::fromLatin1("Config files (*.conf);;All files (*)"),
                    0,
                    0)
                );

    if ( !optionsFileName.isEmpty() ) {

        QFile savedOptions(optionsFileName);

        if ( !savedOptions.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        optionsFileName
                        + tr(" could not be opened!")
                        );
            return;
        }

        QTextStream fout(&savedOptions);

        fout << "task"
             << "="
             << QString::number(ui->comboBox_task->currentIndex())
             << "\n"
             << "Vh"
             << "="
             << QString::number(ui->doubleSpinBox_Vh->value())
             << "\n"
             << "standard"
             << "="
             << QString::number(ui->comboBox_standard->currentIndex())
             << "\n"
             << "chargingType"
             << "="
             << QString::number(ui->comboBox_chargingType->currentIndex())
             << "\n"
             << "fuelType"
             << "="
             << QString::number(ui->comboBox_FuelType->currentIndex())
             << "\n"
             << "NOxSample"
             << "="
             << QString::number(ui->comboBox_NOxSample->currentIndex())
             << "\n"
             << "PTcalc"
             << "="
             << QString::number(ui->comboBox_PTcalc->currentIndex())
             << "\n"
             << "PTmass"
             << "="
             << QString::number(ui->doubleSpinBox_PTmass->value())
             << "\n"
             << "addPointsCalc"
             << "="
             << QString::number(ui->comboBox_AddPointsCalc->currentIndex())
             << "\n"
             << "testDate"
             << "="
             << ui->dateEdit->text()
             << "\n"
             << "calcConfigFile"
             << "="
             << m_calculationOptions->val_calcConfigFile()
             << "\n";

        savedOptions.close();
    }
}

void MainWindow::on_action_OpenReport_triggered() {

    const QString dir(m_commonParameters->val_reportsDirName());

    const QString anotherReport(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open report..."),
                    dir,
                    QString::fromLatin1("Text files (*.txt);;All files (*)"),
                    0,
                    0)
                );

    if ( !anotherReport.isEmpty() ) {

        ui->comboBox_OpenedReports->insertItem(0, anotherReport);
        ui->comboBox_OpenedReports->setCurrentIndex(0);
        reportChanged(anotherReport);
        ui->tabWidget_Data->setCurrentIndex(3);
    }
}

void MainWindow::on_action_SaveReportAs_triggered() {

    if ( ui->plainTextEdit_Report->document()->isEmpty() ) {

        QMessageBox::information(
                    this,
                    QR49NAME,
                    tr("Report is empty!")
                    );
        return;
    }

    const QString newReportFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save report as..."),
                    ui->comboBox_OpenedReports->currentText(),
                    QString::fromLatin1("Text files (*.txt);;All files (*)"),
                    0,
                    0)
                );

    if ( !newReportFileName.isEmpty() ) {

        saveReport(newReportFileName);

        m_savingReportNeeded = false;
        m_changedReportFileName.clear();
        ui->label_unsavedReport->setText("");

        ui->comboBox_OpenedReports->
                removeItem(ui->comboBox_OpenedReports->currentIndex());
        ui->comboBox_OpenedReports->insertItem(0, newReportFileName);
        ui->comboBox_OpenedReports->setCurrentIndex(0);
        reportChanged(newReportFileName);
        ui->tabWidget_Data->setCurrentIndex(3);
    }
}

void MainWindow::on_action_ReportToPDF_triggered() {

    if ( ui->plainTextEdit_Report->document()->isEmpty() ) {

        QMessageBox::information(
                    this,
                    QR49NAME,
                    tr("Report is empty!")
                    );
        return;
    }

    const QString filename = ui->comboBox_OpenedReports->currentText() + ".pdf";

    const QString newReportFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Export report to PDF..."),
                    filename,
                    QString::fromLatin1("PDF files (*.pdf);;All files (*)"),
                    0,
                    0)
                );

    if ( newReportFileName.isEmpty() ) {
        return;
    }

    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(10, 15, 10, 10, QPrinter::Millimeter);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(newReportFileName);
    printer.setFontEmbeddingEnabled(true);

    QTextDocument txtDoc;
    txtDoc.setDefaultFont(m_monospacedFont_8);
    txtDoc.setPageSize(QSizeF(printer.width(), printer.height()));
    txtDoc.setPlainText(ui->plainTextEdit_Report->toPlainText());
    txtDoc.print(&printer);
}

void MainWindow::on_action_SaveReportAndExportToPDF_triggered() {

    on_action_SaveReportAs_triggered();
    on_action_ReportToPDF_triggered();
}

void MainWindow::on_action_CloseReport_triggered() {

    if ( ui->comboBox_OpenedReports->count() == 1 ) {

        ui->comboBox_OpenedReports->removeItem(0);
        ui->plainTextEdit_Report->setPlainText("");
    }
    else {

        ui->comboBox_OpenedReports->
                removeItem(ui->comboBox_OpenedReports->currentIndex());

        reportChanged(ui->comboBox_OpenedReports->currentText());
    }

    ui->tabWidget_Data->setCurrentIndex(3);
}

void MainWindow::on_action_CopyLastResultsTo_triggered() {

    if ( m_lastReportsDir.absolutePath().isEmpty() ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("No last results! First, calculate something.")
                    );
        return;
    }

    const QString newResultsDirName(
                QFileDialog::getExistingDirectory(
                    this,
                    tr("Select directory..."),
                    m_lastReportsDir.absolutePath()
                    )
                );

    if ( newResultsDirName.isEmpty() ) {
        return;
    }

    const QString filter("*");
    QStringList fileNamesToCopy(
                m_lastReportsDir.entryList(QDir::nameFiltersFromString(filter), QDir::Files)
                );

    if ( fileNamesToCopy.isEmpty() ) {

        QMessageBox::warning(this, QR49NAME, tr("Nothing to copy!"));
        return;
    }

    for ( ptrdiff_t i=0; i<fileNamesToCopy.size(); i++ ) {

        QFile::copy(m_lastReportsDir.absolutePath()+QDir::separator()+fileNamesToCopy[i],
                    newResultsDirName+QDir::separator()+fileNamesToCopy[i]);
    }

    QMessageBox::information(this,
                             QR49NAME,
                             tr("All files from selected directory were copied.")
                             );
}

void MainWindow::on_action_PrintReport_triggered() {

    if ( ui->plainTextEdit_Report->document()->isEmpty() ) {

        QMessageBox::information(
                    this,
                    QR49NAME,
                    tr("Report is empty!")
                    );
        return;
    }

    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(10, 15, 10, 10, QPrinter::Millimeter);

    QPrintDialog printDialog(&printer, this);

    if ( printDialog.exec() == QDialog::Accepted ) {

        QTextDocument txtDoc;
        txtDoc.setDefaultFont(m_monospacedFont_8);
        txtDoc.setPageSize(QSizeF(printer.width(), printer.height()));
        txtDoc.setPlainText(ui->plainTextEdit_Report->toPlainText());
        txtDoc.print(&printer);
    }
}

void MainWindow::on_action_PrintSelectedCells_triggered() {

    if ( m_table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("No selected cells to printing!")
                    );
        return;
    }
    else if ( m_table->selectedRanges().size() > 1 ) {

        QMessageBox::information(
                    this,
                    QR49NAME,
                    tr("Only first range will be printed!")
                    );
    }

    //

    QTableWidgetSelectionRange selectedRange = m_table->selectedRanges().first();

    ptrdiff_t colnum = selectedRange.columnCount();

    if ( colnum > 7 ) {

        colnum = 7;
        QMessageBox::information(
                    this,
                    QR49NAME,
                    tr("Only 7 columns can be printed at a time!")
                    );
    }

    QString str;
    QTextStream pout(&str);

    pout << right << qSetFieldWidth(toxic::COLUMNWIDTH+3);

    for ( ptrdiff_t j=0; j<colnum; j++ ) {

        pout << m_table->horizontalHeaderItem(selectedRange.leftColumn()+j)->
                text().split("\n").join(" ");
    }

    pout << "\n";

    for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<colnum; j++ ) {

            pout << m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                    text();
        }

        pout << "\n";
    }

    //

    QPrinter printer;
    printer.setPrintRange(QPrinter::AllPages);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

    QPrintDialog printDialog(&printer, this);

    if ( printDialog.exec() == QDialog::Accepted ) {

        QPlainTextEdit pte;
        pte.setLineWrapMode(QPlainTextEdit::NoWrap);
        pte.setFont(m_dejavusansmonoFont_10);
        pte.insertPlainText(str);
        pte.print(&printer);
    }
}

void MainWindow::on_action_Minimize_triggered() {

    showMinimized();
}

void MainWindow::on_action_Quit_triggered() {

    close();
}

void MainWindow::on_action_UndoTable_triggered() {

    tableCellChangedConnect(false);

    if ( m_table == ui->tableWidget_SrcDataEU0 ) {

        m_undoRedo_TableEU0->undoTable();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_SrcDataEU3 ) {

        m_undoRedo_TableEU3->undoTable();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_SrcDataPoints ) {

        m_undoRedo_TablePoints->undoTable();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_FullLoadCurve ) {

        m_undoRedo_TableFullLoadCurve->undoTable();
        getUndoRedoCounters(m_table);
    }

    tableCellChangedConnect(true);
}

void MainWindow::on_action_RedoTable_triggered() {

    tableCellChangedConnect(false);

    if ( m_table == ui->tableWidget_SrcDataEU0 ) {

        m_undoRedo_TableEU0->redoTable();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_SrcDataEU3 ) {

        m_undoRedo_TableEU3->redoTable();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_SrcDataPoints ) {

        m_undoRedo_TablePoints->redoTable();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_FullLoadCurve ) {

        m_undoRedo_TableFullLoadCurve->redoTable();
        getUndoRedoCounters(m_table);
    }

    tableCellChangedConnect(true);
}

void MainWindow::on_action_CutFromTable_triggered() {

    on_action_CopyFromTable_triggered();
    on_action_DeleteFromTable_triggered();
}

void MainWindow::on_action_CopyFromTable_triggered() {

    if ( m_table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("No selected cells to copy!")
                    );
        return;
    }
    else if ( m_table->selectedRanges().size() > 1 ) {

        QMessageBox::information(
                    this,
                    QR49NAME,
                    tr("Only first range will be copied!")
                    );
    }

    //

    QTableWidgetSelectionRange selectedRange =
            m_table->selectedRanges().first();

    QString str;

    for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

            str += m_table->item(selectedRange.topRow()+i,
                                 selectedRange.leftColumn()+j)->text();

            if ( j != (selectedRange.columnCount()-1) ) {
                str += "\t";
            }
        }

        str += "\n";
    }

    QApplication::clipboard()->setText(str);
}

void MainWindow::on_action_PasteToTable_triggered() {

    const QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');

    const ptrdiff_t numRows = rows.count() - 1;
    const ptrdiff_t numColumns = rows.first().count('\t') + 1;

    if ( (m_table->columnCount() - m_table->currentColumn()) < numColumns ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Copied data can not be inserted!")
                    );
        return;
    }

    //

    ptrdiff_t destRows = 0;

    if ( m_table->currentRow() < 0 ) {
        destRows = m_table->rowCount();
    }
    else {
        destRows = m_table->rowCount() - m_table->currentRow();
    }

    tableCellChangedConnect(false);

    if ( numRows > destRows ) {
        addRows(m_table, numRows-destRows, ADDROWS_BOTTOM);
    }

    if ( (m_table->rowCount() > 0) && (m_table->currentRow() < 0) ) {
        m_table->setCurrentCell(0, 0);
    }

    //

    if ( numRows == 0 ) {

        QStringList columns = rows[0].split('\t');

        for ( ptrdiff_t j=0; j<numColumns; j++ ) {

            m_table->item(m_table->currentRow(),
                          m_table->currentColumn()+j)->setText(columns[j]);
        }
    }
    else {

        for ( ptrdiff_t i=0; i<numRows; i++ ) {

            QStringList columns = rows[i].split('\t');

            for ( ptrdiff_t j=0; j<numColumns; j++ ) {

                m_table->item(m_table->currentRow()+i,
                              m_table->currentColumn()+j)->setText(columns[j]);
            }
        }
    }

    tableCellChangedConnect(true);
    saveTableState();
}

void MainWindow::on_action_DeleteFromTable_triggered() {

    if ( m_table->selectedRanges().isEmpty() ) {
        return;
    }

    QTableWidgetSelectionRange selectedRange;

    tableCellChangedConnect(false);

    for ( ptrdiff_t n=0; n<m_table->selectedRanges().size(); n++ ) {

        selectedRange = m_table->selectedRanges()[n];

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                m_table->item(selectedRange.topRow()+i,
                              selectedRange.leftColumn()+j)->setText("0");
            }
        }

    }

    tableCellChangedConnect(true);
    saveTableState();
}

void MainWindow::on_action_Add_triggered() {

    if ( !arithmeticOperation("add") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Arithmetic operation is impossible!")
                    );
    }
}

void MainWindow::on_action_Multiply_triggered() {

    if ( !arithmeticOperation("multiply") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Arithmetic operation is impossible!")
                    );
    }
}

void MainWindow::on_action_Divide_triggered() {

    if ( !arithmeticOperation("divide") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Arithmetic operation is impossible!")
                    );
    }
}

void MainWindow::on_action_Equal_triggered() {

    if ( !arithmeticOperation("equal") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Operation is impossible!")
                    );
    }
}

void MainWindow::on_action_Randomize_triggered() {

    qsrand(QDateTime::currentMSecsSinceEpoch());

    if ( !arithmeticOperation("randomize") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Arithmetic operation is impossible!")
                    );
    }
}

void MainWindow::on_action_LowerAccuracy_triggered() {

    if ( m_table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(this, QR49NAME, tr("No selected cells!"));
        return;
    }

    QTableWidgetSelectionRange selectedRange;

    QString cellstr;
    double cellval = 0.0;
    QStringList parts;

    tableCellChangedConnect(false);

    for ( ptrdiff_t n=0; n<m_table->selectedRanges().size(); n++ ) {

        selectedRange = m_table->selectedRanges()[n];

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                cellstr = m_table->item(
                            selectedRange.topRow()+i, selectedRange.leftColumn()+j
                            )->text();
                cellval = cellstr.toDouble();
                parts = cellstr.split(".", QString::SkipEmptyParts);

                if ( parts.size() == 2 ) {

                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(cellval, 'f', parts[1].size()-1));
                }
                else {

                    m_table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(cellval, 'f', 0));
                }
            }
        }

    }

    tableCellChangedConnect(true);
    saveTableState();
}

void MainWindow::on_action_InsertRowAboveCurrent_triggered() {

    if ( m_table != ui->tableWidget_SrcDataPoints &&
         m_table != ui->tableWidget_FullLoadCurve ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("This action is not available for the current table!")
                    );
        return;
    }

    tableCellChangedConnect(false);
    addRows(m_table, 1, ADDROWS_ABOVE);
    tableCellChangedConnect(true);

    saveTableState();
}

void MainWindow::on_action_InsertRowBelowCurrent_triggered() {

    if ( m_table != ui->tableWidget_SrcDataPoints &&
         m_table != ui->tableWidget_FullLoadCurve ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("This action is not available for the current table!")
                    );
        return;
    }

    tableCellChangedConnect(false);
    addRows(m_table, 1, ADDROWS_BELOW);
    tableCellChangedConnect(true);

    saveTableState();
}

void MainWindow::on_action_DeleteSelectedRows_triggered() {

    if ( m_table != ui->tableWidget_SrcDataPoints &&
         m_table != ui->tableWidget_FullLoadCurve ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("This action is not available for the current table!")
                    );
        return;
    }

    if ( m_table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(this, QR49NAME, tr("No selected rows!"));
        return;
    }

    QTableWidgetSelectionRange selectedRange = m_table->selectedRanges().first();

    for ( ptrdiff_t i=selectedRange.bottomRow(); i>=selectedRange.topRow(); i-- ) {
        m_table->removeRow(i);
    }

    saveTableState();
}

void MainWindow::on_action_Toolbar_triggered() {

    if ( ui->action_Toolbar->isChecked() ) {
        ui->toolBar->setVisible(true);
    }
    else {
        ui->toolBar->setVisible(false);
    }
}

void MainWindow::on_action_English_triggered() {

    ui->action_English->setChecked(true);
    ui->action_Russian->setChecked(false);

    writeProgramSettings();
    tableCellChangedConnect(false);

    qApp->removeTranslator(translator);
    translator = new QTranslator(this);
    qApp->installTranslator(translator);

    ui->retranslateUi(this);
    m_filterMassDialog->retranslate();
    m_valueDialog->retranslate();
    m_preferencesDialog->retranslate();
    m_checkoutDataDialog->retranslate();
    m_dataImportDialog->retranslate();
    m_reportsetdialog->retranslate();

    tableCellChangedConnect(true);
    readProgramSettings();

    getUndoRedoCounters(m_table);
}

void MainWindow::on_action_Russian_triggered() {

    ui->action_English->setChecked(false);
    ui->action_Russian->setChecked(true);

    writeProgramSettings();
    tableCellChangedConnect(false);

    qApp->removeTranslator(translator);
    translator = new QTranslator(this);

    if ( !translator->load("langs/qr49_ru.qm") &&
         !translator->load("/usr/share/r49/translations/qr49_ru.qm") ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Can not load russian translation for GUI!")
                    );
    }

    qApp->installTranslator(translator);

    ui->retranslateUi(this);
    m_filterMassDialog->retranslate();
    m_valueDialog->retranslate();
    m_preferencesDialog->retranslate();
    m_checkoutDataDialog->retranslate();
    m_dataImportDialog->retranslate();
    m_reportsetdialog->retranslate();

    tableCellChangedConnect(true);
    readProgramSettings();

    getUndoRedoCounters(m_table);
}

void MainWindow::on_action_OptimizeTalbleCells_triggered() {

    m_table->resizeRowsToContents();
    m_table->resizeColumnsToContents();
}

void MainWindow::on_action_ExpandTableColumns_triggered() {

    ptrdiff_t maxWidth = 0;

    for ( ptrdiff_t i=0; i<m_table->columnCount(); i++ ) {

        if ( m_table->columnWidth(i) > maxWidth ) {
            maxWidth = m_table->columnWidth(i);
        }
    }

    for ( ptrdiff_t i=0; i<m_table->columnCount(); i++ ) {
        m_table->setColumnWidth(i, maxWidth);
    }

    for ( ptrdiff_t i=0; i<m_table->columnCount(); i++ ) {
        m_table->setColumnWidth(i, maxWidth+1);
    }
}

void MainWindow::on_action_ConstrictTableColumns_triggered() {

    ptrdiff_t maxWidth = 0;

    for ( ptrdiff_t i=0; i<m_table->columnCount(); i++ ) {

        if ( m_table->columnWidth(i) > maxWidth ) {
            maxWidth = m_table->columnWidth(i);
        }
    }

    for ( ptrdiff_t i=0; i<m_table->columnCount(); i++ ) {
        m_table->setColumnWidth(i, maxWidth);
    }

    for ( ptrdiff_t i=0; i<m_table->columnCount(); i++ ) {
        m_table->setColumnWidth(i, maxWidth-1);
    }
}

void MainWindow::on_action_Execute_triggered() {

    QVector< QVector<double> > array_DataForCalc;
    QVector<double> row;

    for ( ptrdiff_t i=0; i<m_table->rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

            row.push_back( m_table->item(i, j)->text().toDouble() );
        }

        array_DataForCalc.push_back(row);
        row.clear();
    }

    //

    QString message("\nCalculation completed!\n\n");

    fillParameters();

    if ( ui->comboBox_task->currentIndex() == toxic::TASK_POINTS ) {

        try {

            QSharedPointer<toxic::txPointsOfCycle>
                    myPoints(new toxic::txPointsOfCycle(m_commonParameters, m_calculationOptions));

            myPoints->setSourceData(array_DataForCalc);
            myPoints->calculate();

            message += myPoints->createReports();
        }
        catch(const toxic::txError &toxerr) {

            QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
            return;
        }

        //

        const QString filenamePoints = m_commonParameters->val_srcFileNamePoints();

        if ( QFile::exists(filenamePoints) ) {

            tableCellChangedConnect(false);

            if ( !fillTablePoints(filenamePoints) ) {

                QMessageBox::critical(
                            this,
                            QR49NAME,
                            tr("Can not fill table!")
                            );
            }

            tableCellChangedConnect(true);

            //

            m_table = ui->tableWidget_SrcDataPoints;
            saveTableState();
        }
        else {

            QMessageBox::warning(
                        this,
                        QR49NAME,
                        filenamePoints
                        + tr(" not found!")
                        );
        }

        //

        ui->tabWidget_Data->setCurrentIndex(1);
    }
    else if ( ui->comboBox_task->currentIndex() == toxic::TASK_EMISSIONS ) {

        QSharedPointer<toxic::txEmissionsOnCycle> myEmissions;

        try {

            myEmissions = QSharedPointer<toxic::txEmissionsOnCycle>
                    (new toxic::txEmissionsOnCycle(m_commonParameters, m_calculationOptions));

            myEmissions->setSourceData(array_DataForCalc);
            myEmissions->calculate();

            message += myEmissions->results();
        }
        catch(const toxic::txError &toxerr) {

            QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
            return;
        }

        if ( ui->checkBox_reports->isChecked() ) {

            try {
                message += myEmissions->createReports();
            }
            catch(const toxic::txError &toxerr) {
                QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
                return;
            }

            //

            m_lastReportsDir = myEmissions->lastReportDir();

            QDirIterator dit(m_lastReportsDir, QDirIterator::NoIteratorFlags);
            while ( dit.hasNext() ) {
                const QString ditem = dit.next();
                if ( dit.fileName().startsWith("CheckoutData") ) {
                    m_lastCheckoutDataFileName = ditem;
                    break;
                }
            }

            //

            if ( ui->comboBox_standard->currentIndex() == toxic::STD_FREECALC ) {
                ui->tabWidget_Data->setCurrentIndex(1);
            }
            else {

                ui->tabWidget_Data->setCurrentIndex(3);

                const QString txtfilter("*.txt");
                QStringList reports(m_lastReportsDir.entryList(
                                        QDir::nameFiltersFromString(txtfilter),
                                        QDir::Files,
                                        QDir::Time)
                                    );

                if ( reports.isEmpty() ) {
                    QMessageBox::warning(this, QR49NAME, tr("Cannot find the report files!"));
                }

                QString repgas;
                QString reppt;
                QDirIterator dit(m_lastReportsDir, QDirIterator::NoIteratorFlags);
                while ( dit.hasNext() ) {
                    const QString ditem = dit.next();
                    if ( dit.fileName().startsWith("ReportGAS") ) {
                        repgas = ditem;
                    }
                    else if ( dit.fileName().startsWith("ReportPT") ) {
                        reppt = ditem;
                    }
                }

                if ( !reppt.isEmpty() ) {
                    ui->comboBox_OpenedReports->insertItem(0, reppt);
                }

                m_lastReportFileName = repgas;

                ui->comboBox_OpenedReports->insertItem(0, m_lastReportFileName);
                ui->comboBox_OpenedReports->setCurrentIndex(0);

                //

                reportChanged(m_lastReportFileName);
            }
        }
    }
    else if ( ui->comboBox_task->currentIndex() == toxic::TASK_REDUCEDPOWER ) {

        QSharedPointer<toxic::txReducedPower> myReducedPower;

        try {
            myReducedPower = QSharedPointer<toxic::txReducedPower>
                    (new toxic::txReducedPower(m_commonParameters, m_calculationOptions));

            myReducedPower->setSourceData(array_DataForCalc);
            myReducedPower->calculate();

            message += myReducedPower->createReports();
        }
        catch(const toxic::txError &toxerr) {
            QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
            return;
        }

        //

        m_lastReportsDir = myReducedPower->lastReportDir();

        QDirIterator dit(m_lastReportsDir, QDirIterator::NoIteratorFlags);
        while ( dit.hasNext() ) {
            const QString ditem = dit.next();
            if ( dit.fileName().startsWith("CheckoutData") ) {
                m_lastCheckoutDataFileName = ditem;
                break;
            }
        }
    }
    else if ( ui->comboBox_task->currentIndex() == toxic::TASK_ABCSPEEDS ) {

        ui->toolBox->setCurrentIndex(1);
        return;
    }
    else {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Unknown calculation task!")
                    );
        return;
    }

    QMessageBox::information(this, QR49NAME, message);

    if ( (ui->comboBox_standard->currentIndex() == toxic::STD_FREECALC) ||
         (ui->comboBox_task->currentIndex() == toxic::TASK_REDUCEDPOWER) ) {

        on_action_CheckoutData_triggered();
    }
}

void MainWindow::on_action_CheckoutData_triggered() {

    QPlainTextEdit *myPlainTextEdit_CheckoutData =
            m_checkoutDataDialog->findChild<QPlainTextEdit *>("plainTextEdit_CheckoutData");
    QLineEdit *myLineEdit_file =
            m_checkoutDataDialog->findChild<QLineEdit *>("lineEdit_file");
    QPushButton *myPushButton_AltCopy =
            m_checkoutDataDialog->findChild<QPushButton *>("pushButton_AltCopy");

    if ( !myPlainTextEdit_CheckoutData || !myLineEdit_file || !myPushButton_AltCopy ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    QString::fromLatin1(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child objects not found!")
                    );
        return;
    }

    QFile arrayFile(m_lastCheckoutDataFileName);
    QString data;

    if ( arrayFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        data.append(arrayFile.readAll());
    }
    else {
        QMessageBox::critical(this, QR49NAME, tr("Can not open file!"));
        return;
    }

    arrayFile.close();

    myPlainTextEdit_CheckoutData->setPlainText(data);
    myLineEdit_file->setText(m_lastCheckoutDataFileName);

    if ( std::regex_match(myLineEdit_file->text().toStdString(), std::regex("(.*)(_R85_)(.*)")) ) {
        myPushButton_AltCopy->setEnabled(true);
    }
    else {
        myPushButton_AltCopy->setEnabled(false);
    }

    m_checkoutDataDialog->exec();
}

void MainWindow::on_action_Statistic_triggered() {

    if ( m_table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(this, QR49NAME, tr("No selected cells!"));
        return;
    }

    QTableWidgetSelectionRange selectedRange;
    QVector<double> v;

    for ( ptrdiff_t n=0; n<m_table->selectedRanges().size(); n++ ) {

        selectedRange = m_table->selectedRanges()[n];

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                v.push_back(m_table->
                            item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            text().toDouble());
            }
        }
    }

    QMessageBox::information(this, QR49NAME,
                             QString("<table border=\"1\">")
                             + "<tr><td>" + tr("Number") + "</td><td>" + QString::number(v.size())         + "</td></tr>"
                             + "<tr><td>" + tr("Sum")    + "</td><td>" + QString::number(toxic::sum(v))    + "</td></tr>"
                             + "<tr><td>" + tr("Min")    + "</td><td>" + QString::number(toxic::minVal(v)) + "</td></tr>"
                             + "<tr><td>" + tr("Max")    + "</td><td>" + QString::number(toxic::maxVal(v)) + "</td></tr>"
                             + "<tr><td>" + tr("Mean")   + "</td><td>" + QString::number(toxic::mean(v))   + "</td></tr>"
                             + "</table>");
}

void MainWindow::on_action_SmokeValuesConverting_triggered() {

    if ( m_table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Smoke values converting function usage:\n"
                       "1. Select source column (Ka[m-1] or Ka[%]) by click on column's header.\n"
                       "2. Activate menu item \"Calculation -> Convert smoke values\". "
                       "Results will be displayed in adjacent column.\n")
                    );
        return;
    }

    QTableWidgetSelectionRange selectedRange = m_table->selectedRanges().first();

    if ( (selectedRange.columnCount() != 1) ||
         (!ui->tableWidget_SrcDataPoints->
          horizontalHeaderItem(selectedRange.leftColumn())->text().contains("Ka")) ) {

        QMessageBox::warning(
                    this,
                    QR49NAME,
                    tr("Smoke values converting function usage:\n"
                       "1. Select source column (Ka[m-1] or Ka[%]) by click on column's header.\n"
                       "2. Activate menu item \"Calculation -> Convert smoke values\". "
                       "Results will be displayed in adjacent column.\n")
                    );
        return;
    }

    tableCellChangedConnect(false);

    if ( ui->tableWidget_SrcDataPoints->
         horizontalHeaderItem(selectedRange.leftColumn())->text().contains("%") ) {

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            ui->tableWidget_SrcDataPoints->
                    item(i, selectedRange.leftColumn()-1)->
                    setText(QString::number(toxic::Ka1m(ui->tableWidget_SrcDataPoints->item(i, selectedRange.leftColumn())->text().toDouble(), m_commonParameters->val_L()), 'f', 3));
        }
    }
    else {

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            ui->tableWidget_SrcDataPoints->
                    item(i, selectedRange.leftColumn()+1)->
                    setText(QString::number(toxic::KaPerc(ui->tableWidget_SrcDataPoints->item(i, selectedRange.leftColumn())->text().toDouble(), m_commonParameters->val_L()), 'f', 3));
        }
    }

    tableCellChangedConnect(true);
    saveTableState();
}

void MainWindow::on_action_Preferences_triggered() {

    QLineEdit *myLineEdit_filenameSourceEU3 =
            m_preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU3");
    QLineEdit *myLineEdit_filenameSourceEU0 =
            m_preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU0");
    QLineEdit *myLineEdit_filenamePoints =
            m_preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePoints");
    QLineEdit *myLineEdit_filenamePowers =
            m_preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePowers");
    QLineEdit *myLineEdit_dirnameReports =
            m_preferencesDialog->findChild<QLineEdit *>("lineEdit_dirnameReports");
    QDoubleSpinBox *myDoubleSpinBox_Dn =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Dn");
    QDoubleSpinBox *myDoubleSpinBox_ConcO2air =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcO2air");
    QDoubleSpinBox *myDoubleSpinBox_Rr =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Rr");
    QDoubleSpinBox *myDoubleSpinBox_L0 =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L0");
    QDoubleSpinBox *myDoubleSpinBox_L =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L");
    QDoubleSpinBox *myDoubleSpinBox_ConcCO2air =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcCO2air");
    QDoubleSpinBox *myDoubleSpinBox_WH =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WH");
    QDoubleSpinBox *myDoubleSpinBox_WO2 =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WO2");
    QDoubleSpinBox *myDoubleSpinBox_WN =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WN");
    QDoubleSpinBox *myDoubleSpinBox_muNO2 =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muNO2");
    QDoubleSpinBox *myDoubleSpinBox_muCO =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCO");
    QDoubleSpinBox *myDoubleSpinBox_muCH =
            m_preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCH");

    if ( !myLineEdit_filenameSourceEU3 ||
         !myLineEdit_filenameSourceEU0 ||
         !myLineEdit_filenamePoints    ||
         !myLineEdit_filenamePowers    ||
         !myLineEdit_dirnameReports    ||
         !myDoubleSpinBox_Dn           ||
         !myDoubleSpinBox_ConcO2air    ||
         !myDoubleSpinBox_Rr           ||
         !myDoubleSpinBox_L0           ||
         !myDoubleSpinBox_L            ||
         !myDoubleSpinBox_ConcCO2air   ||
         !myDoubleSpinBox_WH           ||
         !myDoubleSpinBox_WO2          ||
         !myDoubleSpinBox_WN           ||
         !myDoubleSpinBox_muNO2        ||
         !myDoubleSpinBox_muCO         ||
         !myDoubleSpinBox_muCH ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    QString::fromLatin1(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child object not found!")
                    );
        return;
    }

    //

    myLineEdit_filenameSourceEU3->setText(m_commonParameters->val_srcFileNameEU3());
    myLineEdit_filenameSourceEU0->setText(m_commonParameters->val_srcFileNameEU0());
    myLineEdit_filenamePoints->setText(m_commonParameters->val_srcFileNamePoints());
    myLineEdit_filenamePowers->setText(m_commonParameters->val_srcFileNameRedPwr());
    myLineEdit_dirnameReports->setText(m_commonParameters->val_reportsDirName());
    myDoubleSpinBox_Dn->setValue(m_commonParameters->val_Dn());
    myDoubleSpinBox_ConcO2air->setValue(m_commonParameters->val_concO2air());
    myDoubleSpinBox_Rr->setValue(m_commonParameters->val_Rr());
    myDoubleSpinBox_L0->setValue(m_commonParameters->val_L0());
    myDoubleSpinBox_L->setValue(m_commonParameters->val_L());
    myDoubleSpinBox_ConcCO2air->setValue(m_commonParameters->val_concCO2air());
    myDoubleSpinBox_WH->setValue(m_commonParameters->val_WH());
    myDoubleSpinBox_WO2->setValue(m_commonParameters->val_WO2());
    myDoubleSpinBox_WN->setValue(m_commonParameters->val_WN());
    myDoubleSpinBox_muNO2->setValue(m_commonParameters->val_muNO2());
    myDoubleSpinBox_muCO->setValue(m_commonParameters->val_muCO());
    myDoubleSpinBox_muCH->setValue(m_commonParameters->val_muCH());

    //

    if ( m_preferencesDialog->exec() == QDialog::Accepted ) {
        readPreferences();
    }
}

void MainWindow::on_action_ReportSettings_triggered() {

    QPlainTextEdit *myPlainTextEdit_EngineDescription =
            m_reportsetdialog->findChild<QPlainTextEdit *>("plainTextEdit_EngineDescription");
    QPlainTextEdit *myPlainTextEdit_TechnicalFluids =
            m_reportsetdialog->findChild<QPlainTextEdit *>("plainTextEdit_TechnicalFluids");

    if ( !myPlainTextEdit_EngineDescription ||
         !myPlainTextEdit_TechnicalFluids ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    QString::fromLatin1(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child objects not found!")
                    );
        return;
    }

    QFile engDescrFile("r49data/engdescr.conf");

    if ( !engDescrFile.exists() ) {

        myPlainTextEdit_EngineDescription->
                setPlainText(
                    "//\n// This is engine description file.\n"
                    "// Parameter-Value delimeter is \"=\" symbol.\n"
                    "// Text after \"//\" is comment.\n//\n\n"
                    );
    }
    else {

        if ( engDescrFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {

            myPlainTextEdit_EngineDescription->
                    setPlainText(QString(engDescrFile.readAll()));
        }
        else {
            QMessageBox::critical(this, QR49NAME, tr("Can not open file!"));
            return;
        }

        engDescrFile.close();
    }

    QFile techFluidsFile("r49data/techfluids.conf");

    if ( !techFluidsFile.exists() ) {

        myPlainTextEdit_TechnicalFluids->
                setPlainText(
                    "//\n// This is technical fluids description file.\n"
                    "// Parameter-Value delimeter is \"=\" symbol.\n"
                    "// Text after \"//\" is comment.\n//\n\n"
                    );
    }
    else {

        if ( techFluidsFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {

            myPlainTextEdit_TechnicalFluids->
                    setPlainText(QString(techFluidsFile.readAll()));
        }
        else {
            QMessageBox::critical(this, QR49NAME, tr("Can not open file!"));
            return;
        }

        techFluidsFile.close();
    }

    m_reportsetdialog->exec();
}

void MainWindow::on_action_UserManual_triggered() {

    const QString userManualLocation1 =
            appPath + DOCDIRNAME + "/r49_user_manual_ru.pdf";
    const QString userManualLocation2 =
            "/usr/share/r49/doc/r49_user_manual_ru.pdf";

    if ( QFile::exists(userManualLocation1) ) {

        if ( !QDesktopServices::openUrl(
                 QUrl::fromLocalFile(userManualLocation1) ) ) {

            QMessageBox::warning(this,
                                 QR49NAME,
                                 tr("Can not open documentation file!"));
            return;
        }
    }
    else if ( QFile::exists(userManualLocation2) ) {

        if ( !QDesktopServices::openUrl(QUrl(userManualLocation2)) ) {

            QMessageBox::warning(this,
                                 QR49NAME,
                                 tr("Can not open documentation file!"));
            return;
        }
    }
    else {

        QMessageBox::warning(this,
                             QR49NAME,
                             tr("Can not find documentation file!"));
        return;
    }
}

void MainWindow::on_action_StandardsDescription_triggered() {

    const QString standardDescLocation1 =
            appPath + DOCDIRNAME + "/diesel_engine_standards_ru.html";
    const QString standardDescLocation2 =
            "/usr/share/r49/doc/diesel_engine_standards_ru.html";

    if ( QFile::exists(standardDescLocation1) ) {

        if ( !QDesktopServices::openUrl(
                 QUrl::fromLocalFile(standardDescLocation1) ) ) {

            QMessageBox::warning(this,
                                 QR49NAME,
                                 tr("Can not open documentation file!"));
            return;
        }
    }
    else if ( QFile::exists(standardDescLocation2) ) {

        if ( !QDesktopServices::openUrl(QUrl(standardDescLocation2)) ) {

            QMessageBox::warning(this,
                                 QR49NAME,
                                 tr("Can not open documentation file!"));
            return;
        }
    }
    else {

        QMessageBox::warning(this,
                             QR49NAME,
                             tr("Can not find documentation file!"));
        return;
    }
}

void MainWindow::on_action_AboutQr49_triggered() {

    const QString str =
            "<b>"
            + QR49NAME + " v" + QR49VERSION
            + ", "
            + toxic::toxicIdentification{}.name()
            + " v"
            + toxic::toxicIdentification{}.version()
            + "</b><br><br>Build: "
            + QString(__DATE__)
            + " "
            + QString(__TIME__)
            + "<br><br>Calculation of modes and specific emissions for "
            "stationary diesel engine test cycles (UN ECE Regulation No. 49, "
            "UN ECE Regulation No. 96, UN ECE Regulation No. 85, "
            "OST 37.001.234-81, GOST 17.2.2.05-97, GOST 30574-98, GOST R "
            "51249-99)."
            "<br><br>Copyright (C) 2009-2017 Artem Petrov "
            "<a href= \"mailto:pa2311@gmail.com\" >pa2311@gmail.com</a>"
            "<br><br>Special thanks to Dmitry Epaneshnikov for a set of icons."
            "<br><br>Source code hosting: <a href= \"https://github.com/pa23/r49\">"
            "https://github.com/pa23/r49</a>"
            "<br>Author's blog (RU): "
            "<a href= \"http://pa2311.blogspot.com\">"
            "http://pa2311.blogspot.com</a>"
            "<br><br>This program is free software: you can redistribute it "
            "and/or modify "
            "it under the terms of the GNU General Public License as "
            "published by "
            "the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version. "
            "<br>This program is distributed in the hope that it will "
            "be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
            "GNU General Public License for more details. "
            "<br>You should have received a copy of the GNU General "
            "Public License "
            "along with this program. If not, see <a href= "
            "\"http://www.gnu.org/licenses/\" >"
            "http://www.gnu.org/licenses/</a>.<br>";

    QMessageBox::about(this, tr("About program"), str);
}

void MainWindow::on_action_AboutQt_triggered() {

    QMessageBox::aboutQt(this);
}

void MainWindow::on_pushButton_EnterPTmass_clicked() {

    if ( m_filterMassDialog->exec() == QDialog::Accepted ) {

        QDoubleSpinBox *m1c = m_filterMassDialog->findChild<QDoubleSpinBox *>
                ("doubleSpinBox_1stFilterWeightClean");
        QDoubleSpinBox *m1d = m_filterMassDialog->findChild<QDoubleSpinBox *>
                ("doubleSpinBox_1stFilterWeightDirty");
        QDoubleSpinBox *m2c = m_filterMassDialog->findChild<QDoubleSpinBox *>
                ("doubleSpinBox_2ndFilterWeightClean");
        QDoubleSpinBox *m2d = m_filterMassDialog->findChild<QDoubleSpinBox *>
                ("doubleSpinBox_2ndFilterWeightDirty");

        if ( (!m1c) || (!m1d) || (!m2c) || (!m2d) ) {

            QMessageBox::critical(
                        this,
                        QR49NAME,
                        QString::fromLatin1(Q_FUNC_INFO)
                        + ":::"
                        + tr("Child object not found!")
                        );
            return;
        }

        ui->doubleSpinBox_PTmass->setValue(
                    (m1d->value() - m1c->value()) + (m2d->value() - m2c->value())
                    );
    }
}

void MainWindow::on_pushButton_resetDate_clicked() {

    ui->dateEdit->setDate(QDate::currentDate());
}

void MainWindow::taskChanged(const int currtask) {

    if ( currtask == toxic::TASK_POINTS ) {

        ui->doubleSpinBox_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);
        ui->comboBox_chargingType->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->doubleSpinBox_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);

        const ptrdiff_t currstd = ui->comboBox_standard->currentIndex();

        if ( (currstd == toxic::STD_EU6) ||
             (currstd == toxic::STD_EU5) ||
             (currstd == toxic::STD_EU4) ||
             (currstd == toxic::STD_EU3) ) {

            ui->comboBox_AddPointsCalc->setEnabled(true);
        }
        else {

            ui->comboBox_AddPointsCalc->setEnabled(false);
        }

        ui->dateEdit->setEnabled(false);
        ui->pushButton_resetDate->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);

        ui->tab_cycPointsCalc->setEnabled(true);
        ui->tab_cycEmissCalc->setEnabled(false);
        ui->tab_redPowerCalc->setEnabled(false);
        ui->tab_Reports->setEnabled(false);

        if ( currstd == toxic::STD_FREECALC ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
        else if ( (currstd == toxic::STD_EU6) ||
                  (currstd == toxic::STD_EU5) ||
                  (currstd == toxic::STD_EU4) ||
                  (currstd == toxic::STD_EU3) ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);

            ui->tableWidget_SrcDataEU3->setEnabled(true);
            ui->tableWidget_SrcDataEU3->setFocus();

            getUndoRedoCounters(m_table);
        }
        else {

            ui->tableWidget_SrcDataEU0->setEnabled(true);
            ui->tableWidget_SrcDataEU0->setFocus();

            getUndoRedoCounters(m_table);

            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }

        ui->tableWidget_SrcDataPoints->setEnabled(false);
        ui->tableWidget_FullLoadCurve->setEnabled(false);

        ui->tabWidget_Data->setCurrentIndex(0);
    }
    else if ( currtask == toxic::TASK_EMISSIONS ) {

        ui->doubleSpinBox_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);

        standardChanged(ui->comboBox_standard->currentIndex());

        ui->checkBox_reports->setEnabled(true);

        ui->tab_cycPointsCalc->setEnabled(false);
        ui->tab_cycEmissCalc->setEnabled(true);
        ui->tab_redPowerCalc->setEnabled(false);
        ui->tab_Reports->setEnabled(true);

        ui->tableWidget_SrcDataEU0->setEnabled(false);
        ui->tableWidget_SrcDataEU3->setEnabled(false);

        ui->tableWidget_SrcDataPoints->setEnabled(true);
        ui->tableWidget_SrcDataPoints->setFocus();

        getUndoRedoCounters(m_table);

        ui->tableWidget_FullLoadCurve->setEnabled(false);

        ui->tabWidget_Data->setCurrentIndex(1);
    }
    else if ( currtask == toxic::TASK_REDUCEDPOWER ) {

        ui->doubleSpinBox_Vh->setEnabled(true);
        ui->comboBox_standard->setEnabled(false);
        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->doubleSpinBox_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);
        ui->dateEdit->setEnabled(false);
        ui->pushButton_resetDate->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);

        ui->tab_cycPointsCalc->setEnabled(false);
        ui->tab_cycEmissCalc->setEnabled(false);
        ui->tab_redPowerCalc->setEnabled(true);
        ui->tab_Reports->setEnabled(true);

        ui->tableWidget_SrcDataEU0->setEnabled(false);
        ui->tableWidget_SrcDataEU3->setEnabled(false);
        ui->tableWidget_SrcDataPoints->setEnabled(false);

        ui->tableWidget_FullLoadCurve->setEnabled(true);
        ui->tableWidget_FullLoadCurve->setFocus();

        getUndoRedoCounters(m_table);

        ui->tabWidget_Data->setCurrentIndex(2);
    }
    else {

        ui->doubleSpinBox_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(false);
        ui->comboBox_chargingType->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->doubleSpinBox_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);
        ui->dateEdit->setEnabled(false);
        ui->pushButton_resetDate->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);
    }
}

void MainWindow::standardChanged(const int currstd) {

    if ( ui->comboBox_task->currentIndex() != toxic::TASK_POINTS ) {

        ui->comboBox_chargingType->setEnabled(true);

        if ( (currstd == toxic::STD_C1) ||
             (currstd == toxic::STD_D1) ||
             (currstd == toxic::STD_D2) ||
             (currstd == toxic::STD_E1) ||
             (currstd == toxic::STD_E2) ||
             (currstd == toxic::STD_E3) ||
             (currstd == toxic::STD_E5) ||
             (currstd == toxic::STD_F ) ||
             (currstd == toxic::STD_G1) ||
             (currstd == toxic::STD_G2) ) {

            ui->comboBox_FuelType->setEnabled(true);
        }
        else {
            ui->comboBox_FuelType->setEnabled(false);
        }

        ui->comboBox_NOxSample->setEnabled(true);

        if ( (currstd == toxic::STD_EU0          ) ||
             (currstd == toxic::STD_OST3700123481) ||
             (currstd == toxic::STD_GOST17220597 ) ||
             (currstd == toxic::STD_C1) ||
             (currstd == toxic::STD_D1) ||
             (currstd == toxic::STD_D2) ||
             (currstd == toxic::STD_E1) ||
             (currstd == toxic::STD_E2) ||
             (currstd == toxic::STD_E3) ||
             (currstd == toxic::STD_E5) ||
             (currstd == toxic::STD_F ) ||
             (currstd == toxic::STD_G1) ||
             (currstd == toxic::STD_G2) ) {

            ui->comboBox_PTcalc->setEnabled(false);
            ui->doubleSpinBox_PTmass->setEnabled(false);
            ui->pushButton_EnterPTmass->setEnabled(false);
        }
        else {

            ui->comboBox_PTcalc->setEnabled(true);

            if ( ui->comboBox_PTcalc->currentIndex() == toxic::PTCALC_NO ||
                 ui->comboBox_PTcalc->currentIndex() == toxic::PTCALC_THROUGHSMOKE ) {

                ui->doubleSpinBox_PTmass->setEnabled(false);
                ui->pushButton_EnterPTmass->setEnabled(false);
            }
            else {

                ui->doubleSpinBox_PTmass->setEnabled(true);
                ui->pushButton_EnterPTmass->setEnabled(true);
            }
        }
    }

    if ( (currstd == toxic::STD_EU6) ||
         (currstd == toxic::STD_EU5) ||
         (currstd == toxic::STD_EU4) ||
         (currstd == toxic::STD_EU3) ) {

        ui->comboBox_AddPointsCalc->setEnabled(true);

        if ( ui->comboBox_task->currentIndex() == toxic::TASK_POINTS ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);

            ui->tableWidget_SrcDataEU3->setEnabled(true);
            ui->tableWidget_SrcDataEU3->setFocus();

            getUndoRedoCounters(m_table);
        }
    }
    else {

        ui->comboBox_AddPointsCalc->setEnabled(false);

        if ( ui->comboBox_task->currentIndex() == toxic::TASK_POINTS ) {

            if ( currstd == toxic::STD_FREECALC ) {

                ui->tableWidget_SrcDataEU0->setEnabled(false);
                ui->tableWidget_SrcDataEU3->setEnabled(false);
            }
            else {

                ui->tableWidget_SrcDataEU0->setEnabled(true);
                ui->tableWidget_SrcDataEU0->setFocus();

                ui->tableWidget_SrcDataEU3->setEnabled(false);

                getUndoRedoCounters(m_table);
            }
        }
    }

    ui->dateEdit->setEnabled(true);
    ui->pushButton_resetDate->setEnabled(true);
}

void MainWindow::PTcalcChanged(const int currptcalc) {

    if ( (currptcalc == toxic::PTCALC_THROUGHSMOKE) ||
         (currptcalc == toxic::PTCALC_NO) ) {

        ui->doubleSpinBox_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
    }
    else {

        ui->doubleSpinBox_PTmass->setEnabled(true);
        ui->pushButton_EnterPTmass->setEnabled(true);
    }
}

void MainWindow::reportChanged(const QString &path) {

    if ( m_savingReportNeeded ) {

        if ( QMessageBox::question(this,
                                   QR49NAME,
                                   tr("Report was changed. Save it?"),
                                   QMessageBox::Yes,
                                   QMessageBox::No) == QMessageBox::Yes ) {

            saveReport(m_changedReportFileName);
        }
    }

    QFile reportFile(path);

    if ( reportFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {

        ui->plainTextEdit_Report->setPlainText(reportFile.readAll());
    }
    else {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Can not open report file!")
                    );
        return;
    }

    reportFile.close();

    //

    m_savingReportNeeded = false;
    m_changedReportFileName.clear();
    ui->label_unsavedReport->setText("");
}

void MainWindow::reportTextChanged(bool b) {

    m_savingReportNeeded = b;

    if ( b ) {
        m_changedReportFileName = ui->comboBox_OpenedReports->currentText();
        ui->label_unsavedReport->setText("*");
    }
    else {
        m_changedReportFileName.clear();
        ui->label_unsavedReport->setText("");
    }
}

void MainWindow::tabChanged(const int tab) {

    if ( tab == 3 ) {
        ui->menuEdit->setEnabled(false);
    }
    else {

        if ( tab == 0 ) {
            ui->comboBox_task->setCurrentIndex(0);
            taskChanged(ui->comboBox_task->currentIndex());
        }
        else if ( tab == 1 ) {
            ui->comboBox_task->setCurrentIndex(1);
            taskChanged(ui->comboBox_task->currentIndex());
        }
        else if ( tab == 2 ) {
            ui->comboBox_task->setCurrentIndex(2);
            taskChanged(ui->comboBox_task->currentIndex());
        }

        if ( m_undoCount == 0 ) {
            ui->action_UndoTable->setEnabled(false);
        }
        else {
            ui->action_UndoTable->setEnabled(true);
        }

        if ( m_redoCount == 0 ) {
            ui->action_RedoTable->setEnabled(false);
        }
        else {
            ui->action_RedoTable->setEnabled(true);
        }

        ui->menuEdit->setEnabled(true);
    }
}

void MainWindow::tableCellChanged(const int n, const int m) {

    QString str = m_table->item(n, m)->text();
    int pos = 0;

    if ( (m_regExpValidator->validate(str, pos) == QValidator::Invalid) ||
         (str.isEmpty()) ) {

        QMessageBox::warning(this, QR49NAME, tr("Illegal table cell value!"));

        m_table->item(n, m)->setText("0");
        return;
    }

    saveTableState();

    //

    if ( n != m_table->rowCount()-1 ) {
        m_table->setCurrentCell(n+1, m);
    }
}

void MainWindow::getUndoRedoCounters(QTableWidget *tbl) {

    if ( tbl == ui->tableWidget_SrcDataEU0 ) {

        m_undoCount = m_undoRedo_TableEU0->undoTableNumber();
        m_redoCount = m_undoRedo_TableEU0->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if ( tbl == ui->tableWidget_SrcDataEU3 ) {

        m_undoCount = m_undoRedo_TableEU3->undoTableNumber();
        m_redoCount = m_undoRedo_TableEU3->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if ( tbl == ui->tableWidget_SrcDataPoints ) {

        m_undoCount = m_undoRedo_TablePoints->undoTableNumber();
        m_redoCount = m_undoRedo_TablePoints->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if ( tbl == ui->tableWidget_FullLoadCurve ) {

        m_undoCount = m_undoRedo_TableFullLoadCurve->undoTableNumber();
        m_redoCount = m_undoRedo_TableFullLoadCurve->redoTableNumber();

        setUndoRedoButtonState();
    }
}

void MainWindow::setUndoRedoButtonState() {

    QString ttUndo(tr("Undo"));
    QString ttRedo(tr("Redo"));

    ttUndo += " (" + QString::number(m_undoCount) + ")";
    ttRedo += " (" + QString::number(m_redoCount) + ")";

    if ( m_undoCount < 1 ) {

        ui->action_UndoTable->setEnabled(false);
        ui->action_UndoTable->setToolTip(ttUndo);
    }
    else {

        ui->action_UndoTable->setEnabled(true);
        ui->action_UndoTable->setToolTip(ttUndo);
    }

    if ( m_redoCount < 1 ) {

        ui->action_RedoTable->setEnabled(false);
        ui->action_RedoTable->setToolTip(ttRedo);
    }
    else {

        ui->action_RedoTable->setEnabled(true);
        ui->action_RedoTable->setToolTip(ttRedo);
    }
}

void MainWindow::saveTableState() {

    if ( m_table == ui->tableWidget_SrcDataEU0 ) {

        m_undoRedo_TableEU0->saveState();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_SrcDataEU3 ) {

        m_undoRedo_TableEU3->saveState();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_SrcDataPoints ) {

        m_undoRedo_TablePoints->saveState();
        getUndoRedoCounters(m_table);
    }
    else if ( m_table == ui->tableWidget_FullLoadCurve ) {

        m_undoRedo_TableFullLoadCurve->saveState();
        getUndoRedoCounters(m_table);
    }
}

void MainWindow::saveStateForAllTables() {

    m_undoRedo_TableEU0->saveState();
    m_undoRedo_TableEU3->saveState();
    m_undoRedo_TablePoints->saveState();
    m_undoRedo_TableFullLoadCurve->saveState();
}

void MainWindow::saveReport(const QString &newReportFileName) {

    QFile reportFile(newReportFileName);

    if ( !reportFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    newReportFileName
                    + tr(" could not be saved!")
                    );
        return;
    }

    QTextStream fout(&reportFile);
    fout << ui->plainTextEdit_Report->toPlainText();

    reportFile.close();
}

void MainWindow::abcCalculation() {

    const double n_hi = ui->doubleSpinBox_nhi->value();
    const double n_lo = ui->doubleSpinBox_nlo->value();

    double A = 0;
    double B = 0;
    double C = 0;
    double a1 = 0;
    double a2 = 0;
    double a3 = 0;
    double n_ref = 0;

    try {
        toxic::ABC(n_hi, n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref);
    }
    catch(const toxic::txError &toxerr) {

        ui->doubleSpinBox_nhi->setValue(0);
        ui->doubleSpinBox_nlo->setValue(0);

        QMessageBox::critical(this, QR49NAME, toxerr.val_toxicErrMsg());
        return;
    }

    ui->label_A->setText(QString::number(A));
    ui->label_B->setText(QString::number(B));
    ui->label_C->setText(QString::number(C));
    ui->label_a1->setText(QString::number(a1));
    ui->label_a2->setText(QString::number(a2));
    ui->label_a3->setText(QString::number(a3));
    ui->label_nref->setText(QString::number(n_ref));
}

void MainWindow::gairCalculation() {

    ui->label_Gair->
            setText(QString::number(
                        toxic::Gair(ui->doubleSpinBox_Dn->value(),
                                    ui->doubleSpinBox_B0->value(),
                                    ui->doubleSpinBox_t0->value(),
                                    ui->doubleSpinBox_dPn->value())
                        )
                    );
}

void MainWindow::nfanCalculation() {

    ui->label_Nfan->
            setText(QString::number(
                        toxic::N_fan(ui->doubleSpinBox_nFanRated->value(),
                                     ui->doubleSpinBox_n->value(),
                                     ui->doubleSpinBox_nRated->value())
                        )
                    );
}

void MainWindow::on_pushButton_OpenDirectory_clicked() {

    QFileInfo fileInfo(ui->comboBox_OpenedReports->currentText());

    if ( !QDesktopServices::
         openUrl(
             QUrl::fromLocalFile(fileInfo.absoluteDir().absolutePath())
             )
         ) {

        QMessageBox::warning(this, QR49NAME, tr("Can not open directory!"));
        return;
    }
}
