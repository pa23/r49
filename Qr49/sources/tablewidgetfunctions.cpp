/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: tablewidgetfunctions.cpp

    Copyright (C) 2011-2017 Artem Petrov <pa2311@gmail.com>

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

void addRows(
        QTableWidget *table,
        const ptrdiff_t newRowsNumber,
        const ptrdiff_t mode
        ) {

    ptrdiff_t n = 0;

    if ( mode == ADDROWS_ABOVE ) {
        if ( table->currentRow() > -1 ) {
            n = table->currentRow();
        }
    }
    else if ( mode == ADDROWS_BELOW ) {
        if ( table->currentRow() > -1 ) {
            n = table->currentRow() + 1;
        }
    }
    else if ( mode == ADDROWS_BOTTOM ) {
        n = table->rowCount();
    }

    for ( ptrdiff_t i=0; i<newRowsNumber; i++ ) {
        table->insertRow(n);
        formatRow(table, n);
    }
}

void formatRow(QTableWidget *table, ptrdiff_t i) {

    table->setRowHeight(i, tableRowHeight);
    //table->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

    for ( ptrdiff_t j=0; j<table->columnCount(); j++ ) {
        table->setItem(i, j, new QTableWidgetItem("0"));
        table->item(i, j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
}
