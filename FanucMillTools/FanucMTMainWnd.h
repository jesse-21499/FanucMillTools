#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FanucMTMainWnd.h"
#include <QtWidgets/qtableview.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qcheckbox.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE
namespace Ui { class FanucMTMainWndClass;};
QT_END_NAMESPACE
class FanucNC;

class FanucMTMainWnd : public QMainWindow,public Ui::FanucMTMainWndClass
{Q_OBJECT
       
public:
   FanucNC* fanucNC;
   
   FanucMTMainWnd(QWidget *parent = nullptr);
   ~FanucMTMainWnd();
   void CopyQTableToClipboard(QTableView* qtv);
   
private slots:
    void UpdateStatusBar();
    void on_actionGet_Set_Parameters_triggered();
    void on_actionTool_Offsets_triggered();
    void on_actionWork_Zero_Offsets_triggered();
    void on_actionGet_Machine_Position_triggered();
    void on_actionFanuc_NC_Settings_triggered();
    void on_Connect_btn_clicked();
    void on_Disconnect_btn_clicked();
    void OnSetParam_btn_clicked();
    void OnReadToolOffsets_btn_clicked();
    void OnSetToolOffset_btn_clicked();
    void OnReadWorkZeroOffsets_btn_clicked();
    void OnSetWorkZeroOffset_btn_clicked();
    void OnCopyToClipboard_btn_clicked();
    void on_actionDownload_NC_triggered();
    void OnDownloadNCFile_btn_clicked();
    void OnZSurfInspectFile_btn_clicked();
    void OnMonitorMachinePos_btn_clicked();
public slots:
    void OnParamsTable_tv_selectionchanged();
    void OnToolOffsets_tv_selectionchanged();
    void OnWorkZeroOffsets_tv_selectionchanged();
    void OnMachinePos_timer_timeout();
    void OnCurrentTime_timer_timeout();
public:
    QTimer* MachinePos_timer;
    QTimer* CurrentTime_timer;
    QTableWidget* MachinePos_tw;
    QLabel* CncStatusInfo_lbl;
    QPushButton* MonitorMachinePos_btn;
    QLineEdit* PosMonitorInterval_tb;
    QTableView* ExecProg_tv;
    QTableView* MacroVars_tv;
    
    QLabel* GModalData_lbl;
    QLabel* NoGModalData_lbl;
    QLabel* SpindleSpeed_lbl;
    QLabel* FeedRate_lbl;
    QLabel* CurrentTime_lbl;
    QLabel* EllapsedTime_lbl;
    QLabel* ExecProgName_lbl;
    
private:
    
    void OnReadParams_btn_clicked();
    void CreateStatusBar();
private:
    QLabel* CNCIP_lbl;
    QLabel* CNCPort_lbl;
    QLabel* CNCConnectTimeOut_lbl;
    QLabel* FocasHndl_lbl;
    QLabel* ConnectTimeOut_lbl;
    QLabel* InfoLabel_lbl;
    
//private:
public:
    Ui::FanucMTMainWndClass *ui;
    unsigned long ActPtsReaded;
};

