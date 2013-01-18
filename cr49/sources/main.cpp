/*
    cr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: main.cpp

    Copyright (C) 2011-2013 Artem Petrov <pa2311@gmail.com>

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

#include "constants.h"

#include "txAuxiliaryFunctions.h"
#include "txPointsOfCycle.h"
#include "txEmissionsOnCycle.h"
#include "txReducedPower.h"
#include "txConstants.h"
#include "txIdentification.h"
#include "txCommonParameters.h"
#include "txCalculationOptions.h"
#include "txError.h"

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVector>

#include <iostream>

using std::cout;
using std::cin;

void showVersion() {

    cout << "\n"
         << "+------------------------------+\n"
         << "|                              |\n"
         << "| "
         << CR49INFO.toStdString()
         << ", "
         << toxic::toxicIdentification{}.name().toStdString()
         << " v"
         << toxic::toxicIdentification{}.version().toStdString()
         << " |\n"
         << "|                              |\n"
         << "+------------------------------+\n\n";
}

void showAbout() {

    cout << "Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>"
            "\n\nSource code hosting: https://github.com/pa23/r49\n"
            "Author's blog (RU): http://pa2311.blogspot.com\n\n"
            "This program is free software: you can redistribute "
            "it and/or modify\n"
            "it under the terms of the GNU General Public License as "
            "published by\n"
            "the Free Software Foundation, either version 3 of the "
            "License, or\n"
            "(at your option) any later version.\n"
            "This program is distributed in the hope that it will be useful,\n"
            "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
            "GNU General Public License for more details.\n"
            "You should have received a copy of the GNU General Public "
            "License\n"
            "along with this program. If not, see "
            "<http://www.gnu.org/licenses/>.\n\n"
            "Use this program to calculate emissions of "
            "NOx, CO, CH and PT for\n"
            "stationary engines test cycles.\n\n"
            "Run \"cr49 --help\" to help.\n\n";
}

void showHelp() {

    cout << "Usage: cr49 [PARAMETER...]...\n\n"
            "  --help\n"
            "  --task={points|emissions*|reducedPower|ABCspeeds}\n"
            "    points       - test cycle points calculation;\n"
            "    emissions*   - specific emissions calculation;\n"
            "    reducedPower - reduce power calculation (R85)\n"
            "    ABCspeeds    - A,B,C speeds calculation.\n"
            "  --Vh=<NUMBER>\n"
            "    NUMBER - engine capacity, litres.\n"
            "  --standard={eu6|eu5|eu4*|eu3|eu2|eu1|eu0|ost|gost|\n"
            "              r96e8|r96f8|r96g8|r96d8|r96e5|r96f5|r96g5|r96d5|\n"
            "              r96h8|r96i8|r96j8|r96k8|r96h5|r96i5|r96j5|r96k5|\n"
            "              c1|d1|d2|e1|e2|e3|e5|f|g1|g2|freeCalc}\n"
            "  --chargingType={noOrMechanical|gasTurbine*}\n"
            "  --fuelType={diesel*|motor|mazut}\n"
            "  --NOxSample={wet*|dry}\n"
            "  --PTcalc={throughSmoke|throughPTmass|throughSmokeAndPTmass|no*}\n"
            "  --PTmass=<NUMBER>\n"
            "    NUMBER - total PT mass on all filters, mg.\n"
            "  --addPointsCalc={yes|no*}\n"
            "  --calcConfigFile=<FILE>\n"
            "* - default values.\n\n";

    cout << "Notes:\n\n"
            "  Program use formulas from:\n"
            "    UN ECE Regulation No. 49 Revision 4;\n"
            "    UN ECE Regulation No. 49 Revision 1;\n"
            "    UN ECE Regulation No. 96 Revision 1;\n"
            "    UN ECE Regulation No. 85 Revision 1;\n"
            "    OST 37.001.234-81;\n"
            "    GOST 17.2.2.05-97;\n"
            "    GOST 30574-98;\n"
            "    GOST R 51249-99.\n\n"
            "  CO sample is always dry.\n"
            "  CH sample is always wet.\n\n"
            "  Point 14 is between points 4, 8, 10, 12;\n"
            "  Point 15 is between points 3, 4,  5,  6;\n"
            "  Point 16 is between points 3, 9, 11, 13.\n\n"
            "  If you use freeCalc option, then additional points are not calculated and \n"
            "  you can see results only in *.dat file, program will use up-to-date \n"
            "  formulas for Kwr and Khd calc.\n\n"
            "  If you use reverse calculation method for NOx, additional points also \n"
            "  will not be calculated.\n\n";

    cout << "Variant of usage:\n\n"
            "  1. Run \"cr49 --task=ABCspeeds\" to calculate A,B,C speeds and speeds for \n"
            "     additional points.\n"
            "  2. Define powers from full load curve for calculated speeds.\n"
            "  3. Edit \"SourceData1-EU3456.dat\" file.\n"
            "  4. Run \"cr49 --task=points --standard=eu3\" to generate \n"
            "     \"SourceData2-CyclePoints.csv\" file.\n"
            "  5. Edit \"SourceData2-CyclePoints.dat\" file.\n"
            "  6. Run \"cr49 --task=emissions --standard=eu3 --NOxSample=wet\"\n"
            "     to calculate emissions and generate report files.\n\n";
}

void parsingParameters(const QSharedPointer<toxic::txCalculationOptions> &calcopts,
                       int argc, char** argv) {

    QString str;
    QStringList elements;
    QString param;
    QString value;

    for ( ptrdiff_t i=1; i<argc; i++ ) {

        str = QString::fromAscii(argv[i]);

        if ( str == "--help" ) {
            calcopts->setTask(toxic::TASK_HELP);
            return;
        }

        elements = str.split("=");

        if ( elements.size() != 2 ) {

            cout << "\nIncorrect parameter \""
                 << str.toStdString()
                 <<  "\". Default value will be used!\n";
            continue;
        }

        param = elements[0];
        value = elements[1];

        if ( param == "--task" ) {
            calcopts->setTask(value);
        }
        else if ( param == "--Vh" ) {

            const double val = value.toDouble();

            if ( val>0 && val<666000 ) {
                calcopts->setVh(val);
            }
            else {
                cout << "\nIncorrect Vh value! Default value will be used!\n";
                continue;
            }
        }
        else if ( param == "--standard" ) {
            calcopts->setStandard(value);
        }
        else if ( param == "--chargingType" ) {
            calcopts->setChargingType(value);
        }
        else if ( param == "--fuelType" ) {
            calcopts->setFuelType(value);
        }
        else if ( param == "--NOxSample" ) {
            calcopts->setNOxSample(value);
        }
        else if ( param == "--PTcalc" ) {
            calcopts->setPTcalc(value);
        }
        else if ( param == "--PTmass" ) {

            const double val = value.toDouble();

            if ( val>0 && val<666000 ) {
                calcopts->setPTmass(val);
            }
            else {
                cout << "\nIncorrect PTmass value! Default value will be used!\n";
                continue;
            }
        }
        else if ( param == "--addPointsCalc" ) {
            calcopts->setAddPointsCalc(value);
        }
        else if ( param == "--calcConfigFile" ) {
            calcopts->setCalcConfigFile(value);
        }
        else {
            cout << "\nIncorrect program parameters!\n";
            return;
        }

        elements.clear();
    }
}

int main(int argc, char **argv) {

    showVersion();

    if ( argc < 2 ) {
        showAbout();
        return 0;
    }

    QSharedPointer<toxic::txCalculationOptions>
            calcopts(new toxic::txCalculationOptions());

    try {
        parsingParameters(calcopts, argc, argv);
    }
    catch(const toxic::txError &toxerr) {
        cout << toxerr.val_toxicErrMsg().toStdString() << "\n";
        return 1;
    }

    const int currtask = calcopts->val_task();

    if ( currtask == toxic::TASK_ABCSPEEDS ) {

        double n_hi = 0, n_lo = 0;
        double A = 0, B = 0, C = 0, a1 = 0, a2 = 0, a3 = 0, n_ref = 0;

        cout << "n_hi [min-1]: ";

        if( !(cin >> n_hi) ) {
            cout << "cr49 ERROR: Incorrect data!\n";
            return 1;
        }

        cout << "n_lo [min-1]: ";

        if( !(cin >> n_lo) ) {
            cout << "cr49 ERROR: Incorrect data!\n";
            return 1;
        }

        try {
            toxic::ABC(n_hi, n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref);
        }
        catch(const toxic::txError &toxerr) {
            cout << toxerr.val_toxicErrMsg().toStdString() << "\n";
            return 1;
        }

        cout << "\nA    = " << A;
        cout << "\nB    = " << B;
        cout << "\nC    = " << C;
        cout << "\na1   = " << a1;
        cout << "\na2   = " << a2;
        cout << "\na3   = " << a3;
        cout << "\nnref = " << n_ref
             << "\n";

        return 0;
    }
    else if ( currtask == toxic::TASK_HELP ) {

        showAbout();
        showHelp();

        return 0;
    }

    QSharedPointer<toxic::txCommonParameters>
            commpars(new toxic::txCommonParameters());

    try {
        commpars->readConfigFile(CONFIGFILENAME);
    }
    catch(const toxic::txError &toxerr) {
        cout << toxerr.val_toxicErrMsg().toStdString() << "\n";
        return 1;
    }

    if ( currtask == toxic::TASK_POINTS ) {

        try {
            QSharedPointer<toxic::txPointsOfCycle>
                    myPoints(new toxic::txPointsOfCycle(commpars, calcopts));
            myPoints->setSourceData();
            myPoints->calculate();
            cout << myPoints->createReports().toStdString();
        }
        catch(const toxic::txError &toxerr) {
            cout << toxerr.val_toxicErrMsg().toStdString() << "\n";
            return 1;
        }
    }
    else if ( currtask == toxic::TASK_EMISSIONS ) {

        try {
            QSharedPointer<toxic::txEmissionsOnCycle>
                    myEmissions(new toxic::txEmissionsOnCycle(commpars, calcopts));
            myEmissions->setSourceData();
            myEmissions->calculate();
            cout << myEmissions->results().toStdString();
            cout << "\n" << myEmissions->createReports().toStdString();
        }
        catch(const toxic::txError &toxerr) {
            cout << toxerr.val_toxicErrMsg().toStdString() << "\n";
            return 1;
        }
    }
    else if ( currtask == toxic::TASK_REDUCEDPOWER ) {

        try {
            QSharedPointer<toxic::txReducedPower>
                    myReducedPower(new toxic::txReducedPower(commpars, calcopts));
            myReducedPower->setSourceData();
            myReducedPower->calculate();
            cout << myReducedPower->createReports().toStdString();
        }
        catch(const toxic::txError &toxerr) {
            cout << toxerr.val_toxicErrMsg().toStdString() << "\n";
            return 1;
        }
    }
    else {
        cout << "Incorrect values in an array of operands!\n";
    }

    return 0;
}
