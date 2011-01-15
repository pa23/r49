/*
    emissionlimits.h

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

#ifndef EMISSIONLIMITS_H
#define EMISSIONLIMITS_H

#include <QString>

double val_NOxLimit(QString, double n_rated=0);
double val_COLimit(QString);
double val_CHLimit(QString);
double val_PTLimit(QString);
double val_NOxCHLimit(QString);

#endif /* EMISSIONLIMITS_H */
