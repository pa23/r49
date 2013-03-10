/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: preferencesdialog.cpp

    Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>

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

#include "preferencesdialog.h"
#include "constants.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent) {

    ui.setupUi(this);
}

PreferencesDialog::~PreferencesDialog() {
}

void PreferencesDialog::on_pushButton_dirnameReports_clicked() {

    const QString anotherDirnameReports(
                QFileDialog::getExistingDirectory(
                    this,
                    tr("Select directory..."),
                    QDir::currentPath()
                    )
                );

    if ( !anotherDirnameReports.isEmpty() ) {

        ui.lineEdit_dirnameReports->setText(anotherDirnameReports);
    }
}

void PreferencesDialog::on_pushButton_filenameSourceEU3_clicked() {

    const QString anotherFilenameSourceEU3(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Select file..."),
                    "noname.dat",
                    QString::fromLatin1("Data files (*.dat);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !anotherFilenameSourceEU3.isEmpty() ) {

        ui.lineEdit_filenameSourceEU3->setText(anotherFilenameSourceEU3);
    }
}

void PreferencesDialog::on_pushButton_filenameSourceEU0_clicked() {

    const QString anotherFilenameSourceEU0(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Select file..."),
                    "noname.dat",
                    QString::fromLatin1("Data files (*.dat);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !anotherFilenameSourceEU0.isEmpty() ) {

        ui.lineEdit_filenameSourceEU0->setText(anotherFilenameSourceEU0);
    }
}

void PreferencesDialog::on_pushButton_filenamePoints_clicked() {

    const QString anotherFilenamePoints(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Select file..."),
                    "noname.dat",
                    QString::fromLatin1("Data files (*.dat);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !anotherFilenamePoints.isEmpty() ) {

        ui.lineEdit_filenamePoints->setText(anotherFilenamePoints);
    }
}

void PreferencesDialog::on_pushButton_filenamePowers_clicked() {

    const QString anotherFilenamePowers(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Select file..."),
                    "noname.dat",
                    QString::fromLatin1("Data files (*.dat);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !anotherFilenamePowers.isEmpty() ) {

        ui.lineEdit_filenamePowers->setText(anotherFilenamePowers);
    }
}

void PreferencesDialog::on_pushButton_OK_clicked() {

    const QString myPreferences = "//\n// This is r49 configuration file. "
            "Parameter-Value delimeter is \"=\" symbol.\n// Text after "
            "\"//\" is comment.\n//\n\n"
            "// Temporary rewrited source data files\nfilenameSourceEU3="
            + ui.lineEdit_filenameSourceEU3->text()
            + "\n"
            "filenameSourceEU0="
            + ui.lineEdit_filenameSourceEU0->text()
            + "\n"
            "filenamePoints="
            + ui.lineEdit_filenamePoints->text()
            + "\n"
            "filenamePowers="
            + ui.lineEdit_filenamePowers->text()
            + "\n\n"
            "// Directory for calculation results\ndirnameReports="
            + ui.lineEdit_dirnameReports->text()
            + "\n\n"
            "// Measuring nozzle diameter (engine inlet air mass flow "
            "calculation)\nDn="
            + QString::number(ui.doubleSpinBox_Dn->value())
            + "\n\n"
            "// Air oxygen concentration\nConcO2air="
            + QString::number(ui.doubleSpinBox_ConcO2air->value())
            + "\n\n"
            "//\nRr="
            + QString::number(ui.doubleSpinBox_Rr->value())
            + "\n\n"
            "// Stoichiometric air/fuel ratio\nL0="
            + QString::number(ui.doubleSpinBox_L0->value())
            + "\n\n"
            "// Opacimeter effective base\nL="
            + QString::number(ui.doubleSpinBox_L->value())
            + "\n\n"
            "// Air carbon dioxide concentration\nConcCO2air="
            + QString::number(ui.doubleSpinBox_ConcCO2air->value())
            + "\n\n"
            "// Fuel percentage\nWH="
            + QString::number(ui.doubleSpinBox_WH->value())
            + "\n"
            "WO2="
            + QString::number(ui.doubleSpinBox_WO2->value())
            + "\n"
            "WN="
            + QString::number(ui.doubleSpinBox_WN->value())
            + "\n\n"
            "// Molecular weights\nmuNO2="
            + QString::number(ui.doubleSpinBox_muNO2->value())
            + "\n"
            "muCO="
            + QString::number(ui.doubleSpinBox_muCO->value())
            + "\n"
            "muCH="
            + QString::number(ui.doubleSpinBox_muCH->value())
            + "\n";

    QFile preferencesFile1(QCoreApplication::applicationDirPath()
                           + QDir::separator()
                           + CONFIGFILENAME);
    QFile preferencesFile2(QDir::homePath()
                           + QDir::separator()
                           + CONFIGFILENAME);

    if ( preferencesFile1.open(QIODevice::WriteOnly) ) {

        QTextStream in(&preferencesFile1);
        in << myPreferences;

        preferencesFile1.close();
    }
    else if ( preferencesFile2.open(QIODevice::WriteOnly) ) {

        QTextStream in(&preferencesFile2);
        in << myPreferences;

        preferencesFile2.close();
    }
    else {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Preferences could not be saved!")
                    );
        return;
    }

    hide();
}
