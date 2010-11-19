/*
    qr49.cpp

    Copyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>

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
#include "abcspeedscalcdialog.h"
#include "elrsmokecalcdialog.h"
#include "checkoutdatadialog.h"
#include "undoredotable.h"
#include "newversions.h"

#include "r49.h"
#include "qr49constants.h"
#include "libtoxicparameters.h"
#include "libtoxicconstants.h"
#include "double2darray.h"
#include "csvread.h"
#include "cyclepoints.h"
#include "cycleemissions.h"
#include "reducedpower.h"
#include "commonparameters.h"

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
#include <QDebug>
#include <QFontDatabase>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
                                          qr49settings("pa23software", "Qr49"),
                                          tableRowHeight(20),
                                          undoCount(0), redoCount(0) {

    ui->setupUi(this);

    //

    this->setWindowTitle(qr49version);

    contextMenu = new QMenu();

    contextMenu->addMenu(ui->menuFile);
    contextMenu->addMenu(ui->menuEdit);
    contextMenu->addMenu(ui->menuCalculation);

    //

    connect(ui->comboBox_task, SIGNAL(activated(QString)), this, SLOT(TaskChanged(QString)));
    connect(ui->comboBox_standard, SIGNAL(activated(QString)), this, SLOT(StandardChanged(QString)));
    connect(ui->comboBox_PTcalc, SIGNAL(activated(QString)), this, SLOT(PTcalcChanged(QString)));
    connect(ui->comboBox_OpenedReports, SIGNAL(activated(QString)), this, SLOT(ReportChanged(QString)));
    connect(ui->tabWidget_Data, SIGNAL(currentChanged(int)), this, SLOT(TabChanged(int)));

    TableCellChangedConnect(true);

    //

    ui->tableWidget_SrcDataEU0->installEventFilter(this);
    ui->tableWidget_SrcDataEU3->installEventFilter(this);
    ui->tableWidget_SrcDataPoints->installEventFilter(this);
    ui->tableWidget_FullLoadCurve->installEventFilter(this);

    table = ui->tableWidget_SrcDataPoints;

    //

    filterMassDialog = new FilterMassDialog();
    valueDialog = new ValueDialog();
    preferencesDialog = new PreferencesDialog();
    abcSpeedsCalcDialog = new ABCspeedsCalcDialog();
    elrSmokeCalcDialog = new ELRsmokeCalcDialog();
    checkoutDataDialog = new CheckoutDataDialog();
    helpDialog = new HelpDialog();

    //

    params = new LibtoxicParameters();
    config = new CommonParameters();

    ReadPreferences();

    //

    doubleValidator = new QDoubleValidator(this);
    doubleValidator->setBottom(0);
    doubleValidator->setDecimals(4);

    regExp = new QRegExp("[-+]?[0-9]*[.,]?[0-9]+([eE][-+]?[0-9]+)?");
    regExpValidator = new QRegExpValidator(*regExp, 0);

    SetDoubleValidators();

    //

    LoadAllSourceData();

    //

    undoRedo_TableEU0 = new UndoRedoTable(ui->tableWidget_SrcDataEU0);
    undoRedo_TableEU3 = new UndoRedoTable(ui->tableWidget_SrcDataEU3);
    undoRedo_TablePoints = new UndoRedoTable(ui->tableWidget_SrcDataPoints);
    undoRedo_TableFullLoadCurve = new UndoRedoTable(ui->tableWidget_FullLoadCurve);

    SaveStateForAllTables();

    //

    ReadProgramSettings();
    TaskChanged(ui->comboBox_task->currentText());
    StandardChanged(ui->comboBox_standard->currentText());
    PTcalcChanged(ui->comboBox_PTcalc->currentText());

    //

    table = ui->tableWidget_SrcDataPoints;

    //

    ptrdiff_t fontid = QFontDatabase::addApplicationFont("excalib.ttf");

    if (fontid == -1) {

        QMessageBox::warning(0, "Qr49", tr("Monospaced font excalib.ttf not found in program directory!"), 0, 0, 0);
    }
    else {

        monospacedFont_8.setFamily(QFontDatabase::applicationFontFamilies(fontid).first());
        monospacedFont_8.setPointSize(8);
        monospacedFont_10.setFamily(QFontDatabase::applicationFontFamilies(fontid).first());
        monospacedFont_10.setPointSize(10);

        ui->plainTextEdit_Report->setFont(monospacedFont_8);

        QPlainTextEdit *myPlainTextEdit_CheckoutData = checkoutDataDialog->findChild<QPlainTextEdit *>("plainTextEdit_CheckoutData");
        if (!myPlainTextEdit_CheckoutData) {

            QMessageBox::critical(0, "Qr49", tr("on_action_CheckoutData_activated: child object not found!"), 0, 0, 0);
            return;
        }
        myPlainTextEdit_CheckoutData->setFont(monospacedFont_10);
    }

    //

    newVersions = new NewVersions();
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

    WriteProgramSettings();

    delete ui;
    delete contextMenu;
    delete filterMassDialog;
    delete valueDialog;
    delete preferencesDialog;
    delete abcSpeedsCalcDialog;
    delete elrSmokeCalcDialog;
    delete checkoutDataDialog;
    delete helpDialog;
    delete params;
    delete config;
    delete doubleValidator;
    delete regExp;
    delete regExpValidator;
    delete undoRedo_TableEU0;
    delete undoRedo_TableEU3;
    delete undoRedo_TablePoints;
    delete undoRedo_TableFullLoadCurve;
    delete newVersions;
}

void MainWindow::WriteProgramSettings() {

    qr49settings.beginGroup("/Settings");
    qr49settings.setValue("/window_geometry", geometry());
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

void MainWindow::ReadProgramSettings() {

    qr49settings.beginGroup("/Settings");
    setGeometry(qr49settings.value("/window_geometry", QRect(20, 40, 0, 0)).toRect());
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

    if (ui->action_Toolbar->isChecked()) {

        ui->toolBar->setVisible(true);
    }
    else {

        ui->toolBar->setVisible(false);
    }
}

void MainWindow::TableCellChangedConnect(bool b) {

    if (b) {

        connect(ui->tableWidget_SrcDataEU0, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
        connect(ui->tableWidget_SrcDataEU3, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
        connect(ui->tableWidget_SrcDataPoints, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
        connect(ui->tableWidget_FullLoadCurve, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
    }
    else {

        disconnect(ui->tableWidget_SrcDataEU0, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
        disconnect(ui->tableWidget_SrcDataEU3, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
        disconnect(ui->tableWidget_SrcDataPoints, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
        disconnect(ui->tableWidget_FullLoadCurve, SIGNAL(cellChanged(int, int)), this, SLOT(TableCellChanged(int, int)));
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

    if (event->type() == QEvent::FocusIn) {

        if (object == ui->tableWidget_SrcDataEU0) {

            table = ui->tableWidget_SrcDataEU0;
        }
        else if (object == ui->tableWidget_SrcDataEU3) {

            table = ui->tableWidget_SrcDataEU3;
        }
        else if (object == ui->tableWidget_SrcDataPoints) {

            table = ui->tableWidget_SrcDataPoints;
        }
        else if (object == ui->tableWidget_FullLoadCurve) {

            table = ui->tableWidget_FullLoadCurve;
        }
    }

    return false;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *cme) {

    contextMenu->exec(cme->globalPos());
}

void MainWindow::SetDoubleValidators() {

    ui->lineEdit_Vh->setValidator(doubleValidator);
    ui->lineEdit_PTmass->setValidator(doubleValidator);
}

void MainWindow::ReadPreferences() {

    if (!config->ReadConfigFile(configFileName)) {

        QMessageBox::warning(0, "Qr49", tr("ReadConfigFile function returns false! Default values will be used."), 0, 0, 0);
    }
}

void MainWindow::LoadAllSourceData() {

    QString filenameSourceEU0 = config->Get_filenameSourceEU0();

    if ( QFile::exists(filenameSourceEU0) ) {

        TableCellChangedConnect(false);

        if (!FillTableEU0(filenameSourceEU0)) {

            QMessageBox::critical(0, "Qr49", tr("LoadAllSourceData: FillTableEU0 function returns false!"), 0, 0, 0);
        }

        TableCellChangedConnect(true);
    }
    else {

        QString msg = tr("LoadAllSourceData: file ") + filenameSourceEU0 + tr(" not found!");
        QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);
    }

    //

    QString filenameSourceEU3 = config->Get_filenameSourceEU3();

    if ( QFile::exists(filenameSourceEU3) ) {

        TableCellChangedConnect(false);

        if (!FillTableEU3(filenameSourceEU3)) {

            QMessageBox::critical(0, "Qr49", tr("LoadAllSourceData: FillTableEU3 function returns false!"), 0, 0, 0);
        }

        TableCellChangedConnect(true);
    }
    else {

        QString msg = tr("LoadAllSourceData: file ") + filenameSourceEU3 + tr(" not found!");
        QMessageBox::warning(0, "Qr49", msg, 0, 0, 0);
    }

    //

    QString filenamePoints = config->Get_filenamePoints();

    if ( QFile::exists(filenamePoints) ) {

        TableCellChangedConnect(false);

        if (!FillTablePoints(filenamePoints)) {

            QMessageBox::critical(0, "Qr49", tr("LoadAllSourceData: FillTablePoints function returns false!"), 0, 0, 0);
        }

        TableCellChangedConnect(true);
    }
    else {

        QString msg = tr("LoadAllSourceData: file ") + filenamePoints + tr(" not found!");
        QMessageBox::warning(0, "Qr49", msg, 0, 0, 0);
    }

    //

    QString filenamePowers = config->Get_filenamePowers();

    if ( QFile::exists(filenamePowers) ) {

        TableCellChangedConnect(false);

        if (!FillTableFullLoadCurve(filenamePowers)) {

            QMessageBox::critical(0, "Qr49", tr("LoadAllSourceData: FillTableFullLoadCurve function returns false!"), 0, 0, 0);
        }

        TableCellChangedConnect(true);
    }
    else {

        QString msg = tr("LoadAllSourceData: file ") + filenamePowers + tr(" not found!");
        QMessageBox::warning(0, "Qr49", msg, 0, 0, 0);
    }
}

bool MainWindow::FillTableEU0(QString filename) {

    ui->tableWidget_SrcDataEU0->setRowCount(1);
    ui->tableWidget_SrcDataEU0->setRowHeight(0, tableRowHeight);

    ptrdiff_t NumberOfPoints = 0;

    csvRead *ReaderSourceDataEU0 = new csvRead();

    Double2DArray *SourceDataEU0;
    double **arraySourceDataEU0;

    if (!ReaderSourceDataEU0->ReadData(filename, config->Get_csvDelimiter(), &NumberOfPoints)) {

        delete ReaderSourceDataEU0;

        QMessageBox::critical(0, "Qr49", tr("FillTableEU0: libfuns: csvRead: ReadData function returns false!"), 0, 0, 0);

        return false;
    }

    SourceDataEU0 = new Double2DArray(NumberOfPoints, EU0SrcDataParamsNumber);
    arraySourceDataEU0 = SourceDataEU0->GetPointerOnArray();

    if (!ReaderSourceDataEU0->CheckArrayDimension(EU0SrcDataParamsNumber)) {

        delete SourceDataEU0;
        delete ReaderSourceDataEU0;

        QMessageBox::critical(0, "Qr49", tr("FillTableEU0: libfuns: csvRead: CheckArrayDimension function returns false!"), 0, 0, 0);

        return false;
    }

    if (!ReaderSourceDataEU0->FillArray(arraySourceDataEU0)) {

        delete SourceDataEU0;
        delete ReaderSourceDataEU0;

        QMessageBox::critical(0, "Qr49", tr("FillTableEU0: libfuns: csvRead: FillArray function returns false!"), 0, 0, 0);

        return false;
    }

    for (ptrdiff_t j=0; j<EU0SrcDataParamsNumber; j++) {

        ui->tableWidget_SrcDataEU0->setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU0[StrsNumberForColumnCaption][j], 'f', 3)));
        ui->tableWidget_SrcDataEU0->item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    delete SourceDataEU0;
    delete ReaderSourceDataEU0;

    return true;
}

bool MainWindow::FillTableEU3(QString filename) {

    ui->tableWidget_SrcDataEU3->setRowCount(1);
    ui->tableWidget_SrcDataEU3->setRowHeight(0, tableRowHeight);

    ptrdiff_t NumberOfPoints = 0;

    csvRead *ReaderSourceDataEU3 = new csvRead();

    Double2DArray *SourceDataEU3;
    double **arraySourceDataEU3;

    if (!ReaderSourceDataEU3->ReadData(filename, config->Get_csvDelimiter(), &NumberOfPoints)) {

        delete ReaderSourceDataEU3;

        QMessageBox::critical(0, "Qr49", tr("FillTableEU3: libfuns: csvRead: ReadData function returns false!"), 0, 0, 0);

        return false;
    }

    SourceDataEU3 = new Double2DArray(NumberOfPoints, EU3SrcDataParamsNumber);
    arraySourceDataEU3 = SourceDataEU3->GetPointerOnArray();

    if (!ReaderSourceDataEU3->CheckArrayDimension(EU3SrcDataParamsNumber)) {

        delete SourceDataEU3;
        delete ReaderSourceDataEU3;

        QMessageBox::critical(0, "Qr49", tr("FillTableEU3: libfuns: csvRead: CheckArrayDimension function returns false!"), 0, 0, 0);

        return false;
    }

    if (!ReaderSourceDataEU3->FillArray(arraySourceDataEU3)) {

        delete SourceDataEU3;
        delete ReaderSourceDataEU3;

        QMessageBox::critical(0, "Qr49", tr("FillTableEU3: libfuns: csvRead: FillArray function returns false!"), 0, 0, 0);

        return false;
    }

    for (ptrdiff_t j=0; j<EU3SrcDataParamsNumber; j++) {

        ui->tableWidget_SrcDataEU3->setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU3[StrsNumberForColumnCaption][j], 'f', 3)));
        ui->tableWidget_SrcDataEU3->item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    delete SourceDataEU3;
    delete ReaderSourceDataEU3;

    return true;
}

bool MainWindow::FillTablePoints(QString filename) {

    ptrdiff_t NumberOfPoints = 0;

    csvRead *ReaderSourceDataPoints = new csvRead();

    Double2DArray *SourceDataPoints;
    double **arraySourceDataPoints;

    if (!ReaderSourceDataPoints->ReadData(filename, config->Get_csvDelimiter(), &NumberOfPoints)) {

        delete ReaderSourceDataPoints;

        QMessageBox::critical(0, "Qr49", tr("FillTablePoints: libfuns: csvRead: ReadData function returns false!"), 0, 0, 0);

        return false;
    }

    SourceDataPoints = new Double2DArray(NumberOfPoints, PointsFileColumnsNumber);
    arraySourceDataPoints = SourceDataPoints->GetPointerOnArray();

    if (!ReaderSourceDataPoints->CheckArrayDimension(PointsFileColumnsNumber)) {

        delete SourceDataPoints;
        delete ReaderSourceDataPoints;

        QMessageBox::critical(0, "Qr49", tr("FillTablePoints: libfuns: csvRead: CheckArrayDimension function returns false!"), 0, 0, 0);

        return false;
    }

    if (!ReaderSourceDataPoints->FillArray(arraySourceDataPoints)) {

        delete SourceDataPoints;
        delete ReaderSourceDataPoints;

        QMessageBox::critical(0, "Qr49", tr("FillTablePoints: libfuns: csvRead: FillArray function returns false!"), 0, 0, 0);

        return false;
    }

    NumberOfPoints -= StrsNumberForColumnCaption;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        ui->tableWidget_SrcDataPoints->setRowCount(i+1);
        ui->tableWidget_SrcDataPoints->setRowHeight(i, tableRowHeight);
        ui->tableWidget_SrcDataPoints->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_SrcDataPoints->setItem(i, 0, new QTableWidgetItem(QString::number(arraySourceDataPoints[i+1][0], 'f', 0)));
        ui->tableWidget_SrcDataPoints->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_SrcDataPoints->setItem(i, 1, new QTableWidgetItem(QString::number(arraySourceDataPoints[i+1][1], 'f', 0)));
        ui->tableWidget_SrcDataPoints->item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (ptrdiff_t j=2; j<PointsFileColumnsNumber; j++) {

            ui->tableWidget_SrcDataPoints->setItem(i, j, new QTableWidgetItem(QString::number(arraySourceDataPoints[i+1][j], 'f', 3)));
            ui->tableWidget_SrcDataPoints->item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    delete SourceDataPoints;
    delete ReaderSourceDataPoints;

    return true;
}

bool MainWindow::FillTableFullLoadCurve(QString filename) {

    ptrdiff_t NumberOfPoints = 0;

    csvRead *ReaderFullLoadCurve = new csvRead();

    Double2DArray *FullLoadCurve;
    double **arrayFullLoadCurve;

    if (!ReaderFullLoadCurve->ReadData(filename, config->Get_csvDelimiter(), &NumberOfPoints)) {

        delete ReaderFullLoadCurve;

        QMessageBox::critical(0, "Qr49", tr("FillTableFullLoadCurve: libfuns: csvRead: ReadData function returns false!"), 0, 0, 0);

        return false;
    }

    FullLoadCurve = new Double2DArray(NumberOfPoints, PowersFileColumnsNumber);
    arrayFullLoadCurve = FullLoadCurve->GetPointerOnArray();

    if (!ReaderFullLoadCurve->CheckArrayDimension(PowersFileColumnsNumber)) {

        delete FullLoadCurve;
        delete ReaderFullLoadCurve;

        QMessageBox::critical(0, "Qr49", tr("FillTableFullLoadCurve: libfuns: csvRead: CheckArrayDimension function returns false!"), 0, 0, 0);

        return false;
    }

    if (!ReaderFullLoadCurve->FillArray(arrayFullLoadCurve)) {

        delete FullLoadCurve;
        delete ReaderFullLoadCurve;

        QMessageBox::critical(0, "Qr49", tr("FillTableFullLoadCurve: libfuns: csvRead: FillArray function returns false!"), 0, 0, 0);

        return false;
    }

    NumberOfPoints -= StrsNumberForColumnCaption;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        ui->tableWidget_FullLoadCurve->setRowCount(i+1);
        ui->tableWidget_FullLoadCurve->setRowHeight(i, tableRowHeight);

        ui->tableWidget_FullLoadCurve->setItem(i, 0, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i+1][0], 'f', 0)));
        ui->tableWidget_FullLoadCurve->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_FullLoadCurve->setItem(i, 1, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i+1][1], 'f', 0)));
        ui->tableWidget_FullLoadCurve->item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (ptrdiff_t j=2; j<PowersFileColumnsNumber; j++) {

            ui->tableWidget_FullLoadCurve->setItem(i, j, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i+1][j], 'f', 3)));
            ui->tableWidget_FullLoadCurve->item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    delete FullLoadCurve;
    delete ReaderFullLoadCurve;

    return true;
}

bool MainWindow::FillParameters() {

    params->SetTask(ui->comboBox_task->currentText());
    double Vh = ui->lineEdit_Vh->text().toDouble(); params->SetVh(&Vh);
    params->SetStandard(ui->comboBox_standard->currentText());
    params->SetFuelType(ui->comboBox_FuelType->currentText());
    params->SetNOxSample(ui->comboBox_NOxSample->currentText());
    params->SetPTcalc(ui->comboBox_PTcalc->currentText());
    double PTMass = ui->lineEdit_PTmass->text().toDouble(); params->SetPTmass(&PTMass);
    params->SetAddPointsCalc(ui->comboBox_AddPointsCalc->currentText());

    return true;
}

bool MainWindow::ArithmeticOperation(QString operation) {

    QLineEdit *value = valueDialog->findChild<QLineEdit *>("lineEdit_Value");

    if (!value) {

        QMessageBox::critical(0, "Qr49", tr("ArithmeticOperation: child object not found!"), 0, 0, 0);

        return false;
    }

    value->setFocus();
    value->selectAll();

    if (operation == "add") {

        valueDialog->setWindowTitle(tr("Qr49: add"));
    }
    else if (operation == "multiply") {

        valueDialog->setWindowTitle(tr("Qr49: multiply"));
    }
    else if (operation == "divide") {

        valueDialog->setWindowTitle(tr("Qr49: divide"));
    }
    else if (operation == "equal") {

        valueDialog->setWindowTitle(tr("Qr49: equal"));
    }
    else {

        QMessageBox::critical(0, "Qr49", tr("ArithmeticOperation: unknown arithmetic operation!"), 0, 0, 0);

        return false;
    }

    if (valueDialog->exec() == QDialog::Accepted) {

        if (table->selectedRanges().empty()) {

            QMessageBox::critical(0, "Qr49", tr("No selected cells!"), 0, 0, 0);

            return false;
        }

        QTableWidgetSelectionRange selectedRange = table->selectedRanges().first();

        double x = 0.0;
        double y = value->text().toDouble();

        if ( (y == 0.0) && (operation == "divide") ) {

            QMessageBox::warning(0, "Qr49", tr("Illegal operation \"Divide by zero\"!"), 0, 0, 0);

            return false;
        }

        TableCellChangedConnect(false);

        for (ptrdiff_t i=0; i<selectedRange.rowCount(); i++) {

            for (ptrdiff_t j=0; j<selectedRange.columnCount(); j++) {

                x = table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->text().toDouble();

                if (operation == "add") {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->setText(QString::number(x+y, 'f', 3));
                }
                else if (operation == "multiply") {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->setText(QString::number(x*y, 'f', 3));
                }
                else if (operation == "divide") {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->setText(QString::number(x/y, 'f', 3));
                }
                else if (operation == "equal") {

                    table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->setText(QString::number(y, 'f', 3));
                }
            }
        }

        TableCellChangedConnect(true);
    }

    SaveState();

    return true;
}

void MainWindow::on_action_LoadSourceData_activated() {

    QString dir(config->Get_dirnameReports());

    QString anotherSourceFile(QFileDialog::getOpenFileName(
            this,
            tr("Open Source Data File..."),
            dir,
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (table == ui->tableWidget_SrcDataEU0) {

        ui->tabWidget_Data->setCurrentIndex(0);

        TableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!FillTableEU0(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", tr("on_action_ChangeSourceData_activated: FillTableEU0 returns false!"), 0, 0, 0);
            }
        }

        TableCellChangedConnect(true);

        SaveState();
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        ui->tabWidget_Data->setCurrentIndex(0);

        TableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!FillTableEU3(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", tr("on_action_ChangeSourceData_activated: FillTableEU3 returns false!"), 0, 0, 0);
            }
        }

        TableCellChangedConnect(true);

        SaveState();
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        ui->tabWidget_Data->setCurrentIndex(0);

        TableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!FillTablePoints(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", tr("on_action_ChangeSourceData_activated: FillTablePoints returns false!"), 0, 0, 0);
            }
        }

        TableCellChangedConnect(true);

        SaveState();
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        ui->tabWidget_Data->setCurrentIndex(2);

        TableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!FillTableFullLoadCurve(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", tr("on_action_ChangeSourceData_activated: FillTableFullLoadCurve returns false!"), 0, 0, 0);
            }
        }

        TableCellChangedConnect(true);

        SaveState();
    }
}

void MainWindow::on_action_SaveSourceData_activated() {

    if (table == ui->tableWidget_SrcDataEU0) {

        QString filenameSourceEU0 = config->Get_filenameSourceEU0();

        QFile SrcDataEU0File(filenameSourceEU0);

        if (!SrcDataEU0File.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SaveAllSourceData_activated: file ") + filenameSourceEU0 + tr(" could not be opened!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        SrcDataEU0File.write("idle[min-1];n_interim[min-1];n_rated[min-1];N_fan_rated[kW];Ne_interim[Nm];Ne_rated[Nm];\n");

        for (ptrdiff_t j=0; j<EU0SrcDataParamsNumber; j++) {

            SrcDataEU0File.write(ui->tableWidget_SrcDataEU0->item(0, j)->text().toAscii().data());
            SrcDataEU0File.write(";");
        }

        SrcDataEU0File.write("\n");

        SrcDataEU0File.close();
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        QString filenameSourceEU3 = config->Get_filenameSourceEU3();

        QFile SrcDataEU3File(filenameSourceEU3);

        if (!SrcDataEU3File.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SaveAllSourceData_activated: file ") + filenameSourceEU3 + tr(" could not be opened!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        SrcDataEU3File.write("n_hi[min-1];n_lo[min-1];idle[min-1];n_rated[min-1];N_fan_rated[kW];Ne_A[Nm];Ne_B[Nm];Ne_C[Nm];Ne_a1[Nm];Ne_a2[Nm];Ne_a3[Nm];\n");

        for (ptrdiff_t j=0; j<EU3SrcDataParamsNumber; j++) {

            SrcDataEU3File.write(ui->tableWidget_SrcDataEU3->item(0, j)->text().toAscii().data());
            SrcDataEU3File.write(";");
        }

        SrcDataEU3File.write("\n");

        SrcDataEU3File.close();
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        QString filenamePoints = config->Get_filenamePoints();

        QFile SrcDataPointsFile(filenamePoints);

        if (!SrcDataPointsFile.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SaveAllSourceData_activated: file ") + filenamePoints + tr(" could not be opened!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        SrcDataPointsFile.write("Point[-];n[min-1];Me_b[Nm];Ne_b[kW];N_fan[kW];w[-];t0[oC];B0[kPa];Ra[%];dPn[mmH2O];Gair[kg/h];Gfuel[kg/h];CNOx[ppm];gNOx[g/kWh];CCO[ppm];CCH[ppm];CCO2in[%];CCO2out[%];CO2[%];Ka[m-1];Ka[%];FSN[-];Pr[kPa];ts[oC];tauf[s];qmdw[g/s];qmdew[g/s];rd[-];\n");

        for (ptrdiff_t i=0; i<ui->tableWidget_SrcDataPoints->rowCount(); i++) {

            for (ptrdiff_t j=0; j<PointsFileColumnsNumber; j++) {

                SrcDataPointsFile.write(ui->tableWidget_SrcDataPoints->item(i, j)->text().toAscii().data());
                SrcDataPointsFile.write(";");
            }

            SrcDataPointsFile.write("\n");
        }

        SrcDataPointsFile.close();
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        QString filenamePowers = config->Get_filenamePowers();

        QFile SrcDataPowersFile(filenamePowers);

        if (!SrcDataPowersFile.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SaveAllSourceData_activated: file ") + filenamePowers + tr(" could not be opened!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        SrcDataPowersFile.write("Point[-];n[min-1];Me_b[Nm];t0[oC];B0[kPa];Ra[%];S[kPa];pk[kPa];Gfuel[kg/h];N_k[kW];N_fan[kW];\n");

        for (ptrdiff_t i=0; i<ui->tableWidget_FullLoadCurve->rowCount(); i++) {

            for (ptrdiff_t j=0; j<PowersFileColumnsNumber; j++) {

                SrcDataPowersFile.write(ui->tableWidget_FullLoadCurve->item(i, j)->text().toAscii().data());
                SrcDataPowersFile.write(";");
            }

            SrcDataPowersFile.write("\n");
        }

        SrcDataPowersFile.close();
    }
}

void MainWindow::on_action_SaveSourceDataAs_activated() {

    QString newSourceDataFileName(QFileDialog::getSaveFileName(
            this,
            tr("Save Source Data File As..."),
            "noname.csv",
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (!newSourceDataFileName.isEmpty()) {

        QFile SrcDataFile(newSourceDataFileName);

        if (!SrcDataFile.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SavePointsFileAs_activated: file ") + newSourceDataFileName + tr(" could not be opened!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        if (table == ui->tableWidget_SrcDataEU0) {

            SrcDataFile.write("idle[min-1];n_interim[min-1];n_rated[min-1];N_fan_rated[kW];Ne_interim[Nm];Ne_rated[Nm];\n");
        }
        else if (table == ui->tableWidget_SrcDataEU3) {

            SrcDataFile.write("n_hi[min-1];n_lo[min-1];idle[min-1];n_rated[min-1];N_fan_rated[kW];Ne_A[Nm];Ne_B[Nm];Ne_C[Nm];Ne_a1[Nm];Ne_a2[Nm];Ne_a3[Nm];\n");
        }
        else if (table == ui->tableWidget_SrcDataPoints) {

            SrcDataFile.write("Point[-];n[min-1];Me_b[Nm];Ne_b[kW];N_fan[kW];w[-];t0[oC];B0[kPa];Ra[%];dPn[mmH2O];Gair[kg/h];Gfuel[kg/h];CNOx[ppm];gNOx[g/kWh];CCO[ppm];CCH[ppm];CCO2in[%];CCO2out[%];CO2[%];Ka[m-1];Ka[%];FSN[-];Pr[kPa];ts[oC];tauf[s];qmdw[g/s];qmdew[g/s];rd[-];\n");
        }
        else if (table == ui->tableWidget_FullLoadCurve) {

            SrcDataFile.write("Point[-];n[min-1];Me_b[Nm];t0[oC];B0[kPa];Ra[%];S[kPa];pk[kPa];Gfuel[kg/h];N_k[kW];N_fan[kW];\n");
        }

        for (ptrdiff_t i=0; i<table->rowCount(); i++) {

            for (ptrdiff_t j=0; j<table->columnCount(); j++) {

                SrcDataFile.write(table->item(i, j)->text().toAscii().data());
                SrcDataFile.write(";");
            }

            SrcDataFile.write("\n");
        }

        SrcDataFile.close();
    }
}

void MainWindow::on_action_LoadCalculationOptions_activated() {

    QString dir(config->Get_dirnameReports());

    QString anotherOptions(QFileDialog::getOpenFileName(
            this,
            tr("Open Calculation Options File..."),
            dir,
            QString::fromAscii("Config files (*.conf);;All files (*.*)"),
            0,
            0));

    if (!anotherOptions.isEmpty()) {

        params->ReadCalcConfigFile(anotherOptions);

        SetComboIndex(ui->comboBox_task, params->GetTask());
        ui->lineEdit_Vh->setText(QString::number(params->GetVh()));
        SetComboIndex(ui->comboBox_standard, params->GetStandard());
        SetComboIndex(ui->comboBox_FuelType, params->GetFuelType());
        SetComboIndex(ui->comboBox_NOxSample, params->GetNOxSample());
        SetComboIndex(ui->comboBox_PTcalc, params->GetPTcalc());
        ui->lineEdit_PTmass->setText(QString::number(params->GetPTmass()));
        SetComboIndex(ui->comboBox_AddPointsCalc, params->GetAddPointsCalc());

        TaskChanged(ui->comboBox_task->currentText());
        StandardChanged(ui->comboBox_standard->currentText());
        PTcalcChanged(ui->comboBox_PTcalc->currentText());
    }
}

void MainWindow::SetComboIndex(QComboBox *combo, QString str) {

    for (ptrdiff_t i=0; i<combo->count(); i++) {

        if (str == combo->itemText(i)) {

            combo->setCurrentIndex(i);
            break;
        }
    }
}

void MainWindow::on_action_SaveCalculationOptionsAs_activated() {

    QString optionsFileName(QFileDialog::getSaveFileName(
            this,
            tr("Save Options..."),
            "noname.conf",
            QString::fromAscii("Config files (*.conf);;All files (*.*)"),
            0,
            0));

    if (!optionsFileName.isEmpty()) {

        QFile savedOptions(optionsFileName);

        if (!savedOptions.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SaveCalculationOptions_activated: file ") + optionsFileName + tr(" could not be opened!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        savedOptions.write("task");           savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->comboBox_task->currentText().toAscii());          savedOptions.write("\n");
        savedOptions.write("Vh");             savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->lineEdit_Vh->text().toAscii());                   savedOptions.write("\n");
        savedOptions.write("standard");       savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->comboBox_standard->currentText().toAscii());      savedOptions.write("\n");
        savedOptions.write("FuelType");       savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->comboBox_FuelType->currentText().toAscii());      savedOptions.write("\n");
        savedOptions.write("NOxSample");      savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->comboBox_NOxSample->currentText().toAscii());     savedOptions.write("\n");
        savedOptions.write("PTcalc");         savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->comboBox_PTcalc->currentText().toAscii());        savedOptions.write("\n");
        savedOptions.write("PTmass");         savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->lineEdit_PTmass->text().toAscii());               savedOptions.write("\n");
        savedOptions.write("AddPointsCalc");  savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->comboBox_AddPointsCalc->currentText().toAscii()); savedOptions.write("\n");
        savedOptions.write("CalcConfigFile"); savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(params->GetCalcConfigFile().toAscii());               savedOptions.write("\n");

        savedOptions.close();
    }
}

void MainWindow::on_action_OpenReport_activated() {

    QString dir(config->Get_dirnameReports());

    QString anotherReport(QFileDialog::getOpenFileName(
            this,
            tr("Open Report..."),
            dir,
            QString::fromAscii("Text files (*.txt);;All files (*.*)"),
            0,
            0));

    if (!anotherReport.isEmpty()) {

        ui->comboBox_OpenedReports->insertItem(0, anotherReport);
        ui->comboBox_OpenedReports->setCurrentIndex(0);
        ReportChanged(anotherReport);
        ui->tabWidget_Data->setCurrentIndex(1);
    }
}

void MainWindow::on_action_SaveReportAs_activated() {

    QString newReportFileName(QFileDialog::getSaveFileName(
            this,
            tr("Save Report As..."),
            ui->comboBox_OpenedReports->currentText(),
            QString::fromAscii("Text files (*.txt);;All files (*.*)"),
            0,
            0));

    if (!newReportFileName.isEmpty()) {

        QString myreport = ui->plainTextEdit_Report->toPlainText();

        QFile reportFile(newReportFileName);

        if (!reportFile.open(QIODevice::WriteOnly)) {

            QString msg = tr("on_action_SaveReportAs_activated: file ") + newReportFileName + tr(" could not be saved!");
            QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

            return;
        }

        QTextStream in(&reportFile);
        in << myreport;

        reportFile.close();

        ui->comboBox_OpenedReports->removeItem(ui->comboBox_OpenedReports->currentIndex());
        ui->comboBox_OpenedReports->insertItem(0, newReportFileName);
        ui->comboBox_OpenedReports->setCurrentIndex(0);
        ReportChanged(newReportFileName);
        ui->tabWidget_Data->setCurrentIndex(1);
    }
}

void MainWindow::on_action_CloseReport_activated() {

    if (ui->comboBox_OpenedReports->count() != 1) {

        ui->comboBox_OpenedReports->removeItem(ui->comboBox_OpenedReports->currentIndex());
    }

    ReportChanged(ui->comboBox_OpenedReports->currentText());
    ui->tabWidget_Data->setCurrentIndex(1);
}

void MainWindow::on_action_ReportToPDF_activated() {

    QString filename = ui->comboBox_OpenedReports->currentText() + ".pdf";

    QString newReportFileName(QFileDialog::getSaveFileName(
            this,
            tr("Export Report to PDF..."),
            filename,
            QString::fromAscii("PDF files (*.pdf);;All files (*.*)"),
            0,
            0));

    QPrinter printer;

    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(20, 20, 10, 10, QPrinter::Millimeter);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(newReportFileName);
    printer.setFontEmbeddingEnabled(true);

    ui->plainTextEdit_Report->print(&printer);
}

void MainWindow::on_action_PrintReport_activated() {

    QPrinter printer;

    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(20, 20, 10, 10, QPrinter::Millimeter);
//	printer.setPrintRange(QPrinter::PageRange);
//	printer.setFromTo(1, 1);

    QPrintDialog printDialog(&printer, this);

    if (printDialog.exec() == QDialog::Accepted) {

        ui->plainTextEdit_Report->print(&printer);
    }
}

void MainWindow::on_action_Preferences_activated() {

    QComboBox *myComboBox_csvdelimiter = preferencesDialog->findChild<QComboBox *>("comboBox_csvdelimiter");

    if (!myComboBox_csvdelimiter) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenameSourceEU3 = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU3");

    if (!myLineEdit_filenameSourceEU3) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenameSourceEU0 = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU0");

    if (!myLineEdit_filenameSourceEU0) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenamePoints = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePoints");

    if (!myLineEdit_filenamePoints) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenamePowers = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePowers");

    if (!myLineEdit_filenamePowers) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_dirnameReports = preferencesDialog->findChild<QLineEdit *>("lineEdit_dirnameReports");

    if (!myLineEdit_dirnameReports) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_Dn = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Dn");

    if (!myDoubleSpinBox_Dn) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_ConcO2air = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcO2air");

    if (!myDoubleSpinBox_ConcO2air) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_Rr = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Rr");

    if (!myDoubleSpinBox_Rr) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_L0 = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L0");

    if (!myDoubleSpinBox_L0) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_L = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L");

    if (!myDoubleSpinBox_L) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_ConcCO2air = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcCO2air");

    if (!myDoubleSpinBox_ConcCO2air) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_WH = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WH");

    if (!myDoubleSpinBox_WH) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_WO2 = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WO2");

    if (!myDoubleSpinBox_WO2) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_WN = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WN");

    if (!myDoubleSpinBox_WN) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_roAir = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_roAir");

    if (!myDoubleSpinBox_roAir) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_muNO2 = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muNO2");

    if (!myDoubleSpinBox_muNO2) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_muCO = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCO");

    if (!myDoubleSpinBox_muCO) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_muCH = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCH");

    if (!myDoubleSpinBox_muCH) {

        QMessageBox::critical(0, "Qr49", tr("on_action_Preferences_activated: child object not found!"), 0, 0, 0);
        return;
    }

    //

    for (ptrdiff_t i=0; i<myComboBox_csvdelimiter->count(); i++) {

        if (myComboBox_csvdelimiter->itemText(i) == config->Get_csvDelimiter()) {

            myComboBox_csvdelimiter->setCurrentIndex(i);
            break;
        }
    }

    myLineEdit_filenameSourceEU3->setText(config->Get_filenameSourceEU3());
    myLineEdit_filenameSourceEU0->setText(config->Get_filenameSourceEU0());
    myLineEdit_filenamePoints->setText(config->Get_filenamePoints());
    myLineEdit_filenamePowers->setText(config->Get_filenamePowers());
    myLineEdit_dirnameReports->setText(config->Get_dirnameReports());
    myDoubleSpinBox_Dn->setValue(config->Get_Dn());
    myDoubleSpinBox_ConcO2air->setValue(config->Get_ConcO2air());
    myDoubleSpinBox_Rr->setValue(config->Get_Rr());
    myDoubleSpinBox_L0->setValue(config->Get_L0());
    myDoubleSpinBox_L->setValue(config->Get_L());
    myDoubleSpinBox_ConcCO2air->setValue(config->Get_ConcCO2air());
    myDoubleSpinBox_WH->setValue(config->Get_WH());
    myDoubleSpinBox_WO2->setValue(config->Get_WO2());
    myDoubleSpinBox_WN->setValue(config->Get_WN());
    myDoubleSpinBox_roAir->setValue(config->Get_roAir());
    myDoubleSpinBox_muNO2->setValue(config->Get_muNO2());
    myDoubleSpinBox_muCO->setValue(config->Get_muCO());
    myDoubleSpinBox_muCH->setValue(config->Get_muCH());

    //

    if (preferencesDialog->exec() == QDialog::Accepted) {

        ReadPreferences();
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

        QTableWidgetSelectionRange selectedRange = table->selectedRanges().first();

        QString str;

        for (ptrdiff_t i=0; i<selectedRange.rowCount(); i++) {

            for (ptrdiff_t j=0; j<selectedRange.columnCount(); j++) {

                str += table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->text();

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

    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');

    ptrdiff_t numRows = rows.count() - 1;
    ptrdiff_t numColumns = rows.first().count('\t') + 1;

    if (
            (
                    (table->rowCount() - table->currentRow()) < numRows
            ) ||
            (
                    (table->columnCount() - table->currentColumn()) < numColumns
            )
    ) {

        QMessageBox::critical(0, "Qr49", tr("on_action_PasteToPointsTable_activated: copied data can not be inserted!"), 0, 0, 0);

        return;
    }

    TableCellChangedConnect(false);

    for (ptrdiff_t i=0; i<numRows; i++) {

        QStringList columns = rows[i].split('\t');

        for (ptrdiff_t j=0; j<numColumns; j++) {

            table->item(table->currentRow()+i, table->currentColumn()+j)->setText(columns[j]);
        }
    }

    TableCellChangedConnect(true);

    SaveState();
}

void MainWindow::on_action_DeleteFromTable_activated() {

    if ( !table->selectedRanges().isEmpty() ) {

        QTableWidgetSelectionRange selectedRange = table->selectedRanges().first();

        TableCellChangedConnect(false);

        for (ptrdiff_t i=0; i<selectedRange.rowCount(); i++) {

            for (ptrdiff_t j=0; j<selectedRange.columnCount(); j++) {

                table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->setText("0");
            }
        }

        TableCellChangedConnect(true);

        SaveState();
    }
}

void MainWindow::on_action_Add_activated() {

    if (!ArithmeticOperation("add")) {

        QMessageBox::critical(0, "Qr49", tr("Arithmetic operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_Multiply_activated() {

    if (!ArithmeticOperation("multiply")) {

        QMessageBox::critical(0, "Qr49", tr("Arithmetic operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_Divide_activated() {

    if (!ArithmeticOperation("divide")) {

        QMessageBox::critical(0, "Qr49", tr("Arithmetic operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_Equal_activated() {

    if (!ArithmeticOperation("equal")) {

        QMessageBox::critical(0, "Qr49", tr("Operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_AddRow_activated() {

    TableCellChangedConnect(false);

    if ( (table != ui->tableWidget_SrcDataEU0) && (table != ui->tableWidget_SrcDataEU3) ) {

        table->setRowCount(table->rowCount()+1);
        table->setRowHeight(table->rowCount()-1, tableRowHeight);
        table->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ptrdiff_t x = table->rowCount();
        QString s = QString::number(x);

        table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(s));
        table->item(table->rowCount()-1, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (ptrdiff_t j=1; j<table->columnCount(); j++) {

            table->setItem(table->rowCount()-1, j, new QTableWidgetItem("0"));
            table->item(table->rowCount()-1, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    TableCellChangedConnect(true);

    SaveState();
}

void MainWindow::on_action_DeleteRow_activated() {

    if ( (table != ui->tableWidget_SrcDataEU0) && (table != ui->tableWidget_SrcDataEU3) ) {

        table->setRowCount(table->rowCount()-1);
    }

    SaveState();
}

void MainWindow::on_action_Toolbar_activated() {

    if (ui->action_Toolbar->isChecked()) {

        ui->toolBar->setVisible(true);
    }
    else {

        ui->toolBar->setVisible(false);
    }
}

void MainWindow::on_action_Execute_activated() {

    ptrdiff_t n = table->rowCount();
    ptrdiff_t m = table->columnCount();

    Double2DArray *Array_DataForCalc = new Double2DArray(n+1, m);
    double **array_DataForCalc = Array_DataForCalc->GetPointerOnArray();

    for (ptrdiff_t i=1; i<n+1; i++) {

        for (ptrdiff_t j=0; j<m; j++) {

            array_DataForCalc[i][j] = table->item(i-1, j)->text().toDouble();
        }
    }

    //

    QString message("\nCalculation completed!\n\n");

    FillParameters();

    if (ui->comboBox_task->currentText() == "points") {

        CyclePoints *myPoints = new CyclePoints(params, config);

        if (!myPoints->GetDataFromCSV(array_DataForCalc, n, m)) {

            delete myPoints;

            QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: libtoxic: CyclePoints: GetDataFromCSV function returns false!"), 0, 0, 0);

            return;
        }

        if (!myPoints->FillArrays()) {

            delete myPoints;

            QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: libtoxic: CyclePoints: FillArrays function returns false!"), 0, 0, 0);

            return;
        }

        message += myPoints->CreateReport();

        delete myPoints;

        //

        QString filenamePoints = config->Get_filenamePoints();

        if ( QFile::exists(filenamePoints) ) {

            TableCellChangedConnect(false);

            if (!FillTablePoints(filenamePoints)) {

                QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: FillTablePoints function returns false!"), 0, 0, 0);
            }

            TableCellChangedConnect(true);
        }
        else {

            QString msg = tr("on_action_Execute_activated: file ") + filenamePoints + tr(" not found!");
            QMessageBox::warning(0, "Qr49", msg, 0, 0, 0);
        }

        //

        ui->tabWidget_Data->setCurrentIndex(0);
    }
    else if (ui->comboBox_task->currentText() == "emissions") {

        CycleEmissions *myEmissions = new CycleEmissions(params, config);

        if (!myEmissions->GetDataFromCSV(array_DataForCalc, n, m)) {

            delete myEmissions;

            QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: libtoxic: CycleEmissions: GetDataFromCSV function returns false!"), 0, 0, 0);

            return;
        }

        if (!myEmissions->MakeCalculations()) {

            delete myEmissions;

            QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: libtoxic: CycleEmissions: MakeCalculations function returns false!\nMaybe you did not enter all source data?"), 0, 0, 0);

            return;
        }

        if (ui->checkBox_reports->isChecked()) {

            message += myEmissions->CreateReports(true);

            //

            //QDir ReportsDir(config->Get_dirnameReports());
            //QStringList repdirs(ReportsDir.entryList(QDir::Dirs, QDir::Time));

            //lastReportsDir.setPath(ReportsDir.absoluteFilePath(repdirs.value(1)));

            lastReportsDir = myEmissions->GetLastReportsDir();

            QString csvfilter("*.csv");
            QStringList csvfiles(lastReportsDir.entryList(QDir::nameFiltersFromString(csvfilter), QDir::Files, QDir::Time));

            lastCheckoutDataFileName = lastReportsDir.absoluteFilePath(csvfiles.first());

            //

            if (ui->comboBox_standard->currentText() == "FreeCalc") {

                ui->tabWidget_Data->setCurrentIndex(0);
            }
            else {

                ui->tabWidget_Data->setCurrentIndex(1);

                QString txtfilter("*.txt");
                QStringList reports(lastReportsDir.entryList(QDir::nameFiltersFromString(txtfilter), QDir::Files, QDir::Time));

                lastReportFileName = lastReportsDir.absoluteFilePath(reports.first());

                ui->comboBox_OpenedReports->insertItem(0, lastReportFileName);
                ui->comboBox_OpenedReports->setCurrentIndex(0);
                ReportChanged(lastReportFileName);
            }
        }
        else {

            message += myEmissions->CreateReports(false);
        }

        delete myEmissions;
    }
    else if (ui->comboBox_task->currentText() == "ReducedPower") {

        ReducedPower *myReducedPower = new ReducedPower(params, config);

        if (!myReducedPower->GetDataFromCSV(array_DataForCalc, n, m)) {

            delete myReducedPower;

            QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: libtoxic: ReducedPower: GetDataFromCSV function returns false!"), 0, 0, 0);

            return;
        }

        if (!myReducedPower->ReducePower()) {

            delete myReducedPower;

            QMessageBox::critical(0, "Qr49", tr("on_action_Execute_activated: libtoxic: ReducedPower: ReducePower function returns false!"), 0, 0, 0);

            return;
        }

        message += myReducedPower->CreateReports();

        //

        //QDir ReportsDir(config->Get_dirnameReports());
        //QStringList repdirs(ReportsDir.entryList(QDir::Dirs, QDir::Time));

        //lastReportsDir.setPath(ReportsDir.absoluteFilePath(repdirs.value(1)));

        lastReportsDir = myReducedPower->GetLastReportsDir();

        QString csvfilter("*.csv");
        QStringList csvfiles(lastReportsDir.entryList(QDir::nameFiltersFromString(csvfilter), QDir::Files, QDir::Time));

        lastCheckoutDataFileName = lastReportsDir.absoluteFilePath(csvfiles.first());

        delete myReducedPower;
    }
    else {

        QMessageBox::critical(0, "Qr49", tr("Unknown calculation task!"), 0, 0, 0);

        return;
    }

    QMessageBox::information(0, "Qr49", message, 0, 0, 0);

    if ( (ui->comboBox_standard->currentText() == "FreeCalc") || (ui->comboBox_task->currentText() == "ReducedPower") ) {

        on_action_CheckoutData_activated();
    }
}

void MainWindow::on_action_ABCspeeds_activated() {

    abcSpeedsCalcDialog->exec();
}

void MainWindow::on_action_ELRsmoke_activated() {

    elrSmokeCalcDialog->exec();
}

void MainWindow::on_action_CheckoutData_activated() {

    QPlainTextEdit *myPlainTextEdit_CheckoutData = checkoutDataDialog->findChild<QPlainTextEdit *>("plainTextEdit_CheckoutData");

    if (!myPlainTextEdit_CheckoutData) {

        QMessageBox::critical(0, "Qr49", tr("on_action_CheckoutData_activated: child object not found!"), 0, 0, 0);

        return;
    }

    QFile arrayFile(lastCheckoutDataFileName);
    QString data;

    if (arrayFile.open(QIODevice::ReadOnly)) {

        data.append(arrayFile.readAll());
    }
    else {

        QMessageBox::critical(0, "Qr49", tr("on_action_CheckoutData_activated: can not open file!"), 0, 0, 0);

        return;
    }

    arrayFile.close();

    string stdstrdata = data.toStdString();

    myPlainTextEdit_CheckoutData->setPlainText(QString::fromLocal8Bit(stdstrdata.c_str()));

    checkoutDataDialog->exec();
}

void MainWindow::on_action_StandardsDescription_activated() {

    helpDialog->exec();
}

void MainWindow::on_action_AboutQr49_activated() {

    QString str = "<b>r49 distribution version " + r49version + "</b><br><br>" + qr49version + ", libtoxic v" + libtoxicVersion +
                  tr("<br><br>Calculation of modes and specific emissions for stationary diesel engine cycles (UN ECE Regulation No. 49, UN ECE Regulation No. 96, UN ECE Regulation No. 85, OST 37.001.234-81, GOST 17.2.2.05-97, GOST 30574-98, GOST R 51249-99)."
                  "<br><br>Copyright (C) 2009, 2010 Artem Petrov <a href= \"mailto:pa2311@gmail.com\" >pa2311@gmail.com</a>"
                  "<br><br>Web site: <a href= \"https://github.com/pa23/r49\">https://github.com/pa23/r49</a>"
                  "<br><br>This program is free software: you can redistribute it and/or modify"
                  "it under the terms of the GNU General Public License as published by"
                  "the Free Software Foundation, either version 3 of the License, or"
                  "(at your option) any later version."
                  "<br>This program is distributed in the hope that it will be useful,"
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of"
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the"
                  "GNU General Public License for more details."
                  "<br>You should have received a copy of the GNU General Public License"
                  "along with this program. If not, see <a href= \"http://www.gnu.org/licenses/\" >http://www.gnu.org/licenses/</a>.<br>");

    QMessageBox::about(this, tr("About Qr49"), str);
}

void MainWindow::on_action_AboutQt_activated() {

    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_CheckForUpdates_activated() {

    newVersions->CheckAvailableVersions();
}

void MainWindow::on_pushButton_EnterPTmass_clicked() {

    if (filterMassDialog->exec() == QDialog::Accepted) {

        QLineEdit *m1c = filterMassDialog->findChild<QLineEdit *>("lineEdit_1stFilterWeightClean");
        QLineEdit *m1d = filterMassDialog->findChild<QLineEdit *>("lineEdit_1stFilterWeightDirty");
        QLineEdit *m2c = filterMassDialog->findChild<QLineEdit *>("lineEdit_2ndFilterWeightClean");
        QLineEdit *m2d = filterMassDialog->findChild<QLineEdit *>("lineEdit_2ndFilterWeightDirty");

        if ( (!m1c) || (!m1d) || (!m2c) || (!m2d) ) {

            QMessageBox::critical(0, "Qr49", tr("on_pushButton_EnterPTmass_clicked: child object not found!"), 0, 0, 0);

            return;
        }

        double PTmass = (m1d->text().toDouble() - m1c->text().toDouble()) + (m2d->text().toDouble() - m2c->text().toDouble());

        ui->lineEdit_PTmass->setText(QString::number(PTmass, 'f', 3));
    }
}

void MainWindow::TaskChanged(QString str) {

    if (str == "points") {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);

        if ( (ui->comboBox_standard->currentText() == "EU6") ||
             (ui->comboBox_standard->currentText() == "EU5") ||
             (ui->comboBox_standard->currentText() == "EU4") ||
             (ui->comboBox_standard->currentText() == "EU3") ) {

            ui->comboBox_AddPointsCalc->setEnabled(true);
        }
        else {

            ui->comboBox_AddPointsCalc->setEnabled(false);
        }

        ui->checkBox_reports->setEnabled(false);

        ui->tab_Reports->setEnabled(false);
        ui->tab_RedPower->setEnabled(false);

        if (ui->comboBox_standard->currentText() == "FreeCalc") {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
        else if ( (ui->comboBox_standard->currentText() == "EU6") ||
                  (ui->comboBox_standard->currentText() == "EU5") ||
                  (ui->comboBox_standard->currentText() == "EU4") ||
                  (ui->comboBox_standard->currentText() == "EU3") ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(true); ui->tableWidget_SrcDataEU3->setFocus(); GetUndoRedoCounters(table);
        }
        else {

            ui->tableWidget_SrcDataEU0->setEnabled(true); ui->tableWidget_SrcDataEU0->setFocus(); GetUndoRedoCounters(table);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }

        ui->tableWidget_SrcDataPoints->setEnabled(false);

        ui->action_OpenReport->setEnabled(false);
        ui->action_SaveReportAs->setEnabled(false);
        ui->action_CloseReport->setEnabled(false);
        ui->action_PrintReport->setEnabled(false);
        ui->action_AddRow->setEnabled(false);
        ui->action_DeleteRow->setEnabled(false);

        ui->tabWidget_Data->setCurrentIndex(0);
    }
    else if (str == "emissions") {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);

        QString std = ui->comboBox_standard->currentText();
        if ( (std == "C1") || (std == "D1") || (std == "D2") ||
             (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
             (std == "F1") || (std == "G1") || (std == "G2") ) {

            ui->comboBox_FuelType->setEnabled(true);
        }
        else {

            ui->comboBox_FuelType->setEnabled(false);
        }

        ui->comboBox_NOxSample->setEnabled(true);
        ui->comboBox_PTcalc->setEnabled(true);

        if (ui->comboBox_PTcalc->currentText() == "ThroughPTmass") {

            ui->lineEdit_PTmass->setEnabled(true);
            ui->pushButton_EnterPTmass->setEnabled(true);
        }
        else {

            ui->lineEdit_PTmass->setEnabled(false);
            ui->pushButton_EnterPTmass->setEnabled(false);
        }

        ui->comboBox_AddPointsCalc->setEnabled(true);

        ui->checkBox_reports->setEnabled(true);

        ui->tab_Reports->setEnabled(true);
        ui->tab_RedPower->setEnabled(false);

        ui->tableWidget_SrcDataEU0->setEnabled(false);
        ui->tableWidget_SrcDataEU3->setEnabled(false);

        ui->tableWidget_SrcDataPoints->setEnabled(true); ui->tableWidget_SrcDataPoints->setFocus(); GetUndoRedoCounters(table);

        ui->action_OpenReport->setEnabled(true);
        ui->action_SaveReportAs->setEnabled(true);
        ui->action_CloseReport->setEnabled(true);
        ui->action_PrintReport->setEnabled(true);
        ui->action_AddRow->setEnabled(true);
        ui->action_DeleteRow->setEnabled(true);

        ui->tabWidget_Data->setCurrentIndex(0);
    }
    else if (str == "ReducedPower") {

        ui->lineEdit_Vh->setEnabled(true);
        ui->comboBox_standard->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);

        ui->tab_Reports->setEnabled(true);
        ui->tab_RedPower->setEnabled(true); ui->tableWidget_FullLoadCurve->setFocus(); GetUndoRedoCounters(table);

        ui->tableWidget_SrcDataEU0->setEnabled(false);
        ui->tableWidget_SrcDataEU3->setEnabled(false);

        ui->tableWidget_SrcDataPoints->setEnabled(false);

        ui->action_OpenReport->setEnabled(true);
        ui->action_SaveReportAs->setEnabled(true);
        ui->action_CloseReport->setEnabled(true);
        ui->action_PrintReport->setEnabled(true);
        ui->action_AddRow->setEnabled(true);
        ui->action_DeleteRow->setEnabled(true);

        ui->tabWidget_Data->setCurrentIndex(2);
    }
}

void MainWindow::StandardChanged(QString str) {

    if (ui->comboBox_task->currentText() == "ReducedPower") {

        ui->comboBox_AddPointsCalc->setEnabled(false);
        return;
    }

    if ( (str == "EU6") || (str == "EU5") || (str == "EU4") || (str == "EU3") ) {

        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(true);

        if (ui->comboBox_task->currentText() == "points") {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(true); ui->tableWidget_SrcDataEU3->setFocus(); GetUndoRedoCounters(table);
        }
    }
    else if ( (str == "C1") || (str == "D1") || (str == "D2") || (str == "E1") || (str == "E2") || (str == "E3") || (str == "E5") ||
              (str == "F") || (str == "G1") || (str == "G2") ) {

        ui->comboBox_FuelType->setEnabled(true);
    }
    else {

        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        if (ui->comboBox_task->currentText() == "points") {

            ui->tableWidget_SrcDataEU0->setEnabled(true); ui->tableWidget_SrcDataEU0->setFocus(); GetUndoRedoCounters(table);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
    }
}

void MainWindow::PTcalcChanged(QString str) {

    if ( (str == "ThroughSmoke") || (str == "no") || (ui->comboBox_task->currentText() != "emissions") ) {

        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
    }
    else {

        ui->lineEdit_PTmass->setEnabled(true);
        ui->pushButton_EnterPTmass->setEnabled(true);
    }
}

void MainWindow::ReportChanged(QString path) {

    QFile reportFile(path);
    QString myreport;

    if (reportFile.open(QIODevice::ReadOnly)) {

        myreport.append(reportFile.readAll());
    }
    else {

        QMessageBox::critical(0, "Qr49", tr("ReportChanged: can not open report file!"), 0, 0, 0);

        return;
    }

    reportFile.close();

    string stdstrrep = myreport.toStdString();

    ui->plainTextEdit_Report->setPlainText(QString::fromLocal8Bit(stdstrrep.c_str()));
}

void MainWindow::TabChanged(int tab) {

    if (tab == 1) {

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

        if (undoCount == 0) {

            ui->action_UndoTable->setEnabled(false);
        }
        else {

            ui->action_UndoTable->setEnabled(true);
        }

        if (redoCount == 0) {

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

void MainWindow::ArithmeticOperationIsAvailable(bool b) {

    ui->action_Add->setEnabled(b);
    ui->action_Multiply->setEnabled(b);
    ui->action_Divide->setEnabled(b);
    ui->action_Equal->setEnabled(b);
}

void MainWindow::TableCellChanged(int n, int m) {

    QString str = table->item(n, m)->text();
    int pos = 0;

    if ( regExpValidator->validate(str, pos) == QValidator::Invalid ) {

        QMessageBox::warning(0, "Qr49", tr("Illegal table cell value!"), 0, 0, 0);

        table->item(n, m)->setText("0");

        return;
    }

    SaveState();
}

void MainWindow::GetUndoRedoCounters(QTableWidget *tbl) {

    if (tbl == ui->tableWidget_SrcDataEU0) {

        undoCount = undoRedo_TableEU0->GetUndoTableNumber();
        redoCount = undoRedo_TableEU0->GetRedoTableNumber();

        SetUndoRedoButtonState();
    }
    else if (tbl == ui->tableWidget_SrcDataEU3) {

        undoCount = undoRedo_TableEU3->GetUndoTableNumber();
        redoCount = undoRedo_TableEU3->GetRedoTableNumber();

        SetUndoRedoButtonState();
    }
    else if (tbl == ui->tableWidget_SrcDataPoints) {

        undoCount = undoRedo_TablePoints->GetUndoTableNumber();
        redoCount = undoRedo_TablePoints->GetRedoTableNumber();

        SetUndoRedoButtonState();
    }
    else if (tbl == ui->tableWidget_FullLoadCurve) {

        undoCount = undoRedo_TableFullLoadCurve->GetUndoTableNumber();
        redoCount = undoRedo_TableFullLoadCurve->GetRedoTableNumber();

        SetUndoRedoButtonState();
    }
}

void MainWindow::SetUndoRedoButtonState() {

    QString ttUndo(tr("Undo"));
    QString ttRedo(tr("Redo"));

    ttUndo += " (" + QString::number(undoCount) + ")";
    ttRedo += " (" + QString::number(redoCount) + ")";

    if (undoCount < 1) {

        ui->action_UndoTable->setEnabled(false);
        ui->action_UndoTable->setToolTip(ttUndo);
    }
    else {

        ui->action_UndoTable->setEnabled(true);
        ui->action_UndoTable->setToolTip(ttUndo);
    }

    if (redoCount < 1) {

        ui->action_RedoTable->setEnabled(false);
        ui->action_RedoTable->setToolTip(ttRedo);
    }
    else {

        ui->action_RedoTable->setEnabled(true);
        ui->action_RedoTable->setToolTip(ttRedo);
    }
}

void MainWindow::SaveState() {

    if (table == ui->tableWidget_SrcDataEU0) {

        undoRedo_TableEU0->SaveState();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        undoRedo_TableEU3->SaveState();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        undoRedo_TablePoints->SaveState();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        undoRedo_TableFullLoadCurve->SaveState();
        GetUndoRedoCounters(table);
    }
}

void MainWindow::SaveStateForAllTables() {

    undoRedo_TableEU0->SaveState();
    undoRedo_TableEU3->SaveState();
    undoRedo_TablePoints->SaveState();
    undoRedo_TableFullLoadCurve->SaveState();
}

void MainWindow::on_action_UndoTable_activated() {

    TableCellChangedConnect(false);

    if (table == ui->tableWidget_SrcDataEU0) {

        undoRedo_TableEU0->UndoTable();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        undoRedo_TableEU3->UndoTable();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        undoRedo_TablePoints->UndoTable();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        undoRedo_TableFullLoadCurve->UndoTable();
        GetUndoRedoCounters(table);
    }

    TableCellChangedConnect(true);
}

void MainWindow::on_action_RedoTable_activated() {

    TableCellChangedConnect(false);

    if (table == ui->tableWidget_SrcDataEU0) {

        undoRedo_TableEU0->RedoTable();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        undoRedo_TableEU3->RedoTable();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        undoRedo_TablePoints->RedoTable();
        GetUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        undoRedo_TableFullLoadCurve->RedoTable();
        GetUndoRedoCounters(table);
    }

    TableCellChangedConnect(true);
}
