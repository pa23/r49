/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: qr49.cpp

    Copyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>

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
#include "newversions.h"
#include "dataimportdialog.h"
#include "tablewidgetfunctions.h"

#include "r49.h"
#include "qr49constants.h"
#include "libtoxicparameters.h"
#include "libtoxicconstants.h"
#include "csvread.h"
#include "cyclepoints.h"
#include "cycleemissions.h"
#include "reducedpower.h"
#include "commonparameters.h"
#include "precalc.h"
#include "toxicerror.h"

#include <QSharedPointer>
#include <QVector>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QStringList>
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
#include <QDoubleSpinBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :

    QMainWindow(parent),
    ui(new Ui::MainWindow),

    qr49settings("pa23software", "Qr49"),

    contextMenu(new QMenu()),

    filterMassDialog(new FilterMassDialog(this)),
    valueDialog(new ValueDialog(this)),
    preferencesDialog(new PreferencesDialog(this)),
    checkoutDataDialog(new CheckoutDataDialog(this)),
    helpDialog(new HelpDialog(this)),
    dataImportDialog(new DataImportDialog(this)),

    regExp("[-+]?[0-9]*[.,]?[0-9]+([eE][-+]?[0-9]+)?"),

    undoCount(0),
    redoCount(0) {

    ui->setupUi(this);

    //

    this->setWindowTitle(QR49VERSION);

    contextMenu->addMenu(ui->menuFile);
    contextMenu->addMenu(ui->menuEdit);
    contextMenu->addMenu(ui->menuCalculation);

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

    table = ui->tableWidget_SrcDataPoints;

    //

    params = QSharedPointer<LibtoxicParameters>(new LibtoxicParameters());
    config = QSharedPointer<CommonParameters>(new CommonParameters());

    readPreferences();

    //

    doubleValidator = new QDoubleValidator(this);

    doubleValidator->setBottom(0);
    doubleValidator->setDecimals(4);

    regExpValidator = new QRegExpValidator(regExp, 0);

    setDoubleValidators();

    //

    loadAllSourceData();

    //

    undoRedo_TableEU0 =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_SrcDataEU0));
    undoRedo_TableEU3 =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_SrcDataEU3));
    undoRedo_TablePoints =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_SrcDataPoints));
    undoRedo_TableFullLoadCurve =
            QSharedPointer<UndoRedoTable>
            (new UndoRedoTable(ui->tableWidget_FullLoadCurve));

    saveStateForAllTables();

    //

    readProgramSettings();

    taskChanged(ui->comboBox_task->currentIndex());
    standardChanged(ui->comboBox_standard->currentIndex());
    PTcalcChanged(ui->comboBox_PTcalc->currentIndex());

    //

    QFile excalibFont(":/fonts/fonts/excalib.ttf");
    excalibFont.open(QIODevice::ReadOnly);

    const ptrdiff_t fontid1 =
            QFontDatabase::addApplicationFontFromData(excalibFont.readAll());

    excalibFont.close();

    if ( fontid1 == -1 ) {

        QMessageBox::warning(
                    this,
                    "Qr49",
                    tr("Can not load monospaced font excalib.ttf "
                       "from program resources!"),
                    0, 0, 0
                    );
    }
    else {

        monospacedFont_8.setFamily(
                    QFontDatabase::applicationFontFamilies(fontid1).first()
                    );

        monospacedFont_8.setPointSize(8);

        monospacedFont_10.setFamily(
                    QFontDatabase::applicationFontFamilies(fontid1).first()
                    );

        monospacedFont_10.setPointSize(10);

        ui->plainTextEdit_Report->setFont(monospacedFont_10);

        QPlainTextEdit *myPlainTextEdit_CheckoutData =
                checkoutDataDialog->findChild<QPlainTextEdit *>
                ("plainTextEdit_CheckoutData");

        if ( !myPlainTextEdit_CheckoutData ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        QString::fromAscii(Q_FUNC_INFO)
                        + ":::"
                        + tr("Child object not found!"),
                        0, 0, 0
                        );

            return;
        }

        myPlainTextEdit_CheckoutData->setFont(monospacedFont_10);
    }

    QFile dejavuFont(":/fonts/fonts/DejaVuSansMono.ttf");
    dejavuFont.open(QIODevice::ReadOnly);

    const ptrdiff_t fontid2 =
            QFontDatabase::addApplicationFontFromData(dejavuFont.readAll());

    dejavuFont.close();

    if ( fontid2 == -1 ) {

        QMessageBox::warning(
                    this,
                    "Qr49",
                    tr("Can not load monospaced font DejaVuSansMono.ttf "
                       "from program resources!"),
                    0, 0, 0
                    );
    }
    else {

        dejavusansmonoFont_10.setFamily(
                    QFontDatabase::applicationFontFamilies(fontid2).first()
                    );

        dejavusansmonoFont_10.setPointSize(10);
    }

    //

    newVersions = QSharedPointer<NewVersions>(new NewVersions());

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

    connect(ui->doubleSpinBox_L,
            SIGNAL(editingFinished()),
            this,
            SLOT(smokeBaseChanged()));

    connect(ui->doubleSpinBox_Ka1m,
            SIGNAL(editingFinished()),
            this,
            SLOT(kapercCalculation()));

    connect(ui->doubleSpinBox_KaPerc,
            SIGNAL(editingFinished()),
            this,
            SLOT(ka1mCalculation()));
}

MainWindow::~MainWindow() {

    table = ui->tableWidget_SrcDataEU0;
    on_action_SaveSourceData_activated();

    table = ui->tableWidget_SrcDataEU3;
    on_action_SaveSourceData_activated();

    table = ui->tableWidget_SrcDataPoints;
    on_action_SaveSourceData_activated();

    table = ui->tableWidget_FullLoadCurve;
    on_action_SaveSourceData_activated();

    writeProgramSettings();

    delete ui;
    delete contextMenu;
    delete filterMassDialog;
    delete valueDialog;
    delete preferencesDialog;
    delete checkoutDataDialog;
    delete helpDialog;
    delete dataImportDialog;
    delete doubleValidator;
    delete regExpValidator;
}

void MainWindow::writeProgramSettings() {

    qr49settings.beginGroup("/Settings");
    qr49settings.setValue("/window_geometry", geometry());
    qr49settings.setValue("/toolbars_state", QMainWindow::saveState());
    qr49settings.setValue("/action_toolbar_checked", ui->action_Toolbar->isChecked());
    qr49settings.setValue("/active_tab", ui->tabWidget_Data->currentIndex());
    qr49settings.setValue("/task_index", ui->comboBox_task->currentIndex());
    qr49settings.setValue("/Vh_value", ui->lineEdit_Vh->text());
    qr49settings.setValue("/standard_index", ui->comboBox_standard->currentIndex());
    qr49settings.setValue("/FuelType_index", ui->comboBox_FuelType->currentIndex());
    qr49settings.setValue("/NOxSample_index", ui->comboBox_NOxSample->currentIndex());
    qr49settings.setValue("/PTcalc_index", ui->comboBox_PTcalc->currentIndex());
    qr49settings.setValue("/PTmass_value", ui->lineEdit_PTmass->text());
    qr49settings.setValue("/AddPointsCalc_index", ui->comboBox_AddPointsCalc->currentIndex());
    qr49settings.setValue("/CreateReports", ui->checkBox_reports->isChecked());
    qr49settings.setValue("/lastReportsDir", lastReportsDir.absolutePath());
    qr49settings.setValue("/lastCheckoutDataFileName", lastCheckoutDataFileName);
    qr49settings.setValue("/lastReportFileName", lastReportFileName);
    qr49settings.endGroup();
}

void MainWindow::readProgramSettings() {

    qr49settings.beginGroup("/Settings");
    setGeometry(qr49settings.value("/window_geometry", QRect(20, 40, 0, 0)).toRect());
    restoreState(qr49settings.value("/toolbars_state").toByteArray());
    ui->action_Toolbar->setChecked(qr49settings.value("/action_toolbar_checked", true).toBool());
    ui->tabWidget_Data->setCurrentIndex(qr49settings.value("active_tab", ui->tabWidget_Data->currentIndex()).toInt());
    ui->comboBox_task->setCurrentIndex(qr49settings.value("/task_index", ui->comboBox_task->currentIndex()).toInt());
    ui->lineEdit_Vh->setText(qr49settings.value("/Vh_value", ui->lineEdit_Vh->text()).toString());
    ui->comboBox_standard->setCurrentIndex(qr49settings.value("/standard_index", ui->comboBox_standard->currentIndex()).toInt());
    ui->comboBox_FuelType->setCurrentIndex(qr49settings.value("/FuelType_index", ui->comboBox_FuelType->currentIndex()).toInt());
    ui->comboBox_NOxSample->setCurrentIndex(qr49settings.value("/NOxSample_index", ui->comboBox_NOxSample->currentIndex()).toInt());
    ui->comboBox_PTcalc->setCurrentIndex(qr49settings.value("/PTcalc_index", ui->comboBox_PTcalc->currentIndex()).toInt());
    ui->lineEdit_PTmass->setText(qr49settings.value("/PTmass_value", ui->lineEdit_PTmass->text()).toString());
    ui->comboBox_AddPointsCalc->setCurrentIndex(qr49settings.value("/AddPointsCalc_index", ui->comboBox_AddPointsCalc->currentIndex()).toInt());
    ui->checkBox_reports->setChecked(qr49settings.value("/CreateReports", ui->checkBox_reports->isChecked()).toBool());
    lastReportsDir.setPath(qr49settings.value("/lastReportsDir", "").toString());
    lastCheckoutDataFileName = qr49settings.value("/lastCheckoutDataFileName", "").toString();
    lastReportFileName = qr49settings.value("/lastReportFileName", "").toString();
    qr49settings.endGroup();

    if ( ui->action_Toolbar->isChecked() ) {

        ui->toolBar->setVisible(true);
    }
    else {

        ui->toolBar->setVisible(false);
    }
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

        table = (QTableWidget*)object;
    }

    return false;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *cme) {

    contextMenu->exec(cme->globalPos());
}

void MainWindow::setDoubleValidators() {

    ui->lineEdit_Vh->setValidator(doubleValidator);
    ui->lineEdit_PTmass->setValidator(doubleValidator);
}

void MainWindow::readPreferences() {

    try {

        config->readConfigFile(CONFIGFILENAME);
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
        return;
    }
}

void MainWindow::loadAllSourceData() {

    const QString filenameSourceEU0 = config->valFileNameSourceEU0();

    if ( QFile::exists(filenameSourceEU0) ) {

        tableCellChangedConnect(false);

        if ( !fillTableEU0(filenameSourceEU0) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        tr("Can not fill table!"),
                        0, 0, 0
                        );
        }

        tableCellChangedConnect(true);
    }

    //

    const QString filenameSourceEU3 = config->valFileNameSourceEU3();

    if ( QFile::exists(filenameSourceEU3) ) {

        tableCellChangedConnect(false);

        if ( !fillTableEU3(filenameSourceEU3) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        tr("Can not fill table!"),
                        0, 0, 0
                        );
        }

        tableCellChangedConnect(true);
    }

    //

    const QString filenamePoints = config->valFileNamePoints();

    if ( QFile::exists(filenamePoints) ) {

        tableCellChangedConnect(false);

        if ( !fillTablePoints(filenamePoints) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        tr("Can not fill table!"),
                        0, 0, 0
                        );
        }

        tableCellChangedConnect(true);
    }

    //

    const QString filenamePowers = config->valFileNamePowers();

    if ( QFile::exists(filenamePowers) ) {

        tableCellChangedConnect(false);

        if ( !fillTableFullLoadCurve(filenamePowers) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        tr("Can not fill table!"),
                        0, 0, 0
                        );
        }

        tableCellChangedConnect(true);
    }
}

bool MainWindow::fillTableEU0(const QString &filename) {

    ui->tableWidget_SrcDataEU0->setRowCount(1);
    ui->tableWidget_SrcDataEU0->setRowHeight(0, tableRowHeight);

    QSharedPointer<csvRead> readerSourceDataEU0(
                new csvRead(filename, " ", STRSNUMBERFORCOLUMNCAPTION)
                );

    try{

        readerSourceDataEU0->readFile();
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
        return false;
    }

    QVector< QVector<double> > arraySourceDataEU0 =
            readerSourceDataEU0->csvData();

    if ( arraySourceDataEU0.at(0).size() != EU0SRCDATAPARAMSNUMBER ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Incorrect source data! Check number of points "
                       "and calculation parameters."),
                    0, 0, 0
                    );
        return false;
    }

    for ( ptrdiff_t j=0; j<arraySourceDataEU0.at(0).size(); j++ ) {

        ui->tableWidget_SrcDataEU0->
                setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU0.at(0).at(j), 'f', 3)));
        ui->tableWidget_SrcDataEU0->
                item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    return true;
}

bool MainWindow::fillTableEU3(const QString &filename) {

    ui->tableWidget_SrcDataEU3->setRowCount(1);
    ui->tableWidget_SrcDataEU3->setRowHeight(0, tableRowHeight);

    QSharedPointer<csvRead> readerSourceDataEU3(
                new csvRead(filename, " ", STRSNUMBERFORCOLUMNCAPTION)
                );

    try{

        readerSourceDataEU3->readFile();
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
        return false;
    }

    QVector< QVector<double> > arraySourceDataEU3 = readerSourceDataEU3->csvData();

    if ( arraySourceDataEU3.at(0).size() != EU3SRCDATAPARAMSNUMBER ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Incorrect source data! Check number of points and "
                       "calculation parameters."),
                    0, 0, 0
                    );
        return false;
    }

    for ( ptrdiff_t j=0; j<arraySourceDataEU3.at(0).size(); j++ ) {

        ui->tableWidget_SrcDataEU3->
                setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU3.at(0).at(j), 'f', 3)));
        ui->tableWidget_SrcDataEU3->
                item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    return true;
}

bool MainWindow::fillTablePoints(const QString &filename) {

    QSharedPointer<csvRead>
            readerSourceDataPoints(new csvRead(filename, " ", STRSNUMBERFORCOLUMNCAPTION));

    try{

        readerSourceDataPoints->readFile();
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
        return false;
    }

    QVector< QVector<double> > arraySourceDataPoints =
            readerSourceDataPoints->csvData();

    if ( arraySourceDataPoints.isEmpty() ) {

        return false;
    }

    if ( arraySourceDataPoints.at(0).size() != POINTSFILECOLUMNSNUMBER ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Incorrect source data! Check number of points and "
                       "calculation parameters."),
                    0, 0, 0
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

        for ( ptrdiff_t j=2; j<arraySourceDataPoints.at(i).size(); j++ ) {

            ui->tableWidget_SrcDataPoints->
                    setItem(i, j, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][j], 'f', 3)));
            ui->tableWidget_SrcDataPoints->
                    item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return true;
}

bool MainWindow::fillTableFullLoadCurve(const QString &filename) {

    QSharedPointer<csvRead>
            readerFullLoadCurve(new csvRead(filename, " ", STRSNUMBERFORCOLUMNCAPTION));

    try{

        readerFullLoadCurve->readFile();
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
        return false;
    }

    QVector< QVector<double> > arrayFullLoadCurve =
            readerFullLoadCurve->csvData();

    if ( arrayFullLoadCurve.isEmpty() ) {

        return false;
    }

    if ( arrayFullLoadCurve.at(0).size() != POWERSFILECOLUMNSNUMBER ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Incorrect source data! Check number of points and "
                       "calculation parameters."),
                    0, 0, 0
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

        for ( ptrdiff_t j=2; j<arrayFullLoadCurve.at(i).size(); j++ ) {

            ui->tableWidget_FullLoadCurve->
                    setItem(i, j, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][j], 'f', 3)));
            ui->tableWidget_FullLoadCurve->
                    item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return true;
}

bool MainWindow::fillParameters() {

    if ( ui->comboBox_task->currentIndex() == TASK_EMISSIONS &&
         ui->comboBox_PTcalc->currentIndex() == PTCALC_THROUGHPTMASS &&
         ui->lineEdit_PTmass->text().toDouble() == 0 ) {

        on_pushButton_EnterPTmass_clicked();
    }

    params->setTask(ui->comboBox_task->currentIndex());
    params->setVh(ui->lineEdit_Vh->text().toDouble());
    params->setStandard(ui->comboBox_standard->currentIndex());
    params->setChargingType(ui->comboBox_chargingType->currentIndex());
    params->setFuelType(ui->comboBox_FuelType->currentIndex());
    params->setNOxSample(ui->comboBox_NOxSample->currentIndex());
    params->setPTcalc(ui->comboBox_PTcalc->currentIndex());
    params->setPTmass(ui->lineEdit_PTmass->text().toDouble());
    params->setAddPointsCalc(ui->comboBox_AddPointsCalc->currentIndex());

    return true;
}

bool MainWindow::arithmeticOperation(const QString &operation) {

    QLineEdit *value = valueDialog->findChild<QLineEdit *>("lineEdit_Value");

    if ( !value ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    QString::fromAscii(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child object not found!"),
                    0, 0, 0
                    );
        return false;
    }

    value->setFocus();
    value->selectAll();

    if ( operation == "add" ) {

        valueDialog->setWindowTitle(tr("Qr49: add"));
    }
    else if ( operation == "multiply" ) {

        valueDialog->setWindowTitle(tr("Qr49: multiply"));
    }
    else if ( operation == "divide" ) {

        valueDialog->setWindowTitle(tr("Qr49: divide"));
    }
    else if ( operation == "equal" ) {

        valueDialog->setWindowTitle(tr("Qr49: equal"));
    }
    else {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Unknown arithmetic operation!"),
                    0, 0, 0
                    );
        return false;
    }

    if ( valueDialog->exec() == QDialog::Accepted ) {

        if ( table->selectedRanges().empty() ) {

            QMessageBox::critical(this, "Qr49", tr("No selected cells!"), 0, 0, 0);

            return false;
        }

        QTableWidgetSelectionRange selectedRange = table->selectedRanges().first();

        double x = 0.0;
        double y = value->text().toDouble();

        if ( (y == 0.0) && (operation == "divide") ) {

            QMessageBox::warning(
                        this,
                        "Qr49",
                        tr("Illegal operation \"Divide by zero\"!"),
                        0, 0, 0
                        );

            return false;
        }

        tableCellChangedConnect(false);

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                x = table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                        text().toDouble();

                if ( operation == "add" ) {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(x+y, 'f', 3));
                }
                else if ( operation == "multiply" ) {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(x*y, 'f', 3));
                }
                else if ( operation == "divide" ) {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(x/y, 'f', 3));
                }
                else if ( operation == "equal" ) {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
                            setText(QString::number(y, 'f', 3));
                }
            }
        }

        tableCellChangedConnect(true);
    }

    saveTableState();

    return true;
}

void MainWindow::on_action_DataImport_activated() {

    if ( table == ui->tableWidget_SrcDataPoints ) {

        dataImportDialog->init(2, table);
    }
    else if ( table == ui->tableWidget_FullLoadCurve ) {

        dataImportDialog->init(3, table);
    }
    else {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Data import is not available for the current table!"),
                    0, 0, 0
                    );
        return;
    }

    tableCellChangedConnect(false);

    if ( dataImportDialog->exec() == QDialog::Accepted ) {

        tableCellChangedConnect(true);
        saveTableState();
    }
    else {

        tableCellChangedConnect(true);
    }
}

void MainWindow::on_action_LoadSourceData_activated() {

    const QString dir(config->valDirNameReports());

    const QString anotherSourceFile(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open Source Data File..."),
                    dir,
                    QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
                    0,
                    0)
                );

    if ( table == ui->tableWidget_SrcDataEU0 ) {

        ui->tabWidget_Data->setCurrentIndex(0);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTableEU0(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            "Qr49",
                            tr("Can not fill table!"),
                            0, 0, 0
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
    else if ( table == ui->tableWidget_SrcDataEU3 ) {

        ui->tabWidget_Data->setCurrentIndex(0);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTableEU3(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            "Qr49",
                            tr("Can not fill table!"),
                            0, 0, 0
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
    else if ( table == ui->tableWidget_SrcDataPoints ) {

        ui->tabWidget_Data->setCurrentIndex(1);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTablePoints(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            "Qr49",
                            tr("Can not fill table!"),
                            0, 0, 0
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
    else if ( table == ui->tableWidget_FullLoadCurve ) {

        ui->tabWidget_Data->setCurrentIndex(2);

        tableCellChangedConnect(false);

        if ( !anotherSourceFile.isEmpty() ) {

            if ( !fillTableFullLoadCurve(anotherSourceFile) ) {

                QMessageBox::critical(
                            this,
                            "Qr49",
                            tr("Can not fill table!"),
                            0, 0, 0
                            );
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
}

void MainWindow::on_action_SaveSourceData_activated() {

    if ( table == ui->tableWidget_SrcDataEU0 ) {

        const QString filenameSourceEU0 = config->valFileNameSourceEU0();

        QFile SrcDataEU0File(filenameSourceEU0);

        if ( !SrcDataEU0File.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        filenameSourceEU0
                        + tr(" could not be opened!"),
                        0, 0, 0
                        );
            return;
        }

        SrcDataEU0File.write("idle[min-1] n_interim[min-1] n_rated[min-1] "
                             "N_fan_rated[kW] Ne_interim[Nm] Ne_rated[Nm]\n");

        for ( ptrdiff_t j=0; j<EU0SRCDATAPARAMSNUMBER; j++ ) {

            SrcDataEU0File.write(table->item(0, j)->text().toAscii().data());
            SrcDataEU0File.write(" ");
        }

        SrcDataEU0File.write("\n");

        SrcDataEU0File.close();
    }
    else if ( table == ui->tableWidget_SrcDataEU3 ) {

        const QString filenameSourceEU3 = config->valFileNameSourceEU3();

        QFile SrcDataEU3File(filenameSourceEU3);

        if ( !SrcDataEU3File.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        filenameSourceEU3
                        + tr(" could not be opened!"),
                        0, 0, 0
                        );
            return;
        }

        SrcDataEU3File.write("n_hi[min-1] n_lo[min-1] idle[min-1] "
                             "n_rated[min-1] N_fan_rated[kW] Ne_A[Nm] "
                             "Ne_B[Nm] Ne_C[Nm] Ne_a1[Nm] Ne_a2[Nm] "
                             "Ne_a3[Nm]\n");

        for ( ptrdiff_t j=0; j<EU3SRCDATAPARAMSNUMBER; j++ ) {

            SrcDataEU3File.write(table->item(0, j)->text().toAscii().data());
            SrcDataEU3File.write(" ");
        }

        SrcDataEU3File.write("\n");

        SrcDataEU3File.close();
    }
    else if ( table == ui->tableWidget_SrcDataPoints ) {

        if ( table->rowCount() == 0 ) {

            on_action_AddRow_activated();
        }

        const QString filenamePoints = config->valFileNamePoints();

        QFile SrcDataPointsFile(filenamePoints);

        if ( !SrcDataPointsFile.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        filenamePoints
                        + tr(" could not be opened!"),
                        0, 0, 0
                        );
            return;
        }

        SrcDataPointsFile.write("Point[-] n[min-1] Me_b[Nm] Ne_b[kW] "
                                "N_fan[kW] w[-] t0[oC] B0[kPa] Ra[%] "
                                "dPn[mmH2O] Gair[kg/h] Gfuel[kg/h] C_NOx[ppm] "
                                "gNOx[g/kWh] C_CO[ppm] C_CH[ppm] C_CO2in[%] "
                                "C_CO2out[%] C_O2[%] Ka[m-1] Ka[%] FSN[-] "
                                "Pr[kPa] ts[oC] tauf[s] qmdw[g/s] qmdew[g/s] "
                                "rd[-]\n");

        for ( ptrdiff_t i=0; i<table->rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<POINTSFILECOLUMNSNUMBER; j++ ) {

                SrcDataPointsFile.
                        write(table->item(i, j)->text().toAscii().data());
                SrcDataPointsFile.
                        write(" ");
            }

            SrcDataPointsFile.write("\n");
        }

        SrcDataPointsFile.close();
    }
    else if ( table == ui->tableWidget_FullLoadCurve ) {

        if ( table->rowCount() == 0 ) {

            on_action_AddRow_activated();
        }

        const QString filenamePowers = config->valFileNamePowers();

        QFile SrcDataPowersFile(filenamePowers);

        if ( !SrcDataPowersFile.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        filenamePowers
                        + tr(" could not be opened!"),
                        0, 0, 0
                        );
            return;
        }

        SrcDataPowersFile.write("Point[-] n[min-1] Me_b[Nm] t0[oC] B0[kPa] "
                                "Ra[%] S[kPa] pk[kPa] Gfuel[kg/h] N_k[kW] "
                                "N_fan[kW]\n");

        for ( ptrdiff_t i=0; i<table->rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<POWERSFILECOLUMNSNUMBER; j++ ) {

                SrcDataPowersFile.
                        write(table->item(i, j)->text().toAscii().data());
                SrcDataPowersFile.
                        write(" ");
            }

            SrcDataPowersFile.write("\n");
        }

        SrcDataPowersFile.close();
    }
}

void MainWindow::on_action_SaveSourceDataAs_activated() {

    const QString newSourceDataFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save Source Data File As..."),
                    "noname.csv",
                    QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !newSourceDataFileName.isEmpty() ) {

        QFile SrcDataFile(newSourceDataFileName);

        if ( !SrcDataFile.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        newSourceDataFileName
                        + tr(" could not be opened!"),
                        0, 0, 0
                        );
            return;
        }

        if ( table == ui->tableWidget_SrcDataEU0 ) {

            SrcDataFile.write("idle[min-1] n_interim[min-1] n_rated[min-1] "
                              "N_fan_rated[kW] Ne_interim[Nm] Ne_rated[Nm]\n");
        }
        else if ( table == ui->tableWidget_SrcDataEU3 ) {

            SrcDataFile.write("n_hi[min-1] n_lo[min-1] idle[min-1] "
                              "n_rated[min-1] N_fan_rated[kW] Ne_A[Nm] "
                              "Ne_B[Nm] Ne_C[Nm] Ne_a1[Nm] Ne_a2[Nm] "
                              "Ne_a3[Nm]\n");
        }
        else if ( table == ui->tableWidget_SrcDataPoints ) {

            SrcDataFile.write("Point[-] n[min-1] Me_b[Nm] Ne_b[kW] N_fan[kW] "
                              "w[-] t0[oC] B0[kPa] Ra[%] dPn[mmH2O] Gair[kg/h] "
                              "Gfuel[kg/h] C_NOx[ppm] gNOx[g/kWh] C_CO[ppm] "
                              "C_CH[ppm] C_CO2in[%] C_CO2out[%] C_O2[%] "
                              "Ka[m-1] Ka[%] FSN[-] Pr[kPa] ts[oC] tauf[s] "
                              "qmdw[g/s] qmdew[g/s] rd[-]\n");
        }
        else if ( table == ui->tableWidget_FullLoadCurve ) {

            SrcDataFile.write("Point[-] n[min-1] Me_b[Nm] t0[oC] B0[kPa] "
                              "Ra[%] S[kPa] pk[kPa] Gfuel[kg/h] N_k[kW] "
                              "N_fan[kW]\n");
        }

        for ( ptrdiff_t i=0; i<table->rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<table->columnCount(); j++ ) {

                SrcDataFile.write(table->item(i, j)->text().toAscii().data());
                SrcDataFile.write(" ");
            }

            SrcDataFile.write("\n");
        }

        SrcDataFile.close();
    }
}

void MainWindow::on_action_LoadCalculationOptions_activated() {

    const QString dir(config->valDirNameReports());

    const QString anotherOptions(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open Calculation Options File..."),
                    dir,
                    QString::fromAscii("Config files (*.conf);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !anotherOptions.isEmpty() ) {

        try {

            params->readCalcConfigFile(anotherOptions);
            params->setCalcConfigFile("_._");
        }
        catch(const ToxicError &toxerr) {

            QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
            return;
        }

        ui->comboBox_task->setCurrentIndex(params->valTask());
        ui->lineEdit_Vh->setText(QString::number(params->valVh()));
        ui->comboBox_standard->setCurrentIndex(params->valStandard());
        ui->comboBox_chargingType->setCurrentIndex(params->valChargingType());
        ui->comboBox_FuelType->setCurrentIndex(params->valFuelType());
        ui->comboBox_NOxSample->setCurrentIndex(params->valNOxSample());
        ui->comboBox_PTcalc->setCurrentIndex(params->valPTcalc());
        ui->lineEdit_PTmass->setText(QString::number(params->valPTmass()));
        ui->comboBox_AddPointsCalc->setCurrentIndex(params->valAddPointsCalc());

        taskChanged(ui->comboBox_task->currentIndex());
        standardChanged(ui->comboBox_standard->currentIndex());
        PTcalcChanged(ui->comboBox_PTcalc->currentIndex());
    }
}

void MainWindow::on_action_SaveCalculationOptionsAs_activated() {

    const QString optionsFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save Options..."),
                    "noname.conf",
                    QString::fromAscii("Config files (*.conf);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !optionsFileName.isEmpty() ) {

        QFile savedOptions(optionsFileName);

        if ( !savedOptions.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        optionsFileName
                        + tr(" could not be opened!"),
                        0, 0, 0
                        );
            return;
        }

        QTextStream fout(&savedOptions);

        fout << "task"           << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_task->currentIndex())          << "\n"
             << "Vh"             << PARAMETERVALUEDELIMITER
             << ui->lineEdit_Vh->text()                                     << "\n"
             << "standard"       << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_standard->currentIndex())      << "\n"
             << "ChargingType"   << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_chargingType->currentIndex())  << "\n"
             << "FuelType"       << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_FuelType->currentIndex())      << "\n"
             << "NOxSample"      << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_NOxSample->currentIndex())     << "\n"
             << "PTcalc"         << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_PTcalc->currentIndex())        << "\n"
             << "PTmass"         << PARAMETERVALUEDELIMITER
             << ui->lineEdit_PTmass->text()                                 << "\n"
             << "AddPointsCalc"  << PARAMETERVALUEDELIMITER
             << QString::number(ui->comboBox_AddPointsCalc->currentIndex()) << "\n"
             << "CalcConfigFile" << PARAMETERVALUEDELIMITER
             << params->valCalcConfigFile()                         << "\n";

        savedOptions.close();
    }
}

void MainWindow::on_action_OpenReport_activated() {

    const QString dir(config->valDirNameReports());

    const QString anotherReport(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Open Report..."),
                    dir,
                    QString::fromAscii("Text files (*.txt);;All files (*.*)"),
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

void MainWindow::on_action_SaveReportAs_activated() {

    const QString newReportFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save Report As..."),
                    ui->comboBox_OpenedReports->currentText(),
                    QString::fromAscii("Text files (*.txt);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !newReportFileName.isEmpty() ) {

        QFile reportFile(newReportFileName);

        if ( !reportFile.open(QIODevice::WriteOnly) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        newReportFileName
                        + tr(" could not be saved!"),
                        0, 0, 0
                        );
            return;
        }

        QTextStream fout(&reportFile);
        fout << ui->plainTextEdit_Report->toPlainText();

        reportFile.close();

        ui->comboBox_OpenedReports->
                removeItem(ui->comboBox_OpenedReports->currentIndex());
        ui->comboBox_OpenedReports->insertItem(0, newReportFileName);
        ui->comboBox_OpenedReports->setCurrentIndex(0);
        reportChanged(newReportFileName);
        ui->tabWidget_Data->setCurrentIndex(3);
    }
}

void MainWindow::on_action_CloseReport_activated() {

    if ( ui->comboBox_OpenedReports->count() != 1 ) {

        ui->comboBox_OpenedReports->
                removeItem(ui->comboBox_OpenedReports->currentIndex());
    }

    reportChanged(ui->comboBox_OpenedReports->currentText());
    ui->tabWidget_Data->setCurrentIndex(3);
}

void MainWindow::on_action_ReportToPDF_activated() {

    const QString filename = ui->comboBox_OpenedReports->currentText() + ".pdf";

    const QString newReportFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Export Report to PDF..."),
                    filename,
                    QString::fromAscii("PDF files (*.pdf);;All files (*.*)"),
                    0,
                    0)
                );

    QPrinter printer;

    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(20, 20, 10, 10, QPrinter::Millimeter);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(newReportFileName);
    printer.setFontEmbeddingEnabled(true);

    ui->plainTextEdit_Report->setFont(monospacedFont_8);
    ui->plainTextEdit_Report->print(&printer);
    ui->plainTextEdit_Report->setFont(monospacedFont_10);
}

void MainWindow::on_action_PrintReport_activated() {

    QPrinter printer;

    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(20, 20, 10, 10, QPrinter::Millimeter);
    //	printer.setPrintRange(QPrinter::PageRange);
    //	printer.setFromTo(1, 1);

    QPrintDialog printDialog(&printer, this);

    if ( printDialog.exec() == QDialog::Accepted ) {

        ui->plainTextEdit_Report->setFont(monospacedFont_8);
        ui->plainTextEdit_Report->print(&printer);
        ui->plainTextEdit_Report->setFont(monospacedFont_10);
    }
}

void MainWindow::on_action_PrintSelectedCells_activated() {

    if ( table->selectedRanges().isEmpty() ) {

        QMessageBox::warning(
                    this,
                    "Qr49",
                    tr("No selected cells to printing!"),
                    0, 0, 0
                    );
        return;
    }

    //

    QTableWidgetSelectionRange selectedRange = table->selectedRanges().first();

    ptrdiff_t colnum = selectedRange.columnCount();

    if ( colnum > 7 ) {

        colnum = 7;
        QMessageBox::information(
                    this,
                    "Qr49",
                    tr("Only 7 columns can be printed at a time!"),
                    0, 0, 0
                    );
    }

    QString str;
    QTextStream pout(&str);

    pout << right << qSetFieldWidth(WIDTHOFCOLUMN+3);

    for ( ptrdiff_t j=0; j<colnum; j++ ) {

        pout << table->horizontalHeaderItem(selectedRange.leftColumn()+j)->
                text().split("\n").join(" ");
    }

    pout << "\n";

    for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<colnum; j++ ) {

            pout << table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->
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
        pte.setFont(dejavusansmonoFont_10);
        pte.insertPlainText(str);
        pte.print(&printer);
    }
}

void MainWindow::on_action_Preferences_activated() {

    QLineEdit *myLineEdit_filenameSourceEU3 =
            preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU3");
    QLineEdit *myLineEdit_filenameSourceEU0 =
            preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU0");
    QLineEdit *myLineEdit_filenamePoints =
            preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePoints");
    QLineEdit *myLineEdit_filenamePowers =
            preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePowers");
    QLineEdit *myLineEdit_dirnameReports =
            preferencesDialog->findChild<QLineEdit *>("lineEdit_dirnameReports");
    QDoubleSpinBox *myDoubleSpinBox_Dn =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Dn");
    QDoubleSpinBox *myDoubleSpinBox_ConcO2air =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcO2air");
    QDoubleSpinBox *myDoubleSpinBox_Rr =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Rr");
    QDoubleSpinBox *myDoubleSpinBox_L0 =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L0");
    QDoubleSpinBox *myDoubleSpinBox_L =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L");
    QDoubleSpinBox *myDoubleSpinBox_ConcCO2air =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcCO2air");
    QDoubleSpinBox *myDoubleSpinBox_WH =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WH");
    QDoubleSpinBox *myDoubleSpinBox_WO2 =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WO2");
    QDoubleSpinBox *myDoubleSpinBox_WN =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WN");
    QDoubleSpinBox *myDoubleSpinBox_muNO2 =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muNO2");
    QDoubleSpinBox *myDoubleSpinBox_muCO =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCO");
    QDoubleSpinBox *myDoubleSpinBox_muCH =
            preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCH");

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
                    "Qr49",
                    QString::fromAscii(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child object not found!"),
                    0, 0, 0
                    );
        return;
    }

    //

    myLineEdit_filenameSourceEU3->setText(config->valFileNameSourceEU3());
    myLineEdit_filenameSourceEU0->setText(config->valFileNameSourceEU0());
    myLineEdit_filenamePoints->setText(config->valFileNamePoints());
    myLineEdit_filenamePowers->setText(config->valFileNamePowers());
    myLineEdit_dirnameReports->setText(config->valDirNameReports());
    myDoubleSpinBox_Dn->setValue(config->valDn());
    myDoubleSpinBox_ConcO2air->setValue(config->valConcO2air());
    myDoubleSpinBox_Rr->setValue(config->valRr());
    myDoubleSpinBox_L0->setValue(config->valL0());
    myDoubleSpinBox_L->setValue(config->valL());
    myDoubleSpinBox_ConcCO2air->setValue(config->valConcCO2air());
    myDoubleSpinBox_WH->setValue(config->valWH());
    myDoubleSpinBox_WO2->setValue(config->valWO2());
    myDoubleSpinBox_WN->setValue(config->valWN());
    myDoubleSpinBox_muNO2->setValue(config->valmuNO2());
    myDoubleSpinBox_muCO->setValue(config->valmuCO());
    myDoubleSpinBox_muCH->setValue(config->valmuCH());

    //

    if ( preferencesDialog->exec() == QDialog::Accepted ) {

        readPreferences();
    }
}

void MainWindow::on_action_Quit_activated() {

    close();
}

void MainWindow::on_action_CutFromTable_activated() {

    on_action_CopyFromTable_activated();
    on_action_DeleteFromTable_activated();
}

void MainWindow::on_action_CopyFromTable_activated() {

    if ( !table->selectedRanges().isEmpty() ) {

        QTableWidgetSelectionRange selectedRange =
                table->selectedRanges().first();

        QString str;

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                str += table->item(selectedRange.topRow()+i,
                                   selectedRange.leftColumn()+j)->text();

                if ( j != (selectedRange.columnCount()-1) ) {

                    str += "\t";
                }
            }

            str += "\n";
        }

        QApplication::clipboard()->setText(str);
    }
}

void MainWindow::on_action_PasteToTable_activated() {

    const QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');

    const ptrdiff_t numRows = rows.count() - 1;
    const ptrdiff_t numColumns = rows.first().count('\t') + 1;

    if ( (table->columnCount() - table->currentColumn()) < numColumns ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Copied data can not be inserted!"),
                    0, 0, 0
                    );
        return;
    }

    //

    const ptrdiff_t destRows = table->rowCount() - table->currentRow();
    const ptrdiff_t totalRows = table->currentRow() + numRows;

    tableCellChangedConnect(false);

    if ( numRows > destRows ) {

        addRows(table, totalRows);
    }

    //

    for ( ptrdiff_t i=0; i<numRows; i++ ) {

        QStringList columns = rows[i].split('\t');

        for ( ptrdiff_t j=0; j<numColumns; j++ ) {

            table->item(table->currentRow()+i,
                        table->currentColumn()+j)->setText(columns[j]);
        }
    }

    tableCellChangedConnect(true);

    saveTableState();
}

void MainWindow::on_action_DeleteFromTable_activated() {

    if ( !table->selectedRanges().isEmpty() ) {

        QTableWidgetSelectionRange selectedRange =
                table->selectedRanges().first();

        tableCellChangedConnect(false);

        for ( ptrdiff_t i=0; i<selectedRange.rowCount(); i++ ) {

            for ( ptrdiff_t j=0; j<selectedRange.columnCount(); j++ ) {

                table->item(selectedRange.topRow()+i,
                            selectedRange.leftColumn()+j)->setText("0");
            }
        }

        tableCellChangedConnect(true);

        saveTableState();
    }
}

void MainWindow::on_action_Add_activated() {

    if ( !arithmeticOperation("add") ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Arithmetic operation is impossible!"),
                    0, 0, 0
                    );
    }
}

void MainWindow::on_action_Multiply_activated() {

    if ( !arithmeticOperation("multiply") ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Arithmetic operation is impossible!"),
                    0, 0, 0
                    );
    }
}

void MainWindow::on_action_Divide_activated() {

    if ( !arithmeticOperation("divide") ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Arithmetic operation is impossible!"),
                    0, 0, 0
                    );
    }
}

void MainWindow::on_action_Equal_activated() {

    if ( !arithmeticOperation("equal") ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Operation is impossible!"),
                    0, 0, 0
                    );
    }
}

void MainWindow::on_action_AddRow_activated() {

    tableCellChangedConnect(false);

    if ( (table != ui->tableWidget_SrcDataEU0) &&
         (table != ui->tableWidget_SrcDataEU3) ) {

        addRows(table, table->rowCount()+1);
    }

    tableCellChangedConnect(true);

    saveTableState();
}

void MainWindow::on_action_DeleteRow_activated() {

    if ( (table != ui->tableWidget_SrcDataEU0) &&
         (table != ui->tableWidget_SrcDataEU3) ) {

        table->setRowCount(table->rowCount()-1);
    }

    saveTableState();
}

void MainWindow::on_action_Toolbar_activated() {

    if ( ui->action_Toolbar->isChecked() ) {

        ui->toolBar->setVisible(true);
    }
    else {

        ui->toolBar->setVisible(false);
    }
}

void MainWindow::on_action_Execute_activated() {

    QVector< QVector<double> > array_DataForCalc;
    QVector<double> row;

    for ( ptrdiff_t i=0; i<table->rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<table->columnCount(); j++ ) {

            row.push_back( table->item(i, j)->text().toDouble() );
        }

        array_DataForCalc.push_back(row);
        row.clear();
    }

    //

    QString message("\nCalculation completed!\n\n");

    fillParameters();

    if ( ui->comboBox_task->currentIndex() == TASK_POINTS ) {

        try {

            QSharedPointer<CyclePoints>
                    myPoints(new CyclePoints(params, config));

            myPoints->readCSV(array_DataForCalc);
            myPoints->fillArrays();

            message += myPoints->createReport();
        }
        catch(const ToxicError &toxerr) {

            QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
            return;
        }

        //

        const QString filenamePoints = config->valFileNamePoints();

        if ( QFile::exists(filenamePoints) ) {

            tableCellChangedConnect(false);

            if ( !fillTablePoints(filenamePoints) ) {

                QMessageBox::critical(
                            this,
                            "Qr49",
                            tr("Can not fill table!"),
                            0, 0, 0
                            );
            }

            tableCellChangedConnect(true);

            //

            table = ui->tableWidget_SrcDataPoints;
            saveTableState();
        }
        else {

            QMessageBox::warning(
                        this,
                        "Qr49",
                        filenamePoints
                        + tr(" not found!"),
                        0, 0, 0
                        );
        }

        //

        ui->tabWidget_Data->setCurrentIndex(1);
    }
    else if ( ui->comboBox_task->currentIndex() == TASK_EMISSIONS ) {

        QSharedPointer<CycleEmissions> myEmissions;

        try {

            myEmissions = QSharedPointer<CycleEmissions>
                    (new CycleEmissions(params, config));

            myEmissions->readCSV(array_DataForCalc);
            myEmissions->calculate();
        }
        catch(const ToxicError &toxerr) {

            QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
            return;
        }

        if ( ui->checkBox_reports->isChecked() ) {

            try {

                message += myEmissions->createReports();
            }
            catch(const ToxicError &toxerr) {

                QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
                return;
            }

            //

            lastReportsDir = myEmissions->lastReportsDir();

            const QString csvfilter("*.csv");
            QStringList csvfiles(lastReportsDir.entryList(
                                     QDir::nameFiltersFromString(csvfilter),
                                     QDir::Files,
                                     QDir::Time)
                                 );

            lastCheckoutDataFileName =
                    lastReportsDir.absoluteFilePath(csvfiles.first());

            //

            if ( ui->comboBox_standard->currentIndex() == STD_FREECALC ) {

                ui->tabWidget_Data->setCurrentIndex(1);
            }
            else {

                ui->tabWidget_Data->setCurrentIndex(3);

                const QString txtfilter("*.txt");
                QStringList reports(lastReportsDir.entryList(
                                        QDir::nameFiltersFromString(txtfilter),
                                        QDir::Files,
                                        QDir::Time)
                                    );

                lastReportFileName =
                        lastReportsDir.absoluteFilePath(reports.first());

                ui->comboBox_OpenedReports->insertItem(0, lastReportFileName);
                ui->comboBox_OpenedReports->setCurrentIndex(0);
                reportChanged(lastReportFileName);
            }
        }
        else {

            try {

                message += myEmissions->results();
            }
            catch(const ToxicError &toxerr) {

                QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
                return;
            }
        }
    }
    else if ( ui->comboBox_task->currentIndex() == TASK_REDUCEDPOWER ) {

        QSharedPointer<ReducedPower> myReducedPower;

        try {

            myReducedPower =
                    QSharedPointer<ReducedPower>(new ReducedPower(params, config));

            myReducedPower->readCSV(array_DataForCalc);
            myReducedPower->reducePower();

            message += myReducedPower->createReports();
        }
        catch(const ToxicError &toxerr) {

            QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
            return;
        }

        //

        lastReportsDir = myReducedPower->lastReportsDir();

        const QString csvfilter("*.csv");
        QStringList csvfiles(lastReportsDir.entryList(
                                 QDir::nameFiltersFromString(csvfilter),
                                 QDir::Files,
                                 QDir::Time)
                             );

        lastCheckoutDataFileName =
                lastReportsDir.absoluteFilePath(csvfiles.first());
    }
    else if ( ui->comboBox_task->currentIndex() == TASK_ABCSPEEDS ) {

        ui->toolBox->setCurrentIndex(1);
        return;
    }
    else if ( ui->comboBox_task->currentIndex() == TASK_HELP ) {

        QMessageBox::information(
                    this,
                    "Qr49",
                    tr("See the output of the command line by running the "
                       "Qr49 with parameter task=help."),
                    0, 0, 0
                    );
        return;
    }
    else {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Unknown calculation task!"),
                    0, 0, 0
                    );
        return;
    }

    QMessageBox::information(this, "Qr49", message, 0, 0, 0);

    if ( (ui->comboBox_standard->currentIndex() == STD_FREECALC) ||
         (ui->comboBox_task->currentIndex() == TASK_REDUCEDPOWER) ) {

        on_action_CheckoutData_activated();
    }
}

void MainWindow::on_action_CheckoutData_activated() {

    QPlainTextEdit *myPlainTextEdit_CheckoutData =
            checkoutDataDialog->findChild<QPlainTextEdit *>("plainTextEdit_CheckoutData");
    QLineEdit *myLineEdit_file =
            checkoutDataDialog->findChild<QLineEdit *>("lineEdit_file");

    if ( !myPlainTextEdit_CheckoutData ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    QString::fromAscii(Q_FUNC_INFO)
                    + ":::"
                    + tr("Child object not found!"),
                    0, 0, 0
                    );
        return;
    }

    QFile arrayFile(lastCheckoutDataFileName);
    QString data;

    if ( arrayFile.open(QIODevice::ReadOnly) ) {

        data.append(arrayFile.readAll());
    }
    else {

        QMessageBox::critical(this, "Qr49", tr("Can not open file!"), 0, 0, 0);
        return;
    }

    arrayFile.close();

    myPlainTextEdit_CheckoutData->setPlainText(data);
    myLineEdit_file->setText(lastCheckoutDataFileName);

    checkoutDataDialog->exec();
}

void MainWindow::on_action_StandardsDescription_activated() {

    helpDialog->exec();
}

void MainWindow::on_action_AboutQr49_activated() {

    const QString str = "<b>r49 distribution version "
            + R49VERSION
            + "</b><br><br>"
            + QR49VERSION
            + ", libtoxic v"
            + LIBTOXICVERSION
            + "<br><br>Calculation of modes and specific emissions for "
            "stationary diesel engine test cycles (UN ECE Regulation No. 49, "
            "UN ECE Regulation No. 96, UN ECE Regulation No. 85, "
            "OST 37.001.234-81, GOST 17.2.2.05-97, GOST 30574-98, GOST R "
            "51249-99)."
            "<br><br>Copyright (C) 2009-2012 Artem Petrov "
            "<a href= \"mailto:pa2311@gmail.com\" >pa2311@gmail.com</a>"
            "<br><br>Web site: <a href= \"https://github.com/pa23/r49\">"
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

    QMessageBox::about(this, tr("About Qr49"), str);
}

void MainWindow::on_action_AboutQt_activated() {

    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_CheckForUpdates_activated() {

    newVersions->checkAvailableVersions();
}

void MainWindow::on_pushButton_EnterPTmass_clicked() {

    if ( filterMassDialog->exec() == QDialog::Accepted ) {

        QLineEdit *m1c = filterMassDialog->findChild<QLineEdit *>
                ("lineEdit_1stFilterWeightClean");
        QLineEdit *m1d = filterMassDialog->findChild<QLineEdit *>
                ("lineEdit_1stFilterWeightDirty");
        QLineEdit *m2c = filterMassDialog->findChild<QLineEdit *>
                ("lineEdit_2ndFilterWeightClean");
        QLineEdit *m2d = filterMassDialog->findChild<QLineEdit *>
                ("lineEdit_2ndFilterWeightDirty");

        if ( (!m1c) || (!m1d) || (!m2c) || (!m2d) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        QString::fromAscii(Q_FUNC_INFO)
                        + ":::"
                        + tr("Child object not found!"),
                        0, 0, 0
                        );
            return;
        }

        double PTmass = (m1d->text().toDouble() - m1c->text().toDouble()) +
                (m2d->text().toDouble() - m2c->text().toDouble());

        ui->lineEdit_PTmass->setText(QString::number(PTmass, 'f', 3));
    }
}

void MainWindow::taskChanged(const int currtask) {

    if ( currtask == TASK_POINTS ) {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);
        ui->comboBox_chargingType->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);

        const ptrdiff_t currstd = ui->comboBox_standard->currentIndex();

        if ( (currstd == STD_EU6) ||
             (currstd == STD_EU5) ||
             (currstd == STD_EU4) ||
             (currstd == STD_EU3) ) {

            ui->comboBox_AddPointsCalc->setEnabled(true);
        }
        else {

            ui->comboBox_AddPointsCalc->setEnabled(false);
        }

        ui->checkBox_reports->setEnabled(false);

        ui->tab_cycPointsCalc->setEnabled(true);
        ui->tab_cycEmissCalc->setEnabled(false);
        ui->tab_redPowerCalc->setEnabled(false);
        ui->tab_reports->setEnabled(false);

        if ( currstd == STD_FREECALC ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
        else if ( (currstd == STD_EU6) ||
                  (currstd == STD_EU5) ||
                  (currstd == STD_EU4) ||
                  (currstd == STD_EU3) ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);

            ui->tableWidget_SrcDataEU3->setEnabled(true);
            ui->tableWidget_SrcDataEU3->setFocus();

            getUndoRedoCounters(table);
        }
        else {

            ui->tableWidget_SrcDataEU0->setEnabled(true);
            ui->tableWidget_SrcDataEU0->setFocus();

            getUndoRedoCounters(table);

            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }

        ui->tableWidget_SrcDataPoints->setEnabled(false);
        ui->tableWidget_FullLoadCurve->setEnabled(false);

        ui->action_OpenReport->setEnabled(false);
        ui->action_SaveReportAs->setEnabled(false);
        ui->action_CloseReport->setEnabled(false);
        ui->action_PrintReport->setEnabled(false);
        ui->action_ReportToPDF->setEnabled(false);
        ui->action_AddRow->setEnabled(false);
        ui->action_DeleteRow->setEnabled(false);

        ui->tabWidget_Data->setCurrentIndex(0);
    }
    else if ( currtask == TASK_EMISSIONS ) {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);
        ui->comboBox_chargingType->setEnabled(true);

        const ptrdiff_t currstd = ui->comboBox_standard->currentIndex();

        if ( (currstd == STD_C1) ||
             (currstd == STD_D1) ||
             (currstd == STD_D2) ||
             (currstd == STD_E1) ||
             (currstd == STD_E2) ||
             (currstd == STD_E3) ||
             (currstd == STD_E5) ||
             (currstd == STD_F ) ||
             (currstd == STD_G1) ||
             (currstd == STD_G2) ) {

            ui->comboBox_FuelType->setEnabled(true);
        }
        else {

            ui->comboBox_FuelType->setEnabled(false);
        }

        ui->comboBox_NOxSample->setEnabled(true);
        ui->comboBox_PTcalc->setEnabled(true);

        if ( ui->comboBox_PTcalc->currentIndex() == PTCALC_THROUGHPTMASS ) {

            ui->lineEdit_PTmass->setEnabled(true);
            ui->pushButton_EnterPTmass->setEnabled(true);
        }
        else {

            ui->lineEdit_PTmass->setEnabled(false);
            ui->pushButton_EnterPTmass->setEnabled(false);
        }

        ui->comboBox_AddPointsCalc->setEnabled(true);

        ui->checkBox_reports->setEnabled(true);

        ui->tab_cycPointsCalc->setEnabled(false);
        ui->tab_cycEmissCalc->setEnabled(true);
        ui->tab_redPowerCalc->setEnabled(false);
        ui->tab_reports->setEnabled(true);

        ui->tableWidget_SrcDataEU0->setEnabled(false);
        ui->tableWidget_SrcDataEU3->setEnabled(false);

        ui->tableWidget_SrcDataPoints->setEnabled(true);
        ui->tableWidget_SrcDataPoints->setFocus();

        getUndoRedoCounters(table);

        ui->tableWidget_FullLoadCurve->setEnabled(false);

        ui->action_OpenReport->setEnabled(true);
        ui->action_SaveReportAs->setEnabled(true);
        ui->action_CloseReport->setEnabled(true);
        ui->action_PrintReport->setEnabled(true);
        ui->action_ReportToPDF->setEnabled(true);
        ui->action_AddRow->setEnabled(true);
        ui->action_DeleteRow->setEnabled(true);

        ui->tabWidget_Data->setCurrentIndex(1);
    }
    else if ( currtask == TASK_REDUCEDPOWER ) {

        ui->lineEdit_Vh->setEnabled(true);
        ui->comboBox_standard->setEnabled(false);
        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);

        ui->tab_cycPointsCalc->setEnabled(false);
        ui->tab_cycEmissCalc->setEnabled(false);
        ui->tab_redPowerCalc->setEnabled(true);
        ui->tab_reports->setEnabled(false);

        ui->tableWidget_SrcDataEU0->setEnabled(false);
        ui->tableWidget_SrcDataEU3->setEnabled(false);
        ui->tableWidget_SrcDataPoints->setEnabled(false);

        ui->tableWidget_FullLoadCurve->setEnabled(true);
        ui->tableWidget_FullLoadCurve->setFocus();

        getUndoRedoCounters(table);

        ui->action_OpenReport->setEnabled(false);
        ui->action_SaveReportAs->setEnabled(false);
        ui->action_CloseReport->setEnabled(false);
        ui->action_PrintReport->setEnabled(false);
        ui->action_ReportToPDF->setEnabled(false);
        ui->action_AddRow->setEnabled(true);
        ui->action_DeleteRow->setEnabled(true);

        ui->tabWidget_Data->setCurrentIndex(2);
    }
    else {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(false);
        ui->comboBox_chargingType->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);
    }
}

void MainWindow::standardChanged(const int currstd) {

    if ( (currstd == STD_EU6) ||
         (currstd == STD_EU5) ||
         (currstd == STD_EU4) ||
         (currstd == STD_EU3) ) {

        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(true);

        if ( ui->comboBox_task->currentIndex() == TASK_POINTS ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);

            ui->tableWidget_SrcDataEU3->setEnabled(true);
            ui->tableWidget_SrcDataEU3->setFocus();

            getUndoRedoCounters(table);
        }
    }
    else if ( (currstd == STD_C1) ||
              (currstd == STD_D1) ||
              (currstd == STD_D2) ||
              (currstd == STD_E1) ||
              (currstd == STD_E2) ||
              (currstd == STD_E3) ||
              (currstd == STD_E5) ||
              (currstd == STD_F ) ||
              (currstd == STD_G1) ||
              (currstd == STD_G2) ) {

        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(true);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        if ( ui->comboBox_task->currentIndex() == TASK_POINTS ) {

            ui->tableWidget_SrcDataEU0->setEnabled(true);
            ui->tableWidget_SrcDataEU0->setFocus();

            getUndoRedoCounters(table);

            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
    }
    else {

        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        if ( ui->comboBox_task->currentIndex() == TASK_POINTS ) {

            ui->tableWidget_SrcDataEU0->setEnabled(true);
            ui->tableWidget_SrcDataEU0->setFocus();

            getUndoRedoCounters(table);

            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
    }
}

void MainWindow::PTcalcChanged(const int currptcalc) {

    if ( (currptcalc == PTCALC_THROUGHSMOKE) || (currptcalc == PTCALC_NO) ) {

        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
    }
    else {

        ui->lineEdit_PTmass->setEnabled(true);
        ui->pushButton_EnterPTmass->setEnabled(true);
    }
}

void MainWindow::reportChanged(const QString &path) {

    QFile reportFile(path);

    if ( reportFile.open(QIODevice::ReadOnly) ) {

        ui->plainTextEdit_Report->setPlainText(reportFile.readAll());
    }
    else {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Can not open report file!"),
                    0, 0, 0
                    );
        return;
    }

    reportFile.close();
}

void MainWindow::tabChanged(const int tab) {

    if ( tab == 3 ) {

        PTcalcChanged(ui->comboBox_PTcalc->currentIndex());

        ui->action_UndoTable->setEnabled(false);
        ui->action_RedoTable->setEnabled(false);
        ui->action_CutFromTable->setEnabled(false);
        ui->action_CopyFromTable->setEnabled(false);
        ui->action_PasteToTable->setEnabled(false);
        ui->action_DeleteFromTable->setEnabled(false);

        ui->action_Add->setEnabled(false);
        ui->action_Multiply->setEnabled(false);
        ui->action_Divide->setEnabled(false);
        ui->action_Equal->setEnabled(false);
    }
    else {

        if ( tab == 0 ) {

            ui->comboBox_task->setCurrentIndex(0);
        }
        else if ( tab == 1 ) {

            ui->comboBox_task->setCurrentIndex(1);
        }
        else if ( tab == 2 ) {

            ui->comboBox_task->setCurrentIndex(2);
        }

        taskChanged(ui->comboBox_task->currentIndex());
        PTcalcChanged(ui->comboBox_PTcalc->currentIndex());

        if ( undoCount == 0 ) {

            ui->action_UndoTable->setEnabled(false);
        }
        else {

            ui->action_UndoTable->setEnabled(true);
        }

        if ( redoCount == 0 ) {

            ui->action_RedoTable->setEnabled(false);
        }
        else {

            ui->action_RedoTable->setEnabled(true);
        }

        ui->action_CutFromTable->setEnabled(true);
        ui->action_CopyFromTable->setEnabled(true);
        ui->action_PasteToTable->setEnabled(true);
        ui->action_DeleteFromTable->setEnabled(true);

        ui->action_Add->setEnabled(true);
        ui->action_Multiply->setEnabled(true);
        ui->action_Divide->setEnabled(true);
        ui->action_Equal->setEnabled(true);
    }
}

void MainWindow::arithmeticOperationIsAvailable(const bool b) {

    ui->action_Add->setEnabled(b);
    ui->action_Multiply->setEnabled(b);
    ui->action_Divide->setEnabled(b);
    ui->action_Equal->setEnabled(b);
}

void MainWindow::tableCellChanged(const int n, const int m) {

    QString str = table->item(n, m)->text();
    int pos = 0;

    if ( (regExpValidator->validate(str, pos) == QValidator::Invalid) ||
         (str.isEmpty()) ) {

        QMessageBox::warning(this, "Qr49", tr("Illegal table cell value!"), 0, 0, 0);

        table->item(n, m)->setText("0");
        return;
    }

    saveTableState();

    //

    if ( n != table->rowCount()-1 ) {

        table->setCurrentCell(n+1, m);
    }
}

void MainWindow::getUndoRedoCounters(QTableWidget *tbl) {

    if ( tbl == ui->tableWidget_SrcDataEU0 ) {

        undoCount = undoRedo_TableEU0->undoTableNumber();
        redoCount = undoRedo_TableEU0->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if ( tbl == ui->tableWidget_SrcDataEU3 ) {

        undoCount = undoRedo_TableEU3->undoTableNumber();
        redoCount = undoRedo_TableEU3->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if ( tbl == ui->tableWidget_SrcDataPoints ) {

        undoCount = undoRedo_TablePoints->undoTableNumber();
        redoCount = undoRedo_TablePoints->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if ( tbl == ui->tableWidget_FullLoadCurve ) {

        undoCount = undoRedo_TableFullLoadCurve->undoTableNumber();
        redoCount = undoRedo_TableFullLoadCurve->redoTableNumber();

        setUndoRedoButtonState();
    }
}

void MainWindow::setUndoRedoButtonState() {

    QString ttUndo(tr("Undo"));
    QString ttRedo(tr("Redo"));

    ttUndo += " (" + QString::number(undoCount) + ")";
    ttRedo += " (" + QString::number(redoCount) + ")";

    if ( undoCount < 1 ) {

        ui->action_UndoTable->setEnabled(false);
        ui->action_UndoTable->setToolTip(ttUndo);
    }
    else {

        ui->action_UndoTable->setEnabled(true);
        ui->action_UndoTable->setToolTip(ttUndo);
    }

    if ( redoCount < 1 ) {

        ui->action_RedoTable->setEnabled(false);
        ui->action_RedoTable->setToolTip(ttRedo);
    }
    else {

        ui->action_RedoTable->setEnabled(true);
        ui->action_RedoTable->setToolTip(ttRedo);
    }
}

void MainWindow::saveTableState() {

    if ( table == ui->tableWidget_SrcDataEU0 ) {

        undoRedo_TableEU0->saveState();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_SrcDataEU3 ) {

        undoRedo_TableEU3->saveState();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_SrcDataPoints ) {

        undoRedo_TablePoints->saveState();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_FullLoadCurve ) {

        undoRedo_TableFullLoadCurve->saveState();
        getUndoRedoCounters(table);
    }
}

void MainWindow::saveStateForAllTables() {

    undoRedo_TableEU0->saveState();
    undoRedo_TableEU3->saveState();
    undoRedo_TablePoints->saveState();
    undoRedo_TableFullLoadCurve->saveState();
}

void MainWindow::on_action_UndoTable_activated() {

    tableCellChangedConnect(false);

    if ( table == ui->tableWidget_SrcDataEU0 ) {

        undoRedo_TableEU0->undoTable();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_SrcDataEU3 ) {

        undoRedo_TableEU3->undoTable();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_SrcDataPoints ) {

        undoRedo_TablePoints->undoTable();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_FullLoadCurve ) {

        undoRedo_TableFullLoadCurve->undoTable();
        getUndoRedoCounters(table);
    }

    tableCellChangedConnect(true);
}

void MainWindow::on_action_RedoTable_activated() {

    tableCellChangedConnect(false);

    if ( table == ui->tableWidget_SrcDataEU0 ) {

        undoRedo_TableEU0->redoTable();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_SrcDataEU3 ) {

        undoRedo_TableEU3->redoTable();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_SrcDataPoints ) {

        undoRedo_TablePoints->redoTable();
        getUndoRedoCounters(table);
    }
    else if ( table == ui->tableWidget_FullLoadCurve ) {

        undoRedo_TableFullLoadCurve->redoTable();
        getUndoRedoCounters(table);
    }

    tableCellChangedConnect(true);
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

        calcABC(n_hi, n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref);
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg(), 0, 0, 0);
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
                        Gair(ui->doubleSpinBox_Dn->value(),
                             ui->doubleSpinBox_B0->value(),
                             ui->doubleSpinBox_t0->value(),
                             ui->doubleSpinBox_dPn->value())
                        )
                    );
}

void MainWindow::nfanCalculation() {

    ui->label_Nfan->
            setText(QString::number(
                        N_fan(ui->doubleSpinBox_nFanRated->value(),
                              ui->doubleSpinBox_n->value(),
                              ui->doubleSpinBox_nRated->value())
                        )
                    );
}

void MainWindow::ka1mCalculation() {

    ui->doubleSpinBox_Ka1m->
            setValue(KaPerc2Ka1m(ui->doubleSpinBox_KaPerc->value(),
                                 ui->doubleSpinBox_L->value()));
}

void MainWindow::kapercCalculation() {

    ui->doubleSpinBox_KaPerc->
            setValue(Ka1m2KaPerc(ui->doubleSpinBox_Ka1m->value(),
                                 ui->doubleSpinBox_L->value()));
}

void MainWindow::smokeBaseChanged() {

    ka1mCalculation();
    kapercCalculation();
}
