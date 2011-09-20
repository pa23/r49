/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: additionalcalculationsdialog.cpp

    Copyright (C) 2011 Artem Petrov <pa2311@gmail.com>

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

#include "additionalcalculationsdialog.h"
#include "ui_additionalcalculationsdialog.h"

#include "precalc.h"

#include <QMessageBox>
#include <QDoubleValidator>

AdditionalCalculationsDialog::AdditionalCalculationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdditionalCalculationsDialog) {

    ui->setupUi(this);

    doubleValidator = new QDoubleValidator();
    doubleValidator->setDecimals(4);

    ui->lineEdit_n_hi->setValidator(doubleValidator);
    ui->lineEdit_n_lo->setValidator(doubleValidator);

    ui->lineEdit_fA->setValidator(doubleValidator);
    ui->lineEdit_sA->setValidator(doubleValidator);
    ui->lineEdit_tA->setValidator(doubleValidator);
    ui->lineEdit_fB->setValidator(doubleValidator);
    ui->lineEdit_sB->setValidator(doubleValidator);
    ui->lineEdit_tB->setValidator(doubleValidator);
    ui->lineEdit_fC->setValidator(doubleValidator);
    ui->lineEdit_sC->setValidator(doubleValidator);
    ui->lineEdit_tC->setValidator(doubleValidator);
}

AdditionalCalculationsDialog::~AdditionalCalculationsDialog() {

    delete ui;
    delete doubleValidator;
}

void AdditionalCalculationsDialog::on_pushButton_calculate_clicked() {

    if ( ui->tabWidget->currentIndex() == ABCSPEEDS ) {

        double n_hi = ui->lineEdit_n_hi->text().toDouble();
        double n_lo = ui->lineEdit_n_lo->text().toDouble();

        double A = 0;
        double B = 0;
        double C = 0;
        double a1 = 0;
        double a2 = 0;
        double a3 = 0;
        double n_ref = 0;

        if (!calcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

            QMessageBox::critical(0, "Qr49", tr("Error during calculation A, B, C speeds!"), 0, 0, 0);
        }

        ui->lineEdit_A->setText(QString::number(A));
        ui->lineEdit_B->setText(QString::number(B));
        ui->lineEdit_C->setText(QString::number(C));
        ui->lineEdit_a1->setText(QString::number(a1));
        ui->lineEdit_a2->setText(QString::number(a2));
        ui->lineEdit_a3->setText(QString::number(a3));
        ui->lineEdit_n_ref->setText(QString::number(n_ref));
    }
    else if ( ui->tabWidget->currentIndex() == ELRSMOKE ) {

        double fA = ui->lineEdit_fA->text().toDouble();
        double sA = ui->lineEdit_sA->text().toDouble();
        double tA = ui->lineEdit_tA->text().toDouble();
        double fB = ui->lineEdit_fB->text().toDouble();
        double sB = ui->lineEdit_sB->text().toDouble();
        double tB = ui->lineEdit_tB->text().toDouble();
        double fC = ui->lineEdit_fC->text().toDouble();
        double sC = ui->lineEdit_sC->text().toDouble();
        double tC = ui->lineEdit_tC->text().toDouble();

        double ELRsmoke = 0;

        if (!calcELR(&fA, &sA, &tA, &fB, &sB, &tB, &fC, &sC, &tC, &ELRsmoke)) {

            QMessageBox::critical(0, "Qr49", tr("Error during calculation ELR smoke!"), 0, 0, 0);
        }

        ui->lineEdit_ELRsmoke->setText(QString::number(ELRsmoke));
    }
}
