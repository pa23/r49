/*
    libtoxic

    File: datetime.cpp

    Copyright (C) 2009, 2010, 2011 Artem Petrov <pa2311@gmail.com>

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

#include "datetime.h"
#include "stringoperations.h"
#include <ctime>

using std::string;

string addZero(string str) {

    if (str.length() == 1) { str = "0" + str; }

    return str;
}

string dateTimeNow() {

    time_t timenow = time(NULL);
    struct tm *timeinfo = localtime(&timenow);

    string mytime = "";

    mytime = addZero(numberToString((ptrdiff_t)timeinfo->tm_mday       )) + "-" +
             addZero(numberToString((ptrdiff_t)timeinfo->tm_mon  +    1)) + "-" +
                     numberToString((ptrdiff_t)timeinfo->tm_year + 1900)  + "_" +
             addZero(numberToString((ptrdiff_t)timeinfo->tm_hour       )) + "-" +
             addZero(numberToString((ptrdiff_t)timeinfo->tm_min        )) + "-" +
             addZero(numberToString((ptrdiff_t)timeinfo->tm_sec        ));

    return mytime;
}
