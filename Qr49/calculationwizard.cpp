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

#include "calculationwizard.h"

CalculationWizard::CalculationWizard(QWidget *parent) : QWizard(parent) {

    setPage(p_task, new Page_task);
    setPage(p_std, new Page_std);
    setPage(p_fuelType, new Page_fuelType);
    setPage(p_NOx, new Page_NOx);
    setPage(p_PT, new Page_PT);
    setPage(p_report, new Page_report);
    setPage(p_Vh, new Page_Vh);
    setPage(p_conclusion, new Page_conclusion);

    setStartId(p_task);

    setWindowTitle(tr("Calculation Wizard"));
}

Page_task::Page_task(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("Calculation"));
    setSubTitle(tr("Please select what you want to calculate"));

    groupBox_task = new QGroupBox("Calculation task");
    groupBox_taskNote = new QGroupBox("Note");
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
    QVBoxLayout *layout1 = new QVBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();

    layout1->addWidget(comboBox_task);
    groupBox_task->setLayout(layout1);

    layout2->addWidget(label_taskNote);
    groupBox_taskNote->setLayout(layout2);

    layout->addWidget(groupBox_task);
    layout->addWidget(groupBox_taskNote);

    setLayout(layout);
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

Page_std::Page_std(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("Standard and additional cycle points"));
    setSubTitle(tr("Please select a standard against which will be calculated and the need to calculate the additional points"));

    groupBox_std = new QGroupBox("Standard");
    groupBox_addPoints = new QGroupBox("Additional points");
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
    comboBox_addPointsCalc->addItems(QStringList() << "yes" << "no");

    registerField("standard", comboBox_standard);
    registerField("addPointsCacl", comboBox_addPointsCalc);

    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *layout1 = new QVBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();

    layout1->addWidget(comboBox_standard);
    groupBox_std->setLayout(layout1);

    layout2->addWidget(comboBox_addPointsCalc);
    groupBox_addPoints->setLayout(layout2);

    layout->addWidget(groupBox_std);
    layout->addWidget(groupBox_addPoints);

    setLayout(layout);
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

Page_fuelType::Page_fuelType(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("Fuel type"));
    setSubTitle(tr("Please select the fuel type"));

    groupBox_fuelType = new QGroupBox("Fuel type");
    comboBox_fuelType = new QComboBox();

    comboBox_fuelType->addItems(QStringList() << "diesel" << "motor" << "mazut");

    registerField("fuelType", comboBox_fuelType);

    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *layout1 = new QVBoxLayout();

    layout1->addWidget(comboBox_fuelType);
    groupBox_fuelType->setLayout(layout1);

    layout->addWidget(groupBox_fuelType);

    setLayout(layout);
}

int Page_fuelType::nextId() const {

    return CalculationWizard::p_NOx;
}

Page_NOx::Page_NOx(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("NOx sample type"));
    setSubTitle(tr("Please select the NOx sample type"));

    groupBox_NOxSample = new QGroupBox("NOx sample");
    comboBox_NOxSample = new QComboBox();

    comboBox_NOxSample->addItems(QStringList() << "wet" << "dry");

    registerField("NOxSample", comboBox_NOxSample);

    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *layout1 = new QVBoxLayout();

    layout1->addWidget(comboBox_NOxSample);
    groupBox_NOxSample->setLayout(layout1);

    layout->addWidget(groupBox_NOxSample);

    setLayout(layout);
}

int Page_NOx::nextId() const {

    return CalculationWizard::p_PT;
}

Page_PT::Page_PT(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("Particulate calculation"));
    setSubTitle(tr("Please select the particulate calculating method. If necessary, enter the mass of particles or filters"));

    groupBox_PTcalc = new QGroupBox("PT calculating method");
    groupBox_PTmass = new QGroupBox("PT mass, mg");
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
    QVBoxLayout *layout1 = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();

    layout1->addWidget(comboBox_PTcalc);
    groupBox_PTcalc->setLayout(layout1);

    layout2->addWidget(lineEdit_PTmass);
    layout2->addWidget(pushButton_enterPTmass);
    groupBox_PTmass->setLayout(layout2);

    layout->addWidget(groupBox_PTcalc);
    layout->addWidget(groupBox_PTmass);

    setLayout(layout);
}

int Page_PT::nextId() const {

    return CalculationWizard::p_report;
}

Page_report::Page_report(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("Reports creating"));
    setSubTitle(tr("Please specify the need to create reports"));

    groupBox_reports = new QGroupBox("Create reports");
    groupBox_reportsNote = new QGroupBox("Note");
    checkBox_reports = new QCheckBox();
    label_reportsNote = new QLabel();

    checkBox_reports->setChecked(true);

    label_reportsNote->setWordWrap(true);
    label_reportsNote->setText("Checkbox should be checked if you make a FreeCalc");

    registerField("reports", checkBox_reports);

    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *layout1 = new QVBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();

    layout1->addWidget(checkBox_reports);
    groupBox_reports->setLayout(layout1);

    layout2->addWidget(label_reportsNote);
    groupBox_reportsNote->setLayout(layout2);

    layout->addWidget(groupBox_reports);
    layout->addWidget(groupBox_reportsNote);

    setLayout(layout);
}

int Page_report::nextId() const {

    return CalculationWizard::p_conclusion;
}

Page_Vh::Page_Vh(QWidget *parent) : QWizardPage(parent) {

    setTitle(tr("Engine capacity"));
    setSubTitle(tr("Please specify engine capacity in liters"));

    groupBox_Vh = new QGroupBox("Capacity, l");
    lineEdit_Vh = new QLineEdit();

    lineEdit_Vh->setText("0");

    registerField("Vh", lineEdit_Vh);

    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *layout1 = new QVBoxLayout();

    layout1->addWidget(lineEdit_Vh);
    groupBox_Vh->setLayout(layout1);

    layout->addWidget(groupBox_Vh);

    setLayout(layout);
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
