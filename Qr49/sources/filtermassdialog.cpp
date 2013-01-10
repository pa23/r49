/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: filtermassdialog.cpp

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

#include "filtermassdialog.h"

#include <QDoubleValidator>

FilterMassDialog::FilterMassDialog(QWidget *parent) :
    QDialog(parent),
    m_doubleValidator(new QDoubleValidator()) {

    ui.setupUi(this);
    m_doubleValidator->setDecimals(4);

    ui.lineEdit_1stFilterWeightClean->setValidator(m_doubleValidator);
    ui.lineEdit_1stFilterWeightDirty->setValidator(m_doubleValidator);
    ui.lineEdit_2ndFilterWeightClean->setValidator(m_doubleValidator);
    ui.lineEdit_2ndFilterWeightDirty->setValidator(m_doubleValidator);
}

FilterMassDialog::~FilterMassDialog() {

    delete m_doubleValidator;
}
