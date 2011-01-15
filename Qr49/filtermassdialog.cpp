/*
    filtermassdialog.cpp

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

#include "filtermassdialog.h"

#include <QtGui/QDoubleValidator>

FilterMassDialog::FilterMassDialog(QWidget *parent) :
        QDialog(parent) {

    ui.setupUi(this);

    doubleValidator = new QDoubleValidator();
    doubleValidator->setDecimals(4);

    ui.lineEdit_1stFilterWeightClean->setValidator(doubleValidator);
    ui.lineEdit_1stFilterWeightDirty->setValidator(doubleValidator);
    ui.lineEdit_2ndFilterWeightClean->setValidator(doubleValidator);
    ui.lineEdit_2ndFilterWeightDirty->setValidator(doubleValidator);
}

FilterMassDialog::~FilterMassDialog() {
}
