/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: newversions.cpp

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

#include "newversions.h"
#include "libtoxicconstants.h"
#include "qr49constants.h"
#include "r49.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QByteArray>
#include <QStringList>
#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QStringList>
#include <QDebug>

NewVersions::NewVersions() :
        htmlData("") {

    netmanager = new QNetworkAccessManager();
    connect(netmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

NewVersions::~NewVersions() {

    delete netmanager;
}

void NewVersions::checkAvailableVersions() {

    htmlData = "";
    urls.clear();
    files.clear();

    netmanager->get(QNetworkRequest(QUrl(pageUrl)));
}

void NewVersions::replyFinished(QNetworkReply* reply) {

    if (reply->error()) {

        QString msg = QString::fromAscii(Q_FUNC_INFO) + ":::" + reply->errorString();
        QMessageBox::critical(0, "Qr49", msg, 0, 0, 0);

        return;
    }

    QByteArray text = reply->readAll();
    htmlData = text.data();

    parseHtmlData();
}

void NewVersions::parseHtmlData() {

    QStringList strs = htmlData.split("\n", QString::SkipEmptyParts);

    QString regexpstr1 = "(<a href=\"" + filesUrl + "/r49-.+</a>)";
    QString regexpstr2 = "(\"" + filesUrl + "/r49-.+\")";

    QRegExp regExp1(regexpstr1);
    QRegExp regExp2(regexpstr2);
    QRegExp regExp3("(r49-[\\w]+-[\\w]+-[\\w.]+[\\w.]+)");

    for (ptrdiff_t i=0; i<strs.count(); i++) {

        if (regExp1.indexIn(strs.at(i)) != -1) {

            if (regExp2.indexIn(regExp1.cap(1)) != -1) {

                if (regExp3.indexIn(regExp2.cap(1)) != -1) {

                    files << regExp3.cap(1);
                }
            }
        }
    }

    for (ptrdiff_t i=0; i<files.count(); i++) {

        urls << pageUrl + files.at(i);
    }

    //

    QString allfiles;

    for (ptrdiff_t i=0; i<files.count(); i++) {

        allfiles += files.at(i) + " (" + "<a href= \"" + urls.at(i) + "\" >" + tr("Download") + "</a>)<br>";
    }

    QString msg = tr("You use ") + tr("r49 distribution version ") + r49version + "<br><br>" + tr("Available distributions:<br><br>") + allfiles;
    QMessageBox::information(0, "Qr49", msg, 0, 0, 0);
}
