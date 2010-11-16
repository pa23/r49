/*
    abcspeedscalcdialog.cpp

    Copyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>

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

#include "abcspeedscalcdialog.h"

#include "precalc.h"

#include <QtGui/QMessageBox>
#include <QtGui/QDoubleValidator>

ABCspeedsCalcDialog::ABCspeedsCalcDialog(QWidget *parent) : QDialog(parent) {

    ui.setupUi(this);

    doubleValidator = new QDoubleValidator(this);
    doubleValidator->setDecimals(4);

    ui.lineEdit_n_hi->setValidator(doubleValidator);
    ui.lineEdit_n_lo->setValidator(doubleValidator);
}

ABCspeedsCalcDialog::~ABCspeedsCalcDialog() {

    delete doubleValidator;
}

void ABCspeedsCalcDialog::on_pushButton_CalcABCspeeds_clicked() {

    double n_hi = ui.lineEdit_n_hi->text().toDouble();
    double n_lo = ui.lineEdit_n_lo->text().toDouble();

    double A = 0;
    double B = 0;
    double C = 0;
    double a1 = 0;
    double a2 = 0;
    double a3 = 0;
    double n_ref = 0;

    if (!CalcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

        QMessageBox::critical(0, "Qr49", tr("Error during calculation A, B, C speeds!"), 0, 0, 0);
    }

    ui.lineEdit_A->setText(QString::number(A));
    ui.lineEdit_B->setText(QString::number(B));
    ui.lineEdit_C->setText(QString::number(C));
    ui.lineEdit_a1->setText(QString::number(a1));
    ui.lineEdit_a2->setText(QString::number(a2));
    ui.lineEdit_a3->setText(QString::number(a3));
    ui.lineEdit_n_ref->setText(QString::number(n_ref));
}
