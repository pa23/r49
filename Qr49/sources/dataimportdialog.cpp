/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: dataimportdialog.cpp

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

#include "dataimportdialog.h"
#include "ui_dataimportdialog.h"
#include "tablewidgetfunctions.h"
#include "constants.h"

#include <QSharedPointer>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>
#include <QTableWidget>
#include <QVector>
#include <QDateTime>

#include <QDebug>

#include "txDataReader.h"
#include "txError.h"
#include "txConstants.h"

DataImportDialog::DataImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataImportDialog),
    m_delimiter("\t"),
    m_dataDirName(QDir::currentPath()),
    m_table_lid(0),
    m_dtable(0),
    m_destTableDataChanged(false),
    m_templdir(),
    m_sj(0),
    m_dj(0),
    m_manual(true) {

    ui->setupUi(this);

    connect(ui->comboBox_delimiter,
            SIGNAL(activated(const QString &)),
            this,
            SLOT(combosUpdate(const QString &)));

    connect(ui->lineEdit_DataFile,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(combosUpdate(const QString &)));
}

DataImportDialog::~DataImportDialog() {

    delete ui;
}

void DataImportDialog::init(const ptrdiff_t tlid,
                            QTableWidget *dt) {

    m_table_lid = tlid;
    m_dtable = dt;

    combosUpdate(0);

    //

    QStringList dirnames = TEMPLATEDIRNAME.split(QDir::separator(),
                                                 QString::SkipEmptyParts);

    const QDir commondir(dirnames[0]);
    const QDir templdir1(TEMPLATEDIRNAME);
    const QDir templdir2(QDir::homePath() + QDir::separator() + TEMPLATEDIRNAME);

    if ( templdir1.exists() ) {
        m_templdir = templdir1;
    }
    else if ( templdir2.exists() ) {
        m_templdir = templdir2;
    }
    else {

        if ( !commondir.exists() ) {

            if ( !commondir.mkdir(commondir.absolutePath()) ) {
                QMessageBox::critical(
                            this,
                            "Qr49",
                            tr("Can not create common directory!")
                            );
                return;
            }
        }

        if ( templdir1.mkdir(templdir1.absolutePath()) ) {
            m_templdir = templdir1;
        }
        else if ( templdir2.mkdir(templdir2.absolutePath()) ) {
            m_templdir = templdir2;
        }
        else {
            QMessageBox::critical(
                        this,
                        "Qr49",
                        tr("Can not create directory for templates!")
                        );
            return;
        }
    }

    updateTemplList();
}

void DataImportDialog::updateTemplList() {

    const QStringList templfiles(m_templdir.entryList(QDir::Files, QDir::Time));

    ui->comboBox_Templates->clear();

    for ( ptrdiff_t i=0; i<templfiles.size(); i++ ) {
        ui->comboBox_Templates->addItem(templfiles[i]);
    }

    if ( ui->comboBox_Templates->count() != 0 ) {
        ui->comboBox_Templates->setCurrentIndex(0);
    }
}

void DataImportDialog::on_pushButton_SelectDataFile_clicked() {

    if ( !m_dtable ) {
        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Invalid pointer to the destination table!")
                    );
        return;
    }

    m_dataFileName = QFileDialog::getOpenFileName(
                this,
                tr("Open data file..."),
                m_dataDirName,
                QString::fromLatin1("Text files (*.txt);;"
                                    "Data files (*.dat);;"
                                    "All files (*)"));

    QFileInfo fileInfo(m_dataFileName);
    m_dataDirName = fileInfo.absolutePath();

    if ( !m_dataFileName.isEmpty() ) {
        ui->lineEdit_DataFile->setText(m_dataFileName);
    }
    else {
        QMessageBox::critical(this,
                              "Qr49",
                              tr("You have to select data file!")
                              );
    }
}

void DataImportDialog::on_pushButton_NextManual_clicked() {

    const ptrdiff_t scount = m_arrayImportedData.count();
    const ptrdiff_t dcount = m_dtable->rowCount();

    if ( scount > dcount ) {
        addRows(m_dtable, scount-dcount, ADDROWS_BOTTOM);
    }

    ptrdiff_t tmp_sj = 0;
    ptrdiff_t tmp_dj = 0;

    if ( m_manual ) {
        tmp_sj = ui->comboBox_AnotherParameter->currentIndex();
        tmp_dj = ui->comboBox_r49parameter->currentIndex();
    }
    else {

        tmp_sj = m_sj;
        tmp_dj = m_dj;

        if ( tmp_sj > (ui->comboBox_AnotherParameter->count()-1) ||
             tmp_dj > (ui->comboBox_r49parameter->count()-1) ) {

            QMessageBox::warning(
                        this,
                        "Qr49",
                        tr("Wrong index! Copying skipped!")
                        );
            return;
        }
    }

    //

    for (ptrdiff_t i=0; i<scount; i++) {

        m_dtable->setCurrentCell(0, tmp_dj);
        m_dtable->item(i, tmp_dj)->
                setText(QString::number(m_arrayImportedData.at(i).at(tmp_sj)));
    }

    //

    ui->plainTextEdit_importLog->
            insertPlainText(ui->comboBox_r49parameter->currentText()
                            + ";"
                            + ui->comboBox_AnotherParameter->currentText()
                            + "\n");

    //

    if ( ui->comboBox_r49parameter->currentIndex() ==
         ui->comboBox_r49parameter->count()-1 ) {

        ui->comboBox_r49parameter->setCurrentIndex(0);
    }
    else {

        ui->comboBox_r49parameter->setCurrentIndex(
                    ui->comboBox_r49parameter->currentIndex() + 1
                    );
    }

    if ( ui->comboBox_AnotherParameter->currentIndex() ==
         ui->comboBox_AnotherParameter->count()-1 ) {

        ui->comboBox_AnotherParameter->setCurrentIndex(0);
    }
    else {

        ui->comboBox_AnotherParameter->setCurrentIndex(
                    ui->comboBox_AnotherParameter->currentIndex() + 1
                    );
    }

    //

    m_destTableDataChanged = true;
}

void DataImportDialog::on_pushButton_NextAuto_clicked() {

    m_manual = false;

    QFile templFile(m_templdir.absolutePath()
                    + QDir::separator()
                    + ui->comboBox_Templates->currentText());

    if ( !templFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {

        QMessageBox::critical(this, "Qr49", "Can not open selected template file!");
        m_manual = true;
        return;
    }

    QString str;
    QStringList templstrlst;
    QStringList capts_49;
    QStringList capts_a;

    while ( !templFile.atEnd() ) {

        str = templFile.readLine().trimmed();

        if ( str.isEmpty() ) {
            continue;
        }

        templstrlst = str.split(";", QString::SkipEmptyParts);

        if ( templstrlst.size() != 2 ) {

            QMessageBox::critical(this, "Qr49", "Incorrect template file!");
            templFile.close();
            m_manual = true;
            return;
        }

        capts_49.push_back(templstrlst[0]);
        capts_a.push_back(templstrlst[1]);
    }

    templFile.close();

    if ( capts_49.isEmpty() || capts_a.isEmpty() ) {

        QMessageBox::critical(this, "Qr49", "Incorrect or empty template file!");
        m_manual = true;
        return;
    }

    //

    for ( ptrdiff_t n=0; n<ui->comboBox_r49parameter->count(); n++ ) {

        for ( ptrdiff_t m=0; m<capts_49.size(); m++ ) {

            if ( QString::compare(ui->comboBox_r49parameter->itemText(n), capts_49[m]) == 0 ) {

                for ( ptrdiff_t k=0; k<ui->comboBox_AnotherParameter->count(); k++ ) {

                    if ( QString::compare(capts_a[m], ui->comboBox_AnotherParameter->itemText(k)) == 0 ) {

                        m_dj = n;
                        m_sj = k;

                        ui->comboBox_r49parameter->setCurrentIndex(n);
                        ui->comboBox_AnotherParameter->setCurrentIndex(k);

                        on_pushButton_NextManual_clicked();
                    }
                }
            }
        }
    }

    //

    m_manual = true;
}

void DataImportDialog::on_pushButton_OpenTemplate_clicked() {

    QString templFileName =
            QFileDialog::getOpenFileName(
                this,
                tr("Open template..."),
                m_templdir.absolutePath(),
                QString::fromLatin1("All files (*)")
                );

    if ( templFileName.isEmpty() ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("File not selected!")
                    );
        return;
    }

    QFile templFile(templFileName);

    if ( !templFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    templFile.fileName()
                    + tr(" could not be opened!")
                    );
        return;
    }

    ui->plainTextEdit_importLog->clear();
    ui->plainTextEdit_importLog->insertPlainText(QString(templFile.readAll()));

    templFile.close();
}

void DataImportDialog::on_pushButton_SaveTemplate_clicked() {

    if ( ui->plainTextEdit_importLog->document()->isEmpty() ) {

        QMessageBox::information(
                    this,
                    "Qr49",
                    tr("Template is empty!")
                    );
        return;
    }

    const QString filename(
                QFileDialog::getSaveFileName(
                    this,
                    tr("Save template..."),
                    m_templdir.absolutePath()
                    + QDir::separator()
                    + QDateTime::currentDateTime().
                    toString("dd-MM-yyyy_hh-mm-ss.txt"))
                );

    if ( filename.isEmpty() ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("File not selected!")
                    );
        return;
    }

    QFile savedTemplate(filename);

    if ( !savedTemplate.open(QIODevice::WriteOnly | QIODevice::Text) ) {

        QMessageBox::critical(
                    this,
                    "Qr49",
                    savedTemplate.fileName()
                    + tr(" could not be opened!")
                    );
        return;
    }

    savedTemplate.write(ui->plainTextEdit_importLog->toPlainText().toUtf8());
    savedTemplate.close();

    ui->plainTextEdit_importLog->clear();

    updateTemplList();
}

void DataImportDialog::combosUpdate(const QString &str) {

    (void)str;

    //

    ui->plainTextEdit_importLog->clear();

    //

    if (ui->comboBox_delimiter->currentText() == "tab") {
        m_delimiter = "\t";
    }
    else if (ui->comboBox_delimiter->currentText() == ";") {
        m_delimiter = ";";
    }
    else if (ui->comboBox_delimiter->currentText() == ",") {
        m_delimiter = ",";
    }
    else if (ui->comboBox_delimiter->currentText() == "space") {
        m_delimiter = " ";
    }
    else {
        QMessageBox::critical(
                    this,
                    "Qr49",
                    tr("Unknown delimiter for selected file parsing!")
                    );
        return;
    }

    //

    ui->comboBox_r49parameter->clear();
    ui->comboBox_AnotherParameter->clear();

    //

    if ( !ui->lineEdit_DataFile->text().isEmpty() ) {

        QSharedPointer<toxic::txDataReader>
                importedDataReader(new toxic::txDataReader());

        try{
            importedDataReader->readFile(m_dataFileName, m_delimiter);
        }
        catch(const toxic::txError &toxerr) {
            QMessageBox::critical(this, "Qr49", toxerr.val_toxicErrMsg());
            return;
        }

        m_arrayImportedData = importedDataReader->val_data();
        m_headersImportedData = importedDataReader->val_headers();

        if (m_table_lid == 2) {
            ui->comboBox_r49parameter->
                    addItems(toxic::SRCDATACAPTIONS_EMISSIONS);
        }
        else if (m_table_lid == 3) {
            ui->comboBox_r49parameter->
                    addItems(toxic::SRCDATACAPTIONS_REDPOWER);
        }
        else {
            QMessageBox::critical(this,
                                  "Qr49",
                                  tr("Invalid destination table!")
                                  );
        }

        if ( !m_headersImportedData.isEmpty() ) {
            ui->comboBox_AnotherParameter->
                    addItems(m_headersImportedData[0].split(m_delimiter));
        }
    }
}

void DataImportDialog::on_pushButton_Close_clicked() {

    ui->plainTextEdit_importLog->clear();

    if ( m_destTableDataChanged ) {
        m_destTableDataChanged = false;
        accept();
    }
    else {
        reject();
    }
}
