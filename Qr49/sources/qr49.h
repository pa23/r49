/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: qr49.h

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSharedPointer>
#include <QMainWindow>
#include <QTableWidget>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QObject>
#include <QEvent>
#include <QComboBox>
#include <QSettings>
#include <QDir>
#include <QFont>
#include <QRegExp>

#include "commonparameters.h"
#include "libtoxicparameters.h"
#include "filtermassdialog.h"
#include "valuedialog.h"
#include "preferencesdialog.h"
#include "checkoutdatadialog.h"
#include "undoredotable.h"
#include "helpdialog.h"
#include "newversions.h"
#include "dataimportdialog.h"

namespace Ui {

class MainWindow;

}

class MainWindow : public QMainWindow {

    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    Ui::MainWindow *ui;

    QSettings qr49settings;
    QMenu *contextMenu;
    QTableWidget *table;

    FilterMassDialog *filterMassDialog;
    ValueDialog *valueDialog;
    PreferencesDialog *preferencesDialog;
    CheckoutDataDialog *checkoutDataDialog;
    HelpDialog *helpDialog;
    DataImportDialog *dataImportDialog;

    QSharedPointer<LibtoxicParameters> params;
    QSharedPointer<CommonParameters> config;

    QDoubleValidator *doubleValidator;
    QRegExp regExp;
    QRegExpValidator *regExpValidator;

    QSharedPointer<UndoRedoTable> undoRedo_TableEU0;
    QSharedPointer<UndoRedoTable> undoRedo_TableEU3;
    QSharedPointer<UndoRedoTable> undoRedo_TablePoints;
    QSharedPointer<UndoRedoTable> undoRedo_TableFullLoadCurve;
    ptrdiff_t undoCount;
    ptrdiff_t redoCount;

    bool eventFilter(QObject *, QEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void setDoubleValidators();
    void readPreferences();
    void writeProgramSettings();
    void readProgramSettings();
    void loadAllSourceData();
    bool fillTableEU0(QString filename);
    bool fillTableEU3(QString filename);
    bool fillTablePoints(QString filename);
    bool fillTableFullLoadCurve(QString filename);
    bool fillParameters();
    bool arithmeticOperation(QString);
    void arithmeticOperationIsAvailable(bool);
    void tableCellChangedConnect(bool);
    void getUndoRedoCounters(QTableWidget *);
    void setUndoRedoButtonState();
    void saveState();
    void saveStateForAllTables();

    QDir lastReportsDir;
    QString lastCheckoutDataFileName;
    QString lastReportFileName;
    QFont monospacedFont_8;
    QFont monospacedFont_10;
    QFont liberationMonoFont_10;

    QSharedPointer<NewVersions> newVersions;

private slots:

    void on_action_DataImport_activated();
    void on_action_LoadSourceData_activated();
    void on_action_SaveSourceData_activated();
    void on_action_SaveSourceDataAs_activated();
    void on_action_LoadCalculationOptions_activated();
    void on_action_SaveCalculationOptionsAs_activated();
    void on_action_OpenReport_activated();
    void on_action_SaveReportAs_activated();
    void on_action_CloseReport_activated();
    void on_action_ReportToPDF_activated();
    void on_action_PrintReport_activated();
    void on_action_PrintSelectedCells_activated();
    void on_action_Preferences_activated();
    void on_action_Quit_activated();
    void on_action_UndoTable_activated();
    void on_action_RedoTable_activated();
    void on_action_CutFromTable_activated();
    void on_action_CopyFromTable_activated();
    void on_action_PasteToTable_activated();
    void on_action_DeleteFromTable_activated();
    void on_action_Add_activated();
    void on_action_Multiply_activated();
    void on_action_Divide_activated();
    void on_action_Equal_activated();
    void on_action_AddRow_activated();
    void on_action_DeleteRow_activated();
    void on_action_Toolbar_activated();
    void on_action_Execute_activated();
    void on_action_CheckoutData_activated();
    void on_action_StandardsDescription_activated();
    void on_action_AboutQr49_activated();
    void on_action_AboutQt_activated();
    void on_action_CheckForUpdates_activated();

    void on_pushButton_EnterPTmass_clicked();
    void taskChanged(int);
    void standardChanged(int);
    void PTcalcChanged(int);
    void reportChanged(QString);
    void tabChanged(int);
    void tableCellChanged(int, int);

    void abcCalculation();
    void gairCalculation();
    void nfanCalculation();
    void ka1mCalculation();
    void kapercCalculation();
    void smokeBaseChanged();

};

#endif // MAINWINDOW_H
