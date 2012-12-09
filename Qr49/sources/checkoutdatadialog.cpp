/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: checkoutdatadialog.cpp

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

#include "checkoutdatadialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

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
                    tr("Save Checkout Data As..."),
                    "Qr49_CheckoutData.csv",
                    QString::fromAscii("CSV files (*.csv);;All files (*.*)"),
                    0,
                    0)
                );

    if ( !newCheckoutDataFileName.isEmpty() ) {

        const QString myCheckoutData =
                ui.plainTextEdit_CheckoutData->toPlainText();

        QFile checkoutDataFile(newCheckoutDataFileName);

        if ( !checkoutDataFile.open(QIODevice::WriteOnly) ) {

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
