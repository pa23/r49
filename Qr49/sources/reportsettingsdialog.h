/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: reportsettingsdialog.h

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

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

#ifndef REPORTSETTINGSDIALOG_H
#define REPORTSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class ReportSettingsDialog;
}

class ReportSettingsDialog : public QDialog {

    Q_OBJECT

public:

    explicit ReportSettingsDialog(QWidget *parent = 0);
    ~ReportSettingsDialog();

private slots:

    void on_pushButton_OK_clicked();
    void on_pushButton_Cancel_clicked();

private:

    Ui::ReportSettingsDialog *ui;

};

#endif // REPORTSETTINGSDIALOG_H
