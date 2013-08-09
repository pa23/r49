/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: qr49.h

    Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>

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

#ifndef QR49_H
#define QR49_H

#include <QSharedPointer>
#include <QMainWindow>
#include <QTableWidget>
#include <QRegExpValidator>
#include <QObject>
#include <QEvent>
#include <QComboBox>
#include <QSettings>
#include <QDir>
#include <QFont>
#include <QRegExp>
#include <QPlainTextEdit>
#include <QCheckBox>

#include "txCommonParameters.h"
#include "txCalculationOptions.h"

#include "filtermassdialog.h"
#include "valuedialog.h"
#include "preferencesdialog.h"
#include "checkoutdatadialog.h"
#include "undoredotable.h"
#include "dataimportdialog.h"
#include "reportprocessingsettingsdialog.h"

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

    QSettings m_qr49settings;
    QMenu *m_contextMenu;
    QTableWidget *m_table;

    FilterMassDialog *m_filterMassDialog;
    ValueDialog *m_valueDialog;
    PreferencesDialog *m_preferencesDialog;
    CheckoutDataDialog *m_checkoutDataDialog;
    DataImportDialog *m_dataImportDialog;
    ReportProcessingSettingsDialog *m_reportprocsetdialog;

    QPlainTextEdit *m_engFieldTextEdit;
    QCheckBox *m_applyEngFieldText;
    QCheckBox *m_createCommonReport;

    QSharedPointer<toxic::txCalculationOptions> m_calculationOptions;
    QSharedPointer<toxic::txCommonParameters> m_commonParameters;

    QRegExp m_regExp;
    QRegExpValidator *m_regExpValidator;

    QSharedPointer<UndoRedoTable> m_undoRedo_TableEU0;
    QSharedPointer<UndoRedoTable> m_undoRedo_TableEU3;
    QSharedPointer<UndoRedoTable> m_undoRedo_TablePoints;
    QSharedPointer<UndoRedoTable> m_undoRedo_TableFullLoadCurve;
    ptrdiff_t m_undoCount;
    ptrdiff_t m_redoCount;

    bool eventFilter(QObject *, QEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void readPreferences();
    void writeProgramSettings();
    void readProgramSettings();
    void loadAllSourceData();
    bool fillTableEU0(const QString &);
    bool fillTableEU3(const QString &);
    bool fillTablePoints(const QString &);
    bool fillTableFullLoadCurve(const QString &);
    bool fillParameters();
    bool arithmeticOperation(const QString &);
    void tableCellChangedConnect(const bool);
    void getUndoRedoCounters(QTableWidget *);
    void setUndoRedoButtonState();
    void saveTableState();
    void saveStateForAllTables();

    QDir m_lastReportsDir;
    QString m_lastCheckoutDataFileName;
    QString m_lastReportFileName;
    QFont m_monospacedFont_8;
    QFont m_monospacedFont_10;
    QFont m_dejavusansmonoFont_10;

private slots:

    void on_action_DataImport_triggered();
    void on_action_LoadSourceData_triggered();
    void on_action_SaveSourceData_triggered();
    void on_action_SaveSourceDataAs_triggered();
    void on_action_LoadCalculationOptions_triggered();
    void on_action_SaveCalculationOptionsAs_triggered();
    void on_action_OpenReport_triggered();
    void on_action_SaveReportAs_triggered();
    void on_action_ReportToPDF_triggered();
    void on_action_SaveReportAndExportToPDF_triggered();
    void on_action_CloseReport_triggered();
    void on_action_CopyLastResultsTo_triggered();
    void on_action_PrintReport_triggered();
    void on_action_PrintSelectedCells_triggered();
    void on_action_Quit_triggered();
    void on_action_UndoTable_triggered();
    void on_action_RedoTable_triggered();
    void on_action_CutFromTable_triggered();
    void on_action_CopyFromTable_triggered();
    void on_action_PasteToTable_triggered();
    void on_action_DeleteFromTable_triggered();
    void on_action_Add_triggered();
    void on_action_Multiply_triggered();
    void on_action_Divide_triggered();
    void on_action_Equal_triggered();
    void on_action_Randomize_triggered();
    void on_action_LowerAccuracy_triggered();
    void on_action_AddRow_triggered();
    void on_action_DeleteRows_triggered();
    void on_action_Toolbar_triggered();
    void on_action_Execute_triggered();
    void on_action_CheckoutData_triggered();
    void on_action_Preferences_triggered();
    void on_action_ReportsProcessing_triggered();
    void on_action_UserManual_triggered();
    void on_action_StandardsDescription_triggered();
    void on_action_AboutQr49_triggered();
    void on_action_AboutQt_triggered();

    void on_pushButton_EnterPTmass_clicked();
    void taskChanged(const int);
    void standardChanged(const int);
    void PTcalcChanged(const int);
    void reportChanged(const QString &);
    void tabChanged(const int);
    void tableCellChanged(const int, const int);

    void abcCalculation();
    void gairCalculation();
    void nfanCalculation();
    void ka1mCalculation();
    void kapercCalculation();
    void smokeBaseChanged();

};

#endif // QR49_H
