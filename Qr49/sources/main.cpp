/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: main.cpp

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

#include "qr49.h"

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>

int main(int argc, char **argv) {

    QApplication a(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QTranslator translator;

    if ( !translator.load("qr49_" + QLocale::system().name()) ) {

        translator.load("/usr/share/r49/translations/qr49_" + QLocale::system().name());
    }

    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
