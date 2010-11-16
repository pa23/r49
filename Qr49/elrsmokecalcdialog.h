/*
    elrsmokecalcdialog.h

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

#ifndef ELRSMOKECALCDIALOG_H
#define ELRSMOKECALCDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QDoubleValidator>

#include "ui_elrsmokecalcdialog.h"

class ELRsmokeCalcDialog : public QDialog {

    Q_OBJECT

public:
    ELRsmokeCalcDialog(QWidget *parent = 0);
    ~ELRsmokeCalcDialog();

private:
    Ui::ELRsmokeCalcDialogClass ui;
    QDoubleValidator *doubleValidator;

private slots:
    void on_pushButton_CalcELRsmoke_clicked();

};

#endif // ELRSMOKECALCDIALOG_H
