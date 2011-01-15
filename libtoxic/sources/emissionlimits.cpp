/*
    emissionlimits.cpp

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

#include "emissionlimits.h"

#include <cmath>

#include <QDebug>
#include <QString>

double val_NOxLimit(QString stnd, double n_rated) {

    if      (stnd == "EU6" ) { return  2.00; }
    else if (stnd == "EU5" ) { return  2.00; }
    else if (stnd == "EU4" ) { return  3.50; }
    else if (stnd == "EU3" ) { return  5.00; }
    else if (stnd == "EU2" ) { return  7.00; }
    else if (stnd == "EU1" ) { return  9.00; }
    else if (stnd == "EU0" ) { return 15.80; }
    else if (stnd == "OST" ) { return 18.35; }
    else if (stnd == "GOSTnu" ) { return 18.0; }
    else if (stnd == "GOSTnl" ) { return  9.0; }
    else if (stnd == "GOSTou" ) { return 18.0; }
    else if (stnd == "GOSTol" ) { return  9.0; }
    else if ( (stnd == "r96E8") || (stnd == "r96E5") ) { return  6.00; }
    else if ( (stnd == "r96F8") || (stnd == "r96F5") ) { return  6.00; }
    else if ( (stnd == "r96G8") || (stnd == "r96G5") ) { return  7.00; }
    else if ( (stnd == "r96D8") || (stnd == "r96D5") ) { return  8.00; }
    else if ( (stnd == "Fnew") ) { return        12.0; }
    else if ( (stnd == "Fold") ) { return 0.95 * 12.0; }
    else if ( (stnd == "C1new") || (stnd == "D1new") || (stnd == "D2new") || (stnd == "G1new") || (stnd == "G2new") ) { return        10.0; }
    else if ( (stnd == "C1old") || (stnd == "D1old") || (stnd == "D2old") || (stnd == "G1old") || (stnd == "G2old") ) { return 0.95 * 10.0; }
    else if ( (stnd == "E1new") || (stnd == "E2new") || (stnd == "E3new") || (stnd == "E5new") ) {

        if ( (n_rated < 130) || (n_rated == 130) ) {

            return 17.0;
        }
        else if ( (n_rated > 130) || (n_rated < 2000) || (n_rated == 2000) ) {

            return 45.0 * pow(n_rated, -0.2);
        }
        else if (n_rated > 2000) {

            return 9.8;
        }
    }
    else if ( (stnd == "E1old") || (stnd == "E2old") || (stnd == "E3old") || (stnd == "E5old") ) {

        if ( (n_rated < 130) || (n_rated == 130) ) {

            return 0.95 * 17.0;
        }
        else if ( (n_rated > 130) || (n_rated < 2000) || (n_rated == 2000) ) {

            return 0.95 * 45.0 * pow(n_rated, -0.2);
        }
        else if (n_rated > 2000) {

            return 0.95 * 9.8;
        }
    }
    else {

        qDebug() << "libtoxic WARNING: EmissionLimits: val_NOxLimit: emission limit is unknown!";

        return 0;
    }

    return 0;
}

double val_COLimit(QString stnd) {

    if      (stnd == "EU6" ) { return  1.50; }
    else if (stnd == "EU5" ) { return  1.50; }
    else if (stnd == "EU4" ) { return  1.50; }
    else if (stnd == "EU3" ) { return  2.10; }
    else if (stnd == "EU2" ) { return  4.00; }
    else if (stnd == "EU1" ) { return  4.90; }
    else if (stnd == "EU0" ) { return 12.30; }
    else if (stnd == "OST" ) { return  9.50; }
    else if (stnd == "GOSTnu" ) { return 10.0; }
    else if (stnd == "GOSTnl" ) { return  4.0; }
    else if (stnd == "GOSTou" ) { return 14.0; }
    else if (stnd == "GOSTol" ) { return  5.6; }
    else if ( (stnd == "r96E8") || (stnd == "r96E5") ) { return  3.50; }
    else if ( (stnd == "r96F8") || (stnd == "r96F5") ) { return  5.00; }
    else if ( (stnd == "r96G8") || (stnd == "r96G5") ) { return  5.00; }
    else if ( (stnd == "r96D8") || (stnd == "r96D5") ) { return  5.50; }
    else if ( (stnd == "r96H8") || (stnd == "r96H5") ) { return  3.50; }
    else if ( (stnd == "r96I8") || (stnd == "r96I5") ) { return  5.00; }
    else if ( (stnd == "r96J8") || (stnd == "r96J5") ) { return  5.00; }
    else if ( (stnd == "r96K8") || (stnd == "r96K5") ) { return  5.50; }
    else if ( (stnd == "Fnew") || (stnd == "C1new") ||
              (stnd == "D1new") || (stnd == "D2new") ||
              (stnd == "G1new") || (stnd == "G2new") ||
              (stnd == "E1new") || (stnd == "E2new") || (stnd == "E3new") || (stnd == "E5new") ) { return       3.0; }
    else if ( (stnd == "Fold") || (stnd == "C1old") ||
              (stnd == "D1old") || (stnd == "D2old") ||
              (stnd == "G1old") || (stnd == "G2old") ||
              (stnd == "E1old") || (stnd == "E2old") || (stnd == "E3old") || (stnd == "E5old") ) { return 1.2 * 3.0; }
    else {

        qDebug() << "libtoxic WARNING: EmissionLimits: val_COLimit: emission limit is unknown!";

        return 0;
    }
}

double val_CHLimit(QString stnd) {

    if      (stnd == "EU6" ) { return  0.25; }
    else if (stnd == "EU5" ) { return  0.46; }
    else if (stnd == "EU4" ) { return  0.46; }
    else if (stnd == "EU3" ) { return  0.66; }
    else if (stnd == "EU2" ) { return  1.10; }
    else if (stnd == "EU1" ) { return  1.23; }
    else if (stnd == "EU0" ) { return  2.60; }
    else if (stnd == "OST" ) { return  3.40; }
    else if (stnd == "GOSTnu" ) { return 3.0; }
    else if (stnd == "GOSTnl" ) { return 1.5; }
    else if (stnd == "GOSTou" ) { return 4.5; }
    else if (stnd == "GOSTol" ) { return 2.2; }
    else if ( (stnd == "r96E8") || (stnd == "r96E5") ) { return  1.00; }
    else if ( (stnd == "r96F8") || (stnd == "r96F5") ) { return  1.00; }
    else if ( (stnd == "r96G8") || (stnd == "r96G5") ) { return  1.30; }
    else if ( (stnd == "r96D8") || (stnd == "r96D5") ) { return  1.50; }
    else if ( (stnd == "Fnew") || (stnd == "C1new") ||
              (stnd == "D1new") || (stnd == "D2new") ||
              (stnd == "G1new") || (stnd == "G2new") ||
              (stnd == "E1new") || (stnd == "E2new") || (stnd == "E3new") || (stnd == "E5new") ) { return        1.0; }
    else if ( (stnd == "Fold") || (stnd == "C1old") ||
              (stnd == "D1old") || (stnd == "D2old") ||
              (stnd == "G1old") || (stnd == "G2old") ||
              (stnd == "E1old") || (stnd == "E2old") || (stnd == "E3old") || (stnd == "E5old") ) { return 1.25 * 1.0; }
    else {

        qDebug() << "libtoxic WARNING: EmissionLimits: val_CHLimit: emission limit is unknown!";

        return 0;
    }
}

double val_PTLimit(QString stnd) {

    if      (stnd == "EU6" ) { return  0.02; }
    else if (stnd == "EU5" ) { return  0.02; }
    else if (stnd == "EU4" ) { return  0.02; }
    else if (stnd == "EU3" ) { return  0.10; }
    else if (stnd == "EU2" ) { return  0.15; }
    else if (stnd == "EU1" ) { return  0.40; }
    else if ( (stnd == "r96E8") || (stnd == "r96E5") ) { return  0.20; }
    else if ( (stnd == "r96F8") || (stnd == "r96F5") ) { return  0.30; }
    else if ( (stnd == "r96G8") || (stnd == "r96G5") ) { return  0.40; }
    else if ( (stnd == "r96D8") || (stnd == "r96D5") ) { return  0.80; }
    else if ( (stnd == "r96H8") || (stnd == "r96H5") ) { return  0.20; }
    else if ( (stnd == "r96I8") || (stnd == "r96I5") ) { return  0.30; }
    else if ( (stnd == "r96J8") || (stnd == "r96J5") ) { return  0.40; }
    else if ( (stnd == "r96K8") || (stnd == "r96K5") ) { return  0.60; }
    else {

        qDebug() << "libtoxic WARNING: EmissionLimits: val_PTLimit: emission limit is unknown!";

        return 0;
    }
}

double val_NOxCHLimit(QString stnd) {

    if      ( (stnd == "r96H8") || (stnd == "r96H5") ) { return  4.00; }
    else if ( (stnd == "r96I8") || (stnd == "r96I5") ) { return  4.00; }
    else if ( (stnd == "r96J8") || (stnd == "r96J5") ) { return  4.70; }
    else if ( (stnd == "r96K8") || (stnd == "r96K5") ) { return  7.50; }
    else {

        qDebug() << "libtoxic WARNING: EmissionLimits: val_NOxCHLimit: emission limit is unknown!";

        return 0;
    }
}
