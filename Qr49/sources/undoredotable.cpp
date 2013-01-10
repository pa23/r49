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
    m_table(tbl),
    m_position(-1) {
}

UndoRedoTable::~UndoRedoTable() {
}

void UndoRedoTable::saveState() {

    if ( (m_data.count() - m_position) > 1 ) {

        for ( ptrdiff_t i=(m_data.count()-1); i>m_position; i-- ) {

            m_data.remove(i);
        }
    }

    QVector<QString> row;
    QVector< QVector<QString> > matrix;

    for ( ptrdiff_t i=0; i<m_table->rowCount(); i++ ) {

        for ( ptrdiff_t j=0; j<m_table->columnCount(); j++ ) {

            row.push_back(m_table->item(i, j)->text());
        }

        matrix.push_back(row);
        row.clear();
    }

    m_data.push_back(matrix);
    matrix.clear();

    m_position++;
}

void UndoRedoTable::undoTable() {

    m_position--;

    if ( m_table->rowCount() < m_data[m_position].count() ) {

        addRows(m_table, m_data[m_position].count());
    }
    else if ( m_table->rowCount() > m_data[m_position].count() ) {

        m_table->setRowCount(m_data[m_position].count());
    }

    for ( ptrdiff_t i=0; i<m_data[m_position].count(); i++ ) {

        for ( ptrdiff_t j=0; j<m_data[m_position][i].count(); j++ ) {

            m_table->item(i, j)->setText(m_data[m_position][i][j]);
        }
    }
}

void UndoRedoTable::redoTable() {

    m_position++;
    m_position++;

    undoTable();
}

ptrdiff_t UndoRedoTable::undoTableNumber() const {

    return m_position;
}

ptrdiff_t UndoRedoTable::redoTableNumber() const {

    return m_data.count() - m_position - 1;
}

void UndoRedoTable::freeMemory() {

    m_data.clear();
}
