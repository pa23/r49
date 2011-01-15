/*
    preferencesdialog.h

    Copyright (C) 2009, 2010, 2011 Artem Petrov <pa2311@gmail.com>

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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtGui/QDialog>
#include "ui_preferencesdialog.h"

class PreferencesDialog : public QDialog {

    Q_OBJECT

public:

    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

private:

    Ui::PreferencesDialogClass ui;

private slots:

    void on_pushButton_dirnameReports_clicked();
    void on_pushButton_filenamePowers_clicked();
    void on_pushButton_filenamePoints_clicked();
    void on_pushButton_filenameSourceEU0_clicked();
    void on_pushButton_filenameSourceEU3_clicked();
    void on_pushButton_OK_clicked();

};

#endif // PREFERENCESDIALOG_H
