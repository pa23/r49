/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txAuxiliaryFunctions.cpp

    Copyright (C) 2009-2014 Artem Petrov <pa2311@gmail.com>

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

#include "txAuxiliaryFunctions.h"
#include "txConstants.h"
#include "txError.h"

#include <cmath>

#include <QString>
#include <QStringList>
#include <QFile>
#include <QRegExp>

namespace toxic {

void ABC(double n_hi,
         double n_lo,
         double *A,
         double *B,
         double *C,
         double *a1,
         double *a2,
         double *a3,
         double *n_ref) {

    if (n_hi < n_lo) {
        throw txError("n_hi must be greater than n_lo!");
    }

    const double delta = (n_hi - n_lo) / 4.0;

    *A = n_lo + delta;
    *B = *A   + delta;
    *C = *B   + delta;

    *a1 = (*B + *C) / 2.0;
    *a2 = (*A + *B) / 2.0;
    *a3 = (*B + *C) / 2.0;

    *n_ref = n_lo + 0.95 * (n_hi - n_lo);
}

double Gair(double Dn, double B0, double t0, double dPn) {
    return 0.233897 * pow(Dn, 2.0) * sqrt((dPn * B0)/(t0 + 273.0));
}

double N_fan(double N_fan_rated, double n, double n_rated) {
    return N_fan_rated * pow(n / n_rated, 3.0);
}

double KaPerc(double Ka1m, double L) {
    return 100.0 * (1.0 - exp(- Ka1m * L));
}

double Ka1m(double KaPerc, double L) {
    return (-1.0 / L) * log(1 - KaPerc / 100.0);
}

double FSN(double KaPerc) {

    return
            6.6527E-017           * pow(KaPerc,10.0) +
            -0.000000000000026602 * pow(KaPerc, 9.0) +
            0.0000000000040987    * pow(KaPerc, 8.0) +
            -0.00000000026927     * pow(KaPerc, 7.0) +
            0.00000000040933      * pow(KaPerc, 6.0) +
            0.0000010658          * pow(KaPerc, 5.0) +
            -0.000069165          * pow(KaPerc, 4.0) +
            0.0020088             * pow(KaPerc, 3.0) +
            -0.028758             * pow(KaPerc, 2.0) +
            0.26502               * pow(KaPerc, 1.0) +
            0.0087517             * pow(KaPerc, 0.0);
}

double pa(double t0) {

    const QVector< QVector<double> > arr =
    {
        // t0
        {-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100},
        // pa
        {0.1029076,0.113305,0.1246355,0.1368991,0.1503624,0.1650254,0.1808881,0.1980838,0.2168791,0.237274,0.2594018,0.2832625,0.3093893,0.3375156,0.3680413,0.4009664,0.4366908,0.4753478,0.5170707,0.5619928,0.6103807,0.657169,0.705157,0.758477,0.81313,0.871782,0.934433,1.001083,1.073065,1.147713,1.227693,1.311672,1.402316,1.496959,1.598267,1.704907,1.816879,1.936849,2.063484,2.196784,2.338082,2.486045,2.643339,2.808631,2.983254,3.167208,3.360493,3.564442,3.779055,4.004332,4.241606,4.49221,4.753478,5.029409,5.31867,5.622594,5.939848,6.274431,6.618345,6.990252,7.374156,7.776722,8.19795,8.63784,9.099058,9.581604,10.084145,10.61068,11.158543,11.733066,12.331583,12.95676,13.60993,14.28976,14.99625,15.7294,16.50254,17.30234,18.14213,19.00858,19.91502,20.84812,21.83454,22.84762,23.90069,24.99375,26.14013,27.3265,28.55286,29.81921,31.15221,32.51187,33.93818,35.41781,36.95076,38.53703,40.17662,41.86953,43.62909,45.4553,47.33483,49.28101,51.30717,53.39998,55.55944,57.79888,60.10497,62.47771,64.93043,67.46313,70.08914,72.79513,75.5811,78.46038,81.43297,84.49887,87.65808,90.92393,94.28309,97.73556,101.308}
    };

    if ( arr[0].size() != arr[1].size() ) {
        throw txError("Wrong dependence of \"pa\" to \"t0\"!");
    }

    for ( ptrdiff_t i=0; i<arr[0].size(); i++ ) {

        if ( t0 > arr[0][i] && t0 < arr[0][i+1] ) {
            return arr[1][i] +
                    (arr[1][i+1] - arr[1][i]) * (t0 - arr[0][i]) /
                    (arr[0][i+1] - arr[0][i]);
        }
        else if (t0 == arr[0][i]) {
            return arr[1][i];
        }
        else if (t0 == arr[0][i+1]) {
            return arr[1][i+1];
        }
    }

    return 0;
}

double rhoAir(double B0, double t0) {
    return (B0 * 28.98) / (8.314472 * (t0 + 273));
}

bool zeroArray(const QVector<double> &data) {

    double sum = 0;

    for ( double elem : data ) {
        sum += elem;
    }

    if ( sum < 0.0000001 ) {
        return true;
    }
    else {
        return false;
    }
}

double sum(const QVector<double> &v) {

    double ret = 0;

    for ( double elem : v ) {
        ret += elem;
    }

    return ret;
}

double minVal(const QVector<double> &v) {

    double ret = v[0];

    for ( double elem : v ) {

        if ( elem < ret ) {
            ret = elem;
        }
    }

    return ret;
}

double maxVal(const QVector<double> &v) {

    double ret = v[0];

    for ( double elem : v ) {

        if ( elem > ret ) {
            ret = elem;
        }
    }

    return ret;
}

double mean(const QVector<double> &v) {

    return sum(v) / v.size();
}

QString objDescr() {

    QFile engDescrFile("r49data/engdescr.conf");

    if ( !engDescrFile.exists() ) {
        return "...";
    }

    if ( !engDescrFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {

        throw txError("Can not open engine description file "
                      + engDescrFile.fileName() + "!");
    }

    QString str;
    QStringList elements;
    QRegExp comment("^//");

    QStringList formattedBlocks;
    QString formattedDescr;

    while ( !engDescrFile.atEnd() ) {

        str = engDescrFile.readLine().trimmed();

        if ( str.isEmpty() || str.contains(comment) ) {
            continue;
        }

        elements = str.split("=");

        if ( elements.size() != 2 ) {
            continue;
        }

        formattedBlocks.push_back(elements[0] + " " + elements[1] + "; ");

        elements.clear();
    }

    engDescrFile.close();

    if ( formattedBlocks.isEmpty() ) {
        return "...";
    }

    if ( formattedBlocks.size() == 1 ) {

        QString retstr = formattedBlocks[0];
        retstr.truncate(retstr.size()-2);

        return retstr;
    }
    else {

        QString currStr = formattedBlocks[0];
        ptrdiff_t currStrNum = 0;
        ptrdiff_t maxStrWidth = 110;

        for ( ptrdiff_t i=1; i<formattedBlocks.size(); i++ ) {

            if ( currStrNum > 0 ) {
                maxStrWidth = 135;
            }

            if ( (currStr.size() + formattedBlocks[i].size()) <= maxStrWidth ) {

                currStr.append(formattedBlocks[i]);
            }
            else {

                formattedDescr.append(currStr);
                currStr = "\n                         ";
                currStr.append(formattedBlocks[i]);

                currStrNum++;
            }
        }

        formattedDescr.append(currStr);
    }

    formattedDescr.truncate(formattedDescr.size()-2);

    return formattedDescr;
}

} // namespace toxic
