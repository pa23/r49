/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: additionalcalculationsdialog.h

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

#ifndef ADDITIONALCALCULATIONSDIALOG_H
#define ADDITIONALCALCULATIONSDIALOG_H

#include <QDialog>
#include <QDoubleValidator>

namespace Ui {

    class AdditionalCalculationsDialog;
}

class AdditionalCalculationsDialog : public QDialog {

    Q_OBJECT

public:

    explicit AdditionalCalculationsDialog(QWidget *parent = 0);
    ~AdditionalCalculationsDialog();

private:

    Ui::AdditionalCalculationsDialog *ui;

    QDoubleValidator *doubleValidator;
    enum{ ABCSPEEDS, ELRSMOKE, AIRCONSUMPTION };

private slots:

    void on_pushButton_calculate_clicked();

};

#endif // ADDITIONALCALCULATIONSDIALOG_H
