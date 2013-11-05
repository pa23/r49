/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: checkoutdatadialog.cpp

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

#include "checkoutdatadialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include <QRegExp>

CheckoutDataDialog::CheckoutDataDialog(QWidget *parent) :
    QDialog(parent) {

    ui.setupUi(this);
}

CheckoutDataDialog::~CheckoutDataDialog() {
}

void CheckoutDataDialog::on_pushButton_SaveAs_clicked() {

    const QString newCheckoutDataFileName(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save checkout data as..."),
                    "Qr49_CheckoutData.dat",
                    QString::fromLatin1("Data files (*.dat);;All files (*)"),
                    0,
                    0)
                );

    if ( !newCheckoutDataFileName.isEmpty() ) {

        const QString myCheckoutData =
                ui.plainTextEdit_CheckoutData->toPlainText();

        QFile checkoutDataFile(newCheckoutDataFileName);

        if ( !checkoutDataFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            QMessageBox::critical(
                        this,
                        "Qr49",
                        newCheckoutDataFileName + tr(" could not be saved!")
                        );
            return;
        }

        QTextStream in(&checkoutDataFile);
        in << myCheckoutData;

        checkoutDataFile.close();

        ui.lineEdit_file->setText(newCheckoutDataFileName);
    }
}

void CheckoutDataDialog::on_pushButton_Copy_clicked() {

    QApplication::clipboard()->setText(
                ui.plainTextEdit_CheckoutData->
                toPlainText().replace(QRegExp("[ ]+"), "\t")
                );
}

void CheckoutDataDialog::on_pushButton_AltCopy_clicked() {

    QStringList strs =
            ui.plainTextEdit_CheckoutData->
            toPlainText().split("\n", QString::SkipEmptyParts);
    QVector<QStringList> arr;
    QString str;

    for ( ptrdiff_t i=0; i<strs.size(); i++ ) {
        arr.push_back(strs[i].split(QRegExp("[ ]+")));
    }

    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // n
        if ( i != arr.size()-1 ) {
            str += arr[i][2] + "\t";
        }
        else {
            str += arr[i][2] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Me_b
        if ( i != arr.size()-1 ) {
            str += arr[i][3] + "\t";
        }
        else {
            str += arr[i][3] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Ne_b
        if ( i != arr.size()-1 ) {
            str += arr[i][19] + "\t";
        }
        else {
            str += arr[i][19] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Gfuel
        if ( i != arr.size()-1 ) {
            str += arr[i][9] + "\t";
        }
        else {
            str += arr[i][9] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // B0
        if ( i != arr.size()-1 ) {
            str += arr[i][5] + "\t";
        }
        else {
            str += arr[i][5] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Ra
        if ( i != arr.size()-1 ) {
            str += arr[i][6] + "\t";
        }
        else {
            str += arr[i][6] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // pa
        if ( i != arr.size()-1 ) {
            str += arr[i][23] + "\t";
        }
        else {
            str += arr[i][23] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // t0
        if ( i != arr.size()-1 ) {
            str += QString::number(arr[i][4].toDouble()+273) + "\t";
        }
        else {
            str += QString::number(arr[i][4].toDouble()+273) + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // N_k
        if ( i != arr.size()-1 ) {
            str += arr[i][10] + "\t";
        }
        else {
            str += arr[i][10] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // fa
        if ( i != arr.size()-1 ) {
            str += arr[i][25] + "\t";
        }
        else {
            str += arr[i][25] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // alphad
        if ( i != arr.size()-1 ) {
            str += arr[i][26] + "\t";
        }
        else {
            str += arr[i][26] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // N_r
        if ( i != arr.size()-1 ) {
            str += arr[i][27] + "\t";
        }
        else {
            str += arr[i][27] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // N_fan
        if ( i != arr.size()-1 ) {
            str += arr[i][11] + "\t";
        }
        else {
            str += arr[i][11] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Ne_nr
        if ( i != arr.size()-1 ) {
            str += arr[i][28] + "\t";
        }
        else {
            str += arr[i][28] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Me_nr
        if ( i != arr.size()-1 ) {
            str += arr[i][29] + "\t";
        }
        else {
            str += arr[i][29] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // ge_nr
        if ( i != arr.size()-1 ) {
            str += arr[i][30] + "\t";
        }
        else {
            str += arr[i][30] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // t_c
        if ( i != arr.size()-1 ) {
            str += QString::number(arr[i][12].toDouble()+273) + "\t";
        }
        else {
            str += QString::number(arr[i][12].toDouble()+273) + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // t_oil
        if ( i != arr.size()-1 ) {
            str += QString::number(arr[i][13].toDouble()+273) + "\t";
        }
        else {
            str += QString::number(arr[i][13].toDouble()+273) + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // tk
        if ( i != arr.size()-1 ) {
            str += QString::number(arr[i][14].toDouble()+273) + "\t";
        }
        else {
            str += QString::number(arr[i][14].toDouble()+273) + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // tks
        if ( i != arr.size()-1 ) {
            str += QString::number(arr[i][15].toDouble()+273) + "\t";
        }
        else {
            str += QString::number(arr[i][15].toDouble()+273) + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // t_fuel
        if ( i != arr.size()-1 ) {
            str += QString::number(arr[i][16].toDouble()+273) + "\t";
        }
        else {
            str += QString::number(arr[i][16].toDouble()+273) + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // pk
        if ( i != arr.size()-1 ) {
            str += arr[i][8] + "\t";
        }
        else {
            str += arr[i][8] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // pks
        if ( i != arr.size()-1 ) {
            str += arr[i][17] + "\t";
        }
        else {
            str += arr[i][17] + "\n";
        }
    }
    for ( ptrdiff_t i=1; i<arr.size(); i++ ) { // Gair
        if ( i != arr.size()-1 ) {
            str += arr[i][18] + "\t";
        }
        else {
            str += arr[i][18];
        }
    }

    QApplication::clipboard()->setText(str);
}
