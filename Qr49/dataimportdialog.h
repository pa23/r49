/*
    dataimportdialog.h

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

#ifndef DATAIMPORTDIALOG_H
#define DATAIMPORTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVector>

#include "qr49constants.h"

namespace Ui {

    class DataImportDialog;
}

class DataImportDialog : public QDialog {

    Q_OBJECT

public:

    explicit DataImportDialog(QWidget *parent = 0);
    ~DataImportDialog();

    void SetDestinationTable(ptrdiff_t, QTableWidget *);

private:

    Ui::DataImportDialog *ui;

    QString dataFileName;
    QString dataDirName;
    ptrdiff_t table_lid;
    QTableWidget *dtable;
    ptrdiff_t tableRowHeight;
    QVector< QVector<double> > arrayImportedData;

private slots:

    void on_pushButton_SelectDataFile_clicked();
    void on_pushButton_Next_clicked();

};

#endif // DATAIMPORTDIALOG_H