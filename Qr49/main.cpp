/*
    main.cpp

    Copyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>

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

#include "qr49.h"
#include "precalc.h"
#include "cyclepoints.h"
#include "cycleemissions.h"
#include "qr49constants.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "reducedpower.h"
#include "commonparameters.h"

#include <iostream>
#include <string>

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>
#include <QtCore/QTranslator>

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::vector;

void ShowAbout() {

    cout << "\n\t" << "r49 distribution version " << r49version.toStdString();
    cout << "\n\t" << qr49version.toStdString() << " (libtoxic v" << libtoxicVersion.toStdString() << ")";

    cout << "\n\nCopyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>"
            "\n\nSite: http://sites.google.com/site/pa23r49/\n\n"
            "This program is free software: you can redistribute it and/or modify\n"
            "it under the terms of the GNU General Public License as published by\n"
            "the Free Software Foundation, either version 3 of the License, or\n"
            "(at your option) any later version.\n"
            "This program is distributed in the hope that it will be useful,\n"
            "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
            "GNU General Public License for more details.\n"
            "You should have received a copy of the GNU General Public License\n"
            "along with this program. If not, see <http://www.gnu.org/licenses/>.\n\n"
            "Use this program to calculate emissions of NOx, CO, CH and PT for\n"
            "stationary engines test cycles.\n\n"
            "Run \"Qr49 task=help\" to help.\n\n";
}

void ShowHelp() {

    cout << "Usage:\n\n  Qr49 [task=TASK] [Vh=VH] [standard=STANDARD] [FuelType=FUELTYPE] \\\n"
            "       [NOxSample=NOXSAMPLE] [PTcalc=PTCALC]                       \\\n"
            "       [PTmass=PTMASS] [AddPoinsCalc=ADDPOINTSCALC]                \\\n"
            "       [CalcConfigFile=CALCCONFIGFILE]\n\n"
            "  task=TASK\t\tvariants of TASK:\n"
            "\t\t\tABCspeeds    - for A,B,C calculation;\n"
            "\t\t\tpoints       - for ESC points calculation;\n"
            "\t\t\temissions*   - for specific emissions calculation;\n"
            "\t\t\tReducedPower - for reduce power calculation\n"
            "\t\t\t               (regulation 85);\n"
            "\t\t\tELRsmoke     - for ELR smoke calculation;\n"
            "\t\t\thelp         - for show help.\n\n"
            "  Vh=VH\t\t\tvariants of VH:\n"
            "\t\t\tNumber - engine capacity, litres.\n\n"
            "  standard=STANDARD\tvariants of STANDARD:\n"
            "\t\t\tEU6; EU5; EU4*; EU3; EU2; EU1; EU0; OST; GOST\n"
            "\t\t\tr96E8; r96F8; r96G8; r96D8;\n"
            "\t\t\tr96E5; r96F5; r96G5; r96D5;\n"
            "\t\t\tr96H8; r96I8; r96J8; r96K8;\n"
            "\t\t\tr96H5; r96I5; r96J5; r96K5;\n"
            "\t\t\tC1; D1; D2; E1; E2; E3; E5;\n"
            "\t\t\tF; G1; G2;\n"
            "\t\t\tFreeCalc - free calculation without report and\n"
            "\t\t\t           limitation points number (adjusting\n"
            "\t\t\t           characteristics for example).\n\n"
            "  FuelType=FUELTYPE\tvariants of FUELTYPE:\n"
            "\t\t\tdiesel* - if you use diesel fuel;\n"
            "\t\t\tmotor   - if you use motor fuel;\n"
            "\t\t\tmazut   - if you use mazut.\n\n"
            "  NOxSample=NOXSAMPLE\tvariants of NOXSAMPLE:\n"
            "\t\t\twet* - if your measurements based on wet sample;\n"
            "\t\t\tdry  - if your measurements based on dry sample.\n\n"
            "  PTcalc=PTCALC\t\tvariants of PTCALC:\n"
            "\t\t\tThroughSmoke* - for calculate specific PT emissions\n"
            "\t\t\t                through smoke measurements;\n"
            "\t\t\tThroughPTmass - for calculate specific PT emissions\n"
            "\t\t\t                through PT mass;\n"
            "\t\t\tno            - if you don't need PT calculation.\n\n"
            "  PTmass=PTMASS\t\tvariants of PTMASS:\n"
            "\t\t\tNumber - total PT mass on all filters, mg.\n\n"
            "  AddPointsCalc=ADDPOINTSCALC\tvariants of ADDPOINTSCALC:\n"
            "\t\t\tyes - if you want calculate additional points;\n"
            "\t\t\tno* - if you don't need additional points.\n\n"
            "  CalcConfigFile=CALCCONFIGFILE\tvariants of CALCCONFIGFILE:\n"
            "\t\t\tCalcConfigFile - if you want get calculation settings\n"
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
            "\t1. Run \"Qr49 task=ABCspeeds\" to calculate A,B,C speeds and\n"
            "\t   speeds of additional points.\n"
            "\t2. Define powers from full load curve for calculated speeds.\n"
            "\t3. Edit \"SourceData1-EU3456.csv\" file.\n"
            "\t4. Run \"Qr49 task=points standard=EU3\" to generate\n"
            "\t   \"SourceData2-CyclePoints.csv\" file.\n"
            "\t5. Edit \"SourceData2-CyclePoints.csv\" file.\n"
            "\t6. Run \"Qr49 task=emissions standard=EU3 NOxSample=wet\n"
            "\t   AddPointsCalc=yes\" to calculate emissions and generate\n"
            "\t   report files.\n\n";
}

bool ParsingParameters(LibtoxicParameters *params, int argc, char **argv) {

    QString operandstr;
    QStringList elements;
    QString param;
    QString value;

    for (ptrdiff_t i=1; i<argc; i++) {

        operandstr = QString::fromAscii(argv[i]);

        elements = operandstr.split(oparandsDelimiter);

        param = elements.at(0);
        value = elements.at(1);

        if (param == "task") {

            if ( (value == "ABCspeeds"   ) ||
                 (value == "points"      ) ||
                 (value == "emissions"   ) ||
                 (value == "ReducedPower") ||
                 (value == "ELRsmoke"    ) ||
                 (value == "help"        ) ) { params->SetTask(value); }

            else {

                qDebug() << "Qr49 WARNING: ParsingParameters: I am in \"else\" of ParsingParameters function!";
                qDebug() << "Qr49 WARNING: ParsingParameters: Default value will be using for task parameter!";
            }
        }
        else if (param == "Vh") {

            double val = value.toDouble();

            if ( (val>0) && (val<666) ) {

                params->SetVh(&val);
            }
            else {

                elements.clear();

                qDebug() << "Qr49 ERROR: ParsingParameters: incorrect Vh value!";

                return 1;
            }
        }
        else if (param == "standard") {

            if ( (value == "EU6"   ) ||
                 (value == "EU5"   ) ||
                 (value == "EU4"   ) ||
                 (value == "EU3"   ) ||
                 (value == "EU2"   ) ||
                 (value == "EU1"   ) ||
                 (value == "EU0"   ) ||
                 (value == "OST"   ) ||
                 (value == "GOST"  ) ||
                 (value == "r96E8" ) ||
                 (value == "r96F8" ) ||
                 (value == "r96G8" ) ||
                 (value == "r96D8" ) ||
                 (value == "r96E5" ) ||
                 (value == "r96F5" ) ||
                 (value == "r96G5" ) ||
                 (value == "r96D5" ) ||
                 (value == "r96H8" ) ||
                 (value == "r96I8" ) ||
                 (value == "r96J8" ) ||
                 (value == "r96K8" ) ||
                 (value == "r96H5" ) ||
                 (value == "r96I5" ) ||
                 (value == "r96J5" ) ||
                 (value == "r96K5" ) ||
                 (value == "C1" ) ||
                 (value == "D1" ) ||
                 (value == "D2" ) ||
                 (value == "E1" ) ||
                 (value == "E2" ) ||
                 (value == "E3" ) ||
                 (value == "E5" ) ||
                 (value == "F" )  ||
                 (value == "G1" ) ||
                 (value == "G2" ) ||
                 (value == "FreeCalc" ) ) { params->SetStandard(value); }

            else {

                qDebug() << "Qr49 WARNING: ParsingParameters: I am in \"else\" of ParsingParameters function!";
                qDebug() << "Qr49 WARNING: ParsingParameters: Default value will be using for cycle parameter!";
            }
        }
        else if (param == "FuelType") {

            if ( (value == "diesel") ||
                 (value == "motor" ) ||
                 (value == "mazut" ) ) { params->SetFuelType(value); }

            else {

                qDebug() << "Qr49 WARNING: ParsingParameters: I am in \"else\" of ParsingParameters function!";
                qDebug() << "Qr49 WARNING: ParsingParameters: Default value will be using for FuelType parameter!";
            }
        }
        else if (param == "NOxSample") {

            if ( (value == "wet") ||
                 (value == "dry") ) { params->SetNOxSample(value); }

            else {

                qDebug() << "Qr49 WARNING: ParsingParameters: I am in \"else\" of ParsingParameters function!";
                qDebug() << "Qr49 WARNING: ParsingParameters: Default value will be using for NOxSample type!";
            }
        }
        else if (param == "PTcalc") {

            if ( (value == "ThroughSmoke" ) ||
                 (value == "ThroughPTmass") ||
                 (value == "no"           ) ) { params->SetPTcalc(value); }

            else {

                qDebug() << "Qr49 WARNING: ParsingParameters: I am in \"else\" of ParsingParameters function!";
                qDebug() << "Qr49 WARNING: ParsingParameters: Default value will be using for PT calc method!";
            }
        }
        else if (param == "PTmass") {

            double val = value.toDouble();

            if ( (val>0) && (val<666000) ) {

                params->SetPTmass(&val);
            }
            else {

                elements.clear();

                qDebug() << "Qr49 ERROR: ParsingParameters: incorrect PTmass value!";

                return false;
            }
        }
        else if (param == "AddPointsCalc") {

            if ( (value == "yes") ||
                 (value == "no" ) ) { params->SetAddPointsCalc(value); }

            else {

                qDebug() << "Qr49 WARNING: ParsingParameters: I am in \"else\" of ParsingParameters function!";
                qDebug() << "Qr49 WARNING: ParsingParameters: Default value will be using for AddPointsCalc value!";
            }
        }
        else if (param == "CalcConfigFile") {

            params->SetCalcConfigFile(value);
        }
        else {

            elements.clear();

            qDebug() << "Qr49 ERROR: ParsingParameters: incorrect program parameters!";

            return false;
        }

        elements.clear();
    }

    return true;
}

int main(int argc, char **argv) {

    if (argc > 1) {

        LibtoxicParameters *params = new LibtoxicParameters();

        if (!ParsingParameters(params, argc, argv)) {

            delete params;

            qDebug() << "Qr49 ERROR: main: ParsingParameters function returns false!";
            cout << "Press Enter to exit...";
            cin.get();

            return false;
        }

        CommonParameters *config = new CommonParameters();

        if (!config->ReadConfigFile(configFileName)) {

            qDebug() << "Qr49 WARNING: main: ReadConfigFile function returns false! Default values will be used.";
        }

        if (params->GetTask() == "ABCspeeds") {

            double n_hi = 0, n_lo = 0;
            double A = 0, B = 0, C = 0, a1 = 0, a2 = 0, a3 = 0, n_ref = 0;

            cout << endl << "n_hi [min-1]: "; if(!(cin >> n_hi)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "n_lo [min-1]: "; if(!(cin >> n_lo)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }

            if (!CalcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: CalcABC function returns false!";
                cout << "Press Enter to exit...";
                cin.get(); cin.get();

                return false;
            }

            cout << endl << "A    = " << A;
            cout << endl << "B    = " << B;
            cout << endl << "C    = " << C;
            cout << endl << "a1   = " << a1;
            cout << endl << "a2   = " << a2;
            cout << endl << "a3   = " << a3;
            cout << endl << "nref = " << n_ref << endl << endl;

            cout << "Press Enter to exit...";
            cin.get(); cin.get();
        }
        else if (params->GetTask() == "points") {

            CyclePoints *myPoints = new CyclePoints(params, config);

            if (!myPoints->GetDataFromCSV()) {

                delete myPoints;
                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: GetDataFromCSV function returns false!";
                cout << "Press Enter to exit...";
                cin.get();

                return false;
            }

            if (!myPoints->FillArrays()) {

                delete myPoints;
                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: FillArrays function returns false!";
                cout << "Press Enter to exit...";
                cin.get();

                return false;
            }

            myPoints->CreateReport();

            delete myPoints;

            cout << "Press Enter to exit...";
            cin.get();
        }
        else if (params->GetTask() == "emissions") {

            CycleEmissions *myEmissions = new CycleEmissions(params, config);

            if (!myEmissions->GetDataFromCSV()) {

                delete myEmissions;
                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: GetDataFromCSV function returns false!";
                cout << "Press Enter to exit...";
                cin.get();

                return false;
            }

            if (!myEmissions->MakeCalculations()) {

                delete myEmissions;
                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: MakeCalculations function returns false!";
                cout << "Press Enter to exit...";
                cin.get();

                return false;
            }

            myEmissions->CreateReports(true);

            delete myEmissions;

            cout << "Press Enter to exit...";
            cin.get();
        }
        else if (params->GetTask() == "ReducedPower") {

            ReducedPower *myReducedPower = new ReducedPower(params, config);

            if (!myReducedPower->GetDataFromCSV()) {

                delete myReducedPower;
                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: GetDataFromCSV function returns false!";
                cout << "Press Enter to exit...";
                cin.get();

                return false;
            }

            if (!myReducedPower->ReducePower()) {

                delete myReducedPower;
                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: ReducePower function returns false!";
                cout << "Press Enter to exit...";
                cin.get();

                return false;
            }

            myReducedPower->CreateReports();

            delete myReducedPower;

            cout << "Press Enter to exit...";
            cin.get();
        }
        else if (params->GetTask() == "ELRsmoke") {

            double smoke_A1 = 0, smoke_A2 = 0, smoke_A3 = 0;
            double smoke_B1 = 0, smoke_B2 = 0, smoke_B3 = 0;
            double smoke_C1 = 0, smoke_C2 = 0, smoke_C3 = 0;

            double smokeELR = 0;

            cout << endl << "First  smoke peak at speed A: "; if(!(cin >> smoke_A1)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "Second smoke peak at speed A: "; if(!(cin >> smoke_A2)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "Third  smoke peak at speed A: "; if(!(cin >> smoke_A3)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }

            cout << endl << "First  smoke peak at speed B: "; if(!(cin >> smoke_B1)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "Second smoke peak at speed B: "; if(!(cin >> smoke_B2)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "Third  smoke peak at speed B: "; if(!(cin >> smoke_B3)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }

            cout << endl << "First  smoke peak at speed C: "; if(!(cin >> smoke_C1)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "Second smoke peak at speed C: "; if(!(cin >> smoke_C2)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }
            cout         << "Third  smoke peak at speed C: "; if(!(cin >> smoke_C3)) { cout << "\nQr49 ERROR: Bad data!\n\n"; return false; }

            if (!CalcELR(&smoke_A1, &smoke_A2, &smoke_A3,
                         &smoke_B1, &smoke_B2, &smoke_B3,
                         &smoke_C1, &smoke_C2, &smoke_C3,
                         &smokeELR)) {

                delete params;
                delete config;

                qDebug() << "Qr49 ERROR: main: CalcELR function returns false!";
                cout << "Press Enter to exit...";
                cin.get(); cin.get();

                return false;
            }

            cout << endl << "ELR smoke = " << smokeELR << endl << endl;

            cout << "Press Enter to exit...";
            cin.get(); cin.get();
        }
        else if (params->GetTask() == "help") {

            ShowAbout();
            ShowHelp();

            cout << "Press Enter to exit...";
            cin.get();
        }
        else {

            qDebug() << "Qr49 ERROR: main: incorrect values in an array of operands!";
        }

        delete params;
        delete config;

        return 0;
    }
    else {

        ShowAbout();

        QApplication a(argc, argv);

        QTranslator translator;
        translator.load("qr49_ru.qm");
        a.installTranslator(&translator);

        MainWindow w;
        w.show();

        return a.exec();
    }
}
