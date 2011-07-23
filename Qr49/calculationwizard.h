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
#include <QSharedPointer>

#include "filtermassdialog.h"

#include "libtoxicparameters.h"

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QSpacerItem;
QT_END_NAMESPACE

class Page_task : public QWizardPage {

    Q_OBJECT

public:

    Page_task(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    QComboBox *p_combo_task() const;

    int nextId() const;

private:

    QLabel *label_task;
    QComboBox *comboBox_task;
    QLabel *label_taskNote;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void taskChanged(QString);

};

class Page_std : public QWizardPage {

    Q_OBJECT

public:

    Page_std(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    int nextId() const;

private:

    QLabel *label_std;
    QLabel *label_addPoints;
    QComboBox *comboBox_standard;
    QComboBox *comboBox_addPointsCalc;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void standardChanged(QString);
    void addPointsCalcChanged(QString);

};

class Page_fuelType : public QWizardPage {

    Q_OBJECT

public:

    Page_fuelType(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    int nextId() const;

private:

    QLabel *label_fuelType;
    QComboBox *comboBox_fuelType;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void fuelTypeChanged(QString);

};

class Page_NOx : public QWizardPage {

    Q_OBJECT

public:

    Page_NOx(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    int nextId() const;

private:

    QLabel *label_NOxSample;
    QComboBox *comboBox_NOxSample;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void NOxSampleChanged(QString);

};

class Page_PT : public QWizardPage {

    Q_OBJECT

public:

    Page_PT(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    int nextId() const;

private:

    FilterMassDialog *filterMassDialog;

    QLabel *label_PTcalc;
    QLabel *label_PTmass;
    QComboBox *comboBox_PTcalc;
    QLineEdit *lineEdit_PTmass;
    QPushButton *pushButton_enterPTmass;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void PTcalcMethodChanged(QString);
    void PTmassChanged(QString);
    void on_pushButton_EnterPTmass_clicked();

};

class Page_report : public QWizardPage {

    Q_OBJECT

public:

    Page_report(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    int nextId() const;

private:

    QCheckBox *checkBox_reports;
    QLabel *label_reportsNote;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void reportsChanged(int);

};

class Page_Vh : public QWizardPage {

    Q_OBJECT

public:

    Page_Vh(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    int nextId() const;

private:

    QLabel *label_Vh;
    QLineEdit *lineEdit_Vh;

    QSharedPointer<LibtoxicParameters> calcParams;

private slots:

    void VhChanged(QString);

};

class Page_conclusion : public QWizardPage {

    Q_OBJECT

public:

    Page_conclusion(QWidget *parent = 0);

    int nextId() const;

private:

    QLabel *label_conclusionNote;

};

void setComboIndex(QComboBox *, QString);

class CalculationWizard : public QWizard {

    Q_OBJECT

public:

    CalculationWizard(QSharedPointer<LibtoxicParameters>, QWidget *parent = 0);

    enum { p_task, p_std, p_fuelType, p_NOx, p_PT, p_report, p_Vh, p_conclusion };

    QComboBox *combo_task() const;

private:

    Page_task *page_task;

};

#endif // CALCULATIONWIZARD_H
