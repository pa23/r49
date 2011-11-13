/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: dataimportdialog.cpp

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

#include "csvread.h"

DataImportDialog::DataImportDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DataImportDialog),
        delimiter("\t"),
        headerLines(2),
        table_points_headers("Point[-] n[min-1] Me_b[Nm] Ne_b[kW] N_fan[kW] w[-] t0[oC] B0[kPa] Ra[%] dPn[mmH2O] Gair[kg/h] Gfuel[kg/h] C_NOx[ppm] gNOx[g/kWh] C_CO[ppm] C_CH[ppm] C_CO2in[%] C_CO2out[%] C_O2[%] Ka[m-1] Ka[%] FSN[-] Pr[kPa] ts[oC] tauf[s] qmdw[g/s] qmdew[g/s] rd[-]"),
        table_fullLoadCurve_headers("Point[-] n[min-1] Me_b[Nm] t0[oC] B0[kPa] Ra[%] S[kPa] pk[kPa] Gfuel[kg/h] N_k[kW] N_fan[kW]"),
        dataDirName(QDir::currentPath()),
        table_lid(0),
        dtable(0) {

    ui->setupUi(this);

    connect(ui->comboBox_delimiter, SIGNAL(activated(QString)), this, SLOT(combosUpdate(QString)));
    connect(ui->spinBox_HeaderLines, SIGNAL(valueChanged(QString)), this, SLOT(combosUpdate(QString)));
    connect(ui->lineEdit_DataFile, SIGNAL(textChanged(QString)), this, SLOT(combosUpdate(QString)));
}

DataImportDialog::~DataImportDialog() {

    delete ui;
}

void DataImportDialog::SetDestinationTable(ptrdiff_t tlid, QTableWidget *dt) {

    table_lid = tlid;
    dtable = dt;

    combosUpdate("");
}

void DataImportDialog::on_pushButton_SelectDataFile_clicked() {

    if (!dtable) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Invalid pointer to the destination table!"), 0, 0, 0);

        return;
    }

    dataFileName = QFileDialog::getOpenFileName(
            this,
            tr("Open Data File..."),
            dataDirName,
            QString::fromAscii("Text files (*.txt);;CSV files (*.csv);;All files (*.*)"),
            0,
            0);

    QFileInfo fileInfo(dataFileName);
    dataDirName = fileInfo.absolutePath();

    if ( !dataFileName.isEmpty() ) {

        ui->lineEdit_DataFile->setText(dataFileName);
    }
    else {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("You have to select data file!"), 0, 0, 0);
    }
}

void DataImportDialog::on_pushButton_Next_clicked() {

    ptrdiff_t scount = arrayImportedData.count();
    ptrdiff_t dcount = dtable->rowCount();

    if (scount > dcount) {

        addRows(dtable, scount);
    }

    ptrdiff_t sj = ui->comboBox_AnotherParameter->currentIndex();
    ptrdiff_t dj = ui->comboBox_r49parameter->currentIndex();

    for (ptrdiff_t i=0; i<scount; i++) {

        dtable->setCurrentCell(0, dj);
        dtable->item(i, dj)->setText(QString::number(arrayImportedData.at(i).at(sj)));
    }
}

void DataImportDialog::combosUpdate(QString str) {

    //

    (void)str;

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

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Unknown delimiter for selected file parsing!"), 0, 0, 0);

        return;
    }

    headerLines = ui->spinBox_HeaderLines->value();

    //

    ui->comboBox_r49parameter->clear();
    ui->comboBox_AnotherParameter->clear();

    //

    if ( !ui->lineEdit_DataFile->text().isEmpty() ) {

        QSharedPointer<csvRead> importedDataReader(new csvRead(dataFileName, delimiter, headerLines));

        arrayImportedData = importedDataReader.data()->csvData();
        headersImportedData = importedDataReader.data()->csvHeaders();

        if (table_lid == 2) {

            ui->comboBox_r49parameter->addItems(table_points_headers.split(" "));
        }
        else if (table_lid == 3) {

            ui->comboBox_r49parameter->addItems(table_fullLoadCurve_headers.split(" "));
        }
        else {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Invalid destination table!"), 0, 0, 0);
        }

        ui->comboBox_AnotherParameter->addItems(headersImportedData);
    }
}
