/*
    newversions.cpp

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

#include "newversions.h"
#include "libtoxicconstants.h"
#include "qr49constants.h"
#include "../r49.h"

#include <QHttp>
#include <QByteArray>
#include <QStringList>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QStringList>
#include <QDebug>

NewVersions::NewVersions() : HtmlData("") {

    http = new QHttp(ServerForUpdates, QHttp::ConnectionModeHttps);
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpFinished(int, bool)));
}

NewVersions::~NewVersions() {

    delete http;
}

void NewVersions::CheckAvailableVersions() {

    HtmlData = "";
    urls.clear();
    files.clear();

    http->get(PageUrl);
}

void NewVersions::httpFinished(int id, bool error) {

    if (error) {

        QString msg = "http Request ID: " + QString::number(id) + ". " + http->errorString();
        QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

        return;
    }

    QByteArray text = http->readAll();
    HtmlData = text.data();

    ParseHtmlData();
}

void NewVersions::ParseHtmlData() {

    QStringList strs = HtmlData.split("\n", QString::SkipEmptyParts);

    QString regexpstr1 = "(<a href=\"" + FilesUrl + "/r49-.+</a>)";
    QString regexpstr2 = "(\"" + FilesUrl + "/r49-.+\")";

    QRegExp regExp1(regexpstr1);
    QRegExp regExp2(regexpstr2);
    QRegExp regExp3("(r49-[\\w]+-[\\w]+-[\\w.]+[\\w.]+)");

    for (ptrdiff_t i=0; i<strs.count(); i++) {

        if (regExp1.indexIn(strs.at(i)) != -1) {

            if (regExp2.indexIn(regExp1.cap(1)) != -1) {

                urls << regExp2.cap(1);
            }
        }
    }

    QStringList templst;
    QString tempstr;

    for (ptrdiff_t i=0; i<urls.count(); i++) {

        templst = urls.at(i).split("\"");
        tempstr = "https://" + ServerForUpdates + templst.at(1);
        urls[i] = tempstr;
    }

    for (ptrdiff_t i=0; i<urls.count(); i++) {

        if (regExp3.indexIn(urls.at(i)) != -1) {

            files << regExp3.cap(1);
        }
    }

    //

    QString allfiles;

    for (ptrdiff_t i=0; i<files.count(); i++) {

        allfiles += files.at(i) + " (" + "<a href= \"" + urls.at(i) + "\" >" + tr("Download") + "</a>)<br>";
    }

    QString msg = tr("You use ") + tr("r49 distribution version ") + r49version + "<br><br>" + tr("Available distributions:<br><br>") + allfiles;
    QMessageBox::information(0, "Qr49", msg, 0, 0, 0);
}
