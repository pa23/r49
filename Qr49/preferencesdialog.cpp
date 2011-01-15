/*
    preferencesdialog.cpp

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

#include "preferencesdialog.h"
#include "qr49constants.h"

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QDir>

using std::string;

PreferencesDialog::PreferencesDialog(QWidget *parent) :
        QDialog(parent) {

    ui.setupUi(this);
}

PreferencesDialog::~PreferencesDialog() {
}

void PreferencesDialog::on_pushButton_filenameSourceEU3_clicked() {

    QString anotherFilenameSourceEU3(QFileDialog::getSaveFileName(
            this,
            tr("Select file..."),
            "noname.csv",
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (!anotherFilenameSourceEU3.isEmpty()) {

        ui.lineEdit_filenameSourceEU3->setText(anotherFilenameSourceEU3);
    }
}

void PreferencesDialog::on_pushButton_filenameSourceEU0_clicked() {

    QString anotherFilenameSourceEU0(QFileDialog::getSaveFileName(
            this,
            tr("Select file..."),
            "noname.csv",
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (!anotherFilenameSourceEU0.isEmpty()) {

        ui.lineEdit_filenameSourceEU0->setText(anotherFilenameSourceEU0);
    }
}

void PreferencesDialog::on_pushButton_filenamePoints_clicked() {

    QString anotherFilenamePoints(QFileDialog::getSaveFileName(
            this,
            tr("Select file..."),
            "noname.csv",
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (!anotherFilenamePoints.isEmpty()) {

        ui.lineEdit_filenamePoints->setText(anotherFilenamePoints);
    }
}

void PreferencesDialog::on_pushButton_filenamePowers_clicked() {

    QString anotherFilenamePowers(QFileDialog::getSaveFileName(
            this,
            tr("Select file..."),
            "noname.csv",
            QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
            0,
            0));

    if (!anotherFilenamePowers.isEmpty()) {

        ui.lineEdit_filenamePowers->setText(anotherFilenamePowers);
    }
}

void PreferencesDialog::on_pushButton_dirnameReports_clicked() {

    QString anotherDirnameReports(QFileDialog::getExistingDirectory(
            this,
            tr("Select directory..."),
            QDir::currentPath()
    ));

    if (!anotherDirnameReports.isEmpty()) {

        ui.lineEdit_dirnameReports->setText(anotherDirnameReports);
    }
}

void PreferencesDialog::on_pushButton_OK_clicked() {

    QString myPreferences = "//\n// This is r49 configuration file. Parameter-Value delimeter is \"=\" symbol.\n// Text after \"//\" is comment.\n//\n\n"
                            "// CSV files delimeter\ncsvdelimiter=" + ui.comboBox_csvdelimiter->currentText() + "\n\n"
                            "// Temporary rewrited source data files\nfilenameSourceEU3=" + ui.lineEdit_filenameSourceEU3->text() + "\n"
                            "filenameSourceEU0=" + ui.lineEdit_filenameSourceEU0->text() + "\n"
                            "filenamePoints=" + ui.lineEdit_filenamePoints->text() + "\n"
                            "filenamePowers=" + ui.lineEdit_filenamePowers->text() + "\n\n"
                            "// Directory for calculation results\ndirnameReports=" + ui.lineEdit_dirnameReports->text() + "\n\n"
                            "// Measuring nozzle diameter (engine inlet air mass flow calculation)\nDn=" + QString::number(ui.doubleSpinBox_Dn->value()) + "\n\n"
                            "// Air oxygen concentration\nConcO2air=" + QString::number(ui.doubleSpinBox_ConcO2air->value()) + "\n\n"
                            "//\nRr=" + QString::number(ui.doubleSpinBox_Rr->value()) + "\n\n"
                            "// Stoichiometric air/fuel ratio\nL0=" + QString::number(ui.doubleSpinBox_L0->value()) + "\n\n"
                            "// Opacimeter effective base\nL=" + QString::number(ui.doubleSpinBox_L->value()) + "\n\n"
                            "// Air carbon dioxide concentration\nConcCO2air=" + QString::number(ui.doubleSpinBox_ConcCO2air->value()) + "\n\n"
                            "// Fuel percentage\nWH=" + QString::number(ui.doubleSpinBox_WH->value()) + "\n"
                            "WO2=" + QString::number(ui.doubleSpinBox_WO2->value()) + "\n"
                            "WN=" + QString::number(ui.doubleSpinBox_WN->value()) + "\n\n"
                            "// Air density\nroAir=" + QString::number(ui.doubleSpinBox_roAir->value()) + "\n\n"
                            "// Molecular weights\nmuNO2=" + QString::number(ui.doubleSpinBox_muNO2->value()) + "\n"
                            "muCO=" + QString::number(ui.doubleSpinBox_muCO->value()) + "\n"
                            "muCH=" + QString::number(ui.doubleSpinBox_muCH->value()) + "\n";

    QFile preferencesFile(configFileName);

    if (!preferencesFile.open(QIODevice::WriteOnly)) {

        QMessageBox::critical(0, "Qr49", tr("PreferencesDialog: on_pushButton_Save_clicked: preferences could not be saved!"), 0, 0, 0);

        return;
    }

    QTextStream in(&preferencesFile);
    in << myPreferences;

    preferencesFile.close();

    hide();
}
