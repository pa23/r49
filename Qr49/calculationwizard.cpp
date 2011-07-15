/*
    calculationwizard.cpp

    Copyright (C) 2011 Artem Petrov <pa2311@gmail.com>

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

#include <QtGui>
#include <QSharedPointer>

#include "calculationwizard.h"
#include "filtermassdialog.h"

#include "libtoxicparameters.h"

CalculationWizard::CalculationWizard(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizard(parent) {

    setPage(p_task, new Page_task(params));
    setPage(p_std, new Page_std(params));
    setPage(p_fuelType, new Page_fuelType(params));
    setPage(p_NOx, new Page_NOx(params));
    setPage(p_PT, new Page_PT(params));
    setPage(p_report, new Page_report(params));
    setPage(p_Vh, new Page_Vh(params));
    setPage(p_conclusion, new Page_conclusion());

    setStartId(p_task);

    setWindowTitle(tr("Calculation Wizard"));
}

Page_task::Page_task(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent) {

    calcParams = params;

    setTitle(tr("Calculation"));
    setSubTitle(tr("Please select what you want to calculate"));

    label_task = new QLabel("Calculation task");
    comboBox_task = new QComboBox();
    label_taskNote = new QLabel();

    comboBox_task->addItems(QStringList() << "points" << "emissions" << "ReducedPower");
    comboBox_task->setCurrentIndex(1);

    label_taskNote->setWordWrap(true);
    label_taskNote->setText(tr("Continue if a table filled with source data:\n"
                               "Table #1 for cycle points calculating (OST..EU2, r96**, GOST 30574);\n"
                               "Table #2 for cycle points calculating (EU3..EU6);\n"
                               "Table #3 for specific emission calculating;\n"
                               "Table #4 for reduced power calculating."));

    registerField("task", comboBox_task);

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout1 = new QHBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();

    QGroupBox *groupBox = new QGroupBox("Note");

    layout1->addWidget(label_task);
    layout1->addWidget(comboBox_task);

    layout2->addWidget(label_taskNote);

    groupBox->setLayout(layout2);

    layout->addLayout(layout1);
    layout->addWidget(groupBox);

    setLayout(layout);

    connect(comboBox_task, SIGNAL(activated(QString)), this, SLOT(taskChanged(QString)));
}

void Page_task::taskChanged(QString str) {

    calcParams.data()->setTask(str);
}

int Page_task::nextId() const {

    QString task = comboBox_task->currentText();

    if ( (task == "points") || (task == "emissions") ) {

        return CalculationWizard::p_std;
    }
    else if (task == "ReducedPower") {

        return CalculationWizard::p_Vh;
    }
    else {

        return CalculationWizard::p_task;
    }
}

Page_std::Page_std(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent) {

    calcParams = params;

    setTitle(tr("Standard and additional cycle points"));
    setSubTitle(tr("Please select a standard against which will be calculated and the need to calculate the additional points"));

    label_std = new QLabel("Standard");
    label_addPoints = new QLabel("Additional points");
    comboBox_standard = new QComboBox();
    comboBox_addPointsCalc = new QComboBox();

    comboBox_standard->addItems(QStringList()
                                << "EU6" << "EU5" << "EU4" << "EU3" << "EU2" << "EU1" << "EU0"
                                << "OST" << "GOST"
                                << "r96E8" << "r96F8" << "r96G8" << "r96D8"
                                << "r96E5" << "r96F5" << "r96G5" << "r96D5"
                                << "r96H8" << "r96I8" << "r96J8" << "r96K8"
                                << "r96H5" << "r96I5" << "r96J5" << "r96K5"
                                << "C1" << "D1" << "D2" << "E1" << "E2" << "E3" << "E5" << "F" << "G1" << "G2"
                                << "FreeCalc");
    comboBox_standard->setCurrentIndex(2);

    comboBox_addPointsCalc->addItems(QStringList() << "yes" << "no");
    comboBox_addPointsCalc->setCurrentIndex(1);

    registerField("standard", comboBox_standard);
    registerField("addPointsCacl", comboBox_addPointsCalc);

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout1 = new QHBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();

    layout1->addWidget(label_std);
    layout1->addWidget(comboBox_standard);

    layout2->addWidget(label_addPoints);
    layout2->addWidget(comboBox_addPointsCalc);

    layout->addLayout(layout1);
    layout->addLayout(layout2);

    setLayout(layout);

    connect(comboBox_standard, SIGNAL(activated(QString)), this, SLOT(standardChanged(QString)));
    connect(comboBox_addPointsCalc, SIGNAL(activated(QString)), this, SLOT(addPointsCalcChanged(QString)));
}

void Page_std::standardChanged(QString str) {

    if ( (str == "EU6") || (str == "EU5") || (str == "EU4") || (str == "EU3") ) {

        comboBox_addPointsCalc->setEnabled(true);
    }
    else {

        comboBox_addPointsCalc->setEnabled(false);
    }

    calcParams.data()->setStandard(str);
}

void Page_std::addPointsCalcChanged(QString str) {

    calcParams.data()->setAddPointsCalc(str);
}

int Page_std::nextId() const {

    int task = field("task").toInt(); // 0 - points, 1 - emissions, 2 - ReducedPower
    QString std = comboBox_standard->currentText();

    if ( task == 0 ) {

        return CalculationWizard::p_conclusion;
    }
    else if ( task == 1 ) {

        if ( (std == "C1") || (std == "D1") || (std == "D2") ||
             (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
             (std == "F") || (std == "G1") || (std == "G2") ) {

            return CalculationWizard::p_fuelType;
        }
        else {

            return CalculationWizard::p_NOx;
        }
    }
    else {

        return CalculationWizard::p_task;
    }
}

Page_fuelType::Page_fuelType(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent) {

    calcParams = params;

    setTitle(tr("Fuel type"));
    setSubTitle(tr("Please select the fuel type"));

    label_fuelType = new QLabel("Fuel type");
    comboBox_fuelType = new QComboBox();

    comboBox_fuelType->addItems(QStringList() << "diesel" << "motor" << "mazut");

    registerField("fuelType", comboBox_fuelType);

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout1 = new QHBoxLayout();

    layout1->addWidget(label_fuelType);
    layout1->addWidget(comboBox_fuelType);

    layout->addLayout(layout1);

    setLayout(layout);

    connect(comboBox_fuelType, SIGNAL(activated(QString)), this, SLOT(fuelTypeChanged(QString)));
}

void Page_fuelType::fuelTypeChanged(QString str) {

    calcParams.data()->setFuelType(str);
}

int Page_fuelType::nextId() const {

    return CalculationWizard::p_NOx;
}

Page_NOx::Page_NOx(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent) {

    calcParams = params;

    setTitle(tr("NOx sample type"));
    setSubTitle(tr("Please select the NOx sample type"));

    label_NOxSample = new QLabel("NOx sample");
    comboBox_NOxSample = new QComboBox();

    comboBox_NOxSample->addItems(QStringList() << "wet" << "dry");

    registerField("NOxSample", comboBox_NOxSample);

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout1 = new QHBoxLayout();

    layout1->addWidget(label_NOxSample);
    layout1->addWidget(comboBox_NOxSample);

    layout->addLayout(layout1);

    setLayout(layout);

    connect(comboBox_NOxSample, SIGNAL(activated(QString)), this, SLOT(NOxSampleChanged(QString)));
}

void Page_NOx::NOxSampleChanged(QString str) {

    calcParams.data()->setNOxSample(str);
}

int Page_NOx::nextId() const {

    return CalculationWizard::p_PT;
}

Page_PT::Page_PT(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent), filterMassDialog(new FilterMassDialog()) {

    calcParams = params;

    setTitle(tr("Particulate calculation"));
    setSubTitle(tr("Please select the particulate calculating method. If necessary, enter the mass of particles or filters"));

    label_PTcalc = new QLabel("PT calculating method");
    label_PTmass = new QLabel("PT mass, mg");
    comboBox_PTcalc = new QComboBox();
    lineEdit_PTmass = new QLineEdit();
    pushButton_enterPTmass = new QPushButton();

    comboBox_PTcalc->addItems(QStringList() << "ThroughSmoke" << "ThroughPTmass" << "no");
    comboBox_PTcalc->setCurrentIndex(2);

    lineEdit_PTmass->setText("0");

    pushButton_enterPTmass->setText("...");
    pushButton_enterPTmass->setMaximumWidth(30);

    registerField("PTcalc", comboBox_PTcalc);
    registerField("PTmass", lineEdit_PTmass);

    QVBoxLayout *layout = new QVBoxLayout();
    QGridLayout *layout1 = new QGridLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();

    layout2->addWidget(lineEdit_PTmass);
    layout2->addWidget(pushButton_enterPTmass);

    layout1->addWidget(label_PTcalc, 0, 0);
    layout1->addWidget(comboBox_PTcalc, 0, 1);
    layout1->addWidget(label_PTmass, 1, 0);
    layout1->addLayout(layout2, 1, 1);

    layout->addLayout(layout1);

    setLayout(layout);

    connect(comboBox_PTcalc, SIGNAL(activated(QString)), this, SLOT(PTcalcMethodChanged(QString)));
    connect(lineEdit_PTmass, SIGNAL(textChanged(QString)), this, SLOT(PTmassChanged(QString)));
    connect(pushButton_enterPTmass, SIGNAL(clicked()), this, SLOT(on_pushButton_EnterPTmass_clicked()));

    PTcalcMethodChanged(comboBox_PTcalc->currentText());
}

void Page_PT::PTcalcMethodChanged(QString str) {

    if ( str == "ThroughPTmass" ) {

        lineEdit_PTmass->setEnabled(true);
        pushButton_enterPTmass->setEnabled(true);
    }
    else {

        lineEdit_PTmass->setEnabled(false);
        pushButton_enterPTmass->setEnabled(false);
    }

    calcParams.data()->setPTcalc(str);
}

void Page_PT::PTmassChanged(QString str) {

    double PTmass = str.toDouble();

    calcParams.data()->setPTmass(&PTmass);
}

void Page_PT::on_pushButton_EnterPTmass_clicked() {

    if (filterMassDialog->exec() == QDialog::Accepted) {

        QLineEdit *m1c = filterMassDialog->findChild<QLineEdit *>("lineEdit_1stFilterWeightClean");
        QLineEdit *m1d = filterMassDialog->findChild<QLineEdit *>("lineEdit_1stFilterWeightDirty");
        QLineEdit *m2c = filterMassDialog->findChild<QLineEdit *>("lineEdit_2ndFilterWeightClean");
        QLineEdit *m2d = filterMassDialog->findChild<QLineEdit *>("lineEdit_2ndFilterWeightDirty");

        if ( (!m1c) || (!m1d) || (!m2c) || (!m2d) ) {

            QMessageBox::critical(0, "Qr49", QString::fromAscii(Q_FUNC_INFO) + ":::" + tr("Child object not found!"), 0, 0, 0);

            return;
        }

        double PTmass = (m1d->text().toDouble() - m1c->text().toDouble()) + (m2d->text().toDouble() - m2c->text().toDouble());

        lineEdit_PTmass->setText(QString::number(PTmass, 'f', 3));
    }
}

int Page_PT::nextId() const {

    return CalculationWizard::p_report;
}

Page_report::Page_report(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent) {

    calcParams = params;

    setTitle(tr("Reports creating"));
    setSubTitle(tr("Please specify the need to create reports"));

    checkBox_reports = new QCheckBox();
    label_reportsNote = new QLabel();

    checkBox_reports->setChecked(true);
    checkBox_reports->setText("Create reports");

    label_reportsNote->setWordWrap(true);
    label_reportsNote->setText("Checkbox should be checked if you make a FreeCalc");

    registerField("reports", checkBox_reports);

    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *layout1 = new QVBoxLayout();

    layout1->addWidget(label_reportsNote);

    QGroupBox *groupBox_reportsNote = new QGroupBox("Note");

    groupBox_reportsNote->setLayout(layout1);

    layout->addWidget(checkBox_reports);
    layout->addWidget(groupBox_reportsNote);

    setLayout(layout);

    connect(checkBox_reports, SIGNAL(stateChanged(int)), this, SLOT(reportsChanged(int)));
}

void Page_report::reportsChanged(int state) {

    if ( state == 2 ) {

        calcParams.data()->setReports("yes");
    }
    else {

        calcParams.data()->setReports("no");
    }
}

int Page_report::nextId() const {

    return CalculationWizard::p_conclusion;
}

Page_Vh::Page_Vh(QSharedPointer<LibtoxicParameters> params, QWidget *parent) : QWizardPage(parent) {

    calcParams = params;

    setTitle(tr("Engine capacity"));
    setSubTitle(tr("Please specify engine capacity in liters"));

    label_Vh = new QLabel("Capacity, l");
    lineEdit_Vh = new QLineEdit();

    lineEdit_Vh->setText("0");

    registerField("Vh", lineEdit_Vh);

    QHBoxLayout *layout = new QHBoxLayout();

    layout->addWidget(label_Vh);
    layout->addWidget(lineEdit_Vh);

    setLayout(layout);

    connect(lineEdit_Vh, SIGNAL(textChanged(QString)), this, SLOT(VhChanged(QString)));
}

void Page_Vh::VhChanged(QString str) {

    double Vh = str.toDouble();

    calcParams.data()->setVh(&Vh);
}

int Page_Vh::nextId() const {

    return CalculationWizard::p_conclusion;
}

Page_conclusion::Page_conclusion(QWidget *parent) : QWizardPage(parent) {

    label_conclusionNote = new QLabel();

    label_conclusionNote->setWordWrap(true);
    label_conclusionNote->setText("Ready to calculate. Let's go!");

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(label_conclusionNote);

    setLayout(layout);
}

int Page_conclusion::nextId() const {

    return -1;
}
