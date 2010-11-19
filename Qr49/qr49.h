/*
    qr49.h

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTableWidget>
#include <QtGui/QDoubleValidator>
#include <QtGui/QRegExpValidator>
#include <QtCore/QObject>
#include <QtCore/QEvent>
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
#include "abcspeedscalcdialog.h"
#include "elrsmokecalcdialog.h"
#include "checkoutdatadialog.h"
#include "undoredotable.h"
#include "helpdialog.h"
#include "newversions.h"

namespace Ui {

    class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings qr49settings;
    QMenu *contextMenu;
    QTableWidget *table;
    FilterMassDialog *filterMassDialog;
    ValueDialog *valueDialog;
    PreferencesDialog *preferencesDialog;
    ABCspeedsCalcDialog *abcSpeedsCalcDialog;
    ELRsmokeCalcDialog *elrSmokeCalcDialog;
    CheckoutDataDialog *checkoutDataDialog;
    HelpDialog *helpDialog;
    CommonParameters *config;
    LibtoxicParameters *params;
    ptrdiff_t tableRowHeight;
    QDoubleValidator *doubleValidator;
    QRegExp *regExp;
    QRegExpValidator *regExpValidator;
    UndoRedoTable *undoRedo_TableEU0;
    UndoRedoTable *undoRedo_TableEU3;
    UndoRedoTable *undoRedo_TablePoints;
    UndoRedoTable *undoRedo_TableFullLoadCurve;
    ptrdiff_t undoCount;
    ptrdiff_t redoCount;
    bool eventFilter(QObject *, QEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void SetDoubleValidators();
    void ReadPreferences();
    void SetComboIndex(QComboBox *, QString);
    void WriteProgramSettings();
    void ReadProgramSettings();
    void LoadAllSourceData();
    bool FillTableEU0(QString filename);
    bool FillTableEU3(QString filename);
    bool FillTablePoints(QString filename);
    bool FillTableFullLoadCurve(QString filename);
    bool FillParameters();
    bool ArithmeticOperation(QString);
    void ArithmeticOperationIsAvailable(bool);
    void TableCellChangedConnect(bool);
    void GetUndoRedoCounters(QTableWidget *);
    void SetUndoRedoButtonState();
    void SaveState();
    void SaveStateForAllTables();
    QDir lastReportsDir;
    QString lastCheckoutDataFileName;
    QString lastReportFileName;
    QFont monospacedFont_8;
    QFont monospacedFont_10;
    NewVersions *newVersions;

private slots:
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
    void on_action_ABCspeeds_activated();
    void on_action_ELRsmoke_activated();
    void on_action_CheckoutData_activated();
    void on_action_StandardsDescription_activated();
    void on_action_AboutQr49_activated();
    void on_action_AboutQt_activated();
    void on_action_CheckForUpdates_activated();
    void on_pushButton_EnterPTmass_clicked();
    void TaskChanged(QString);
    void StandardChanged(QString);
    void PTcalcChanged(QString);
    void ReportChanged(QString);
    void TabChanged(int);
    void TableCellChanged(int, int);

};

#endif // MAINWINDOW_H
