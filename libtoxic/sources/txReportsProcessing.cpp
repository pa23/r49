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
#include <QDir>
#include <QIODevice>

namespace toxic {

void changeEngineInfo(const QString &path, const QString &text) {

    QString replText = text;
    replText.replace("\n", "\n                         ");

    QFile reportFile(path);

    if ( !reportFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        throw txError("Can not open report file " + path + " to read!");
    }

    QString currStr;
    QString newReport;

    while ( !reportFile.atEnd() ) {

        currStr = reportFile.readLine();

        if ( currStr.contains(": ...") ) {
            currStr.replace(": ...", ": " + replText);
        }

        newReport += currStr;
    }

    reportFile.close();

    //

    QFile newReportFile(path);

    if ( !newReportFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        throw txError("Can not open report file " + path + " to write!");
    }

    newReportFile.write(newReport.toUtf8());

    newReportFile.close();
}

QString createCommonReport(
        const QString &pathToGAS,
        const QString &pathToPT
        ) {

    QString commonReportFilePath =
            QFile(pathToGAS).fileName().replace("ReportGAS", "CommonReport");

    QFile commonReportFile(commonReportFilePath);

    if ( !commonReportFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        throw txError(
                    "Can not open report file "
                    + commonReportFilePath
                    + " to write!"
                    );
    }

    QFile reportFile1(pathToGAS);

    if ( !reportFile1.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        throw txError("Can not open report file " + pathToGAS + " to read!");
    }

    QFile reportFile2(pathToPT);

    if ( !reportFile2.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        throw txError("Can not open report file " + pathToPT + " to read!");
    }

    QString currStr;

    while ( !reportFile1.atEnd() ) {

        currStr = reportFile1.readLine();

        if ( currStr.contains("Specific emissions") ) {
            break;
        }
        else {

            if ( currStr.contains("n[min-1]") &&
                 !currStr.contains("diff[%]") ) {
                commonReportFile.write("Gaseous emissions calculation:\n\n");
            }

            commonReportFile.write(currStr.toUtf8());
        }
    }

    reportFile1.reset();

    bool copied = false;

    while ( !reportFile2.atEnd() ) {

        currStr = reportFile2.readLine();

        if ( currStr.contains("Point[-]") ) {
            copied = true;
        }
        else if ( currStr.contains("Limitation") ) {
            copied = false;
        }

        if ( copied )  {

            if ( currStr.contains("n[min-1]") ) {
                commonReportFile.write("PT emissions calculation:\n\n");
            }

            commonReportFile.write(currStr.toUtf8());
        }
        else {
            continue;
        }
    }

    copied = false;

    while ( !reportFile1.atEnd() ) {

        currStr = reportFile1.readLine();

        if ( currStr.contains("Specific emissions") ) {
            copied = true;
        }

        if ( copied )  {
            commonReportFile.write(currStr.toUtf8());
        }
        else {
            continue;
        }
    }

    commonReportFile.close();
    reportFile1.close();
    reportFile2.close();

    return commonReportFilePath;
}

} // namespace toxic
