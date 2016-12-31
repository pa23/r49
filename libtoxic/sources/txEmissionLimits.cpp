/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionLimits.cpp

    Copyright (C) 2009-2016 Artem Petrov <pa2311@gmail.com>

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

#include "txEmissionLimits.h"
#include "txConstants.h"
#include "txError.h"

#include <cmath>

namespace toxic {

double NOxLimit(int standard, double n_rated) {

    if ( standard == STD_EU6 ) {
        return 2;
    }
    else if ( standard == STD_EU5 ) {
        return 2;
    }
    else if ( standard == STD_EU4 ) {
        return 3.5;
    }
    else if ( standard == STD_EU3 ) {
        return 5;
    }
    else if ( standard == STD_EU2 ) {
        return 7;
    }
    else if ( standard == STD_EU1 ) {
        return 9;
    }
    else if ( standard == STD_EU0 ) {
        return 15.8;
    }
    else if ( standard == STD_OST3700123481 ) {
        return 18.35;
    }
    else if ( standard == STD_GOST17220597+10 ) {
        return 18; // nu
    }
    else if ( standard == STD_GOST17220597+20 ) {
        return 9;  // nl
    }
    else if ( standard == STD_GOST17220597+30 ) {
        return 18; // ou
    }
    else if ( standard == STD_GOST17220597+40 ) {
        return 9;  // ol
    }
    else if ( (standard == STD_R96E8) || (standard == STD_R96E5) ) {
        return 6;
    }
    else if ( (standard == STD_R96F8) || (standard == STD_R96F5) ) {
        return 6;
    }
    else if ( (standard == STD_R96G8) || (standard == STD_R96G5) ) {
        return 7;
    }
    else if ( (standard == STD_R96D8) || (standard == STD_R96D5) ) {
        return 8;
    }
    else if ( (standard == STD_F+10) ) {
        return 12;          // new
    }
    else if ( (standard == STD_F+20) ) {
        return 0.95 * 12.0; // old
    }
    else if ( (standard == STD_C1+10) || (standard == STD_D1+10) ||
              (standard == STD_D2+10) || (standard == STD_G1+10) ||
              (standard == STD_G2+10) ) {
        return 10;          // new
    }
    else if ( (standard == STD_C1+20) || (standard == STD_D1+20) ||
              (standard == STD_D2+20) || (standard == STD_G1+20) ||
              (standard == STD_G2+20) ) {
        return 0.95 * 10.0; // old
    }
    else if ( (standard == STD_E1+10) || (standard == STD_E2+10) ||
              (standard == STD_E3+10) || (standard == STD_E5+10) ) {

        if ( n_rated <= 130 ) {
            return 17;
        }
        else if ( (n_rated > 130) && (n_rated <= 2000) ) {
            return 45.0 * pow(n_rated, -0.2);
        }
        else if ( n_rated > 2000 ) {
            return 9.8;
        }
    } // new
    else if ( (standard == STD_E1+20) || (standard == STD_E2+20) ||
              (standard == STD_E3+20) || (standard == STD_E5+20) ) {

        if ( n_rated <= 130 ) {
            return 0.95 * 17.0;
        }
        else if ( (n_rated > 130) && (n_rated <= 2000) ) {
            return 0.95 * 45.0 * pow(n_rated, -0.2);
        }
        else if ( n_rated > 2000 ) {
            return 0.95 * 9.8;
        }
    } // old
    else {
        throw txError("Emission limit is unknown!");
    }

    return 0;
}

double COLimit(int standard) {

    if ( standard == STD_EU6 ) {
        return 1.5;
    }
    else if ( standard == STD_EU5 ) {
        return 1.5;
    }
    else if ( standard == STD_EU4 ) {
        return 1.5;
    }
    else if ( standard == STD_EU3 ) {
        return 2.1;
    }
    else if ( standard == STD_EU2 ) {
        return 4;
    }
    else if ( standard == STD_EU1 ) {
        return 4.9;
    }
    else if ( standard == STD_EU0 ) {
        return 12.3;
    }
    else if ( standard == STD_OST3700123481 ) {
        return 9.5;
    }
    else if ( standard == STD_GOST17220597+10 ) {
        return 10;  // nu
    }
    else if ( standard == STD_GOST17220597+20 ) {
        return 4;   // nl
    }
    else if ( standard == STD_GOST17220597+30 ) {
        return 14;  // ou
    }
    else if ( standard == STD_GOST17220597+40 ) {
        return 5.6; // ol
    }
    else if ( (standard == STD_R96E8) || (standard == STD_R96E5) ) {
        return 3.5;
    }
    else if ( (standard == STD_R96F8) || (standard == STD_R96F5) ) {
        return 5;
    }
    else if ( (standard == STD_R96G8) || (standard == STD_R96G5) ) {
        return 5;
    }
    else if ( (standard == STD_R96D8) || (standard == STD_R96D5) ) {
        return 5.5;
    }
    else if ( (standard == STD_R96H8) || (standard == STD_R96H5) ) {
        return 3.5;
    }
    else if ( (standard == STD_R96I8) || (standard == STD_R96I5) ) {
        return 5;
    }
    else if ( (standard == STD_R96J8) || (standard == STD_R96J5) ) {
        return 5;
    }
    else if ( (standard == STD_R96K8) || (standard == STD_R96K5) ) {
        return 5.5;
    }
    else if ( (standard == STD_F +10) || (standard == STD_C1+10) ||
              (standard == STD_D1+10) || (standard == STD_D2+10) ||
              (standard == STD_G1+10) || (standard == STD_G2+10) ||
              (standard == STD_E1+10) || (standard == STD_E2+10) ||
              (standard == STD_E3+10) ||
              (standard == STD_E5+10) ) {
        return 3;         // new
    }
    else if ( (standard == STD_F +20) || (standard == STD_C1+20) ||
              (standard == STD_D1+20) || (standard == STD_D2+20) ||
              (standard == STD_G1+20) || (standard == STD_G2+20) ||
              (standard == STD_E1+20) || (standard == STD_E2+20) ||
              (standard == STD_E3+20) ||
              (standard == STD_E5+20) ) {
        return 1.2 * 3.0; // old
    }
    else {
        throw txError("Emission limit is unknown!");
    }
}

double CHLimit(int standard) {

    if ( standard == STD_EU6 ) {
        return 0.25;
    }
    else if ( standard == STD_EU5 ) {
        return 0.46;
    }
    else if ( standard == STD_EU4 ) {
        return 0.46;
    }
    else if ( standard == STD_EU3 ) {
        return 0.66;
    }
    else if ( standard == STD_EU2 ) {
        return 1.1;
    }
    else if ( standard == STD_EU1 ) {
        return 1.23;
    }
    else if ( standard == STD_EU0 ) {
        return 2.6;
    }
    else if ( standard == STD_OST3700123481 ) {
        return 3.4;
    }
    else if ( standard == STD_GOST17220597+10 ) {
        return 3;   // nu
    }
    else if ( standard == STD_GOST17220597+20 ) {
        return 1.5; // nl
    }
    else if ( standard == STD_GOST17220597+30 ) {
        return 4.5; // ou
    }
    else if ( standard == STD_GOST17220597+40 ) {
        return 2.2; // ol
    }
    else if ( (standard == STD_R96E8) || (standard == STD_R96E5) ) {
        return 1;
    }
    else if ( (standard == STD_R96F8) || (standard == STD_R96F5) ) {
        return 1;
    }
    else if ( (standard == STD_R96G8) || (standard == STD_R96G5) ) {
        return 1.3;
    }
    else if ( (standard == STD_R96D8) || (standard == STD_R96D5) ) {
        return 1.5;
    }
    else if ( (standard == STD_F +10) || (standard == STD_C1+10) ||
              (standard == STD_D1+10) || (standard == STD_D2+10) ||
              (standard == STD_G1+10) || (standard == STD_G2+10) ||
              (standard == STD_E1+10) || (standard == STD_E2+10) ||
              (standard == STD_E3+10) ||
              (standard == STD_E5+10) ) {
        return 1;          // new
    }
    else if ( (standard == STD_F +20) || (standard == STD_C1+20) ||
              (standard == STD_D1+20) || (standard == STD_D2+20) ||
              (standard == STD_G1+20) || (standard == STD_G2+20) ||
              (standard == STD_E1+20) || (standard == STD_E2+20) ||
              (standard == STD_E3+20) ||
              (standard == STD_E5+20) ) {
        return 1.25 * 1.0; // old
    }
    else {
        throw txError("Emission limit is unknown!");
    }
}

double PTLimit(int standard) {

    if ( standard == STD_EU6 ) {
        return 0.02;
    }
    else if ( standard == STD_EU5 ) {
        return 0.02;
    }
    else if ( standard == STD_EU4 ) {
        return 0.02;
    }
    else if ( standard == STD_EU3 ) {
        return 0.1;
    }
    else if ( standard == STD_EU2 ) {
        return 0.15;
    }
    else if ( standard == STD_EU1 ) {
        return 0.4;
    }
    else if ( (standard == STD_R96E8) || (standard == STD_R96E5) ) {
        return 0.2;
    }
    else if ( (standard == STD_R96F8) || (standard == STD_R96F5) ) {
        return 0.3;
    }
    else if ( (standard == STD_R96G8) || (standard == STD_R96G5) ) {
        return 0.4;
    }
    else if ( (standard == STD_R96D8) || (standard == STD_R96D5) ) {
        return 0.8;
    }
    else if ( (standard == STD_R96H8) || (standard == STD_R96H5) ) {
        return 0.2;
    }
    else if ( (standard == STD_R96I8) || (standard == STD_R96I5) ) {
        return 0.3;
    }
    else if ( (standard == STD_R96J8) || (standard == STD_R96J5) ) {
        return 0.4;
    }
    else if ( (standard == STD_R96K8) || (standard == STD_R96K5) ) {
        return 0.6;
    }
    else {
        throw txError("Emission limit is unknown!");
    }
}

double NOxCHLimit(int standard) {

    if ( (standard == STD_R96H8) || (standard == STD_R96H5) ) {
        return 4;
    }
    else if ( (standard == STD_R96I8) || (standard == STD_R96I5) ) {
        return 4;
    }
    else if ( (standard == STD_R96J8) || (standard == STD_R96J5) ) {
        return 4.7;
    }
    else if ( (standard == STD_R96K8) || (standard == STD_R96K5) ) {
        return 7.5;
    }
    else {
        throw txError("Emission limit is unknown!");
    }
}

} // namespace toxic
