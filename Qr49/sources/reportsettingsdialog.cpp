/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: reportsettingsdialog.cpp

    Copyright (C) 2013-2016 Artem Petrov <pa2311@gmail.com>

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

#include "reportsettingsdialog.h"
#include "ui_reportsettingsdialog.h"
#include "constants.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

ReportSettingsDialog::ReportSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReportSettingsDialog) {

    ui->setupUi(this);
}

ReportSettingsDialog::~ReportSettingsDialog() {
    delete ui;
}

void ReportSettingsDialog::retranslate() {
    ui->retranslateUi(this);
}

void ReportSettingsDialog::on_pushButton_OK_clicked() {

    QFile engDescrFile("r49data/engdescr.conf");

    if ( !engDescrFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Engine description could not be saved!")
                    );
        return;
    }

    QTextStream fout1(&engDescrFile);
    fout1 << ui->plainTextEdit_EngineDescription->toPlainText();

    engDescrFile.close();

    QFile techFluidsFile("r49data/techfluids.conf");

    if ( !techFluidsFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

        QMessageBox::critical(
                    this,
                    QR49NAME,
                    tr("Technical fluids description could not be saved!")
                    );
        return;
    }

    QTextStream fout2(&techFluidsFile);
    fout2 << ui->plainTextEdit_TechnicalFluids->toPlainText();

    techFluidsFile.close();

    hide();
}

void ReportSettingsDialog::on_pushButton_Cancel_clicked() {

    hide();
}
