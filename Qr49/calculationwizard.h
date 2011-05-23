/*
    calculationwizard.h

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

#ifndef CALCULATIONWIZARD_H
#define CALCULATIONWIZARD_H

#include <QWizard>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QSpacerItem;
QT_END_NAMESPACE

class CalculationWizard : public QWizard {

    Q_OBJECT

public:
    CalculationWizard(QWidget *parent = 0);

    enum { p_task, p_std, p_fuelType, p_NOx, p_PT, p_report, p_Vh, p_conclusion };

};

class Page_task : public QWizardPage {

    Q_OBJECT

public:
    Page_task(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_task;
    QComboBox *comboBox_task;
    QLabel *label_taskNote;

};

class Page_std : public QWizardPage {

    Q_OBJECT

public:
    Page_std(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_std;
    QLabel *label_addPoints;
    QComboBox *comboBox_standard;
    QComboBox *comboBox_addPointsCalc;

private slots:
    //void standardChanged();

};

class Page_fuelType : public QWizardPage {

    Q_OBJECT

public:
    Page_fuelType(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_fuelType;
    QComboBox *comboBox_fuelType;

};

class Page_NOx : public QWizardPage {

    Q_OBJECT

public:
    Page_NOx(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_NOxSample;
    QComboBox *comboBox_NOxSample;

};

class Page_PT : public QWizardPage {

    Q_OBJECT

public:
    Page_PT(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_PTcalc;
    QLabel *label_PTmass;
    QComboBox *comboBox_PTcalc;
    QLineEdit *lineEdit_PTmass;
    QPushButton *pushButton_enterPTmass;

private slots:
    //void on_pushButton_EnterPTmass_clicked();

};

class Page_report : public QWizardPage {

    Q_OBJECT

public:
    Page_report(QWidget *parent = 0);

    int nextId() const;

private:
    QCheckBox *checkBox_reports;
    QLabel *label_reportsNote;

};

class Page_Vh : public QWizardPage {

    Q_OBJECT

public:
    Page_Vh(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_Vh;
    QLineEdit *lineEdit_Vh;

};

class Page_conclusion : public QWizardPage {

    Q_OBJECT

public:
    Page_conclusion(QWidget *parent = 0);

    int nextId() const;

private:
    QLabel *label_conclusionNote;

};

#endif // CALCULATIONWIZARD_H
