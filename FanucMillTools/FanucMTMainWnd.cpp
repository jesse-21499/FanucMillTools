#pragma once
#include "FanucMTMainWnd.h"
#include "FanucNC.h"
#include <qclipboard.h>
#include <QtGui/qdesktopservices.h>
#include <QtWidgets/qheaderview.h>
#include <QtCore/qfile.h>
#include <QtWidgets/QFiledialog>

FanucMTMainWnd::FanucMTMainWnd(QWidget *parent):QMainWindow(parent), ui(new Ui::FanucMTMainWndClass())
{   
    ui->setupUi(this);
    fanucNC = new FanucNC(this);
    CreateStatusBar();
    ui->CNCIP_tb->setText(fanucNC->CNCIP);
    MachinePos_timer = new QTimer(this);
    CurrentTime_timer = new QTimer(this);
    ActPtsReaded = 0;
}

FanucMTMainWnd::~FanucMTMainWnd()
{
    delete ui;
}
void FanucMTMainWnd::CreateStatusBar()
{
    CNCIP_lbl = new QLabel;
    FocasHndl_lbl = new QLabel;
    FocasHndl_lbl->setIndent(5);
    CNCPort_lbl = new QLabel;
    CNCPort_lbl->setIndent(5);
    ConnectTimeOut_lbl = new QLabel;
    ConnectTimeOut_lbl->setIndent(5);
    QMainWindow::statusBar()->addWidget(CNCIP_lbl); CNCIP_lbl->setText("CNC IP: " + fanucNC->CNCIP);
    QMainWindow::statusBar()->addWidget(CNCPort_lbl); CNCPort_lbl->setText("CNC PORT: " + QString::number(fanucNC->CNCPort));
    QMainWindow::statusBar()->addWidget(ConnectTimeOut_lbl); ConnectTimeOut_lbl->setText("Connect Timeout: " + QString::number(fanucNC->ConnectTimeOut));
    QMainWindow::statusBar()->addWidget(FocasHndl_lbl); FocasHndl_lbl->setText("Focas Handle: " + QString::number(fanucNC->FHndl));
    //connect(Connect_btn, &QPushButton::clicked, this, &FanucMTMainWnd::UpdateStatusBar);
    //connect(Disconnect_btn, &QPushButton::clicked, this, &FanucMTMainWnd::UpdateStatusBar);
    //UpdateStatusBar();
}
void FanucMTMainWnd::UpdateStatusBar()
{
    CNCIP_lbl->setText("CNC IP: "+ fanucNC->CNCIP);
    CNCPort_lbl->setText("CNC PORT: " + QString::number(fanucNC->CNCPort));
    ConnectTimeOut_lbl->setText("Connect Timeout: " + QString::number(fanucNC->ConnectTimeOut));
    FocasHndl_lbl->setText("Focas Handle: "+ QString::number(fanucNC->FHndl));
}
void FanucMTMainWnd::on_Connect_btn_clicked()
{
    fanucNC->FocasConnect(this);
    //fanucNC->FocasConnect((QWidget*) this);
    UpdateStatusBar();
    
}
void FanucMTMainWnd::on_Disconnect_btn_clicked()
{
    
    fanucNC->FocasDisconnect(this);
    UpdateStatusBar();
}
void FanucMTMainWnd::OnReadParams_btn_clicked()
{QTableView* ParamsTable_tv = this->findChild<QTableView*>("ParamsTable_tv");
 fanucNC->FocasReadParams(this);
 QObject::connect(ParamsTable_tv->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FanucMTMainWnd::OnParamsTable_tv_selectionchanged);
 
}
void FanucMTMainWnd::OnReadToolOffsets_btn_clicked()
{QTableView* ToolOffsets_tv = this->findChild<QTableView*>("ToolOffsets_tv");
 fanucNC->FocasReadToolOffsets(this);
 QObject::connect(ToolOffsets_tv->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FanucMTMainWnd::OnToolOffsets_tv_selectionchanged);
}
void FanucMTMainWnd::OnReadWorkZeroOffsets_btn_clicked()
{
    QTableView* WorkZeroOffsets_tv = this->findChild<QTableView*>("WorkZeroOffsets_tv");
    fanucNC->FocasReadWorkZeroOffsets(this);
    QObject::connect(WorkZeroOffsets_tv->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FanucMTMainWnd::OnWorkZeroOffsets_tv_selectionchanged);
}
void FanucMTMainWnd::OnCopyToClipboard_btn_clicked()
{
 QTableView* TableView = this->findChild<QTableView*>();
 CopyQTableToClipboard(TableView);
}
void FanucMTMainWnd::OnSetParam_btn_clicked()
{
    short ret = fanucNC->FocasWriteParam(this);

}
void FanucMTMainWnd::OnSetToolOffset_btn_clicked()
{
    fanucNC->FocasWriteToolOffset(this);
}
void FanucMTMainWnd::OnSetWorkZeroOffset_btn_clicked()
{
    fanucNC->FocasWriteWorkZeroOffset(this);
}
void FanucMTMainWnd::OnParamsTable_tv_selectionchanged()
{
    QTableView* ParamsTable_tv = this->findChild<QTableView*>("ParamsTable_tv");
    QLineEdit* Param_data_tb = this->findChild<QLineEdit*>("Param_data_tb");
    QLineEdit* Param_Xdata_tb = this->findChild<QLineEdit*>("Param_Xdata_tb");
    QLineEdit* Param_Ydata_tb = this->findChild<QLineEdit*>("Param_Ydata_tb");
    QLineEdit* Param_Zdata_tb = this->findChild<QLineEdit*>("Param_Zdata_tb");
    QLabel* ParamNo_lbl = this->findChild<QLabel*>("ParamNo_lbl");
    QPushButton* SetParam_btn = this->findChild<QPushButton*>("SetParam_btn");
    SetParam_btn->setEnabled(true);
    //////////////////////////////////////////////////
    short ParamNo = 0;
    short ParamType = 0;
    int SelRows = ParamsTable_tv->selectionModel()->selectedRows().count();
    int SelCells = ParamsTable_tv->selectionModel()->selectedIndexes().count();
    int CurrRowindex = ParamsTable_tv->selectionModel()->selectedIndexes().at(0).row();
    if (SelRows == 0 && SelCells > 0)
    {
     ParamsTable_tv->selectRow(CurrRowindex);
    }
    //if (SelRows > 1) {QMessageBox::warning(NULL,NULL,"Select Only one parameter"); return; }
    ParamNo = (short)ParamsTable_tv->model()->index(CurrRowindex, 1).data().toInt();
    ParamType = (short)ParamsTable_tv->model()->index(CurrRowindex, 2).data().toInt();
    switch (ParamType & 0xFF)
    {case 0:
        Param_data_tb->setEnabled(true); Param_Xdata_tb->setEnabled(false); Param_Ydata_tb->setEnabled(false); Param_Zdata_tb->setEnabled(false);
        Param_Xdata_tb->setText(""); Param_Ydata_tb->setText(""); Param_Zdata_tb->setText("");
        switch (ParamType >> 8)
        {

        case 0:
            Param_data_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 5).data().toString());
            break;
        case 1:
            Param_data_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 5).data().toString());
            break;
        case 2:
            Param_data_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 6).data().toString());
            break;
        case 3:
            Param_data_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 7).data().toString());
            break;
        case 4:
            Param_data_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 8).data().toString());
            break;
        }
    case 1:

        break;
    case 2:

        break;
    case 3:

        break;
    case 0xFF:
        Param_data_tb->setEnabled(false); Param_Xdata_tb->setEnabled(true); Param_Ydata_tb->setEnabled(true); Param_Zdata_tb->setEnabled(true);
        Param_data_tb->setText("");
        switch (ParamType >> 8)
        {
        case 0:
            Param_Xdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 9).data().toString());
            Param_Ydata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 10).data().toString());
            Param_Zdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 11).data().toString());
            break;
        case 1:
            Param_Xdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 9).data().toString());
            Param_Ydata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 10).data().toString());
            Param_Zdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 11).data().toString());
            break;
        case 2:
            Param_Xdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 12).data().toString());
            Param_Ydata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 13).data().toString());
            Param_Zdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 14).data().toString());
            break;
        case 3:
            Param_Xdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 15).data().toString());
            Param_Ydata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 16).data().toString());
            Param_Zdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 17).data().toString());
            break;
        case 4:
            Param_Xdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 18).data().toString());
            Param_Ydata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 19).data().toString());
            Param_Zdata_tb->setText(ParamsTable_tv->model()->index(CurrRowindex, 20).data().toString());
            break;
        }
    }
    ParamNo_lbl->setText(QString("Param No: ") + QString::number(ParamNo));
}
void FanucMTMainWnd::OnToolOffsets_tv_selectionchanged()
{   QTableView* ToolOffsets_tv = this->findChild<QTableView*>("ToolOffsets_tv");
    QLineEdit* ToolRadius_tb = this->findChild<QLineEdit*>("ToolRadius_tb");
    QLineEdit* ToolRadiusWear_tb = this->findChild<QLineEdit*>("ToolRadiusWear_tb");
    QLineEdit* ToolLength_tb = this->findChild<QLineEdit*>("ToolLength_tb");
    QLineEdit* ToolLengthWear_tb = this->findChild<QLineEdit*>("ToolLengthWear_tb");
    QLabel* ToolOffsetNo_lbl = this->findChild<QLabel*>("ToolOffsetNo_lbl");
    QPushButton* SetToolOffset_btn = this->findChild<QPushButton*>("SetToolOffset_btn");
    SetToolOffset_btn->setEnabled(true);
    short ToolOffsetNo = 0;
    short ToolOffsetType = 0;
    int SelRows = ToolOffsets_tv->selectionModel()->selectedRows().count();
    int SelCells = ToolOffsets_tv->selectionModel()->selectedIndexes().count();
    int CurrRowindex = ToolOffsets_tv->selectionModel()->selectedIndexes().at(0).row();
    if (SelRows == 0 && SelCells > 0) ToolOffsets_tv->selectRow(CurrRowindex);
    //if (SelRows > 1) { MessageBox::Show("Select Only one ToolOffset"); return; }
    
    ToolOffsetNo = (short)ToolOffsets_tv->model()->index(CurrRowindex, 0).data().toInt();
    //ToolOffsetType = short::Parse(ToolOffsets_dgv->CurrentRow->Cells[2]->Value->ToString());
    ToolRadius_tb->setEnabled(true); ToolRadiusWear_tb->setEnabled(true); ToolLength_tb->setEnabled(true); ToolLengthWear_tb->setEnabled(true);
    
    ToolRadius_tb->setText(ToolOffsets_tv->model()->index(CurrRowindex, 3).data().toString());
    ToolRadiusWear_tb->setText (ToolOffsets_tv->model()->index(CurrRowindex, 4).data().toString());
    ToolLength_tb->setText (ToolOffsets_tv->model()->index(CurrRowindex, 1).data().toString());
    ToolLengthWear_tb->setText(ToolOffsets_tv->model()->index(CurrRowindex, 2).data().toString());
    ToolOffsetNo_lbl->setText(QString("ToolOffset No: ") + QString::number(ToolOffsetNo));

}
void FanucMTMainWnd::OnWorkZeroOffsets_tv_selectionchanged()
{   QTableView* WorkZeroOffsets_tv = this->findChild<QTableView*>("WorkZeroOffsets_tv");
    QLineEdit* WorkZeroOffsetX_tb = this->findChild<QLineEdit*>("WorkZeroOffsetX_tb");
    QLineEdit* WorkZeroOffsetY_tb = this->findChild<QLineEdit*>("WorkZeroOffsetY_tb");
    QLineEdit* WorkZeroOffsetZ_tb = this->findChild<QLineEdit*>("WorkZeroOffsetZ_tb");
    QLabel* WorkZeroOffsetNo_lbl = this->findChild<QLabel*>("WorkZeroOffsetNo_lbl");
    QPushButton* SetWorkZeroOffset_btn = this->findChild<QPushButton*>("SetWorkZeroOffset_btn");
    SetWorkZeroOffset_btn->setEnabled(true);
    short WorkZeroOffsetNo = 0;
    short WorkZeroOffsetType = 0;
    int SelRows = WorkZeroOffsets_tv->selectionModel()->selectedRows().count();
    int SelCells = WorkZeroOffsets_tv->selectionModel()->selectedIndexes().count();
    int CurrRowindex = WorkZeroOffsets_tv->selectionModel()->selectedIndexes().at(0).row();
    if (SelRows == 0 && SelCells > 0) WorkZeroOffsets_tv->selectRow(CurrRowindex);
    //if (SelRows > 1) { MessageBox::Show("Select Only one WorkZeroOffset"); return; }
    WorkZeroOffsetNo = (short)WorkZeroOffsets_tv->model()->index(CurrRowindex, 0).data().toInt();
    //WorkZeroOffsetType = short::Parse(WorkZeroOffsets_dgv->CurrentRow->Cells[2]->Value->ToString());
    WorkZeroOffsetX_tb->setEnabled(true); WorkZeroOffsetY_tb->setEnabled(true); WorkZeroOffsetZ_tb->setEnabled(true);
    WorkZeroOffsetX_tb->setText(WorkZeroOffsets_tv->model()->index(CurrRowindex, 1).data().toString());
    WorkZeroOffsetY_tb->setText(WorkZeroOffsets_tv->model()->index(CurrRowindex, 2).data().toString());
    WorkZeroOffsetZ_tb->setText(WorkZeroOffsets_tv->model()->index(CurrRowindex, 3).data().toString());
    WorkZeroOffsetNo_lbl->setText(QString("WorkZeroOffset No: ") + QString::number(WorkZeroOffsetNo));

}
void FanucMTMainWnd::on_actionGet_Set_Parameters_triggered()
{   
    
    QList<QWidget*> DynamicControls = this->findChildren<QWidget*>();
    for (QWidget* qWidget : DynamicControls)
    {
        if (qWidget->property("Dynamic").toString() == "Yes")
        {
            qWidget->deleteLater();
            
        }
    }
     

    QLabel* FromParam_lbl = new QLabel("FROM PARAM:", this); FromParam_lbl->setGeometry(50, 100, 150, 20); FromParam_lbl->setObjectName("FromParam_lbl"); FromParam_lbl->setProperty("Dynamic", "Yes"); FromParam_lbl->show();
    QLineEdit* FromParam_tb = new QLineEdit(this); FromParam_tb->setGeometry(150, 100, 50, 20); FromParam_tb->setObjectName("FromParam_tb"); FromParam_tb->setProperty("Dynamic", "Yes"); FromParam_tb->setStyleSheet("background-color: rgb(100,100,25);"); FromParam_tb->show();
    QLabel* ToParam_lbl = new QLabel("TO PARAM:", this); ToParam_lbl->setGeometry(50, 140, 150, 20); ToParam_lbl->setObjectName("ToParam_lbl"); ToParam_lbl->setProperty("Dynamic", "Yes"); ToParam_lbl->show();
    QLineEdit* ToParam_tb = new QLineEdit(this); ToParam_tb->setGeometry(150, 140, 50, 20); ToParam_tb->setObjectName("ToParam_tb"); ToParam_tb->setProperty("Dynamic", "Yes"); ToParam_tb->setStyleSheet("background-color: rgb(100,100,25);"); ToParam_tb->show();
    QPushButton* ReadParams_btn = new QPushButton("GET PARAMETERS", this); ReadParams_btn->setGeometry(250, 120, 125, 20); ReadParams_btn->setObjectName("ReadParams_btn"); ReadParams_btn->setProperty("Dynamic", "Yes"); ReadParams_btn->show();
    QObject::connect(ReadParams_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnReadParams_btn_clicked);
    QLabel* ParamNo_lbl = new QLabel(this); ParamNo_lbl->setGeometry(450, 120, 150, 20); ParamNo_lbl->setObjectName("ParamNo_lbl"); ParamNo_lbl->setProperty("Dynamic", "Yes"); ParamNo_lbl->show();
    QLabel* Param_data_lbl = new QLabel("PARAMETER VALUE:",this); Param_data_lbl->setGeometry(650, 100, 150, 20); Param_data_lbl->setObjectName("Param_data_lbl"); Param_data_lbl->setProperty("Dynamic", "Yes"); Param_data_lbl->show();
    QLabel* Param_Xdata_lbl = new QLabel("PARAMETER VALUE(X):", this); Param_Xdata_lbl->setGeometry(650, 130, 150, 20); Param_Xdata_lbl->setObjectName("Param_Xdata_lbl"); Param_Xdata_lbl->setProperty("Dynamic", "Yes"); Param_Xdata_lbl->show();
    QLabel* Param_Ydata_lbl = new QLabel("PARAMETER VALUE(Y):", this); Param_Ydata_lbl->setGeometry(650, 160, 150, 20); Param_Ydata_lbl->setObjectName("Param_Ydata_lbl"); Param_Ydata_lbl->setProperty("Dynamic", "Yes"); Param_Ydata_lbl->show();
    QLabel* Param_Zdata_lbl = new QLabel("PARAMETER VALUE(Z):", this); Param_Zdata_lbl->setGeometry(650, 190, 150, 20); Param_Zdata_lbl->setObjectName("Param_Zdata_lbl"); Param_Zdata_lbl->setProperty("Dynamic", "Yes"); Param_Zdata_lbl->show();
    QLineEdit* Param_data_tb = new QLineEdit(this); Param_data_tb->setGeometry(850, 100, 75, 20); Param_data_tb->setObjectName("Param_data_tb"); Param_data_tb->setProperty("Dynamic", "Yes"); Param_data_tb->setStyleSheet("background-color: rgb(100,100,25);"); Param_data_tb->show();
    QLineEdit* Param_Xdata_tb = new QLineEdit(this); Param_Xdata_tb->setGeometry(850, 130, 75, 20); Param_Xdata_tb->setObjectName("Param_Xdata_tb"); Param_Xdata_tb->setProperty("Dynamic", "Yes"); Param_Xdata_tb->setStyleSheet("background-color: rgb(100,100,25);"); Param_Xdata_tb->show();
    QLineEdit* Param_Ydata_tb = new QLineEdit(this); Param_Ydata_tb->setGeometry(850, 160, 75, 20); Param_Ydata_tb->setObjectName("Param_Ydata_tb"); Param_Ydata_tb->setProperty("Dynamic", "Yes"); Param_Ydata_tb->setStyleSheet("background-color: rgb(100,100,25);"); Param_Ydata_tb->show();
    QLineEdit* Param_Zdata_tb = new QLineEdit(this); Param_Zdata_tb->setGeometry(850, 190, 75, 20); Param_Zdata_tb->setObjectName("Param_Zdata_tb"); Param_Zdata_tb->setProperty("Dynamic", "Yes"); Param_Zdata_tb->setStyleSheet("background-color: rgb(100,100,25);"); Param_Zdata_tb->show();
    QTableView* ParamsTable_tv = new QTableView(this); ParamsTable_tv->setGeometry(50, 230, 1750, 800); ParamsTable_tv->setObjectName("ParamsTable_tv"); ParamsTable_tv->setProperty("Dynamic", "Yes");  ParamsTable_tv->setStyleSheet("background-color: rgb(53,53,53);"); ParamsTable_tv->show();
    QPushButton* SetParam_btn = new QPushButton("SET PARAMETER", this); SetParam_btn->setGeometry(970, 150, 150, 20); SetParam_btn->setObjectName("SetParam_btn"); SetParam_btn->setProperty("Dynamic", "Yes"); SetParam_btn->show(); SetParam_btn->setEnabled(false);
    QObject::connect(SetParam_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnSetParam_btn_clicked);
    QPushButton* CopyToClipboard_btn = new QPushButton("COPY TO CLIPBOARD", this); CopyToClipboard_btn->setGeometry(250, 150, 125, 20); CopyToClipboard_btn->setObjectName("CopyToClipboard_btn"); CopyToClipboard_btn->setProperty("Dynamic", "Yes"); CopyToClipboard_btn->show();
    QObject::connect(CopyToClipboard_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnCopyToClipboard_btn_clicked);
    fanucNC->FocasReadNumParams(this);
}
void FanucMTMainWnd::CopyQTableToClipboard(QTableView* qtv)
{QItemSelectionModel* selectionModel = qtv->selectionModel();
    qtv->setFocus();
    QModelIndex index1 = qtv->model()->index(0, 0);
    QModelIndex index2 = qtv->model()->index(qtv->model()->rowCount()-1, qtv->model()->columnCount()-1);
    QItemSelection itemSelection = QItemSelection(index1, index2);
    selectionModel->select(itemSelection, QItemSelectionModel::Rows | QItemSelectionModel::Select);
    //
    //for(int Row=0;Row<5;Row++)
    //({QModelIndex modelindex = qtv->model()->index(Row, 0);
     //selectionModel->select(modelindex, QItemSelectionModel::Rows);
    //}
  QModelIndexList lIndices = selectionModel->selectedIndexes();

    if (lIndices.isEmpty()) return;
    
    QMap<int, bool> selectedColumnsMap;
    foreach(QModelIndex current, lIndices) {
        selectedColumnsMap[current.column()] = true;
    }
    QList<int> selectedColumns = selectedColumnsMap.keys();
    int minCol = selectedColumns.first();
    //qtv->verticalHeader
    // prepend headers for selected columnss
    
    QString selectedText;
    
    foreach(int column, selectedColumns) 
    {
        selectedText += qtv->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
        if (column != selectedColumns.last())
            selectedText += QLatin1Char('\t');
    }
    selectedText += QLatin1Char('\n');
    
    // QModelIndex::operator < sorts first by row, then by column.
    // this is what we need
    std::sort(lIndices.begin(), lIndices.end());

    int lastRow = lIndices.first().row();
    int lastColumn = minCol;
    
    foreach(QModelIndex current, lIndices) {

        if (current.row() != lastRow) {
            selectedText += QLatin1Char('\n');
            lastColumn = minCol;
            lastRow = current.row();
        }

        if (current.column() != lastColumn) {
            for (int i = 0; i < current.column() - lastColumn; ++i)
                selectedText += QLatin1Char('\t');
            lastColumn = current.column();
        }
        //const double lValue = qtv->model()->data(current).toDouble();
        selectedText += qtv->model()->data(current).toString();
    }

    selectedText += QLatin1Char('\n');
    QApplication::clipboard()->setText(selectedText, QClipboard::Clipboard);
    

}
void FanucMTMainWnd::on_actionTool_Offsets_triggered()
{QList<QWidget*> DynamicControls = this->findChildren<QWidget*>();
 for (QWidget* qWidget : DynamicControls)
    {if (qWidget->property("Dynamic").toString() == "Yes")
        {
         qWidget->deleteLater();
        }
    }
    short ToolOffsets = 0;
    QLabel* InfoLabel_lbl = this->findChild<QLabel*>("InfoLabel_lbl");
    QLabel* FromToolOffset_lbl = new QLabel("FROM Tool:", this); FromToolOffset_lbl->setGeometry(50, 100, 150, 20); FromToolOffset_lbl->setObjectName("FromToolOffset_lbl"); FromToolOffset_lbl->setProperty("Dynamic", "Yes"); FromToolOffset_lbl->show();
    QLineEdit* FromToolOffset_tb = new QLineEdit(this); FromToolOffset_tb->setGeometry(200, 100, 50, 20); FromToolOffset_tb->setObjectName("FromToolOffset_tb"); FromToolOffset_tb->setProperty("Dynamic", "Yes"); FromToolOffset_tb->setStyleSheet("background-color: rgb(100,100,25);"); FromToolOffset_tb->show();
    QLabel* ToToolOffset_lbl = new QLabel("To Tool:", this); ToToolOffset_lbl->setGeometry(50, 140, 150, 20); ToToolOffset_lbl->setObjectName("ToToolOffset_lbl"); ToToolOffset_lbl->setProperty("Dynamic", "Yes"); ToToolOffset_lbl->show();
    QLineEdit* ToToolOffset_tb = new QLineEdit(this); ToToolOffset_tb->setGeometry(200, 140, 50, 20); ToToolOffset_tb->setObjectName("ToToolOffset_tb"); ToToolOffset_tb->setProperty("Dynamic", "Yes"); ToToolOffset_tb->setStyleSheet("background-color: rgb(100,100,25);"); ToToolOffset_tb->show();
    QPushButton* ReadToolOffsets_btn = new QPushButton("READ TOOL OFFSETS", this); ReadToolOffsets_btn->setGeometry(275, 120, 175, 20); ReadToolOffsets_btn->setObjectName("ReadToolOffsets_btn"); ReadToolOffsets_btn->setProperty("Dynamic", "Yes"); ReadToolOffsets_btn->show();
    QObject::connect(ReadToolOffsets_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnReadToolOffsets_btn_clicked);
    QLabel* ToolOffsetNo_lbl = new QLabel(NULL, this); ToolOffsetNo_lbl->setGeometry(450, 120, 150, 20); ToolOffsetNo_lbl->setObjectName("ToolOffsetNo_lbl"); ToolOffsetNo_lbl->setProperty("Dynamic", "Yes"); ToolOffsetNo_lbl->show();
    QLabel* ToolRadius_lbl = new QLabel("RADIUS:", this); ToolRadius_lbl->setGeometry(650, 160, 150, 20); ToolRadius_lbl->setObjectName("ToolRadius_lbl"); ToolRadius_lbl->setProperty("Dynamic", "Yes"); ToolRadius_lbl->show();
    QLabel* ToolRadiusWear_lbl = new QLabel("RADIUS (WEAR):", this); ToolRadiusWear_lbl->setGeometry(650, 190, 150, 20); ToolRadiusWear_lbl->setObjectName("ToolRadiusWear_lbl"); ToolRadiusWear_lbl->setProperty("Dynamic", "Yes"); ToolRadiusWear_lbl->show();
    QLabel* ToolLength_lbl = new QLabel("LENGTH:", this); ToolLength_lbl->setGeometry(650, 100, 150, 20); ToolLength_lbl->setObjectName("ToolLength_lbl"); ToolLength_lbl->setProperty("Dynamic", "Yes"); ToolLength_lbl->show();
    QLabel* ToolLengthWear_lbl = new QLabel("LENGTH (WEAR):", this); ToolLengthWear_lbl->setGeometry(650, 130, 150, 20); ToolLengthWear_lbl->setObjectName("ToolLengthWear_lbl"); ToolLengthWear_lbl->setProperty("Dynamic", "Yes"); ToolLengthWear_lbl->show();
    QLineEdit* ToolRadius_tb = new QLineEdit(this); ToolRadius_tb->setGeometry(850, 160, 75, 20); ToolRadius_tb->setObjectName("ToolRadius_tb"); ToolRadius_tb->setProperty("Dynamic", "Yes");ToolRadius_tb->setStyleSheet("background-color: rgb(100,100,25);"); ToolRadius_tb->show();
    QLineEdit* ToolRadiusWear_tb = new QLineEdit(this); ToolRadiusWear_tb->setGeometry(850, 190, 75, 20); ToolRadiusWear_tb->setObjectName("ToolRadiusWear_tb"); ToolRadiusWear_tb->setProperty("Dynamic", "Yes"); ToolRadiusWear_tb->setStyleSheet("background-color: rgb(100,100,25);"); ToolRadiusWear_tb->show();
    QLineEdit* ToolLength_tb = new QLineEdit(this); ToolLength_tb->setGeometry(850, 100, 75, 20); ToolLength_tb->setObjectName("ToolLength_tb"); ToolLength_tb->setProperty("Dynamic", "Yes"); ToolLength_tb->setStyleSheet("background-color: rgb(100,100,25);"); ToolLength_tb->show();
    QLineEdit* ToolLengthWear_tb = new QLineEdit(this); ToolLengthWear_tb->setGeometry(850, 130, 75, 20); ToolLengthWear_tb->setObjectName("ToolLengthWear_tb"); ToolLengthWear_tb->setProperty("Dynamic", "Yes"); ToolLengthWear_tb->setStyleSheet("background-color: rgb(100,100,25);"); ToolLengthWear_tb->show();
    QPushButton* SetToolOffset_btn = new QPushButton("SET TOOL OFFSET", this); SetToolOffset_btn->setGeometry(970, 150, 150, 20); SetToolOffset_btn->setObjectName("SetToolOffset_btn"); SetToolOffset_btn->setProperty("Dynamic", "Yes"); SetToolOffset_btn->show(); SetToolOffset_btn->setEnabled(false);
    QObject::connect(SetToolOffset_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnSetToolOffset_btn_clicked);
    QPushButton* CopyToClipboard_btn = new QPushButton("COPY TO CLIPBOARD", this); CopyToClipboard_btn->setGeometry(270, 150, 125, 20); CopyToClipboard_btn->setObjectName("CopyToClipboard_btn"); CopyToClipboard_btn->setProperty("Dynamic", "Yes"); CopyToClipboard_btn->show();
    QObject::connect(CopyToClipboard_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnCopyToClipboard_btn_clicked);
    QTableView* ToolOffsets_tv = new QTableView(this); ToolOffsets_tv->setGeometry(50, 230, 520, 800); ToolOffsets_tv->setObjectName("ToolOffsets_tv"); ToolOffsets_tv->setProperty("Dynamic", "Yes"); ToolOffsets_tv->setStyleSheet("background-color: rgb(53,53,53);"); ToolOffsets_tv->show();
    fanucNC->FocasReadNumToolOffsets(this, ToolOffsets);
    FromToolOffset_tb->setText("1");
    ToToolOffset_tb->setText(QString::number(ToolOffsets));
    //ToToolOffset_tb->Update();
}
void FanucMTMainWnd::on_actionWork_Zero_Offsets_triggered()
{
    QList<QWidget*> DynamicControls = this->findChildren<QWidget*>();
    for (QWidget* qWidget : DynamicControls)
    {
        if (qWidget->property("Dynamic").toString() == "Yes")
        {
            qWidget->deleteLater();

        }
    }
    short WorkZeroOffsets = 0;
    QLabel* InfoLabel_lbl = this->findChild<QLabel*>("InfoLabel_lbl");
    QLabel* FromWorkZeroOffset_lbl = new QLabel("FROM WorkOffset:", this); FromWorkZeroOffset_lbl->setGeometry(50, 100, 150, 20); FromWorkZeroOffset_lbl->setObjectName("FromWorkZeroOffset_lbl"); FromWorkZeroOffset_lbl->setProperty("Dynamic", "Yes"); FromWorkZeroOffset_lbl->show();
    QLineEdit* FromWorkZeroOffset_tb = new QLineEdit(this); FromWorkZeroOffset_tb->setGeometry(200, 100, 50, 20); FromWorkZeroOffset_tb->setObjectName("FromWorkZeroOffset_tb"); FromWorkZeroOffset_tb->setProperty("Dynamic", "Yes"); FromWorkZeroOffset_tb->show();
    QLabel* ToWorkZeroOffset_lbl = new QLabel("TO WorkOffset:", this); ToWorkZeroOffset_lbl->setGeometry(50, 140, 150, 20); ToWorkZeroOffset_lbl->setObjectName("ToWorkZeroOffset_lbl"); ToWorkZeroOffset_lbl->setProperty("Dynamic", "Yes"); ToWorkZeroOffset_lbl->show();
    QLineEdit* ToWorkZeroOffset_tb = new QLineEdit(this); ToWorkZeroOffset_tb->setGeometry(200, 140, 50, 20); ToWorkZeroOffset_tb->setObjectName("ToWorkZeroOffset_tb"); ToWorkZeroOffset_tb->setProperty("Dynamic", "Yes"); ToWorkZeroOffset_tb->show();
    QPushButton* ReadWorkZeroOffsets_btn = new QPushButton("READ WORKZERO OFFSETS", this); ReadWorkZeroOffsets_btn->setGeometry(275, 120, 175, 20); ReadWorkZeroOffsets_btn->setObjectName("ReadWorkZeroOffsets_btn"); ReadWorkZeroOffsets_btn->setProperty("Dynamic", "Yes"); ReadWorkZeroOffsets_btn->show();
    QObject::connect(ReadWorkZeroOffsets_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnReadWorkZeroOffsets_btn_clicked);
    QLabel* WorkZeroOffsetNo_lbl = new QLabel(NULL, this); WorkZeroOffsetNo_lbl->setGeometry(450, 120, 150, 20); WorkZeroOffsetNo_lbl->setObjectName("WorkZeroOffsetNo_lbl"); WorkZeroOffsetNo_lbl->setProperty("Dynamic", "Yes"); WorkZeroOffsetNo_lbl->show();
    QLabel* WorkZeroOffsetX_lbl = new QLabel("WORKZERO OFFSET(X):", this); WorkZeroOffsetX_lbl->setGeometry(720, 100, 150, 20); WorkZeroOffsetX_lbl->setObjectName("WorkZeroOffsetX_lbl"); WorkZeroOffsetX_lbl->setProperty("Dynamic", "Yes"); WorkZeroOffsetX_lbl->show();
    QLabel* WorkZeroOffsetY_lbl = new QLabel("WORKZERO OFFSET(Y)):", this); WorkZeroOffsetY_lbl->setGeometry(720, 130, 150, 20); WorkZeroOffsetY_lbl->setObjectName("WorkZeroOffsetY_lbl"); WorkZeroOffsetY_lbl->setProperty("Dynamic", "Yes"); WorkZeroOffsetY_lbl->show();
    QLabel* WorkZeroOffsetZ_lbl = new QLabel("WORKZERO OFFSET (Z):", this); WorkZeroOffsetZ_lbl->setGeometry(720, 160, 150, 20); WorkZeroOffsetZ_lbl->setObjectName("WorkZeroOffsetZ_lbl"); WorkZeroOffsetZ_lbl->setProperty("Dynamic", "Yes"); WorkZeroOffsetZ_lbl->show();
    QLineEdit* WorkZeroOffsetX_tb = new QLineEdit(this); WorkZeroOffsetX_tb->setGeometry(850, 100, 75, 20); WorkZeroOffsetX_tb->setObjectName("WorkZeroOffsetX_tb"); WorkZeroOffsetX_tb->setProperty("Dynamic", "Yes"); WorkZeroOffsetX_tb->show();
    QLineEdit* WorkZeroOffsetY_tb = new QLineEdit(this); WorkZeroOffsetY_tb->setGeometry(850, 130, 75, 20); WorkZeroOffsetY_tb->setObjectName("WorkZeroOffsetY_tb"); WorkZeroOffsetY_tb->setProperty("Dynamic", "Yes"); WorkZeroOffsetY_tb->show();
    QLineEdit* WorkZeroOffsetZ_tb = new QLineEdit(this); WorkZeroOffsetZ_tb->setGeometry(850, 160, 75, 20); WorkZeroOffsetZ_tb->setObjectName("WorkZeroOffsetZ_tb"); WorkZeroOffsetZ_tb->setProperty("Dynamic", "Yes"); WorkZeroOffsetZ_tb->show();
    QPushButton* SetWorkZeroOffset_btn = new QPushButton("SET WORKZERO OFFSET", this); SetWorkZeroOffset_btn->setGeometry(970, 150, 150, 20); SetWorkZeroOffset_btn->setObjectName("SetWorkZeroOffset_btn"); SetWorkZeroOffset_btn->setProperty("Dynamic", "Yes"); SetWorkZeroOffset_btn->show(); SetWorkZeroOffset_btn->setEnabled(false);
    QObject::connect(SetWorkZeroOffset_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnSetWorkZeroOffset_btn_clicked);
    QPushButton* CopyToClipboard_btn = new QPushButton("COPY TO CLIPBOARD", this); CopyToClipboard_btn->setGeometry(270, 150, 125, 20); CopyToClipboard_btn->setObjectName("CopyToClipboard_btn"); CopyToClipboard_btn->setProperty("Dynamic", "Yes"); CopyToClipboard_btn->show();
    QObject::connect(CopyToClipboard_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnCopyToClipboard_btn_clicked);
    QTableView* WorkZeroOffsets_tv = new QTableView(this); WorkZeroOffsets_tv->setGeometry(50, 230, 420, 800); WorkZeroOffsets_tv->setObjectName("WorkZeroOffsets_tv"); WorkZeroOffsets_tv->setProperty("Dynamic", "Yes");WorkZeroOffsets_tv->setStyleSheet("background-color: rgb(53,53,53);"); WorkZeroOffsets_tv->show();
    fanucNC->FocasReadNumWorkOffsets(this, WorkZeroOffsets);
    FromWorkZeroOffset_tb->setText("0");
    ToWorkZeroOffset_tb->setText(QString::number(WorkZeroOffsets));
    //ToWorkZeroOffset_tb->Update();
    }
 void FanucMTMainWnd::on_actionGet_Machine_Position_triggered()
{QList<QWidget*> DynamicControls = this->findChildren<QWidget*>();
 for (QWidget* qWidget : DynamicControls)
  {if (qWidget->property("Dynamic").toString() == "Yes")
    {qWidget->deleteLater();
    }
  }
    //ui->InfoLabel_lbl->show();
    MachinePos_tw = new QTableWidget(fanucNC->MachineAxes, 5, this); MachinePos_tw->setGeometry(50, 200, 550, 350); MachinePos_tw->setObjectName("MachinePos_tw");MachinePos_tw->setProperty("Dynamic", "Yes"); MachinePos_tw->setStyleSheet("background-color: rgb(100,100,25);");  MachinePos_tw->show();
    SpindleSpeed_lbl = new QLabel("", this); SpindleSpeed_lbl->setGeometry(100, 100, 100, 20); SpindleSpeed_lbl->setObjectName("SpindleSpeed_lbl"); SpindleSpeed_lbl->setProperty("Dynamic", "Yes"); SpindleSpeed_lbl->show();
    FeedRate_lbl = new QLabel("", this); FeedRate_lbl->setGeometry(300, 100, 100, 20); FeedRate_lbl->setObjectName("FeedRate_lbl"); FeedRate_lbl->setProperty("Dynamic", "Yes"); FeedRate_lbl->show();
    CurrentTime_lbl = new QLabel("", this); CurrentTime_lbl->setGeometry(750, 100, 120, 20); CurrentTime_lbl->setObjectName("CurrentTime_lbl"); CurrentTime_lbl->setProperty("Dynamic", "Yes"); CurrentTime_lbl->show();
    EllapsedTime_lbl = new QLabel("0", this); EllapsedTime_lbl->setGeometry(900, 100, 50, 20); EllapsedTime_lbl->setObjectName("EllapsedTime_lbl"); EllapsedTime_lbl->setProperty("Dynamic", "Yes"); EllapsedTime_lbl->show();
    MonitorMachinePos_btn = new QPushButton("MONITOR MACHINE POSITION", this); MonitorMachinePos_btn->setGeometry(500, 100, 175, 20); MonitorMachinePos_btn->setObjectName("MonitorMachinePos_btn"); MonitorMachinePos_btn->setProperty("Dynamic", "Yes"); MonitorMachinePos_btn->setCheckable(true); MonitorMachinePos_btn->show();
    ExecProgName_lbl = new QLabel("", this); ExecProgName_lbl->setGeometry(100, 130, 350, 20); ExecProgName_lbl->setObjectName("ExecProgName_lbl"); ExecProgName_lbl->setProperty("Dynamic", "Yes"); ExecProgName_lbl->show();
    CncStatusInfo_lbl = new QLabel("", this); CncStatusInfo_lbl->setGeometry(450, 130, 1200, 20); CncStatusInfo_lbl->setObjectName("CncStatusInfo_lbl"); CncStatusInfo_lbl->setProperty("Dynamic", "Yes"); CncStatusInfo_lbl->show();
    GModalData_lbl = new QLabel("", this); GModalData_lbl->setGeometry(100, 160, 1000, 20); GModalData_lbl->setObjectName("GModalData_lbl"); GModalData_lbl->setProperty("Dynamic", "Yes"); GModalData_lbl->show();
    NoGModalData_lbl = new QLabel("", this); NoGModalData_lbl->setGeometry(100, 180, 750, 20); NoGModalData_lbl->setObjectName("NoGModalData_lbl"); NoGModalData_lbl->setProperty("Dynamic", "Yes"); NoGModalData_lbl->show();
    ExecProg_tv = new QTableView(this); ExecProg_tv->setGeometry(750, 200, 800, 800); ExecProg_tv->setObjectName("ExecProg_tv"); ExecProg_tv->setProperty("Dynamic", "Yes"); ExecProg_tv->setStyleSheet("background-color: rgb(100,100,25);selection-background-color: rgb(70,70,70);"); ExecProg_tv->show();
    MacroVars_tv = new QTableView(this); MacroVars_tv->setGeometry(1600, 200, 200, 800); MacroVars_tv->setObjectName("MacroVars_tv"); MacroVars_tv->setProperty("Dynamic", "Yes"); MacroVars_tv->setStyleSheet("background-color: rgb(100,100,25);"); MacroVars_tv->show();
    PosMonitorInterval_tb = new QLineEdit(this); PosMonitorInterval_tb->setGeometry(700, 100,40, 20); PosMonitorInterval_tb->setObjectName("PosMonitorInterval_tb"); PosMonitorInterval_tb->setProperty("Dynamic", "Yes"); PosMonitorInterval_tb->setStyleSheet("background-color: rgb(100,100,25);"); PosMonitorInterval_tb->show();
    QObject::connect(MonitorMachinePos_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnMonitorMachinePos_btn_clicked);
    QObject::connect(CurrentTime_timer, &QTimer::timeout, this, &FanucMTMainWnd::OnCurrentTime_timer_timeout);
    //QObject::connect(MachinePos_timer, &QTimer::timeout, this, &FanucMTMainWnd::OnMachinePos_timer_timeout);
    CurrentTime_timer->setInterval(1000);
    CurrentTime_timer->start();
    PosMonitorInterval_tb->setText(QString::number(fanucNC->FanucMTSettings->value("NCSettings/MachinePosMonitorInterval", 0.5).toDouble()));
    fanucNC->FocasReadMachinePos(this);
    //delete[] fanucNC->ExecProgBlock; //Error if ExeProgBlock=null
}
void FanucMTMainWnd::on_actionFanuc_NC_Settings_triggered()
{QString SettingsFile="C://ProgramData//FanucMillTools//FanucMillTools.ini";
 if (QDesktopServices::openUrl(SettingsFile) != true) QMessageBox::warning(NULL,"ERROR","FanucMillTools.ini SETTINGS FILE NOT FOUND IN C:\ProgramData\FanucMillTools");
}
void FanucMTMainWnd::on_actionDownload_NC_triggered()
{QList<QWidget*> DynamicControls = this->findChildren<QWidget*>();
 for (QWidget* qWidget : DynamicControls)
  {if (qWidget->property("Dynamic").toString() == "Yes")
    {qWidget->deleteLater();
    }
  }
  QPushButton* DownloadNCFile_btn = new QPushButton("DOWNLOAD NC FILE", this); DownloadNCFile_btn->setGeometry(250, 120, 125, 20); DownloadNCFile_btn->setObjectName("DownloadNCFile_btn"); DownloadNCFile_btn->setProperty("Dynamic", "Yes"); DownloadNCFile_btn->show();
  QPushButton* ZSurfInspectFile_btn = new QPushButton("OPEN SURFACE Z INSPECTION FILE", this); ZSurfInspectFile_btn->setGeometry(250, 150, 220, 20); ZSurfInspectFile_btn->setObjectName("ZSurfInspectFile_btn"); ZSurfInspectFile_btn->setProperty("Dynamic", "Yes"); ZSurfInspectFile_btn->show();
  QCheckBox* ZSurfInspectFile_chkbox = new QCheckBox("CORRECT Z FLATNESS DEFECTS", this); ZSurfInspectFile_chkbox->setGeometry(500, 150, 200, 20); ZSurfInspectFile_chkbox->setObjectName("ZSurfInspectFile_chkbox"); ZSurfInspectFile_chkbox->setProperty("Dynamic", "Yes"); ZSurfInspectFile_chkbox->show();
  QObject::connect(DownloadNCFile_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnDownloadNCFile_btn_clicked);
  QObject::connect(ZSurfInspectFile_btn, &QPushButton::clicked, this, &FanucMTMainWnd::OnZSurfInspectFile_btn_clicked);

}
void FanucMTMainWnd::OnDownloadNCFile_btn_clicked()
{QString NCFilePath = QFileDialog::getOpenFileName(NULL, QObject::tr("Select NC File"), "C:\\", QObject::tr("NC Files (*.nc *.cnc )"));
 QFile NCFile(NCFilePath);
 if (!NCFile.open(QFile::ReadOnly|QFile::Text)) return;
 QTextStream NCFileStreamReader(&NCFile);
 fanucNC->FocasDownloadNCFile(this,&NCFileStreamReader);
 NCFile.close();
}
void FanucMTMainWnd::OnZSurfInspectFile_btn_clicked()
{
    //Waiting to adquire more knowledge on surface inspection routines with probes and Fanuc macros to get inspection results.
}
void FanucMTMainWnd::OnMonitorMachinePos_btn_clicked()
{if (this->MonitorMachinePos_btn->isChecked())
  {//connect(MachinePos_timer, &QTimer::timeout, this, &FanucMTMainWnd::OnMachinePos_timer_timeout);
    QObject::connect(MachinePos_timer, &QTimer::timeout, this, &FanucMTMainWnd::OnMachinePos_timer_timeout);
    double PosMonitorInterval = PosMonitorInterval_tb->text().toDouble();
    if (PosMonitorInterval > 1) { PosMonitorInterval = 1; PosMonitorInterval_tb->setText("1"); }
    if (PosMonitorInterval < 0.05) { PosMonitorInterval = 0.05; PosMonitorInterval_tb->setText("0.05"); }
   MachinePos_timer->setInterval(PosMonitorInterval* 1000);
   MachinePos_timer->start();
   MonitorMachinePos_btn->setStyleSheet("background-color: rgb(0,150,0);");
   ui->InfoLabel_lbl->setStyleSheet("background-color: transparent;color: orange;");
   ui->InfoLabel_lbl->show();
  }
else
  {MachinePos_timer->stop();
   MachinePos_timer->disconnect();
   MonitorMachinePos_btn->setStyleSheet("background-color: transparent;");
   ui->InfoLabel_lbl->hide();
  }
}
void FanucMTMainWnd::OnMachinePos_timer_timeout()
{fanucNC->FocasMonitorMachinePos(this);
 ActPtsReaded++;
}
void FanucMTMainWnd::OnCurrentTime_timer_timeout()
{
    CurrentTime_lbl->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));

}