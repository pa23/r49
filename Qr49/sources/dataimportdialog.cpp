/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: dataimportdialog.cpp

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

#include "dataimportdialog.h"
#include "ui_dataimportdialog.h"
#include "tablewidgetfunctions.h"

#include <QSharedPointer>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>
#include <QTableWidget>
#include <QVector>
#include <QDateTime>

#include "csvread.h"
#include "toxicerror.h"
#include "qr49constants.h"

DataImportDialog::DataImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataImportDialog),
    delimiter("\t"),
    headerLines(2),
    table_points_headers("Point[-] n[min-1] Me_b[Nm] Ne_b[kW] N_fan[kW] "
                         "w[-] t0[oC] B0[kPa] Ra[%] dPn[mmH2O] Gair[kg/h] "
                         "Gfuel[kg/h] C_NOx[ppm] gNOx[g/kWh] C_CO[ppm] "
                         "C_CH[ppm] C_CO2in[%] C_CO2out[%] C_O2[%] Ka[m-1] "
                         "Ka[%] FSN[-] Pr[kPa] ts[oC] tauf[s] qmdw[g/s] "
                         "qmdew[g/s] rd[-]"),
    table_fullLoadCurve_headers("Point[-] n[min-1] Me_b[Nm] t0[oC] B0[kPa] "
                                "Ra[%] S[kPa] pk[kPa] Gfuel[kg/h] N_k[kW] "
                                "N_fan[kW]"),
    dataDirName(QDir::currentPath()),
    table_lid(0),
    dtable(0),
    templ(),
    destTableDataChanged(false),
    templdir(),
    sj(0),
    dj(0),
    manual(true) {

    ui->setupUi(this);

    connect(ui->comboBox_delimiter,
            SIGNAL(activated(const QString &)),
            this,
            SLOT(combosUpdate(const QString &)));

    connect(ui->spinBox_HeaderLines,
            SIGNAL(valueChanged(const QString &)),
            this,
            SLOT(combosUpdate(const QString &)));

    connect(ui->lineEdit_DataFile,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(combosUpdate(const QString &)));
}

DataImportDialog::~DataImportDialog() {

    delete ui;
}

void DataImportDialog::init(const ptrdiff_t tlid,
                            QTableWidget *dt) {

    table_lid = tlid;
    dtable = dt;

    combosUpdate(0);

    //

    const QDir templdir1(TEMPLATEDIR);
    const QDir templdir2(QDir::homePath() + QDir::separator() + TEMPLATEDIR);

    if ( templdir1.exists() ) {

        templdir = templdir1;
    }
    else if ( templdir2.exists() ) {

        templdir = templdir2;
    }
    else {

        if ( templdir1.mkdir(templdir1.absolutePath()) ) {

            templdir = templdir1;
        }
        else if ( templdir2.mkdir(templdir2.absolutePath()) ) {

            templdir = templdir2;
        }
        else {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        tr("Can not create directory for templates!")
                        );
            return;
        }
    }

    updateTemplList();
}

void DataImportDialog::updateTemplList() {

    const QStringList templfiles(templdir.entryList(QDir::Files, QDir::Time));

    ui->comboBox_Templates->clear();

    for ( ptrdiff_t i=0; i<templfiles.size(); i++ ) {

        ui->comboBox_Templates->addItem(templfiles[i]);
    }

    if ( ui->comboBox_Templates->count() != 0 ) {

        ui->comboBox_Templates->setCurrentIndex(0);
    }
}

void DataImportDialog::on_pushButton_SelectDataFile_clicked() {

    if ( !dtable ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Invalid pointer to the destination table!")
                    );

        return;
    }

    dataFileName = QFileDialog::getOpenFileName(
                this,
                tr("Open Data File..."),
                dataDirName,
                QString::fromAscii("Text files (*.txt);;"
                                   "CSV files (*.csv);;"
                                   "All files (*.*)"));

    QFileInfo fileInfo(dataFileName);
    dataDirName = fileInfo.absolutePath();

    if ( !dataFileName.isEmpty() ) {

        ui->lineEdit_DataFile->setText(dataFileName);
    }
    else {

        QMessageBox::critical(this,
                              "Qr49",
                              tr("You have to select data file!")
                              );
    }
}

void DataImportDialog::on_pushButton_NextManual_clicked() {

    const ptrdiff_t scount = arrayImportedData.count();
    const ptrdiff_t dcount = dtable->rowCount();

    if ( scount > dcount ) {

        addRows(dtable, scount);
    }

    ptrdiff_t tmp_sj = 0;
    ptrdiff_t tmp_dj = 0;

    if ( manual ) {

        tmp_sj = ui->comboBox_AnotherParameter->currentIndex();
        tmp_dj = ui->comboBox_r49parameter->currentIndex();
    }
    else {

        tmp_sj = sj;
        tmp_dj = dj;

        if ( tmp_sj > (ui->comboBox_AnotherParameter->count()-1) ||
             tmp_dj > (ui->comboBox_r49parameter->count()-1) ) {

            QMessageBox::warning(
                        this,
                        "Qr49",
                        tr("Wrong index! Copying skipped!")
                        );
            return;
        }
    }

    //

    for (ptrdiff_t i=0; i<scount; i++) {

        dtable->setCurrentCell(0, tmp_dj);
        dtable->item(i, tmp_dj)->
                setText(QString::number(arrayImportedData.at(i).at(tmp_sj)));
    }

    //

    if ( ui->comboBox_r49parameter->currentIndex() ==
         ui->comboBox_r49parameter->count()-1 ) {

        ui->comboBox_r49parameter->setCurrentIndex(0);
    }
    else {

        ui->comboBox_r49parameter->setCurrentIndex(
                    ui->comboBox_r49parameter->currentIndex() + 1
                    );
    }

    if ( ui->comboBox_AnotherParameter->currentIndex() ==
         ui->comboBox_AnotherParameter->count()-1 ) {

        ui->comboBox_AnotherParameter->setCurrentIndex(0);
    }
    else {

        ui->comboBox_AnotherParameter->setCurrentIndex(
                    ui->comboBox_AnotherParameter->currentIndex() + 1
                    );
    }

    //

    templ += QString::number(tmp_dj) + " " + QString::number(tmp_sj) + "\n";

    destTableDataChanged = true;
}

void DataImportDialog::on_pushButton_NextAuto_clicked() {

    manual = false;

    QVector< QVector<double> > indexes;

    try {

        QSharedPointer<csvRead> readerTemplData(
                    new csvRead(
                        templdir.absolutePath()
                        + QDir::separator()
                        + ui->comboBox_Templates->currentText(),
                        " ",
                        0
                        )
                    );

        readerTemplData->readFile();
        indexes = readerTemplData->csvData();
    }
    catch(const ToxicError &toxerr) {

        QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg());
        manual = true;
        return;
    }

    //

    if ( indexes.isEmpty() ) {

        QMessageBox::warning(
                    this,
                    "Qr49",
                    tr("Template file is empty!")
                    );
        manual = true;
        return;
    }

    for ( ptrdiff_t i=0; i<indexes.size(); i++ ) {

        if ( indexes[i].size() == 2 ) {

            dj = indexes[i][0];
            sj = indexes[i][1];

            on_pushButton_NextManual_clicked();
        }
        else {

            QMessageBox::warning(
                        this,
                        "Qr49",
                        tr("Wrong array with indexes! Copying skipped!")
                        );
        }
    }

    manual = true;
}

void DataImportDialog::on_pushButton_SaveTemplate_clicked() {

    if ( templ.isEmpty() ) {

        QMessageBox::information(
                    this,
                    "Qr49",
                    tr("Template is empty!")
                    );
        return;
    }

    const QString filename(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save Template..."),
                    templdir.absolutePath()
                    + QDir::separator()
                    + QDateTime::currentDateTime().
                    toString("dd-MM-yyyy_hh-mm-ss.txt"))
                );

    if ( filename.isEmpty() ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("File not selected!")
                    );
        return;
    }

    QFile savedTemplate(filename);

    if ( !savedTemplate.open(QIODevice::WriteOnly) ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    savedTemplate.fileName()
                    + tr(" could not be opened!")
                    );
        return;
    }

    savedTemplate.write(templ.toAscii());
    savedTemplate.close();

    templ.clear();

    updateTemplList();
}

void DataImportDialog::combosUpdate(const QString &str) {

    (void)str;

    //

    templ.clear();

    //

    if (ui->comboBox_delimiter->currentText() == "tab") {

        delimiter = "\t";
    }
    else if (ui->comboBox_delimiter->currentText() == ";") {

        delimiter = ";";
    }
    else if (ui->comboBox_delimiter->currentText() == ",") {

        delimiter = ",";
    }
    else if (ui->comboBox_delimiter->currentText() == "space") {

        delimiter = " ";
    }
    else {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Unknown delimiter for selected file parsing!")
                    );

        return;
    }

    headerLines = ui->spinBox_HeaderLines->value();

    //

    ui->comboBox_r49parameter->clear();
    ui->comboBox_AnotherParameter->clear();

    //

    if ( !ui->lineEdit_DataFile->text().isEmpty() ) {

        QSharedPointer<csvRead>
                importedDataReader(new csvRead(dataFileName,
                                               delimiter,
                                               headerLines));

        try{

            importedDataReader->readFile();
        }
        catch(const ToxicError &toxerr) {

            QMessageBox::critical(this, "Qr49", toxerr.toxicErrMsg());
            return;
        }

        arrayImportedData = importedDataReader->csvData();
        headersImportedData = importedDataReader->csvHeaders();

        if (table_lid == 2) {

            ui->comboBox_r49parameter->
                    addItems(table_points_headers.split(" "));
        }
        else if (table_lid == 3) {

            ui->comboBox_r49parameter->
                    addItems(table_fullLoadCurve_headers.split(" "));
        }
        else {

            QMessageBox::critical(this,
                                  "Qr49",
                                  tr("Invalid destination table!")
                                  );
        }

        ui->comboBox_AnotherParameter->addItems(headersImportedData);
    }
}

void DataImportDialog::on_pushButton_Close_clicked() {

    templ.clear();

    if ( destTableDataChanged ) {

        destTableDataChanged = false;

        accept();
    }
    else {

        reject();
    }
}
