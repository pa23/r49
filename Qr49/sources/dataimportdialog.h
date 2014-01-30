/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: dataimportdialog.h

    Copyright (C) 2009-2014 Artem Petrov <pa2311@gmail.com>

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
#include <QDir>

#include "constants.h"

namespace Ui {

class DataImportDialog;

}

class DataImportDialog : public QDialog {

    Q_OBJECT

public:
    explicit DataImportDialog(QWidget *parent = 0);
    ~DataImportDialog();
    void retranslate();

    void init(const ptrdiff_t, // table local id
              QTableWidget *);

private:
    Ui::DataImportDialog *ui;

    QString m_delimiter;
    QString m_dataFileName;
    QString m_dataDirName;
    ptrdiff_t m_table_lid;
    QTableWidget *m_dtable;
    QVector< QVector<double> > m_arrayImportedData;
    QStringList m_headersImportedData;
    bool m_destTableDataChanged;
    QDir m_templdir;
    ptrdiff_t m_sj;
    ptrdiff_t m_dj;
    bool m_manual;

    void updateTemplList();

private slots:
    void on_pushButton_SelectDataFile_clicked();
    void on_pushButton_NextManual_clicked();
    void on_pushButton_NextAuto_clicked();
    void on_pushButton_OpenTemplate_clicked();
    void on_pushButton_SaveTemplate_clicked();
    void on_pushButton_Close_clicked();

    void combosUpdate(const QString &);

};

#endif // DATAIMPORTDIALOG_H
