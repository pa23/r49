/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: filtermassdialog.h

    Copyright (C) 2009-2017 Artem Petrov <pa2311@gmail.com>

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

#ifndef FILTERMASSDIALOG_H
#define FILTERMASSDIALOG_H

#include <QDialog>

#include "ui_filtermassdialog.h"

class FilterMassDialog : public QDialog {

    Q_OBJECT

public:
    explicit FilterMassDialog(QWidget *parent = 0);
    ~FilterMassDialog();
    void retranslate();

private:
    Ui::FilterMassDialog ui;

};

#endif // FILTERMASSDIALOG_H
