/*
    newversions.h

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

#ifndef NEWVERSIONS_H
#define NEWVERSIONS_H

#include <QObject>
#include <QHttp>
#include <QStringList>

class NewVersions : public QObject {

    Q_OBJECT

public:
    NewVersions();
    virtual ~NewVersions();
    void CheckAvailableVersions();

private:
    void ParseHtmlData();
    QHttp *http;
    QString HtmlData;
    QStringList urls;
    QStringList files;

private slots:
    void httpFinished(int, bool);

};

#endif // NEWVERSIONS_H
