/*
    elrsmokecalcdialog.cpp

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

#include "elrsmokecalcdialog.h"

#include "precalc.h"

#include <QtGui/QMessageBox>
#include <QtGui/QDoubleValidator>

ELRsmokeCalcDialog::ELRsmokeCalcDialog(QWidget *parent) : QDialog(parent) {

    ui.setupUi(this);

    doubleValidator = new QDoubleValidator(this);
    doubleValidator->setDecimals(4);

    ui.lineEdit_fA->setValidator(doubleValidator);
    ui.lineEdit_sA->setValidator(doubleValidator);
    ui.lineEdit_tA->setValidator(doubleValidator);
    ui.lineEdit_fB->setValidator(doubleValidator);
    ui.lineEdit_sB->setValidator(doubleValidator);
    ui.lineEdit_tB->setValidator(doubleValidator);
    ui.lineEdit_fC->setValidator(doubleValidator);
    ui.lineEdit_sC->setValidator(doubleValidator);
    ui.lineEdit_tC->setValidator(doubleValidator);
}

ELRsmokeCalcDialog::~ELRsmokeCalcDialog() {

    delete doubleValidator;
}

void ELRsmokeCalcDialog::on_pushButton_CalcELRsmoke_clicked() {

    double fA = ui.lineEdit_fA->text().toDouble();
    double sA = ui.lineEdit_sA->text().toDouble();
    double tA = ui.lineEdit_tA->text().toDouble();
    double fB = ui.lineEdit_fB->text().toDouble();
    double sB = ui.lineEdit_sB->text().toDouble();
    double tB = ui.lineEdit_tB->text().toDouble();
    double fC = ui.lineEdit_fC->text().toDouble();
    double sC = ui.lineEdit_sC->text().toDouble();
    double tC = ui.lineEdit_tC->text().toDouble();

    double ELRsmoke = 0;

    if (!CalcELR(&fA, &sA, &tA, &fB, &sB, &tB, &fC, &sC, &tC, &ELRsmoke)) {

        QMessageBox::critical(0, "Qr49", tr("Error during calculation ELR smoke!"), 0, 0, 0);
    }

    ui.lineEdit_ELRsmoke->setText(QString::number(ELRsmoke));
}
