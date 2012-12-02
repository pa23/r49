/*
    cr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: main.cpp

    Copyright (C) 2011-2012 Artem Petrov <pa2311@gmail.com>

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

#include "precalc.h"
#include "cyclepoints.h"
#include "cycleemissions.h"
#include "cr49constants.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "reducedpower.h"
#include "commonparameters.h"
#include "r49.h"
#include "toxicerror.h"

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVector>

#include <iostream>

using std::cout;
using std::cin;

void showAbout() {

    cout << "\n\t"
         << "r49 distribution version "
         << R49VERSION.toStdString();

    cout << "\n\t"
         << CR49VERSION.toStdString()
         << " libtoxic v"
         << LIBTOXICVERSION.toStdString();

    cout << "\n\nCopyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>"
            "\n\nProgram hosting: https://github.com/pa23/r49\n"
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
            "Run \"cr49 task=help\" to help.\n\n";
}

void showHelp() {

    cout << "Usage:\n\n  cr49 [task=TASK] [Vh=VH] [standard=STANDARD] "
            "[ChargingType=CHARGINGTYPE]  \\\n"
            "       [FuelType=FUELTYPE] [NOxSample=NOXSAMPLE] "
            "[PTcalc=PTCALC]            \\\n"
            "       [PTmass=PTMASS] "
            "[AddPoinsCalc=ADDPOINTSCALC]                         \\\n"
            "       [CalcConfigFile=CALCCONFIGFILE]\n\n"
            "  task=TASK\t\tvariants of TASK:\n"
            "\t\t\tpoints       - for ESC points calculation;\n"
            "\t\t\temissions*   - for specific emissions calculation;\n"
            "\t\t\treducedPower - for reduce power calculation\n"
            "\t\t\t               (regulation 85);\n"
            "\t\t\tABCspeeds    - for A,B,C calculation;\n"
            "\t\t\thelp         - for show help.\n\n"
            "  Vh=VH\t\t\tvariants of VH:\n"
            "\t\t\tNumber - engine capacity, litres.\n\n"
            "  standard=STANDARD\tvariants of STANDARD:\n"
            "\t\t\teu6; eu5; eu4*; eu3; eu2; eu1; eu0; ost; gost\n"
            "\t\t\tr96e8; r96f8; r96g8; r96d8;\n"
            "\t\t\tr96e5; r96f5; r96g5; r96d5;\n"
            "\t\t\tr96h8; r96i8; r96j8; r96k8;\n"
            "\t\t\tr96h5; r96i5; r96j5; r96k5;\n"
            "\t\t\tc1; d1; d2; e1; e2; e3; e5;\n"
            "\t\t\tf; g1; g2;\n"
            "\t\t\tfreeCalc - free calculation without report and\n"
            "\t\t\t           limitation points number (adjusting\n"
            "\t\t\t           characteristics for example).\n\n"
            "  chargingType=CHARGINGTYPE\tvariants of CHARGINGTYPE:\n"
            "\t\t\tnoOrMechanical - engine w/o charging or with\n"
            "\t\t\t                 mechanical charging;\n"
            "\t\t\tgasTurbine*    - engine with a gas turbine charging.\n\n"
            "  fuelType=FUELTYPE\tvariants of FUELTYPE:\n"
            "\t\t\tdiesel* - if you use diesel fuel;\n"
            "\t\t\tmotor   - if you use motor fuel;\n"
            "\t\t\tmazut   - if you use mazut.\n\n"
            "  NOxSample=NOXSAMPLE\tvariants of NOXSAMPLE:\n"
            "\t\t\twet* - if your measurements based on wet sample;\n"
            "\t\t\tdry  - if your measurements based on dry sample.\n\n"
            "  PTcalc=PTCALC\t\tvariants of PTCALC:\n"
            "\t\tthroughSmoke          - for calculate specific PT emissions\n"
            "\t\t                        through smoke measurements;\n"
            "\t\tthroughPTmass         - for calculate specific PT emissions\n"
            "\t\t                        through PT mass;\n"
            "\t\tthroughSmokeAndPTmass - for calculate specific PT emissions\n"
            "\t\t                        through PT mass and through smoke\n"
            "\t\t                        measurements;\n"
            "\t\tno*                   - if you don't need PT calculation.\n\n"
            "  PTmass=PTMASS\t\tvariants of PTMASS:\n"
            "\t\t\tNumber - total PT mass on all filters, mg.\n\n"
            "  addPointsCalc=ADDPOINTSCALC\tvariants of ADDPOINTSCALC:\n"
            "\t\t\tyes - if you want calculate additional points;\n"
            "\t\t\tno* - if you don't need additional points.\n\n"
            "  calcConfigFile=CALCCONFIGFILE\tvariants of CALCCONFIGFILE:\n"
            "\t\t\tcalcConfigFile - if you want get calculation settings\n"
            "\t\t\t                 from another file.\n\n"
            "  * - default values.\n\n";

    cout << "Notes:\n\n"
            "\tProgram use formulas from:\n"
            "\tUN ECE Regulation No. 49 Revision 4;\n"
            "\tUN ECE Regulation No. 49 Revision 1;\n"
            "\tUN ECE Regulation No. 96 Revision 1;\n"
            "\tUN ECE Regulation No. 85 Revision 1;\n"
            "\tOST 37.001.234-81;\n"
            "\tGOST 17.2.2.05-97;\n"
            "\tGOST 30574-98;\n"
            "\tGOST R 51249-99.\n\n"
            "\tCO sample is always dry.\n"
            "\tCH sample is always wet.\n\n"
            "\tPoint 14 is between points 4, 8, 10, 12;\n"
            "\tPoint 15 is between points 3, 4,  5,  6;\n"
            "\tPoint 16 is between points 3, 9, 11, 13.\n\n"
            "\tIf you use FreeCalc option, then additional points are not\n"
            "\tcalculated and you can see results only in Array.csv file,\n"
            "\tprogram will use up-to-date formulas for Kwr and Khd calc.\n\n"
            "\tIf you use reverse calculation method for NOx, additional\n"
            "\tpoints also will not be calculated.\n\n";

    cout << "Variant of usage:\n\n"
            "\t1. Run \"cr49 task=ABCspeeds\" to calculate A,B,C speeds and\n"
            "\t   speeds of additional points.\n"
            "\t2. Define powers from full load curve for calculated speeds.\n"
            "\t3. Edit \"SourceData1-EU3456.csv\" file.\n"
            "\t4. Run \"cr49 task=points standard=eu3\" to generate\n"
            "\t   \"SourceData2-CyclePoints.csv\" file.\n"
            "\t5. Edit \"SourceData2-CyclePoints.csv\" file.\n"
            "\t6. Run \"cr49 task=emissions standard=eu3 NOxSample=wet\n"
            "\t   addPointsCalc=yes\" to calculate emissions and generate\n"
            "\t   report files.\n\n";
}

bool parsingParameters(QSharedPointer<LibtoxicParameters> params,
                       int argc,
                       char** argv) {

    QString operandstr;
    QStringList elements;
    QString param;
    QString value;

    for ( ptrdiff_t i=1; i<argc; i++ ) {

        operandstr = QString::fromAscii(argv[i]);

        elements = operandstr.split(OPERANDSDELIMITER);

        param = elements.at(0);
        value = elements.at(1);

        if ( param == "task" ) {

            params->setTask(value);
        }
        else if ( param == "Vh" ) {

            const double val = value.toDouble();

            if ( (val>0) && (val<666000) ) {

                params->setVh(val);
            }
            else {

                elements.clear();

                cout << "\nIncorrect Vh value!";
                return false;
            }
        }
        else if ( param == "standard" ) {

            params->setStandard(value);
        }
        else if ( param == "chargingType" ) {

            params->setChargingType(value);
        }
        else if ( param == "fuelType" ) {

            params->setFuelType(value);
        }
        else if ( param == "NOxSample" ) {

            params->setNOxSample(value);
        }
        else if ( param == "PTcalc" ) {

            params->setPTcalc(value);
        }
        else if ( param == "PTmass" ) {

            const double val = value.toDouble();

            if ( (val>0) && (val<666000) ) {

                params->setPTmass(val);
            }
            else {

                elements.clear();

                cout << "\nIncorrect PTmass value!";
                return false;
            }
        }
        else if ( param == "addPointsCalc" ) {

            params->setAddPointsCalc(value);
        }
        else if ( param == "calcConfigFile" ) {

            params->setCalcConfigFile(value);
        }
        else {

            elements.clear();

            cout << "Incorrect program parameters!";
            return false;
        }

        elements.clear();
    }

    return true;
}

int main(int argc, char **argv) {

    if ( argc > 1 ) {

        //

        QSharedPointer<LibtoxicParameters> params(new LibtoxicParameters());

        if ( !parsingParameters(params, argc, argv) ) {

            cout << "\nErrors during parsing parameters!\n";
            return 1;
        }

        const ptrdiff_t currtask = params->valTask();

        if ( currtask == TASK_ABCSPEEDS ) {

            double n_hi = 0, n_lo = 0;
            double A = 0, B = 0, C = 0, a1 = 0, a2 = 0, a3 = 0, n_ref = 0;

            cout << "\n" << "n_hi [min-1]: ";

            if( !(cin >> n_hi) ) {

                cout << "\ncr49 ERROR: Bad data!\n\n";
                return 0;
            }

            cout << "n_lo [min-1]: ";

            if( !(cin >> n_lo) ) {

                cout << "\ncr49 ERROR: Bad data!\n\n";
                return 0;
            }

            try {

                calcABC(n_hi, n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref);
            }
            catch(const ToxicError &toxerr) {

                cout << "\n" << toxerr.toxicErrMsg().toStdString();
                return 0;
            }

            cout << "\nA    = " << A;
            cout << "\nB    = " << B;
            cout << "\nC    = " << C;
            cout << "\na1   = " << a1;
            cout << "\na2   = " << a2;
            cout << "\na3   = " << a3;
            cout << "\nnref = " << n_ref << "\n\n";

            return 0;
        }
        else if ( currtask == TASK_HELP ) {

            showAbout();
            showHelp();

            return 0;
        }

        QSharedPointer<CommonParameters> config(new CommonParameters());

        try {

            config->readConfigFile(CONFIGFILENAME);
        }
        catch(const ToxicError &toxerr) {

            cout << "\n" << toxerr.toxicErrMsg().toStdString() << "\n";
            return 1;
        }

        QVector< QVector<double> > data;

        if ( currtask == TASK_POINTS ) {

            try {

                QSharedPointer<CyclePoints>
                        myPoints(new CyclePoints(params, config));

                myPoints->readCSV(data);
                myPoints->fillArrays();

                cout << "\n" << myPoints->createReport().toStdString();
            }
            catch(const ToxicError &toxerr) {

                cout << "\n" << toxerr.toxicErrMsg().toStdString() << "\n";
                return 1;
            }
        }
        else if ( currtask == TASK_EMISSIONS ) {

            try {

                QSharedPointer<CycleEmissions>
                        myEmissions(new CycleEmissions(params, config));

                myEmissions->readCSV(data);
                myEmissions->calculate();

                cout << "\n" << myEmissions->createReports().toStdString();
            }
            catch(const ToxicError &toxerr) {

                cout << "\n" << toxerr.toxicErrMsg().toStdString() << "\n";
                return 1;
            }
        }
        else if ( currtask == TASK_REDUCEDPOWER ) {

            try {

                QSharedPointer<ReducedPower>
                        myReducedPower(new ReducedPower(params, config));

                myReducedPower->readCSV(data);
                myReducedPower->reducePower();

                cout << "\n" << myReducedPower->createReports().toStdString();
            }
            catch(const ToxicError &toxerr) {

                cout << "\n" << toxerr.toxicErrMsg().toStdString() << "\n";
                return 1;
            }
        }
        else {

            cout << "\n" << "Incorrect values in an array of operands!\n";
        }
    }
    else {

        showAbout();
    }

    return 0;
}
