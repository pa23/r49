/*
    qr49.cpp

    Copyright (C) 2009, 2010, 2011 Artem Petrov <pa2311@gmail.com>

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
#include "stringoperations.h"

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
#include <QDebug>
#include <QFontDatabase>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

MainWindow::MainWindow(QWidget *parent) :

        QMainWindow(parent),
        ui(new Ui::MainWindow),

        qr49settings("pa23software", "Qr49"),

        contextMenu(new QMenu()),

        filterMassDialog(new FilterMassDialog()),
        valueDialog(new ValueDialog()),
        preferencesDialog(new PreferencesDialog()),
        abcSpeedsCalcDialog(new ABCspeedsCalcDialog()),
        elrSmokeCalcDialog(new ELRsmokeCalcDialog()),
        checkoutDataDialog(new CheckoutDataDialog()),
        helpDialog(new HelpDialog()),
        dataImportDialog(new DataImportDialog()),

        regExp("[-+]?[0-9]*[.,]?[0-9]+([eE][-+]?[0-9]+)?"),

        undoCount(0),
        redoCount(0) {

    ui->setupUi(this);

    //

    this->setWindowTitle(qr49version);

    contextMenu->addMenu(ui->menuFile);
    contextMenu->addMenu(ui->menuEdit);
    contextMenu->addMenu(ui->menuCalculation);

    //

    connect(ui->comboBox_task, SIGNAL(activated(int)), this, SLOT(taskChanged(int)));
    connect(ui->comboBox_standard, SIGNAL(activated(int)), this, SLOT(standardChanged(int)));
    connect(ui->comboBox_PTcalc, SIGNAL(activated(int)), this, SLOT(PTcalcChanged(int)));
    connect(ui->comboBox_OpenedReports, SIGNAL(activated(QString)), this, SLOT(reportChanged(QString)));
    connect(ui->tabWidget_Data, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

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

    undoRedo_TableEU0 = QSharedPointer<UndoRedoTable>(new UndoRedoTable(ui->tableWidget_SrcDataEU0));
    undoRedo_TableEU3 = QSharedPointer<UndoRedoTable>(new UndoRedoTable(ui->tableWidget_SrcDataEU3));
    undoRedo_TablePoints = QSharedPointer<UndoRedoTable>(new UndoRedoTable(ui->tableWidget_SrcDataPoints));
    undoRedo_TableFullLoadCurve = QSharedPointer<UndoRedoTable>(new UndoRedoTable(ui->tableWidget_FullLoadCurve));

    saveStateForAllTables();

    //

    readProgramSettings();

    taskChanged(ui->comboBox_task->currentIndex());
    standardChanged(ui->comboBox_standard->currentIndex());
    PTcalcChanged(ui->comboBox_PTcalc->currentIndex());

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

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
            return;
        }
        myPlainTextEdit_CheckoutData->setFont(monospacedFont_10);
    }

    //

    newVersions = QSharedPointer<NewVersions>(new NewVersions());
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
    delete abcSpeedsCalcDialog;
    delete elrSmokeCalcDialog;
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

    if (ui->action_Toolbar->isChecked()) {

        ui->toolBar->setVisible(true);
    }
    else {

        ui->toolBar->setVisible(false);
    }
}

void MainWindow::tableCellChangedConnect(bool b) {

    if (b) {

        connect(ui->tableWidget_SrcDataEU0, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
        connect(ui->tableWidget_SrcDataEU3, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
        connect(ui->tableWidget_SrcDataPoints, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
        connect(ui->tableWidget_FullLoadCurve, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
    }
    else {

        disconnect(ui->tableWidget_SrcDataEU0, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
        disconnect(ui->tableWidget_SrcDataEU3, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
        disconnect(ui->tableWidget_SrcDataPoints, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
        disconnect(ui->tableWidget_FullLoadCurve, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

    if ( object == ui->tableWidget_SrcDataEU0 ||
         object == ui->tableWidget_SrcDataEU3 ||
         object == ui->tableWidget_SrcDataPoints ||
         object == ui->tableWidget_FullLoadCurve ) {

        if ( event->type() == QEvent::FocusIn ) {

            table = (QTableWidget*)object;
        }
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

    if (!config.data()->readConfigFile(configFileName)) {

        QMessageBox::warning(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false! Default values will be used."), 0, 0, 0);
    }
}

void MainWindow::loadAllSourceData() {

    QString filenameSourceEU0 = config.data()->val_filenameSourceEU0();

    if ( QFile::exists(filenameSourceEU0) ) {

        tableCellChangedConnect(false);

        if (!fillTableEU0(filenameSourceEU0)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
        }

        tableCellChangedConnect(true);
    }
    else {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenameSourceEU0 + tr(" not found!"), 0, 0, 0);
    }

    //

    QString filenameSourceEU3 = config.data()->val_filenameSourceEU3();

    if ( QFile::exists(filenameSourceEU3) ) {

        tableCellChangedConnect(false);

        if (!fillTableEU3(filenameSourceEU3)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
        }

        tableCellChangedConnect(true);
    }
    else {

        QMessageBox::warning(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenameSourceEU3 + tr(" not found!"), 0, 0, 0);
    }

    //

    QString filenamePoints = config.data()->val_filenamePoints();

    if ( QFile::exists(filenamePoints) ) {

        tableCellChangedConnect(false);

        if (!fillTablePoints(filenamePoints)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
        }

        tableCellChangedConnect(true);
    }
    else {

        QMessageBox::warning(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenamePoints + tr(" not found!"), 0, 0, 0);
    }

    //

    QString filenamePowers = config.data()->val_filenamePowers();

    if ( QFile::exists(filenamePowers) ) {

        tableCellChangedConnect(false);

        if (!fillTableFullLoadCurve(filenamePowers)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
        }

        tableCellChangedConnect(true);
    }
    else {

        QMessageBox::warning(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenamePowers + tr(" not found!"), 0, 0, 0);
    }
}

bool MainWindow::fillTableEU0(QString filename) {

    ui->tableWidget_SrcDataEU0->setRowCount(1);
    ui->tableWidget_SrcDataEU0->setRowHeight(0, tableRowHeight);

    QSharedPointer<csvRead> readerSourceDataEU0(new csvRead(filename, config.data()->val_csvDelimiter(), StrsNumberForColumnCaption));

    QVector< QVector<double> > arraySourceDataEU0 = readerSourceDataEU0.data()->csvData();

    if (arraySourceDataEU0.at(0).size() != EU0SrcDataParamsNumber) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Incorrect source data!"), 0, 0, 0);

        return false;
    }

    for (ptrdiff_t j=0; j<arraySourceDataEU0.at(0).size(); j++) {

        ui->tableWidget_SrcDataEU0->setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU0.at(0).at(j), 'f', 3)));
        ui->tableWidget_SrcDataEU0->item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    return true;
}

bool MainWindow::fillTableEU3(QString filename) {

    ui->tableWidget_SrcDataEU3->setRowCount(1);
    ui->tableWidget_SrcDataEU3->setRowHeight(0, tableRowHeight);

    QSharedPointer<csvRead> readerSourceDataEU3(new csvRead(filename, config.data()->val_csvDelimiter(), StrsNumberForColumnCaption));

    QVector< QVector<double> > arraySourceDataEU3 = readerSourceDataEU3.data()->csvData();

    if (arraySourceDataEU3.at(0).size() != EU3SrcDataParamsNumber) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Incorrect source data!"), 0, 0, 0);

        return false;
    }

    for (ptrdiff_t j=0; j<arraySourceDataEU3.at(0).size(); j++) {

        ui->tableWidget_SrcDataEU3->setItem(0, j, new QTableWidgetItem(QString::number(arraySourceDataEU3.at(0).at(j), 'f', 3)));
        ui->tableWidget_SrcDataEU3->item(0, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    return true;
}

bool MainWindow::fillTablePoints(QString filename) {

    QSharedPointer<csvRead> readerSourceDataPoints(new csvRead(filename, config.data()->val_csvDelimiter(), StrsNumberForColumnCaption));

    QVector< QVector<double> > arraySourceDataPoints = readerSourceDataPoints.data()->csvData();

    if (arraySourceDataPoints.at(0).size() != PointsFileColumnsNumber) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Incorrect source data!"), 0, 0, 0);

        return false;
    }

    for (ptrdiff_t i=0; i<arraySourceDataPoints.size(); i++) {

        ui->tableWidget_SrcDataPoints->setRowCount(i+1);
        ui->tableWidget_SrcDataPoints->setRowHeight(i, tableRowHeight);
        ui->tableWidget_SrcDataPoints->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_SrcDataPoints->setItem(i, 0, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][0], 'f', 0)));
        ui->tableWidget_SrcDataPoints->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_SrcDataPoints->setItem(i, 1, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][1], 'f', 0)));
        ui->tableWidget_SrcDataPoints->item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (ptrdiff_t j=2; j<arraySourceDataPoints.at(i).size(); j++) {

            ui->tableWidget_SrcDataPoints->setItem(i, j, new QTableWidgetItem(QString::number(arraySourceDataPoints[i][j], 'f', 3)));
            ui->tableWidget_SrcDataPoints->item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return true;
}

bool MainWindow::fillTableFullLoadCurve(QString filename) {

    QSharedPointer<csvRead> readerFullLoadCurve(new csvRead(filename, config.data()->val_csvDelimiter(), StrsNumberForColumnCaption));

    QVector< QVector<double> > arrayFullLoadCurve = readerFullLoadCurve.data()->csvData();

    if (arrayFullLoadCurve.at(0).size() != PowersFileColumnsNumber) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Incorrect source data!"), 0, 0, 0);

        return false;
    }

    for (ptrdiff_t i=0; i<arrayFullLoadCurve.size(); i++) {

        ui->tableWidget_FullLoadCurve->setRowCount(i+1);
        ui->tableWidget_FullLoadCurve->setRowHeight(i, tableRowHeight);

        ui->tableWidget_FullLoadCurve->setItem(i, 0, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][0], 'f', 0)));
        ui->tableWidget_FullLoadCurve->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget_FullLoadCurve->setItem(i, 1, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][1], 'f', 0)));
        ui->tableWidget_FullLoadCurve->item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (ptrdiff_t j=2; j<arrayFullLoadCurve.at(i).size(); j++) {

            ui->tableWidget_FullLoadCurve->setItem(i, j, new QTableWidgetItem(QString::number(arrayFullLoadCurve[i][j], 'f', 3)));
            ui->tableWidget_FullLoadCurve->item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return true;
}

bool MainWindow::fillParameters() {

    params.data()->setTask(ui->comboBox_task->currentIndex());
    double Vh = ui->lineEdit_Vh->text().toDouble(); params.data()->setVh(&Vh);
    params.data()->setStandard(ui->comboBox_standard->currentIndex());
    params.data()->setChargingType(ui->comboBox_chargingType->currentIndex());
    params.data()->setFuelType(ui->comboBox_FuelType->currentIndex());
    params.data()->setNOxSample(ui->comboBox_NOxSample->currentIndex());
    params.data()->setPTcalc(ui->comboBox_PTcalc->currentIndex());
    double PTMass = ui->lineEdit_PTmass->text().toDouble(); params.data()->setPTmass(&PTMass);
    params.data()->setAddPointsCalc(ui->comboBox_AddPointsCalc->currentIndex());

    return true;
}

bool MainWindow::arithmeticOperation(QString operation) {

    QLineEdit *value = valueDialog->findChild<QLineEdit *>("lineEdit_Value");

    if (!value) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);

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

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Unknown arithmetic operation!"), 0, 0, 0);

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

        tableCellChangedConnect(false);

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

        tableCellChangedConnect(true);
    }

    saveState();

    return true;
}

void MainWindow::on_action_DataImport_activated() {

    tableCellChangedConnect(false);

    if (table == ui->tableWidget_SrcDataPoints) {

        dataImportDialog->SetDestinationTable(2, table);
        dataImportDialog->exec();
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        dataImportDialog->SetDestinationTable(3, table);
        dataImportDialog->exec();
    }
    else {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Data import is not available for the current table!"), 0, 0, 0);
    }

    tableCellChangedConnect(true);

    saveState();
}

void MainWindow::on_action_LoadSourceData_activated() {

    QString dir(config.data()->val_dirnameReports());

    QString anotherSourceFile(QFileDialog::getOpenFileName(
            this,
            tr("Open Source Data File..."),
            dir,
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (table == ui->tableWidget_SrcDataEU0) {

        ui->tabWidget_Data->setCurrentIndex(0);

        tableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!fillTableEU0(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
            }
        }

        tableCellChangedConnect(true);

        saveState();
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        ui->tabWidget_Data->setCurrentIndex(0);

        tableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!fillTableEU3(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
            }
        }

        tableCellChangedConnect(true);

        saveState();
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        ui->tabWidget_Data->setCurrentIndex(1);

        tableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!fillTablePoints(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
            }
        }

        tableCellChangedConnect(true);

        saveState();
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        ui->tabWidget_Data->setCurrentIndex(2);

        tableCellChangedConnect(false);

        if (!anotherSourceFile.isEmpty()) {

            if (!fillTableFullLoadCurve(anotherSourceFile)) {

                QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
            }
        }

        tableCellChangedConnect(true);

        saveState();
    }
}

void MainWindow::on_action_SaveSourceData_activated() {

    if (table == ui->tableWidget_SrcDataEU0) {

        QString filenameSourceEU0 = config.data()->val_filenameSourceEU0();

        QFile SrcDataEU0File(filenameSourceEU0);

        if (!SrcDataEU0File.open(QIODevice::WriteOnly)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenameSourceEU0 + tr(" could not be opened!"), 0, 0, 0);

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

        QString filenameSourceEU3 = config.data()->val_filenameSourceEU3();

        QFile SrcDataEU3File(filenameSourceEU3);

        if (!SrcDataEU3File.open(QIODevice::WriteOnly)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenameSourceEU3 + tr(" could not be opened!"), 0, 0, 0);

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

        QString filenamePoints = config.data()->val_filenamePoints();

        QFile SrcDataPointsFile(filenamePoints);

        if (!SrcDataPointsFile.open(QIODevice::WriteOnly)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenamePoints + tr(" could not be opened!"), 0, 0, 0);

            return;
        }

        SrcDataPointsFile.write("Point[-];n[min-1];Me_b[Nm];Ne_b[kW];N_fan[kW];w[-];t0[oC];B0[kPa];Ra[%];dPn[mmH2O];Gair[kg/h];Gfuel[kg/h];C_NOx[ppm];gNOx[g/kWh];C_CO[ppm];C_CH[ppm];C_CO2in[%];C_CO2out[%];C_O2[%];Ka[m-1];Ka[%];FSN[-];Pr[kPa];ts[oC];tauf[s];qmdw[g/s];qmdew[g/s];rd[-];\n");

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

        QString filenamePowers = config.data()->val_filenamePowers();

        QFile SrcDataPowersFile(filenamePowers);

        if (!SrcDataPowersFile.open(QIODevice::WriteOnly)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenamePowers + tr(" could not be opened!"), 0, 0, 0);

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

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + newSourceDataFileName + tr(" could not be opened!"), 0, 0, 0);

            return;
        }

        if (table == ui->tableWidget_SrcDataEU0) {

            SrcDataFile.write("idle[min-1];n_interim[min-1];n_rated[min-1];N_fan_rated[kW];Ne_interim[Nm];Ne_rated[Nm];\n");
        }
        else if (table == ui->tableWidget_SrcDataEU3) {

            SrcDataFile.write("n_hi[min-1];n_lo[min-1];idle[min-1];n_rated[min-1];N_fan_rated[kW];Ne_A[Nm];Ne_B[Nm];Ne_C[Nm];Ne_a1[Nm];Ne_a2[Nm];Ne_a3[Nm];\n");
        }
        else if (table == ui->tableWidget_SrcDataPoints) {

            SrcDataFile.write("Point[-];n[min-1];Me_b[Nm];Ne_b[kW];N_fan[kW];w[-];t0[oC];B0[kPa];Ra[%];dPn[mmH2O];Gair[kg/h];Gfuel[kg/h];C_NOx[ppm];gNOx[g/kWh];C_CO[ppm];C_CH[ppm];C_CO2in[%];C_CO2out[%];C_O2[%];Ka[m-1];Ka[%];FSN[-];Pr[kPa];ts[oC];tauf[s];qmdw[g/s];qmdew[g/s];rd[-];\n");
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

    QString dir(config.data()->val_dirnameReports());

    QString anotherOptions(QFileDialog::getOpenFileName(
            this,
            tr("Open Calculation Options File..."),
            dir,
            QString::fromAscii("Config files (*.conf);;All files (*.*)"),
            0,
            0));

    if (!anotherOptions.isEmpty()) {

        params.data()->readCalcConfigFile(anotherOptions);

        ui->comboBox_task->setCurrentIndex(params.data()->val_Task());
        ui->lineEdit_Vh->setText(QString::number(params.data()->val_Vh()));
        ui->comboBox_standard->setCurrentIndex(params.data()->val_Standard());
        ui->comboBox_chargingType->setCurrentIndex(params.data()->val_ChargingType());
        ui->comboBox_FuelType->setCurrentIndex(params.data()->val_FuelType());
        ui->comboBox_NOxSample->setCurrentIndex(params.data()->val_NOxSample());
        ui->comboBox_PTcalc->setCurrentIndex(params.data()->val_PTcalc());
        ui->lineEdit_PTmass->setText(QString::number(params.data()->val_PTmass()));
        ui->comboBox_AddPointsCalc->setCurrentIndex(params.data()->val_AddPointsCalc());

        taskChanged(ui->comboBox_task->currentIndex());
        standardChanged(ui->comboBox_standard->currentIndex());
        PTcalcChanged(ui->comboBox_PTcalc->currentIndex());
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

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + optionsFileName + tr(" could not be opened!"), 0, 0, 0);

            return;
        }

        savedOptions.write("task");           savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_task->currentIndex()));             savedOptions.write("\n");
        savedOptions.write("Vh");             savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->lineEdit_Vh->text().toAscii());             savedOptions.write("\n");
        savedOptions.write("standard");       savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_standard->currentIndex()));         savedOptions.write("\n");
        savedOptions.write("ChargingType");   savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_chargingType->currentIndex()));     savedOptions.write("\n");
        savedOptions.write("FuelType");       savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_FuelType->currentIndex()));         savedOptions.write("\n");
        savedOptions.write("NOxSample");      savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_NOxSample->currentIndex()));        savedOptions.write("\n");
        savedOptions.write("PTcalc");         savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_PTcalc->currentIndex()));           savedOptions.write("\n");
        savedOptions.write("PTmass");         savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(ui->lineEdit_PTmass->text().toAscii());         savedOptions.write("\n");
        savedOptions.write("AddPointsCalc");  savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(numberToCChar(ui->comboBox_AddPointsCalc->currentIndex()));    savedOptions.write("\n");
        savedOptions.write("CalcConfigFile"); savedOptions.write(parameterValueDelimiter.toAscii()); savedOptions.write(params.data()->val_CalcConfigFile().toAscii()); savedOptions.write("\n");

        savedOptions.close();
    }
}

void MainWindow::on_action_OpenReport_activated() {

    QString dir(config.data()->val_dirnameReports());

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
        reportChanged(anotherReport);
        ui->tabWidget_Data->setCurrentIndex(3);
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

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + newReportFileName + tr(" could not be saved!"), 0, 0, 0);

            return;
        }

        QTextStream fout(&reportFile);
        //fout.setCodec("UTF-8");
        fout << myreport;

        reportFile.close();

        ui->comboBox_OpenedReports->removeItem(ui->comboBox_OpenedReports->currentIndex());
        ui->comboBox_OpenedReports->insertItem(0, newReportFileName);
        ui->comboBox_OpenedReports->setCurrentIndex(0);
        reportChanged(newReportFileName);
        ui->tabWidget_Data->setCurrentIndex(3);
    }
}

void MainWindow::on_action_CloseReport_activated() {

    if (ui->comboBox_OpenedReports->count() != 1) {

        ui->comboBox_OpenedReports->removeItem(ui->comboBox_OpenedReports->currentIndex());
    }

    reportChanged(ui->comboBox_OpenedReports->currentText());
    ui->tabWidget_Data->setCurrentIndex(3);
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

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenameSourceEU3 = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU3");

    if (!myLineEdit_filenameSourceEU3) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenameSourceEU0 = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenameSourceEU0");

    if (!myLineEdit_filenameSourceEU0) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenamePoints = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePoints");

    if (!myLineEdit_filenamePoints) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_filenamePowers = preferencesDialog->findChild<QLineEdit *>("lineEdit_filenamePowers");

    if (!myLineEdit_filenamePowers) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QLineEdit *myLineEdit_dirnameReports = preferencesDialog->findChild<QLineEdit *>("lineEdit_dirnameReports");

    if (!myLineEdit_dirnameReports) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_Dn = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Dn");

    if (!myDoubleSpinBox_Dn) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_ConcO2air = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcO2air");

    if (!myDoubleSpinBox_ConcO2air) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_Rr = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_Rr");

    if (!myDoubleSpinBox_Rr) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_L0 = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L0");

    if (!myDoubleSpinBox_L0) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_L = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_L");

    if (!myDoubleSpinBox_L) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_ConcCO2air = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_ConcCO2air");

    if (!myDoubleSpinBox_ConcCO2air) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_WH = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WH");

    if (!myDoubleSpinBox_WH) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_WO2 = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WO2");

    if (!myDoubleSpinBox_WO2) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_WN = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_WN");

    if (!myDoubleSpinBox_WN) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_roAir = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_roAir");

    if (!myDoubleSpinBox_roAir) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_muNO2 = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muNO2");

    if (!myDoubleSpinBox_muNO2) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_muCO = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCO");

    if (!myDoubleSpinBox_muCO) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    QDoubleSpinBox *myDoubleSpinBox_muCH = preferencesDialog->findChild<QDoubleSpinBox *>("doubleSpinBox_muCH");

    if (!myDoubleSpinBox_muCH) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);
        return;
    }

    //

    for (ptrdiff_t i=0; i<myComboBox_csvdelimiter->count(); i++) {

        if (myComboBox_csvdelimiter->itemText(i) == config.data()->val_csvDelimiter()) {

            myComboBox_csvdelimiter->setCurrentIndex(i);
            break;
        }
    }

    myLineEdit_filenameSourceEU3->setText(config.data()->val_filenameSourceEU3());
    myLineEdit_filenameSourceEU0->setText(config.data()->val_filenameSourceEU0());
    myLineEdit_filenamePoints->setText(config.data()->val_filenamePoints());
    myLineEdit_filenamePowers->setText(config.data()->val_filenamePowers());
    myLineEdit_dirnameReports->setText(config.data()->val_dirnameReports());
    myDoubleSpinBox_Dn->setValue(config.data()->val_Dn());
    myDoubleSpinBox_ConcO2air->setValue(config.data()->val_ConcO2air());
    myDoubleSpinBox_Rr->setValue(config.data()->val_Rr());
    myDoubleSpinBox_L0->setValue(config.data()->val_L0());
    myDoubleSpinBox_L->setValue(config.data()->val_L());
    myDoubleSpinBox_ConcCO2air->setValue(config.data()->val_ConcCO2air());
    myDoubleSpinBox_WH->setValue(config.data()->val_WH());
    myDoubleSpinBox_WO2->setValue(config.data()->val_WO2());
    myDoubleSpinBox_WN->setValue(config.data()->val_WN());
    myDoubleSpinBox_roAir->setValue(config.data()->val_roAir());
    myDoubleSpinBox_muNO2->setValue(config.data()->val_muNO2());
    myDoubleSpinBox_muCO->setValue(config.data()->val_muCO());
    myDoubleSpinBox_muCH->setValue(config.data()->val_muCH());

    //

    if (preferencesDialog->exec() == QDialog::Accepted) {

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

    if ( (table->columnCount() - table->currentColumn()) < numColumns ) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Copied data can not be inserted!"), 0, 0, 0);

        return;
    }

    //

    ptrdiff_t destRows = table->rowCount() - table->currentRow();
    ptrdiff_t totalRows = table->currentRow() + numRows;

    tableCellChangedConnect(false);

    if ( numRows > destRows ) {

        addRows(table, totalRows);
    }

    //

    for (ptrdiff_t i=0; i<numRows; i++) {

        QStringList columns = rows[i].split('\t');

        for (ptrdiff_t j=0; j<numColumns; j++) {

            table->item(table->currentRow()+i, table->currentColumn()+j)->setText(columns[j]);
        }
    }

    tableCellChangedConnect(true);

    saveState();
}

void MainWindow::on_action_DeleteFromTable_activated() {

    if ( !table->selectedRanges().isEmpty() ) {

        QTableWidgetSelectionRange selectedRange = table->selectedRanges().first();

        tableCellChangedConnect(false);

        for (ptrdiff_t i=0; i<selectedRange.rowCount(); i++) {

            for (ptrdiff_t j=0; j<selectedRange.columnCount(); j++) {

                table->item(selectedRange.topRow()+i, selectedRange.leftColumn()+j)->setText("0");
            }
        }

        tableCellChangedConnect(true);

        saveState();
    }
}

void MainWindow::on_action_Add_activated() {

    if (!arithmeticOperation("add")) {

        QMessageBox::critical(0, "Qr49", tr("Arithmetic operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_Multiply_activated() {

    if (!arithmeticOperation("multiply")) {

        QMessageBox::critical(0, "Qr49", tr("Arithmetic operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_Divide_activated() {

    if (!arithmeticOperation("divide")) {

        QMessageBox::critical(0, "Qr49", tr("Arithmetic operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_Equal_activated() {

    if (!arithmeticOperation("equal")) {

        QMessageBox::critical(0, "Qr49", tr("Operation is impossible!"), 0, 0, 0);
    }
}

void MainWindow::on_action_AddRow_activated() {

    tableCellChangedConnect(false);

    if ( (table != ui->tableWidget_SrcDataEU0) && (table != ui->tableWidget_SrcDataEU3) ) {

        addRows(table, table->rowCount()+1);
    }

    tableCellChangedConnect(true);

    saveState();
}

void MainWindow::on_action_DeleteRow_activated() {

    if ( (table != ui->tableWidget_SrcDataEU0) && (table != ui->tableWidget_SrcDataEU3) ) {

        table->setRowCount(table->rowCount()-1);
    }

    saveState();
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

    QVector< QVector<double> > array_DataForCalc;
    QVector<double> row;

    for (ptrdiff_t i=0; i<table->rowCount(); i++) {

        for (ptrdiff_t j=0; j<table->columnCount(); j++) {

            row.push_back( table->item(i, j)->text().toDouble() );
        }

        array_DataForCalc.push_back(row);
        row.clear();
    }

    //

    QString message("\nCalculation completed!\n\n");

    fillParameters();

    if (ui->comboBox_task->currentIndex() == TASK_POINTS) {

        QSharedPointer<CyclePoints> myPoints(new CyclePoints(params, config));

        if (!myPoints.data()->readCSV(array_DataForCalc)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);

            return;
        }

        if (!myPoints.data()->fillArrays()) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);

            return;
        }

        message += myPoints.data()->createReport();

        //

        QString filenamePoints = config.data()->val_filenamePoints();

        if ( QFile::exists(filenamePoints) ) {

            tableCellChangedConnect(false);

            if (!fillTablePoints(filenamePoints)) {

                QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);
            }

            tableCellChangedConnect(true);
        }
        else {

            QMessageBox::warning(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + filenamePoints + tr(" not found!"), 0, 0, 0);
        }

        //

        ui->tabWidget_Data->setCurrentIndex(1);
    }
    else if (ui->comboBox_task->currentIndex() == TASK_EMISSIONS) {

        QSharedPointer<CycleEmissions> myEmissions(new CycleEmissions(params, config));

        if (!myEmissions.data()->readCSV(array_DataForCalc)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);

            return;
        }

        if (!myEmissions.data()->calculate()) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!\nMaybe you did not enter all source data?"), 0, 0, 0);

            return;
        }

        if (ui->checkBox_reports->isChecked()) {

            message += myEmissions.data()->createReports(true);

            //

            //QDir ReportsDir(config.data()->val_dirnameReports());
            //QStringList repdirs(ReportsDir.entryList(QDir::Dirs, QDir::Time));

            //lastReportsDir.setPath(ReportsDir.absoluteFilePath(repdirs.value(1)));

            lastReportsDir = myEmissions.data()->lastReportsDir();

            QString csvfilter("*.csv");
            QStringList csvfiles(lastReportsDir.entryList(QDir::nameFiltersFromString(csvfilter), QDir::Files, QDir::Time));

            lastCheckoutDataFileName = lastReportsDir.absoluteFilePath(csvfiles.first());

            //

            if (ui->comboBox_standard->currentIndex() == STD_FREECALC) {

                ui->tabWidget_Data->setCurrentIndex(1);
            }
            else {

                ui->tabWidget_Data->setCurrentIndex(3);

                QString txtfilter("*.txt");
                QStringList reports(lastReportsDir.entryList(QDir::nameFiltersFromString(txtfilter), QDir::Files, QDir::Time));

                lastReportFileName = lastReportsDir.absoluteFilePath(reports.first());

                ui->comboBox_OpenedReports->insertItem(0, lastReportFileName);
                ui->comboBox_OpenedReports->setCurrentIndex(0);
                reportChanged(lastReportFileName);
            }
        }
        else {

            message += myEmissions.data()->createReports(false);
        }
    }
    else if (ui->comboBox_task->currentIndex() == TASK_REDUCEDPOWER) {

        QSharedPointer<ReducedPower> myReducedPower(new ReducedPower(params, config));

        if (!myReducedPower.data()->readCSV(array_DataForCalc)) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);

            return;
        }

        if (!myReducedPower.data()->reducePower()) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("returns false!"), 0, 0, 0);

            return;
        }

        message += myReducedPower.data()->createReports();

        //

        //QDir ReportsDir(config.data()->val_dirnameReports());
        //QStringList repdirs(ReportsDir.entryList(QDir::Dirs, QDir::Time));

        //lastReportsDir.setPath(ReportsDir.absoluteFilePath(repdirs.value(1)));

        lastReportsDir = myReducedPower.data()->lastReportsDir();

        QString csvfilter("*.csv");
        QStringList csvfiles(lastReportsDir.entryList(QDir::nameFiltersFromString(csvfilter), QDir::Files, QDir::Time));

        lastCheckoutDataFileName = lastReportsDir.absoluteFilePath(csvfiles.first());
    }
    else if (ui->comboBox_task->currentIndex() == TASK_ABCSPEEDS) {

        on_action_ABCspeeds_activated();
        return;
    }
    else if (ui->comboBox_task->currentIndex() == TASK_ELRSMOKE) {

        on_action_ELRsmoke_activated();
        return;
    }
    else if (ui->comboBox_task->currentIndex() == TASK_HELP) {

        QMessageBox::information(0, "Qr49", tr("See the output of the command line by running the Qr49 with parameter task=help."), 0, 0, 0);
        return;
    }
    else {

        QMessageBox::critical(0, "Qr49", tr("Unknown calculation task!"), 0, 0, 0);
        return;
    }

    QMessageBox::information(0, "Qr49", message, 0, 0, 0);

    if ( (ui->comboBox_standard->currentIndex() == STD_FREECALC) || (ui->comboBox_task->currentIndex() == TASK_REDUCEDPOWER) ) {

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
    QLineEdit *myLineEdit_file = checkoutDataDialog->findChild<QLineEdit *>("lineEdit_file");

    if (!myPlainTextEdit_CheckoutData) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);

        return;
    }

    QFile arrayFile(lastCheckoutDataFileName);
    QString data;

    if (arrayFile.open(QIODevice::ReadOnly)) {

        data.append(arrayFile.readAll());
    }
    else {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Can not open file!"), 0, 0, 0);

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

    QString str = "<b>r49 distribution version " + r49version + "</b><br><br>" + qr49version + ", libtoxic v" + libtoxicVersion +
                  "<br><br>Calculation of modes and specific emissions for stationary diesel engine test cycles (UN ECE Regulation No. 49, UN ECE Regulation No. 96, UN ECE Regulation No. 85, OST 37.001.234-81, GOST 17.2.2.05-97, GOST 30574-98, GOST R 51249-99)."
                  "<br><br>Copyright (C) 2009, 2010, 2011 Artem Petrov <a href= \"mailto:pa2311@gmail.com\" >pa2311@gmail.com</a>"
                  "<br><br>Web site: <a href= \"https://github.com/pa23/r49\">https://github.com/pa23/r49</a>"
                  "<br><br>This program is free software: you can redistribute it and/or modify "
                  "it under the terms of the GNU General Public License as published by "
                  "the Free Software Foundation, either version 3 of the License, or "
                  "(at your option) any later version. "
                  "<br>This program is distributed in the hope that it will be useful, "
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                  "GNU General Public License for more details. "
                  "<br>You should have received a copy of the GNU General Public License "
                  "along with this program. If not, see <a href= \"http://www.gnu.org/licenses/\" >http://www.gnu.org/licenses/</a>.<br>";

    QMessageBox::about(this, tr("About Qr49"), str);
}

void MainWindow::on_action_AboutQt_activated() {

    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_CheckForUpdates_activated() {

    newVersions.data()->checkAvailableVersions();
}

void MainWindow::on_pushButton_EnterPTmass_clicked() {

    if (filterMassDialog->exec() == QDialog::Accepted) {

        QLineEdit *m1c = filterMassDialog->findChild<QLineEdit *>("lineEdit_1stFilterWeightClean");
        QLineEdit *m1d = filterMassDialog->findChild<QLineEdit *>("lineEdit_1stFilterWeightDirty");
        QLineEdit *m2c = filterMassDialog->findChild<QLineEdit *>("lineEdit_2ndFilterWeightClean");
        QLineEdit *m2d = filterMassDialog->findChild<QLineEdit *>("lineEdit_2ndFilterWeightDirty");

        if ( (!m1c) || (!m1d) || (!m2c) || (!m2d) ) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);

            return;
        }

        double PTmass = (m1d->text().toDouble() - m1c->text().toDouble()) + (m2d->text().toDouble() - m2c->text().toDouble());

        ui->lineEdit_PTmass->setText(QString::number(PTmass, 'f', 3));
    }
}

void MainWindow::taskChanged(int currtask) {

    if (currtask == TASK_POINTS) {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);
        ui->comboBox_chargingType->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);

        ptrdiff_t currstd = ui->comboBox_standard->currentIndex();

        if ( (currstd == STD_EU6) || (currstd == STD_EU5) || (currstd == STD_EU4) || (currstd == STD_EU3) ) {

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

        if (currstd == STD_FREECALC) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
        else if ( (currstd == STD_EU6) || (currstd == STD_EU5) || (currstd == STD_EU4) || (currstd == STD_EU3) ) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(true); ui->tableWidget_SrcDataEU3->setFocus(); getUndoRedoCounters(table);
        }
        else {

            ui->tableWidget_SrcDataEU0->setEnabled(true); ui->tableWidget_SrcDataEU0->setFocus(); getUndoRedoCounters(table);
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
    else if (currtask == TASK_EMISSIONS) {

        ui->lineEdit_Vh->setEnabled(false);
        ui->comboBox_standard->setEnabled(true);
        ui->comboBox_chargingType->setEnabled(true);

        ptrdiff_t currstd = ui->comboBox_standard->currentIndex();

        if ( (currstd == STD_C1) || (currstd == STD_D1) || (currstd == STD_D2) ||
             (currstd == STD_E1) || (currstd == STD_E2) || (currstd == STD_E3) || (currstd == STD_E5) ||
             (currstd == STD_F ) || (currstd == STD_G1) || (currstd == STD_G2) ) {

            ui->comboBox_FuelType->setEnabled(true);
        }
        else {

            ui->comboBox_FuelType->setEnabled(false);
        }

        ui->comboBox_NOxSample->setEnabled(true);
        ui->comboBox_PTcalc->setEnabled(true);

        if (ui->comboBox_PTcalc->currentIndex() == PTCALC_THROUGHPTMASS) {

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
        ui->tableWidget_SrcDataPoints->setEnabled(true); ui->tableWidget_SrcDataPoints->setFocus(); getUndoRedoCounters(table);
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
    else if (currtask == TASK_REDUCEDPOWER) {

        ui->lineEdit_Vh->setEnabled(true);
        ui->comboBox_standard->setEnabled(false);
        ui->comboBox_chargingType->setEnabled(false);
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
        ui->tableWidget_FullLoadCurve->setEnabled(true); ui->tableWidget_FullLoadCurve->setFocus(); getUndoRedoCounters(table);

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
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_NOxSample->setEnabled(false);
        ui->comboBox_PTcalc->setEnabled(false);
        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        ui->checkBox_reports->setEnabled(false);
    }
}

void MainWindow::standardChanged(int currstd) {

    if ( (currstd == STD_EU6) || (currstd == STD_EU5) || (currstd == STD_EU4) || (currstd == STD_EU3) ) {

        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(true);

        if (ui->comboBox_task->currentIndex() == TASK_POINTS) {

            ui->tableWidget_SrcDataEU0->setEnabled(false);
            ui->tableWidget_SrcDataEU3->setEnabled(true); ui->tableWidget_SrcDataEU3->setFocus(); getUndoRedoCounters(table);
        }
    }
    else if ( (currstd == STD_C1) || (currstd == STD_D1) || (currstd == STD_D2) ||
              (currstd == STD_E1) || (currstd == STD_E2) || (currstd == STD_E3) || (currstd == STD_E5) ||
              (currstd == STD_F ) || (currstd == STD_G1) || (currstd == STD_G2) ) {

        ui->comboBox_chargingType->setEnabled(false);
        ui->comboBox_FuelType->setEnabled(true);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        if (ui->comboBox_task->currentIndex() == TASK_POINTS) {

            ui->tableWidget_SrcDataEU0->setEnabled(true); ui->tableWidget_SrcDataEU0->setFocus(); getUndoRedoCounters(table);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
    }
    else {

        ui->comboBox_chargingType->setEnabled(true);
        ui->comboBox_FuelType->setEnabled(false);
        ui->comboBox_AddPointsCalc->setEnabled(false);

        if (ui->comboBox_task->currentIndex() == TASK_POINTS) {

            ui->tableWidget_SrcDataEU0->setEnabled(true); ui->tableWidget_SrcDataEU0->setFocus(); getUndoRedoCounters(table);
            ui->tableWidget_SrcDataEU3->setEnabled(false);
        }
    }
}

void MainWindow::PTcalcChanged(int currptcalc) {

    if ( (currptcalc == PTCALC_THROUGHSMOKE) || (currptcalc == PTCALC_NO) ) {

        ui->lineEdit_PTmass->setEnabled(false);
        ui->pushButton_EnterPTmass->setEnabled(false);
    }
    else {

        ui->lineEdit_PTmass->setEnabled(true);
        ui->pushButton_EnterPTmass->setEnabled(true);
    }
}

void MainWindow::reportChanged(QString path) {

    QFile reportFile(path);
    QString myreport;

    if (reportFile.open(QIODevice::ReadOnly)) {

        myreport.append(reportFile.readAll());
    }
    else {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Can not open report file!"), 0, 0, 0);

        return;
    }

    reportFile.close();

    ui->plainTextEdit_Report->setPlainText(myreport);
}

void MainWindow::tabChanged(int tab) {

    if (tab == 3) {

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

void MainWindow::arithmeticOperationIsAvailable(bool b) {

    ui->action_Add->setEnabled(b);
    ui->action_Multiply->setEnabled(b);
    ui->action_Divide->setEnabled(b);
    ui->action_Equal->setEnabled(b);
}

void MainWindow::tableCellChanged(int n, int m) {

    QString str = table->item(n, m)->text();
    int pos = 0;

    if ( regExpValidator->validate(str, pos) == QValidator::Invalid ) {

        QMessageBox::warning(0, "Qr49", tr("Illegal table cell value!"), 0, 0, 0);

        table->item(n, m)->setText("0");

        return;
    }

    saveState();
}

void MainWindow::getUndoRedoCounters(QTableWidget *tbl) {

    if (tbl == ui->tableWidget_SrcDataEU0) {

        undoCount = undoRedo_TableEU0.data()->undoTableNumber();
        redoCount = undoRedo_TableEU0.data()->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if (tbl == ui->tableWidget_SrcDataEU3) {

        undoCount = undoRedo_TableEU3.data()->undoTableNumber();
        redoCount = undoRedo_TableEU3.data()->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if (tbl == ui->tableWidget_SrcDataPoints) {

        undoCount = undoRedo_TablePoints.data()->undoTableNumber();
        redoCount = undoRedo_TablePoints.data()->redoTableNumber();

        setUndoRedoButtonState();
    }
    else if (tbl == ui->tableWidget_FullLoadCurve) {

        undoCount = undoRedo_TableFullLoadCurve.data()->undoTableNumber();
        redoCount = undoRedo_TableFullLoadCurve.data()->redoTableNumber();

        setUndoRedoButtonState();
    }
}

void MainWindow::setUndoRedoButtonState() {

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

void MainWindow::saveState() {

    if (table == ui->tableWidget_SrcDataEU0) {

        undoRedo_TableEU0.data()->saveState();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        undoRedo_TableEU3.data()->saveState();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        undoRedo_TablePoints.data()->saveState();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        undoRedo_TableFullLoadCurve.data()->saveState();
        getUndoRedoCounters(table);
    }
}

void MainWindow::saveStateForAllTables() {

    undoRedo_TableEU0.data()->saveState();
    undoRedo_TableEU3.data()->saveState();
    undoRedo_TablePoints.data()->saveState();
    undoRedo_TableFullLoadCurve.data()->saveState();
}

void MainWindow::on_action_UndoTable_activated() {

    tableCellChangedConnect(false);

    if (table == ui->tableWidget_SrcDataEU0) {

        undoRedo_TableEU0.data()->undoTable();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        undoRedo_TableEU3.data()->undoTable();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        undoRedo_TablePoints.data()->undoTable();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        undoRedo_TableFullLoadCurve.data()->undoTable();
        getUndoRedoCounters(table);
    }

    tableCellChangedConnect(true);
}

void MainWindow::on_action_RedoTable_activated() {

    tableCellChangedConnect(false);

    if (table == ui->tableWidget_SrcDataEU0) {

        undoRedo_TableEU0.data()->redoTable();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataEU3) {

        undoRedo_TableEU3.data()->redoTable();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_SrcDataPoints) {

        undoRedo_TablePoints.data()->redoTable();
        getUndoRedoCounters(table);
    }
    else if (table == ui->tableWidget_FullLoadCurve) {

        undoRedo_TableFullLoadCurve.data()->redoTable();
        getUndoRedoCounters(table);
    }

    tableCellChangedConnect(true);
}
