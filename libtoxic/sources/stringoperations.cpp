/*
    stringoperations.cpp

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

#include <vector>
#include <sstream>
#include "stringoperations.h"

using std::string;
using std::vector;
using std::ostringstream;
using std::istringstream;

bool replaceInString(string* str, string* str1, string* str2) {

    size_t n;

    while ( (n = (* str).find( (* str1) )) != string::npos ) {

        (* str).replace( n, (* str1).length(), (* str2) );
    }

    return true;
}

bool splitString(string* fullstr, vector<string>& elements, string delimiter) {

    string::size_type lastpos = (* fullstr).find_first_not_of(delimiter, 0);
    string::size_type pos     = (* fullstr).find_first_of(delimiter, lastpos);

    while ( (string::npos != pos) || (string::npos != lastpos) ) {

        elements.push_back((* fullstr).substr(lastpos, pos-lastpos));

        lastpos = (* fullstr).find_first_not_of(delimiter, pos);
        pos = (* fullstr).find_first_of(delimiter, lastpos);
    }

    return true;
}

string numberToString(ptrdiff_t x) {

    ostringstream stm;
    stm << x;

    return stm.str();
}

string numberToString(double x) {

    ostringstream stm;
    stm << x;

    return stm.str();
}

double stringToDouble(string str) {

    istringstream stm;
    double val = 0;

    stm.str(str);
    stm >> val;

    return val;
}
