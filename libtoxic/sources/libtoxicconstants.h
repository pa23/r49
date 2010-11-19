/*
    libtoxicconstants.h

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

#ifndef _LIBTOXICCONSTANTS_H
#define	_LIBTOXICCONSTANTS_H

#include <QString>

// constants

const QString libtoxicVersion  = "4.18.1";

const QString parameterValueDelimiter = "=";
const QString commentPattern = "^//";

const ptrdiff_t TCyclePointsNumber            =  16; //
const ptrdiff_t TCycleAddPointsNumber         =   3;
const ptrdiff_t ECyclePointsNumber            =   8;
const ptrdiff_t FCyclePointsNumber            =   5;
const ptrdiff_t GC1CylcePointsNumber          =   8;
const ptrdiff_t GD1CylcePointsNumber          =   3;
const ptrdiff_t GD2CylcePointsNumber          =   5;
const ptrdiff_t GE1CylcePointsNumber          =   5;
const ptrdiff_t GE2CylcePointsNumber          =   4;
const ptrdiff_t GE3CylcePointsNumber          =   4;
const ptrdiff_t GE5CylcePointsNumber          =   5;
const ptrdiff_t GFCylcePointsNumber           =   3;
const ptrdiff_t GG1CylcePointsNumber          =   6;
const ptrdiff_t GG2CylcePointsNumber          =   6;
const ptrdiff_t SrcDataPointsNumber           =   1; //
const ptrdiff_t EU3SrcDataParamsNumber        =  11;
const ptrdiff_t EU0SrcDataParamsNumber        =   6;
const ptrdiff_t PointsFileColumnsNumber       =  28; //
const ptrdiff_t PowersFileColumnsNumber       =  11;
const ptrdiff_t StrsNumberForColumnCaption    =   1; //
const ptrdiff_t WidthOfColumn                 =  15; //
const ptrdiff_t Precision                     =   2;

const ptrdiff_t PaArraySize = 121;
const double arrayT0[] = {-20.0,-19.0,-18.0,-17.0,-16.0,-15.0,-14.0,-13.0,-12.0,-11.0,-10.0,-9.0,-8.0,-7.0,-6.0,-5.0,-4.0,-3.0,-2.0,-1.0,0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,23.0,24.0,25.0,26.0,27.0,28.0,29.0,30.0,31.0,32.0,33.0,34.0,35.0,36.0,37.0,38.0,39.0,40.0,41.0,42.0,43.0,44.0,45.0,46.0,47.0,48.0,49.0,50.0,51.0,52.0,53.0,54.0,55.0,56.0,57.0,58.0,59.0,60.0,61.0,62.0,63.0,64.0,65.0,66.0,67.0,68.0,69.0,70.0,71.0,72.0,73.0,74.0,75.0,76.0,77.0,78.0,79.0,80.0,81.0,82.0,83.0,84.0,85.0,86.0,87.0,88.0,89.0,90.0,91.0,92.0,93.0,94.0,95.0,96.0,97.0,98.0,99.0,100.0};
const double arrayPa[] = {0.102933333333333,0.113333333333333,0.124666666666667,0.136933333333333,0.1504,0.165066666666667,0.180933333333333,0.198133333333333,0.216933333333333,0.237333333333333,0.259466666666667,0.283333333333333,0.309466666666667,0.3376,0.368133333333333,0.401066666666667,0.4368,0.475466666666667,0.5172,0.562133333333333,0.610533333333333,0.657333333333333,0.705333333333333,0.758666666666667,0.813333333333333,0.872,0.934666666666667,1.00133333333333,1.07333333333333,1.148,1.228,1.312,1.40266666666667,1.49733333333333,1.59866666666667,1.70533333333333,1.81733333333333,1.93733333333333,2.064,2.19733333333333,2.33866666666667,2.48666666666667,2.644,2.80933333333333,2.984,3.168,3.36133333333333,3.56533333333333,3.78,4.00533333333333,4.24266666666667,4.49333333333333,4.75466666666667,5.03066666666667,5.32,5.624,5.94133333333333,6.276,6.62,6.992,7.376,7.77866666666667,8.2,8.64,9.10133333333333,9.584,10.0866666666667,10.6133333333333,11.1613333333333,11.736,12.3346666666667,12.96,13.6133333333333,14.2933333333333,15.0,15.7333333333333,16.5066666666667,17.3066666666667,18.1466666666667,19.0133333333333,19.92,20.8533333333333,21.84,22.8533333333333,23.9066666666667,25.0,26.1466666666667,27.3333333333333,28.56,29.8266666666667,31.16,32.52,33.9466666666667,35.4266666666667,36.96,38.5466666666667,40.1866666666667,41.88,43.64,45.4666666666667,47.3466666666667,49.2933333333333,51.32,53.4133333333333,55.5733333333333,57.8133333333333,60.12,62.4933333333333,64.9466666666667,67.48,70.1066666666667,72.8133333333333,75.6,78.48,81.4533333333333,84.52,87.68,90.9466666666667,94.3066666666667,97.76,101.333333333333};

#endif	/* _LIBTOXICCONSTANTS_H */
