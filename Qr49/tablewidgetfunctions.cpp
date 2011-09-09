/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: tablewidgetfunctions.cpp

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

#include "tablewidgetfunctions.h"

#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>

void addRows(QTableWidget *table, ptrdiff_t newNumber) {

    for (ptrdiff_t i=table->rowCount(); i<newNumber; i++) {

        table->setRowCount(i+1);
        table->setRowHeight(i, tableRowHeight);
        //table->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

        table->setItem(i, 0, new QTableWidgetItem(QString::number(i+1, 'f', 0)));
        table->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        table->setItem(i, 1, new QTableWidgetItem("0"));
        table->item(i, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (ptrdiff_t j=2; j<table->columnCount(); j++) {

            table->setItem(i, j, new QTableWidgetItem("0.000"));
            table->item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
}
