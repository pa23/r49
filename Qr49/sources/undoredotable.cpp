/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: undoredotable.cpp

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

#include "undoredotable.h"
#include "tablewidgetfunctions.h"

#include <QVector>
#include <QString>
#include <QTableWidget>

UndoRedoTable::UndoRedoTable(QTableWidget *tbl) :
    table(tbl),
    position(-1) {
}

UndoRedoTable::~UndoRedoTable() {
}

void UndoRedoTable::saveState() {

    if ( (data.count() - position) > 1 ) {

        for ( ptrdiff_t i=(data.count()-1); i>position; i-- ) {

            data.remove(i);
        }
    }

    QVector<QString> row;
    QVector< QVector<QString> > matrix;

    for ( ptrdiff_t i=0; i<table->rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<table->columnCount(); j++ ) {

            row.push_back(table->item(i, j)->text());
        }

        matrix.push_back(row);
        row.clear();
    }

    data.push_back(matrix);
    matrix.clear();

    position++;
}

void UndoRedoTable::undoTable() {

    position--;

    if ( table->rowCount() < data[position].count() ) {

        addRows(table, data[position].count());
    }
    else if ( table->rowCount() > data[position].count() ) {

        table->setRowCount(data[position].count());
    }

    for ( ptrdiff_t i=0; i<data[position].count(); i++ ) {

        for ( ptrdiff_t j=0; j<data[position][i].count(); j++ ) {

            table->item(i, j)->setText(data[position][i][j]);
        }
    }
}

void UndoRedoTable::redoTable() {

    position++;
    position++;

    undoTable();
}

ptrdiff_t UndoRedoTable::undoTableNumber() const {

    return position;
}

ptrdiff_t UndoRedoTable::redoTableNumber() const {

    return data.count() - position - 1;
}

void UndoRedoTable::freeMemory() {

    data.clear();
}
