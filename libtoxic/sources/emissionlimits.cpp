/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: emissionlimits.cpp

    Copyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>

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
#include "libtoxicconstants.h"
#include "toxicerror.h"

#include <cmath>

double valNOxLimit(const ptrdiff_t stnd, const double n_rated) {

    if      (stnd == STD_EU6     ) { return  2.00; }
    else if (stnd == STD_EU5     ) { return  2.00; }
    else if (stnd == STD_EU4     ) { return  3.50; }
    else if (stnd == STD_EU3     ) { return  5.00; }
    else if (stnd == STD_EU2     ) { return  7.00; }
    else if (stnd == STD_EU1     ) { return  9.00; }
    else if (stnd == STD_EU0     ) { return 15.80; }
    else if (stnd == STD_OST     ) { return 18.35; }
    else if (stnd == STD_GOST+10 ) { return 18.0;  } // nu
    else if (stnd == STD_GOST+20 ) { return  9.0;  } // nl
    else if (stnd == STD_GOST+30 ) { return 18.0;  } // ou
    else if (stnd == STD_GOST+40 ) { return  9.0;  } // ol
    else if ( (stnd == STD_R96E8) || (stnd == STD_R96E5) ) { return  6.00; }
    else if ( (stnd == STD_R96F8) || (stnd == STD_R96F5) ) { return  6.00; }
    else if ( (stnd == STD_R96G8) || (stnd == STD_R96G5) ) { return  7.00; }
    else if ( (stnd == STD_R96D8) || (stnd == STD_R96D5) ) { return  8.00; }
    else if ( (stnd == STD_F+10) ) { return        12.0; } // new
    else if ( (stnd == STD_F+20) ) { return 0.95 * 12.0; } // old
    else if ( (stnd == STD_C1+10) || (stnd == STD_D1+10) ||
              (stnd == STD_D2+10) || (stnd == STD_G1+10) ||
              (stnd == STD_G2+10) ) { return        10.0; } // new
    else if ( (stnd == STD_C1+20) || (stnd == STD_D1+20) ||
              (stnd == STD_D2+20) || (stnd == STD_G1+20) ||
              (stnd == STD_G2+20) ) { return 0.95 * 10.0; } // old
    else if ( (stnd == STD_E1+10) || (stnd == STD_E2+10) ||
              (stnd == STD_E3+10) || (stnd == STD_E5+10) ) {

        if ( n_rated <= 130 ) {

            return 17.0;
        }
        else if ( (n_rated > 130) && (n_rated <= 2000) ) {

            return 45.0 * pow(n_rated, -0.2);
        }
        else if ( n_rated > 2000 ) {

            return 9.8;
        }
    } // new
    else if ( (stnd == STD_E1+20) || (stnd == STD_E2+20) ||
              (stnd == STD_E3+20) || (stnd == STD_E5+20) ) {

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

        throw ToxicError("Emission limit is unknown!");
    }

    return 0;
}

double valCOLimit(const ptrdiff_t stnd) {

    if      (stnd == STD_EU6     ) { return  1.50; }
    else if (stnd == STD_EU5     ) { return  1.50; }
    else if (stnd == STD_EU4     ) { return  1.50; }
    else if (stnd == STD_EU3     ) { return  2.10; }
    else if (stnd == STD_EU2     ) { return  4.00; }
    else if (stnd == STD_EU1     ) { return  4.90; }
    else if (stnd == STD_EU0     ) { return 12.30; }
    else if (stnd == STD_OST     ) { return  9.50; }
    else if (stnd == STD_GOST+10 ) { return 10.0;  } // nu
    else if (stnd == STD_GOST+20 ) { return  4.0;  } // nl
    else if (stnd == STD_GOST+30 ) { return 14.0;  } // ou
    else if (stnd == STD_GOST+40 ) { return  5.6;  } // ol
    else if ( (stnd == STD_R96E8) || (stnd == STD_R96E5) ) { return  3.50; }
    else if ( (stnd == STD_R96F8) || (stnd == STD_R96F5) ) { return  5.00; }
    else if ( (stnd == STD_R96G8) || (stnd == STD_R96G5) ) { return  5.00; }
    else if ( (stnd == STD_R96D8) || (stnd == STD_R96D5) ) { return  5.50; }
    else if ( (stnd == STD_R96H8) || (stnd == STD_R96H5) ) { return  3.50; }
    else if ( (stnd == STD_R96I8) || (stnd == STD_R96I5) ) { return  5.00; }
    else if ( (stnd == STD_R96J8) || (stnd == STD_R96J5) ) { return  5.00; }
    else if ( (stnd == STD_R96K8) || (stnd == STD_R96K5) ) { return  5.50; }
    else if ( (stnd == STD_F+10 ) || (stnd == STD_C1+10) ||
              (stnd == STD_D1+10) || (stnd == STD_D2+10) ||
              (stnd == STD_G1+10) || (stnd == STD_G2+10) ||
              (stnd == STD_E1+10) || (stnd == STD_E2+10) ||
              (stnd == STD_E3+10) ||
              (stnd == STD_E5+10) ) { return 3.0;       } // new
    else if ( (stnd == STD_F+20 ) || (stnd == STD_C1+20) ||
              (stnd == STD_D1+20) || (stnd == STD_D2+20) ||
              (stnd == STD_G1+20) || (stnd == STD_G2+20) ||
              (stnd == STD_E1+20) || (stnd == STD_E2+20) ||
              (stnd == STD_E3+20) ||
              (stnd == STD_E5+20) ) { return 1.2 * 3.0; } // old
    else {

        throw ToxicError("Emission limit is unknown!");
    }
}

double valCHLimit(const ptrdiff_t stnd) {

    if      (stnd == STD_EU6     ) { return  0.25; }
    else if (stnd == STD_EU5     ) { return  0.46; }
    else if (stnd == STD_EU4     ) { return  0.46; }
    else if (stnd == STD_EU3     ) { return  0.66; }
    else if (stnd == STD_EU2     ) { return  1.10; }
    else if (stnd == STD_EU1     ) { return  1.23; }
    else if (stnd == STD_EU0     ) { return  2.60; }
    else if (stnd == STD_OST     ) { return  3.40; }
    else if (stnd == STD_GOST+10 ) { return 3.0;   } // nu
    else if (stnd == STD_GOST+20 ) { return 1.5;   } // nl
    else if (stnd == STD_GOST+30 ) { return 4.5;   } // ou
    else if (stnd == STD_GOST+40 ) { return 2.2;   } // ol
    else if ( (stnd == STD_R96E8) || (stnd == STD_R96E5) ) { return  1.00; }
    else if ( (stnd == STD_R96F8) || (stnd == STD_R96F5) ) { return  1.00; }
    else if ( (stnd == STD_R96G8) || (stnd == STD_R96G5) ) { return  1.30; }
    else if ( (stnd == STD_R96D8) || (stnd == STD_R96D5) ) { return  1.50; }
    else if ( (stnd == STD_F+10 ) || (stnd == STD_C1+10) ||
              (stnd == STD_D1+10) || (stnd == STD_D2+10) ||
              (stnd == STD_G1+10) || (stnd == STD_G2+10) ||
              (stnd == STD_E1+10) || (stnd == STD_E2+10) ||
              (stnd == STD_E3+10) ||
              (stnd == STD_E5+10) ) { return        1.0; } // new
    else if ( (stnd == STD_F+20 ) || (stnd == STD_C1+20) ||
              (stnd == STD_D1+20) || (stnd == STD_D2+20) ||
              (stnd == STD_G1+20) || (stnd == STD_G2+20) ||
              (stnd == STD_E1+20) || (stnd == STD_E2+20) ||
              (stnd == STD_E3+20) ||
              (stnd == STD_E5+20) ) { return 1.25 * 1.0; } // old
    else {

        throw ToxicError("Emission limit is unknown!");
    }
}

double valPTLimit(const ptrdiff_t stnd) {

    if      (stnd == STD_EU6 ) { return  0.02; }
    else if (stnd == STD_EU5 ) { return  0.02; }
    else if (stnd == STD_EU4 ) { return  0.02; }
    else if (stnd == STD_EU3 ) { return  0.10; }
    else if (stnd == STD_EU2 ) { return  0.15; }
    else if (stnd == STD_EU1 ) { return  0.40; }
    else if ( (stnd == STD_R96E8) || (stnd == STD_R96E5) ) { return  0.20; }
    else if ( (stnd == STD_R96F8) || (stnd == STD_R96F5) ) { return  0.30; }
    else if ( (stnd == STD_R96G8) || (stnd == STD_R96G5) ) { return  0.40; }
    else if ( (stnd == STD_R96D8) || (stnd == STD_R96D5) ) { return  0.80; }
    else if ( (stnd == STD_R96H8) || (stnd == STD_R96H5) ) { return  0.20; }
    else if ( (stnd == STD_R96I8) || (stnd == STD_R96I5) ) { return  0.30; }
    else if ( (stnd == STD_R96J8) || (stnd == STD_R96J5) ) { return  0.40; }
    else if ( (stnd == STD_R96K8) || (stnd == STD_R96K5) ) { return  0.60; }
    else {

        throw ToxicError("Emission limit is unknown!");
    }
}

double valNOxCHLimit(const ptrdiff_t stnd) {

    if      ( (stnd == STD_R96H8) || (stnd == STD_R96H5) ) { return  4.00; }
    else if ( (stnd == STD_R96I8) || (stnd == STD_R96I5) ) { return  4.00; }
    else if ( (stnd == STD_R96J8) || (stnd == STD_R96J5) ) { return  4.70; }
    else if ( (stnd == STD_R96K8) || (stnd == STD_R96K5) ) { return  7.50; }
    else {

        throw ToxicError("Emission limit is unknown!");
    }
}
