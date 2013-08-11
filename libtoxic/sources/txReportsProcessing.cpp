/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txReportsProcessing.cpp

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

#include "txReportsProcessing.h"
#include "txError.h"

#include <QString>
#include <QFile>
#include <QIODevice>

namespace toxic {

void changeEngineInfo(const QString &path, const QString &text) {

    QFile reportFile(path);

    if ( !reportFile.open(QIODevice::ReadOnly) ) {
        throw txError("Can not open report file " + path + " to read!");
    }

    QString currStr;
    QString newReport;

    while ( !reportFile.atEnd() ) {

        currStr = reportFile.readLine();

        if ( currStr.contains(": ...") ) {
            currStr.replace(": ...", ": " + text);
        }

        newReport += currStr;
    }

    reportFile.close();

    //

    QFile newReportFile(path);

    if ( !newReportFile.open(QIODevice::WriteOnly) ) {
        throw txError("Can not open report file " + path + " to write!");
    }

    newReportFile.write(newReport.toUtf8());

    newReportFile.close();
}

QString createCommonReport(const QString &pathToGAS, const QString &pathToPT) {

    //
}

} // namespace toxic
