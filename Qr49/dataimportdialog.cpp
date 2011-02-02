/*
    dataimportdialog.cpp

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
        dataDirName(QDir::currentPath()),
        table_lid(0),
        dtable(0) {

    ui->setupUi(this);
}

DataImportDialog::~DataImportDialog() {

    delete ui;
}

void DataImportDialog::SetDestinationTable(ptrdiff_t tlid, QTableWidget *dt) {

    table_lid = tlid;
    dtable = dt;
}

void DataImportDialog::on_pushButton_SelectDataFile_clicked() {

    if (!dtable) {

        QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Invalid pointer to the destination table!"), 0, 0, 0);

        return;
    }

    //

    QString delimiter;

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

    ptrdiff_t headerLines = ui->spinBox_HeaderLines->value();

    //

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

        //

        QSharedPointer<csvRead> importedDataReader(new csvRead(dataFileName, delimiter, headerLines));

        arrayImportedData = importedDataReader.data()->csvData();
        QStringList headersImportedData = importedDataReader.data()->csvHeaders();

        //

        QString table_points_headers = "Point[-];n[min-1];Me_b[Nm];Ne_b[kW];N_fan[kW];w[-];t0[oC];B0[kPa];Ra[%];dPn[mmH2O];Gair[kg/h];Gfuel[kg/h];CNOx[ppm];gNOx[g/kWh];CCO[ppm];CCH[ppm];CCO2in[%];CCO2out[%];CO2[%];Ka[m-1];Ka[%];FSN[-];Pr[kPa];ts[oC];tauf[s];qmdw[g/s];qmdew[g/s];rd[-]";
        QString table_fullLoadCurve_headers = "Point[-];n[min-1];Me_b[Nm];t0[oC];B0[kPa];Ra[%];S[kPa];pk[kPa];Gfuel[kg/h];N_k[kW];N_fan[kW]";

        //

        for (ptrdiff_t i=ui->comboBox_r49parameter->count(); i>0; i--) {

            ui->comboBox_r49parameter->removeItem(i);
        }

        for (ptrdiff_t i=ui->comboBox_AnotherParameter->count(); i>0; i--) {

            ui->comboBox_AnotherParameter->removeItem(i);
        }

        //

        if (table_lid == 2) {

            ui->comboBox_r49parameter->addItems(table_points_headers.split(";"));
        }
        else if (table_lid == 3) {

            ui->comboBox_r49parameter->addItems(table_fullLoadCurve_headers.split(";"));
        }
        else {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Invalid destination table!"), 0, 0, 0);
        }

        ui->comboBox_AnotherParameter->addItems(headersImportedData);
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

        dtable->item(i, dj)->setText(QString::number(arrayImportedData.at(i).at(sj)));
    }
}
