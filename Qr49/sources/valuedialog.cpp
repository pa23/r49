/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: valuedialog.cpp

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

#include "valuedialog.h"

#include <QDoubleValidator>

ValueDialog::ValueDialog(QWidget *parent) :
    QDialog(parent),
    doubleValidator(new QDoubleValidator()) {

    ui.setupUi(this);
    doubleValidator->setDecimals(4);

    ui.lineEdit_Value->setValidator(doubleValidator);
}

ValueDialog::~ValueDialog() {

    delete doubleValidator;
}
