/*
    stringoperations.h

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

#ifndef STRINGOPERATIONS_H
#define	STRINGOPERATIONS_H

#include <string>
#include <vector>

bool replaceInString(std::string*, std::string*, std::string*);
bool splitString(std::string*, std::vector<std::string>&, std::string);
std::string numberToString(ptrdiff_t);
std::string numberToString(double);
double stringToDouble(std::string);

#endif	/* STRINGOPERATIONS_H */
