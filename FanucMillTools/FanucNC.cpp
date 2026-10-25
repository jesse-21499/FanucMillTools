#pragma once
#include "FanucNC.h"
#include "Utils.h"

#include <QtWidgets/qheaderview.h>

#include <QtWidgets/qdatetimeedit.h>
#include <QtWidgets/qerrormessage.h>
FanucNC::FanucNC(QObject* parent) : QObject(parent)
{   //MainWnd = new FanucMTMainWnd();
    FanucMTSettings = new QSettings("C:\\ProgramData\\FanucMillTools\\FanucMillTools.ini", QSettings::IniFormat);
    Connected = false;
    FHndl = 0;
    CNCIP = FanucMTSettings->value("NCSettings/CNC_IP","127.0.0.1").toString();
    CNCPort = FanucMTSettings->value("NCSettings/CNC_PORT",8193).toInt();
    ConnectTimeOut = FanucMTSettings->value("NCSettings/ConnectTimeOut", 10).toInt();
    MachineAxes= FanucMTSettings->value("NCSettings/MachineAxes", 3).toInt();
    MaxProgBlocksToRead = FanucMTSettings->value("Settings/MaxProgBlocksToRead", 120).toInt();
    MachineRunMode = 0;
    
    //MaxNumbOfAxis = MAX_AXIS;
    MaxParamNo = 27609;
    SpindleSpeed = 0;
    FeedRate = 0;
    MachineIsMoving = 0;
    PreviousBlockNumb = 0;
    ExecProgBlockNum = 0;
    FirstProgBlockNum = 0;
    ProgBlockNum = 0;
    ProgLength = 0;
    MaxProgLength = 2892;
    ProgBlocksReaded = 0;
    MacroVarMatch=QRegularExpression("#[0-9]{1,3}=");
    MacroVarNum = 0;
    ret = 0;

}
FanucNC::~FanucNC()
{ }

short FanucNC::FocasReadNumParams(FanucMTMainWnd* MainWnd)
{   ODBPARANUM ParamsNum;
    QLineEdit* FromParam_tb = MainWnd->findChild<QLineEdit*>("FromParam_tb");
    QLineEdit* ToParam_tb = MainWnd->findChild<QLineEdit*>("ToParam_tb");
    unsigned short Param_min = 0;    //minimum number of parameter
    unsigned short Param_max = 0;    //maximum number of parameter
    unsigned short Total_Params = 0;    //total number of parameter
    short ret = cnc_rdparanum(FHndl, &ParamsNum); if (ret != EW_OK) { QMessageBox::warning(MainWnd,"ERROR", QString("Error Readin Number Of Parmeters\nError returned by cnc_rdparanum: ") + QString::number(ret)); return ret; }
    Param_min = ParamsNum.para_min;
    Param_max = ParamsNum.para_max;
    FromParam_tb->setText(QString::number(Param_min));
    ToParam_tb->setText(QString::number(Param_max));
    MaxParamNo = ParamsNum.para_max;
    return ret;
}
short FanucNC::FocasConnect(FanucMTMainWnd* MainWnd)
{    if (Connected) { QMessageBox::warning(MainWnd,"","ALREADY CONNECTED"); return 0; }
    QLineEdit* CNCIP_tb = MainWnd->findChild<QLineEdit*>("CNCIP_tb");
    QLineEdit* FocasHndl_tb = MainWnd->findChild<QLineEdit*>("FocasHndl_tb");
    QPushButton* Connect_btn = MainWnd->findChild<QPushButton*>("Connect_btn");
    QPushButton* DisConnect_btn = MainWnd->findChild<QPushButton*>("DisConnect_btn");
    CNCIP = CNCIP_tb->text();
    short ret = cnc_allclibhndl3(CNCIP.toStdString().c_str(), CNCPort, ConnectTimeOut, &FHndl);
    FocasReadCNCInfo(MainWnd);
    if (ret == EW_OK)
    {
        FocasHndl_tb->setText(QString::number(FHndl));
        Connect_btn->setStyleSheet("border: 2px solid gray;background-color: green;border-radius: 10px");
        Connect_btn->setText("CONNECTED");
        Connected = true;
    }
    if (ret != EW_OK)
    {
        QMessageBox::warning(MainWnd,"CONNECTION ERROR", QString("Unable to connect to ") + CNCIP_tb->text() + " On Port " + QString::number(CNCPort) + "\nReturn Code: " + QString::number(ret) + "\nExiting....");
        Connected = false;
    }
    return ret;

}
short FanucNC::FocasDisconnect(FanucMTMainWnd* MainWnd)
{  
    short ret = cnc_freelibhndl(FHndl);
    if (ret == EW_OK)
    {  QLineEdit* FocasHndl_tb = MainWnd->findChild<QLineEdit*>("FocasHndl_tb");
        QPushButton* Connect_btn = MainWnd->findChild<QPushButton*>("Connect_btn");
        QLabel* CncInfo_lbl = MainWnd->findChild<QLabel*>("CncInfo_lbl");
        FHndl = 0;
        FocasHndl_tb->setText("0");
        Connect_btn->setStyleSheet("");
        Connect_btn->setText("CONNECT");
        CncInfo_lbl->hide();
        Connected = false;
        QList<QWidget*> DynamicControls = MainWnd->findChildren<QWidget*>();
        for (QWidget* qWidget : DynamicControls)
        {
            if (qWidget->property("Dynamic").toString() == "Yes")
            {
                qWidget->deleteLater();
            }
        }
    }
    
    if (ret != EW_OK)
    {
        QMessageBox::warning(MainWnd,NULL,QString("Error returned by cnc_freelibhndl: ") + QString::number(ret)); return ret;
    }
    return ret;
}
short FanucNC::FocasReadCNCInfo(FanucMTMainWnd* MainWnd)
{
    QLabel* CncInfo_lbl = MainWnd->findChild<QLabel*>("CncInfo_lbl");
   
    ODBSYS CNCSysInfo;
    short ret = cnc_sysinfo(FHndl, &CNCSysInfo); if (ret != EW_OK) { QMessageBox::warning(MainWnd,"ERROR", QString("Error Reading CNC INFO\nError returned by cnc_sysinfo: ") + QString::number(ret)); return ret; }
    QString Cnc_Type = QString::fromLocal8Bit(CNCSysInfo.cnc_type);//2
    QString Cnc_Serie = QString::fromLocal8Bit(CNCSysInfo.series);//4
    QString Cnc_Version = QString::fromLocal8Bit(CNCSysInfo.version);//4
    MaxAxes = CNCSysInfo.max_axis;
    MachineAxes = (CNCSysInfo.axes[0]-'0')*10 + (CNCSysInfo.axes[1]-'0');
    QString CncModel;
    if (GetBit(CNCSysInfo.addinfo, 1) == 1)Cnc_Type = Cnc_Type + "i";
    switch (ExtractBits(CNCSysInfo.addinfo, 8, 15))
    {
    case 1:
        CncModel = "MODEL A";
        break;
    case 2:
        CncModel = "MODEL B";
        break;
    case 3:
        CncModel = "MODEL C";
        break;
    case 4:
        CncModel = "MODEL D";
        break;
    case 6:
        CncModel = "MODEL F";
        break;
    }
    CncInfo_lbl->setText(QString("NC Type: ") + Cnc_Type + QString(" ") + CncModel + QString(" Serie: ") + Cnc_Serie + QString(" Version: ") + Cnc_Version + QString(" Current Axes: ")+ QString::number(MachineAxes)+ QString(" Maximum Axes: ") + QString::number(MaxAxes));
    CncInfo_lbl->show();
    return ret;
}
short FanucNC::FocasReadParams(FanucMTMainWnd* MainWnd)
{
    QTableView* ParamsTable_tv = MainWnd->findChild<QTableView*>("ParamsTable_tv");
    //ParamsTable_tv->resizeRowsToContents();
    ParamsTable_tv->verticalHeader()->setVisible(true);
    ParamsTable_tv->verticalHeader()->setDefaultSectionSize(8);
    //ParamsTable_tv->setStyleSheet("QTableView{font-size: 8pt;} QHeaderView {font-size: 8pt;}");
    //QFont f = ParamsTable_tv->font();
    //f.setPointSize(8); // Set font size
    //ParamsTable_tv->setFont(f);
    //ParamsTable_tv->horizontalHeader()->setFont(f);
    //ParamsTable_tv->verticalHeader()->setFont(f);
    QLineEdit* FromParam_tb = MainWnd->findChild<QLineEdit*>("FromParam_tb");
    QLineEdit* ToParam_tb = MainWnd->findChild<QLineEdit*>("ToParam_tb");
    QLabel* ParamNo_lbl = MainWnd->findChild<QLabel*>("ParamNo_lbl");
    QLineEdit* CNCIP_tb = MainWnd->findChild<QLineEdit*>("CNCIP_tb");
    CNCIP = CNCIP_tb->text();
    QLabel* InfoLabel_lbl = MainWnd->findChild<QLabel*>("InfoLabel_lbl");
    InfoLabel_lbl->show();
    InfoLabel_lbl->setText("READING PARAMETERS FROM CNC");
    InfoLabel_lbl->setStyleSheet("background-color: orange");
    InfoLabel_lbl->repaint();
    paramsModel = new ParamsModel();
    if (ParamsTable_tv->model()) paramsModel->m_Parameters.clear();
    /////////////////////////
    short ret = 0;//return value
    ODBSYS CncSysInfo;
    IODBPSD Param;
    short StartParam = 0;
    short ParamsToRead = 100; //Limit to 100 in each iteration to avoid overflow
    ODBPARAIF2 ParamInfo[100];
    IODBPRM ParamExtens;
    long prm_no[1];
    unsigned short ParamInfoNum = 0;
    short PrevParam = 0;
    short NextParam = 0;
    short ParamNo = 0; //Parameter Number
    short ParamAttrib = 0;
    short ParamType = 0;
    bool IsAxisParam_attr = 0;
    bool IsSignedParam_attr = 0;
    bool Param_SettingsInput_attr = 0;
    bool Param_WriteProtect_attr = 0;
    bool Param_PowerMBeOff_atrr = 0;
    bool Param_ReadProtect_attr = 0;
    bool Param_Spindle_attr = 0;
    short Param_reserved = 0;
    short Param_reserved2 = 0;
    bool Param_IsRealType_attr = 0;
    short ParamSize = 0;
    QString ParamArrangeType = "";
    QString ParamUnitType = "";
    QString ParamUnitDimens = "";
    QString ParamInputType = "";
    QString ParamDisplay = "";
    QString ParamOthers = "";
    BYTE cdata = 0, cdata_x = 0, cdata_y = 0, cdata_z = 0;
    QString s_cdata = ""; //String representation (8-bits Binary format string in case of 8-BIT Parameter TYpe)
    QString s_cdata_x = "";
    QString s_cdata_y = "";
    QString s_cdata_z = "";
    QString s_idata = ""; QString s_idata_x = ""; QString s_idata_y = ""; QString s_idata_z = "";
    QString s_ldata = ""; QString s_ldata_x = ""; QString s_ldata_y = ""; QString s_ldata_z = "";
    QString s_rdata = ""; QString s_rdata_x = ""; QString s_rdata_y = ""; QString s_rdata_z = "";
    BYTE ParamType_UpperByte, ParamAxis_LowerByte;
    QString strParamType = "";
    QString ParamAxis = "";
    QDateTime now_time = QDateTime::currentDateTime();
    QString ActualDate = now_time.toString("MM/dd/yyyy HH:mm:ss");
    //QDateTime dateTime = dateTime.currentDateTime();
    //QString currentDateTime = dateTime.toString("yyyy-MM-dd HHmmss");
    //////////////////////////////////////////////////
    short FromParam = FromParam_tb->text().toInt();
    short ToParam = ToParam_tb->text().toInt();
    short Total_Params = ToParam - FromParam + 1;
    //////////////////////////////////////////////////
    ret = cnc_sysinfo(FHndl, &CncSysInfo); if (ret != EW_OK) {QMessageBox::warning(MainWnd,"ERROR", QString("Error Reading CNC INFO\nError returned by cnc_sysinfo : ") + QString::number(ret)); return ret; }
    short NumAxes = (short)atoi(CncSysInfo.axes);
    short ParamLength = 0;
    StartParam = FromParam;
    //////////////////////////////////////////////////
    for (int i = 0; i <= (Total_Params / ParamsToRead) + 1; i++)
    {ret = cnc_rdparainfo3(FHndl, StartParam, &ParamsToRead, &PrevParam, &NextParam, ParamInfo);
     if (ParamsToRead == 0) break;
     if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdparainfo3: ") + QString::number(ret)); return ret; }
    
     for (int j = 0; j < ParamsToRead; j++)
        {   ParamNo = ParamInfo[j].prm_no;
            if ((ParamNo > ToParam) || (ParamNo > MaxParamNo))break;
            prm_no[0] = ParamNo;
            ret = cnc_rdparam_ext(FHndl, prm_no, 1, &ParamExtens); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdparam_ext: ") + QString::number(ret) + QString("Param: ") + QString::number(ParamNo)); return ret;}
            if (ParamNo != ParamExtens.datano) {QMessageBox::warning(NULL,NULL,QString("ParamNo != ParamExtens.datano: Param: ") + QString::number(ParamNo)); return ret; }
            switch (ParamExtens.type)
            {
            case 0:
                ParamSize = 1;
                break;
            case 1:
                ParamSize = 1;
                break;
            case 2:
                ParamSize = 2;
                break;
            case 3:
                ParamSize = 4;
                break;
            case 4:
                ParamSize = 8;
                break;
            case -1:
                QMessageBox::warning(NULL,NULL,QString("INVALID PARAMETER TYPE (-1) Parameter_No") + QString::number(ParamNo));
                break;
            }
            IsAxisParam_attr = GetBit(ParamExtens.axis, 0);
            Param_Spindle_attr = GetBit(ParamExtens.axis, 1);
            IsSignedParam_attr = !GetBit(ParamExtens.info, 0);
            Param_SettingsInput_attr = GetBit(ParamExtens.info, 1);
            Param_WriteProtect_attr = !GetBit(ParamExtens.info, 2);
            Param_PowerMBeOff_atrr = GetBit(ParamExtens.info, 3);
            Param_ReadProtect_attr = !GetBit(ParamExtens.info, 4);

            switch (ParamInfo[j].array)
            {
            case 0:
                ParamArrangeType = "Independent";
                break;
            case 1:
                ParamArrangeType = "Axes";
                break;
            case 2:
                ParamArrangeType = "Spindle";
                break;
            case 3:
                ParamArrangeType = "Paths";
                break;
            case 4:
                ParamArrangeType = "Machine Group";
                break;
            default:
                break;
            }
            switch (ParamInfo[j].unit)
            {
            case 0:
                ParamUnitType = "No Real Type";
                break;
            case 1:
                ParamUnitType = "Input Unit";
                break;
            case 2:
                ParamUnitType = "Output Unit";
                break;
            case 3:
                ParamUnitType = "Offset Unit";
                break;
            default:
                break;
            }
            switch (ParamInfo[j].dim)
            {
            case 0:
                ParamUnitDimens = "Without Unit";
                break;
            case 1:
                ParamUnitDimens = "Length Unit";
                break;
            case 2:
                ParamUnitDimens = "Angle Unit";
                break;
            case 3:
                ParamUnitDimens = "Length+Angle Unit";
                break;
            case 4:
                ParamUnitDimens = "Velocity Unit";
                break;
            default:
                break;
            }
            switch (ParamInfo[j].input)
            {
            case 0:
                ParamInputType = "Normal";
                break;
            case 1:
                ParamInputType = "Setting Parameter";
                break;
            case 2:
                ParamInputType = "Input Prohibit";
                break;
            default:
                break;
            }
            ParamDisplay = "";
            ParamOthers = "";
            bool DisplayBit0 = GetBit(ParamInfo->display, 0);
            if (DisplayBit0 == 1)ParamDisplay = "Symbol Exists";
            bool DisplayBit1 = GetBit(ParamInfo->display, 1);
            if (DisplayBit1 == 1) ParamDisplay = ParamDisplay+ QString(";Top of Group");
            bool DisplayBit2 = GetBit(ParamInfo->display, 2);
            if (DisplayBit2 == 1) ParamDisplay = ParamDisplay+ QString(";Bottom of Group");
            bool DisplayBit3 = GetBit(ParamInfo->display, 3);
            if (DisplayBit3 == 1) ParamDisplay = ParamDisplay+ QString(";Gap Required");
            bool OthersBit0 = GetBit(ParamInfo->others, 0);
            if (OthersBit0 == 1) ParamOthers = "Unsigned";
            bool OthersBit1 = GetBit(ParamInfo->others, 1);
            if (OthersBit1 == 1) ParamOthers = ParamOthers+ QString(";Power Off Required");
            bool OthersBit2 = GetBit(ParamInfo->others, 2);
            if (OthersBit2 == 1) ParamOthers = ParamOthers+ QString(";Not Output");
            bool OthersBit3 = GetBit(ParamInfo->others, 3);
            if (OthersBit3 == 1) ParamOthers = ParamOthers+ QString(";Not Compared");
            bool OthersBit4 = GetBit(ParamInfo->others, 4);
            if (OthersBit4 == 1) ParamOthers = ParamOthers+ QString(";Loader Attribute");
            ParamLength = 4 + (ParamSize * MAX_AXIS) + ParamSize;
            //ret = cnc_rdparam3(FHndl, ParamNo, ALL_AXES, ParamLength,0, &Param); if (ret != EW_OK) { QMessageBox::warning("Error returned by cnc_rdParam3: " + ret.ToString()+ "Param: "+ParamNo.ToString()); return; }
            ret = cnc_rdparar(FHndl, &ParamNo, ALL_AXES, &ParamNo, &ParamLength, &Param); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdparar: ") + QString::number(ret) + QString("Param: ") + QString::number(ParamNo)); return ret; }
            ParamType = Param.type;
            ParamType_UpperByte = (BYTE)(Param.type >> 8); //0:BIT 1:BYTE 2:WORD 3:2WRD 4:REAL
            ParamAxis_LowerByte = (BYTE)(Param.type & 0xFF);
            cdata = 0; s_idata = ""; s_ldata = ""; s_rdata = "";
            cdata_x = 0; cdata_y = 0; cdata_z = 0; s_idata_x = ""; s_idata_y = ""; s_idata_z = ""; s_ldata_x = ""; s_ldata_y = ""; s_ldata_z = ""; s_rdata_x = ""; s_rdata_y = ""; s_rdata_z = "";
            s_cdata = ""; s_cdata_x = "", s_cdata_y = "", s_cdata_z = "";
            switch (Param.type & 0xFF) //Lower Byte
            {
            case 0: //Parameter without axis
                ParamAxis = "";
                switch (Param.type >> 8) //Upper Byte 0:BitType(8bits) 1:ByteType 2:WordType 3:2-WordType 4:RealType
                {
                case 0:
                    strParamType = "8-BIT";
                    cdata = Param.u.cdata;
                    s_cdata = CharToBinaryString(cdata);
                    //s_cdata = QString("%1").arg(cdata, 8, 2, QChar('0'));
                    s_idata = ""; s_ldata = ""; s_rdata = "";
                    break;
                case 1:
                    strParamType = "SBYTE"; //Signed byte
                    cdata = (qint8)Param.u.cdata;
                    s_cdata = QString::number(Param.u.cdata);
                    //s_cdata = wxString::Format("%hhd", Param.u.cdata);
                    s_idata = ""; s_ldata = ""; s_rdata = "";
                    break;
                case 2:
                    strParamType = "WORD";
                    s_idata = QString::number(Param.u.idata);
                    s_cdata = ""; s_ldata = ""; s_rdata = "";
                    break;
                case 3:
                    strParamType = "DWORD";
                    s_ldata = QString::number(Param.u.ldata);
                    s_cdata = ""; s_idata = ""; s_rdata = "";
                    break;
                case 4:
                    strParamType = "REAL";
                    s_rdata = QString::number(Param.u.rdata.prm_val * pow(10, -Param.u.rdata.dec_val),'f', Param.u.rdata.dec_val);
                    s_cdata = ""; s_idata = ""; s_ldata = "";
                    break;
                default:
                    break;
                }
                paramsModel->AddParameter(CNCIP, ParamNo, ParamType, strParamType, ParamAxis, s_cdata, s_idata, s_ldata, s_rdata, s_cdata_x, s_cdata_y, s_cdata_z, s_idata_x, s_idata_y, s_idata_z, s_ldata_x, s_ldata_y, s_ldata_z, s_rdata_x, s_rdata_y, s_rdata_z, IsAxisParam_attr, Param_Spindle_attr, IsSignedParam_attr, Param_SettingsInput_attr, Param_WriteProtect_attr, Param_PowerMBeOff_atrr, Param_ReadProtect_attr, ParamArrangeType, ParamUnitType, ParamUnitDimens, ParamInputType, ParamDisplay, ParamOthers, ActualDate);
                break;
            case 1:
                ParamAxis = "X";
                break;
            case 2:
                ParamAxis = "Y";
                break;
            case 3:
                ParamAxis = "Z";
                break;
            case 0xFF: //(Lower Byte 255 (-1) Parameter with All Axis
                ParamAxis = "ALL_AXES";
                for (short k = 0; k < NumAxes; k++)
                {
                    switch (Param.type >> 8) //Upper Byte 0:BitType 1:ByteType 2:WordType 3:DWordType 4:RealType
                    {
                    case 0:
                        strParamType = "8-BIT";
                        if (k == 0) { cdata_x = Param.u.cdatas[k]; s_cdata_x = CharToBinaryString(cdata_x); s_idata_x = ""; s_ldata_x = ""; s_rdata_x = ""; }
                        if (k == 1) { cdata_y = Param.u.cdatas[k]; s_cdata_y = CharToBinaryString(cdata_y); s_idata_y = ""; s_ldata_y = ""; s_rdata_y = ""; }
                        if (k == 2) { cdata_z = Param.u.cdatas[k]; s_cdata_z = CharToBinaryString(cdata_z); s_idata_z = ""; s_ldata_z = ""; s_rdata_z = ""; }
                        break;
                    case 1:
                        strParamType = "SBYTE"; //Signed Byte
                        if (k == 0) { cdata_x = (qint8)Param.u.cdatas[k]; s_cdata_x = QString::number(Param.u.cdatas[k]); s_idata_x = ""; s_ldata_x = ""; s_rdata_x = ""; }
                        if (k == 1) { cdata_y = (qint8)Param.u.cdatas[k]; s_cdata_y = QString::number(Param.u.cdatas[k]); s_idata_y = ""; s_ldata_y = ""; s_rdata_y = ""; }
                        if (k == 2) { cdata_z = (qint8)Param.u.cdatas[k]; s_cdata_z = QString::number(Param.u.cdatas[k]); s_idata_z = ""; s_ldata_z = ""; s_rdata_z = ""; }
                        break;
                    case 2:
                        strParamType = "WORD";
                        if (k == 0) { s_idata_x = QString::number(Param.u.idatas[k]); s_cdata_x = ""; s_ldata_x = ""; s_rdata_x = ""; }
                        if (k == 1) { s_idata_y = QString::number(Param.u.idatas[k]); s_cdata_y = ""; s_ldata_y = ""; s_rdata_y = ""; }
                        if (k == 2) { s_idata_z = QString::number(Param.u.idatas[k]); s_cdata_z = ""; s_ldata_z = ""; s_rdata_z = ""; }

                        break;
                    case 3:
                        strParamType = "DWORD";
                        if (k == 0) { s_ldata_x = QString::number(Param.u.ldatas[k]); s_cdata_x = ""; s_idata_x = ""; s_rdata_x = ""; }
                        if (k == 1) { s_ldata_y = QString::number(Param.u.ldatas[k]); s_cdata_y = ""; s_idata_y = ""; s_rdata_y = ""; }
                        if (k == 2) { s_ldata_z = QString::number(Param.u.ldatas[k]); s_cdata_z = ""; s_idata_z = ""; s_rdata_z = "";
                        }

                        break;
                    case 4:
                        strParamType = "REAL";
                        
                        if (k == 0) { s_rdata_x = QString::number(Param.u.rdatas[k].prm_val * pow(10, -Param.u.rdatas[k].dec_val),'f', Param.u.rdatas[k].dec_val); s_cdata_x = ""; s_idata_x = ""; s_ldata_x = ""; }
                        if (k == 1) { s_rdata_y = QString::number(Param.u.rdatas[k].prm_val * pow(10, -Param.u.rdatas[k].dec_val),'f', Param.u.rdatas[k].dec_val); s_cdata_y = ""; s_idata_y = ""; s_ldata_y = ""; }
                        if (k == 2) { s_rdata_z = QString::number(Param.u.rdatas[k].prm_val * pow(10, -Param.u.rdatas[k].dec_val),'f', Param.u.rdatas[k].dec_val); s_cdata_z = ""; s_idata_z = ""; s_ldata_z = ""; }
                        //s_cdata_x = ""; s_cdata_y = ""; s_cdata_z = "";
                        break;
                    default:
                        break;
                    }
                }

                paramsModel->AddParameter(CNCIP, ParamNo, ParamType, strParamType, ParamAxis, s_cdata, s_idata, s_ldata, s_rdata, s_cdata_x, s_cdata_y, s_cdata_z, s_idata_x, s_idata_y, s_idata_z, s_ldata_x, s_ldata_y, s_ldata_z, s_rdata_x, s_rdata_y, s_rdata_z, IsAxisParam_attr, Param_Spindle_attr, IsSignedParam_attr, Param_SettingsInput_attr, Param_WriteProtect_attr, Param_PowerMBeOff_atrr, Param_ReadProtect_attr, ParamArrangeType, ParamUnitType, ParamUnitDimens, ParamInputType, ParamDisplay, ParamOthers, ActualDate);

                break;
            default:
                break;
            }
            StartParam = ParamNo + 1;
            ParamNo_lbl->setText(QString("Param No: ") + QString::number(ParamNo));
            ParamNo_lbl->repaint();
        }
    }
    ParamsTable_tv->setModel(paramsModel);
    
    InfoLabel_lbl->setStyleSheet("background-color: transparent");
    InfoLabel_lbl->hide();
    
    return ret;
}
short FanucNC::FocasWriteParam(FanucMTMainWnd* MainWnd)
{
    QTableView* ParamsTable_tv = MainWnd->findChild<QTableView*>("ParamsTable_tv");
    QLineEdit* Param_data_tb= MainWnd->findChild<QLineEdit*>("Param_data_tb");
    QLineEdit* Param_Xdata_tb = MainWnd->findChild<QLineEdit*>("Param_Xdata_tb");
    QLineEdit* Param_Ydata_tb = MainWnd->findChild<QLineEdit*>("Param_Ydata_tb");
    QLineEdit* Param_Zdata_tb = MainWnd->findChild<QLineEdit*>("Param_Zdata_tb");
    QLabel* InfoLabel_lbl = MainWnd->findChild<QLabel*>("InfoLabel_lbl");
    
    InfoLabel_lbl->show();
    InfoLabel_lbl->setStyleSheet("background-color: orange");
    InfoLabel_lbl->setText("WRITING PARAMETER TO CNC");
    InfoLabel_lbl->repaint();
    IODBPSD Param;
    short ParamType = 0;
    int SelRows = ParamsTable_tv->selectionModel()->selectedRows().count();
    int SelCells = ParamsTable_tv->selectionModel()->selectedIndexes().count();
    
    QString cdata;
    QString cdataX;
    QString cdataY;
    QString cdataZ;
    QString rdata;
    cdata = Param_data_tb->text();
    int DecPointPos = 0;
    
    int strlen = 0;
    QModelIndex targetIndex = ParamsTable_tv->model()->index(2, 0);
    int CurrRowindex = ParamsTable_tv->selectionModel()->selectedIndexes().at(0).row();
    //if (SelRows == 0 && SelCells > 0) ParamsTable_tv->selectionModel()->select(targetIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    if (SelRows == 0 && SelCells > 0)
    {
        
        ParamsTable_tv->selectRow(CurrRowindex);
    }
    //if (SelRows != 1) { QMessageBox::warning(NULL,NULL,"Select Only one parameter"); return 1; }
    
    Param.datano = (short)ParamsTable_tv->model()->index(CurrRowindex, 1).data().toInt();
    ParamType = (short)ParamsTable_tv->model()->index(CurrRowindex, 2).data().toInt();
    short NoAxis = 0;
    //REALPRM ParamRealPrm;
    switch (ParamType & 0xFF)
    {
    case 0:
        NoAxis = 1;
        Param.type = 0;
        switch (ParamType >> 8)
        {
        case 0:
            cdata = Param_data_tb->text();
            
            Param.u.cdata = (unsigned char)cdata.toUShort(NULL, 2);
            break;
        case 1:
            Param.u.cdata = (signed char)Param_data_tb->text().toShort();
            break;
        case 2:
            Param.u.idata = Param_data_tb->text().toShort();
            break;
        case 3:
            Param.u.ldata = Param_data_tb->text().toLong();
            break;
        case 4:
            strlen = Param_data_tb->text().length();
            Param.u.rdata.dec_val = strlen - Param_data_tb->text().indexOf('.') - 1;
            
            Param.u.rdata.prm_val = atol(Param_data_tb->text().remove('.').toUtf8());
            
                        
            break;
        }
        break;
    case 1:
        Param.type = 1;
        NoAxis = 1;
        break;
    case 2:
        Param.type = 2;
        NoAxis = 1;
        break;
    case 3:
        Param.type = 3;
        NoAxis = 1;
        break;
    case 0xFF:
        Param.type = -1;
        NoAxis = MAX_AXIS;
        
        switch (ParamType >> 8)
        {
            
        case 0:
            cdataX = Param_Xdata_tb->text();
            cdataY = Param_Ydata_tb->text();
            cdataZ = Param_Zdata_tb->text();
            Param.u.cdatas[0] = (unsigned char)cdataX.toUShort(NULL, 2);
            Param.u.cdatas[1] = (unsigned char)cdataY.toUShort(NULL, 2);
            Param.u.cdatas[2] = (unsigned char)cdataZ.toUShort(NULL, 2);
            break;
        case 1:
            Param.u.cdatas[0] = (signed char)Param_Xdata_tb->text().toShort();
            Param.u.cdatas[1] = (signed char)Param_Ydata_tb->text().toShort();
            Param.u.cdatas[2] = (signed char)Param_Zdata_tb->text().toShort();
            break;
        case 2:
            Param.u.idatas[0] = Param_Xdata_tb->text().toShort();
            Param.u.idatas[1] = Param_Ydata_tb->text().toShort();
            Param.u.idatas[2] = Param_Zdata_tb->text().toShort();
            break;
        case 3:
            Param.u.ldatas[0] = Param_Xdata_tb->text().toLong();
            Param.u.ldatas[1] = Param_Ydata_tb->text().toLong();
            Param.u.ldatas[2] = Param_Zdata_tb->text().toLong();
            break;
        case 4:
            strlen = Param_Xdata_tb->text().length();
            Param.u.rdatas[0].dec_val = strlen - Param_Xdata_tb->text().indexOf('.') - 1;
            
            Param.u.rdatas[0].prm_val = atol(Param_Xdata_tb->text().remove('.').toUtf8());
            strlen = Param_Ydata_tb->text().length();
            Param.u.rdatas[1].dec_val = strlen - Param_Ydata_tb->text().indexOf('.') - 1;
            
            Param.u.rdatas[1].prm_val = atol(Param_Ydata_tb->text().remove('.').toUtf8());
            strlen = Param_Zdata_tb->text().length();
            Param.u.rdatas[2].dec_val = strlen - Param_Zdata_tb->text().indexOf('.') - 1;
            
            Param.u.rdatas[2].prm_val = atol(Param_Zdata_tb->text().remove('.').toUtf8());
            break;
     
        }
     
        break;
     
    }
     
    short ParamSize = 0;
    switch (ParamType >> 8)
    {
    case 0:
        ParamSize = 1;
        break;
    case 1:
        ParamSize = 1;
        break;
    case 2:
        ParamSize = 2;
        break;
    case 3:
        ParamSize = 4;
        break;
    case 4:
        ParamSize = 8;
        break;
    case -1:
        //MessageBox::Show("INVALID PARAMETER TYPE (-1) " + "Parameter_No" + ParamNo.ToString());
        break;
    }
    short ParamLength = 4 + (ParamSize * NoAxis);
    short ret = cnc_wrparam(FHndl, ParamLength, &Param);
    if (ret == EW_OK)
    {
        InfoLabel_lbl->setStyleSheet("background-color: transparent");
        InfoLabel_lbl->hide();
        QMessageBox::warning(NULL,NULL,"UPDATED PARAMETER IN CNC");
    }
    if (ret != EW_OK)
    {
        InfoLabel_lbl->setStyleSheet("background-color: transparent");
        InfoLabel_lbl->hide();
        QMessageBox::warning(NULL,NULL,QString("ERROR: ") + QString::number(ret));

    }
    
    return ret;
    //Param_data_tb->Update();
    
}
short FanucNC::FocasReadNumToolOffsets(FanucMTMainWnd* MainWnd,short &ToolOffsets)
{  short ret = 0;
    //TextBox^ FromToolOffset_tb = (TextBox^)MainFrm->Controls["FromToolOffset_tb"];
    //TextBox^ ToToolOffset_tb = (TextBox^)MainFrm->Controls["ToToolOffset_tb"];
    ToolOffsets = 0;
    //short ToolOffMemType = 0;
    //String^ sToolOffMemType = "";
    ODBTLINF ToolOffsInfo;
    //ODBTLINF2 ToolOffsInfo2;
    //ODBTOFS ToolOffsRad;
    //ODBTOFS ToolOffsRadWear;
    //ODBTOFS ToolOffsLen;
    //ODBTOFS ToolOffsLenWear;
    //IODBPSD Param;
    //float ToolOffsetUnit = 0;
    //bool AvailableXOffset = false;
    //bool AvailableYOffset = false;
    //bool AvailableZOffset = false;
    //bool AvailableNoseRadOffset = false;
    //bool AvailableImagToolNoseOffset = false;
    //bool AvailableSecondGeomOffset = false;
    //bool Available4thAxisOffset = false;
    //bool Available5thAxisOffset = false;
    //Read tool offset information
    ret = cnc_rdtofsinfo(FHndl, &ToolOffsInfo); if (ret != EW_OK) { QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofsinfo: ") + QString::number(ret)); return ret; }
    //FromToolOffset_tb->Text = "1";
    ToolOffsets = ToolOffsInfo.use_no;
    //ToToolOffset_tb->Text = ToolOffsets.ToString();
    //ToToolOffset_tb->Update();
    return ret;
}
short FanucNC::FocasReadNumWorkOffsets(FanucMTMainWnd* MainWnd, short& WorkOffsets)
{
    
    //TextBox^ FromToolOffset_tb = (TextBox^)MainFrm->Controls["FromToolOffset_tb"];
    //TextBox^ ToToolOffset_tb = (TextBox^)MainFrm->Controls["ToToolOffset_tb"];
    WorkOffsets = 0;
    //short ToolOffMemType = 0;
    //String^ sToolOffMemType = "";
    //ODBTLINF ToolOffsInfo;
    //ODBTLINF2 ToolOffsInfo2;
    //ODBTOFS ToolOffsRad;
    //ODBTOFS ToolOffsRadWear;
    //ODBTOFS ToolOffsLen;
    //ODBTOFS ToolOffsLenWear;
    //IODBPSD Param;
    //float ToolOffsetUnit = 0;
    //bool AvailableXOffset = false;
    //bool AvailableYOffset = false;
    //bool AvailableZOffset = false;
    //bool AvailableNoseRadOffset = false;
    //bool AvailableImagToolNoseOffset = false;
    //bool AvailableSecondGeomOffset = false;
    //bool Available4thAxisOffset = false;
    //bool Available5thAxisOffset = false;
    //Read tool offset information
     short ret = cnc_rdzofsinfo(FHndl, &WorkOffsets); if (ret != EW_OK) { QMessageBox::warning(NULL, NULL, "Error returned by cnc_rdzofsinfo: " + QString::number(ret)); return ret; };
    //FromToolOffset_tb->Text = "1";
    //WorkZeroOffsets = ToolOffsInfo.use_no;
    //ToToolOffset_tb->Text = ToolOffsets.ToString();
    //ToToolOffset_tb->Update();
    return ret;
}
short FanucNC::FocasReadToolOffsets(FanucMTMainWnd* MainWnd)
{QTableView* ToolOffsets_tv = MainWnd->findChild<QTableView*>("ToolOffsets_tv");
 ToolOffsets_tv->verticalHeader()->setVisible(true);
 ToolOffsets_tv->verticalHeader()->setDefaultSectionSize(8);
 QLabel* InfoLabel_lbl= MainWnd->findChild<QLabel*>("InfoLabel_lbl");
 QLineEdit* FromToolOffset_tb = MainWnd->findChild<QLineEdit*>("FromToolOffset_tb");
 //Label^ FromToolOffset_lbl = (Label^)MainFrm->Controls["FromToolOffset_lbl"];
 //Label^ ToToolOffset_lbl = (Label^)MainFrm->Controls["ToToolOffset_lbl"];
 QLineEdit* ToToolOffset_tb = MainWnd->findChild<QLineEdit*>("ToToolOffset_tb");
 QLabel* ToolOffsetNo_lbl = MainWnd->findChild<QLabel*>("ToolOffsetNo_lbl");
 short FirstToolOffset = FromToolOffset_tb->text().toShort();
 short LastToolOffset = ToToolOffset_tb->text().toShort();
 //ToolOffsets_dgv->Rows->Clear();
 InfoLabel_lbl->show();
 InfoLabel_lbl->setText("READING TOOL OFFSETS FROM CNC");
 InfoLabel_lbl->setStyleSheet("background-color: orange");
 InfoLabel_lbl->repaint();
 toolOffsetsModel = new ToolOffsetsModel();
 if (ToolOffsets_tv->model()) toolOffsetsModel->m_ToolOffsets.clear();
 short ToolOffsets = 0;
 short ToolOffMemType = 0;
 QString sToolOffMemType;
 ODBTLINF ToolOffsInfo;
 ODBTLINF2 ToolOffsInfo2;
 ODBTOFS ToolOffsRad;
 ODBTOFS ToolOffsRadWear;
 ODBTOFS ToolOffsLen;
 ODBTOFS ToolOffsLenWear;
 IODBPSD Param;
    double ToolOffsetUnit = 0;
    bool AvailableXOffset = false;
    bool AvailableYOffset = false;
    bool AvailableZOffset = false;
    bool AvailableNoseRadOffset = false;
    bool AvailableImagToolNoseOffset = false;
    bool AvailableSecondGeomOffset = false;
    bool Available4thAxisOffset = false;
    bool Available5thAxisOffset = false;
    //Read tool offset information
    short ret = cnc_rdtofsinfo(FHndl, &ToolOffsInfo); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofsinfo: ") + QString::number(ret)); return ret;}
    switch (ToolOffsInfo.ofs_type)
    {
    case 0:
        sToolOffMemType = "memory type A";
        break;
    case 1:
        sToolOffMemType = "memory type B"; //Includes wear offset
        break;
    case 2:
        sToolOffMemType = "memory type C";//Includes wear offset
        break;
    default:
        sToolOffMemType = "Not Specified";
        break;
    }
    ret = cnc_rdparam(FHndl, 5042, ALL_AXES, 4 + MAX_AXIS, &Param); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdparam (Param 5042): ") + QString::number(ret)); return ret; }
    switch (Param.u.cdata)
    {
    case 0:
        ToolOffsetUnit = 0.001;
        break;
    case 1:
        ToolOffsetUnit = 0.01;
        break;
    case 2:
        ToolOffsetUnit = 0.0001;
        break;
    case 4:
        ToolOffsetUnit = 0.00001;
        break;
    case 8:
        ToolOffsetUnit = 0.000001;
        break;
    }
    ToolOffsets = ToolOffsInfo.use_no;
    //Read tool offset information (2) includes offsets available information
    ret = cnc_rdtofsinfo2(FHndl, &ToolOffsInfo2);  if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofsinfo2: ") +QString::number(ret)); return ret; }
    AvailableXOffset = GetBit(ToolOffsInfo2.ofs_enable, 0);
    AvailableYOffset = GetBit(ToolOffsInfo2.ofs_enable, 1);
    AvailableZOffset = GetBit(ToolOffsInfo2.ofs_enable, 2);
    AvailableNoseRadOffset = GetBit(ToolOffsInfo2.ofs_enable, 3);
    AvailableImagToolNoseOffset = GetBit(ToolOffsInfo2.ofs_enable, 4);
    AvailableSecondGeomOffset = GetBit(ToolOffsInfo2.ofs_enable, 5);
    Available4thAxisOffset = GetBit(ToolOffsInfo2.ofs_enable, 7);
    Available5thAxisOffset = GetBit(ToolOffsInfo2.ofs_enable, 8);
    for (int i = FirstToolOffset; i <= LastToolOffset; i++)
    {ret=cnc_rdtofs(FHndl, i, 1, 8, &ToolOffsRad); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofs: ") + QString::number(ret) + QString(" Offset No (Radius): ") + QString::number(i)); return ret; }
     ret=cnc_rdtofs(FHndl, i, 0, 8, &ToolOffsRadWear); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofs: ") + QString::number(ret) + QString("Offset No (Radius Wear): ") + QString::number(i)); return ret; }
     ret=cnc_rdtofs(FHndl, i, 3, 8, &ToolOffsLen); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofs: ")+QString::number(ret) + QString("Offset No (Length): ") + QString::number(i)); return ret; }
     ret=cnc_rdtofs(FHndl, i, 2, 8, &ToolOffsLenWear); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdtofs: ") + QString::number(ret) + QString("Offset No (Length Wear): ") + QString::number(i)); return ret; }
     toolOffsetsModel->AddToolOffset(ToolOffsRad.datano, ToolOffsLen.data * ToolOffsetUnit, ToolOffsLenWear.data * ToolOffsetUnit, ToolOffsRad.data * ToolOffsetUnit, ToolOffsRadWear.data * ToolOffsetUnit);
     ToolOffsetNo_lbl->setText(QString("Tool No: ") +QString::number(i));
     ToolOffsetNo_lbl->repaint();
    }
    ToolOffsets_tv->setModel(toolOffsetsModel);
    InfoLabel_lbl->setStyleSheet("background-color: transparent");
    InfoLabel_lbl->hide();
    //cnc_rdtofsr //Read tool offset value(area specified)
    //cnc_tofs_rnge //Read the effective setting range of tool offset value
    return ret;

}
short FanucNC::FocasWriteToolOffset(FanucMTMainWnd* MainWnd)
{    //cnc_wrtofsr //Write tool offset value(area specified)
    QTableView* ToolOffsets_tv = MainWnd->findChild<QTableView*>("ToolOffsets_tv");
    QLineEdit* ToolRadius_tb = MainWnd->findChild<QLineEdit*>("ToolRadius_tb");
    QLineEdit* ToolRadiusWear_tb = MainWnd->findChild<QLineEdit*>("ToolRadiusWear_tb");
    QLineEdit* ToolLength_tb = MainWnd->findChild<QLineEdit*>("ToolLength_tb");
    QLineEdit* ToolLengthWear_tb = MainWnd->findChild<QLineEdit*>("ToolLengthWear_tb");
    QLabel* InfoLabel_lbl = MainWnd->findChild<QLabel*>("InfoLabel_lbl");
    InfoLabel_lbl->show();
    InfoLabel_lbl->setStyleSheet("background-color: orange");
    InfoLabel_lbl->setText("WRITING TOOL OFFSET TO CNC");
    InfoLabel_lbl->repaint();
    int SelRows = ToolOffsets_tv->selectionModel()->selectedRows().count();
    int SelCells = ToolOffsets_tv->selectionModel()->selectedIndexes().count();
    int CurrRowindex = ToolOffsets_tv->selectionModel()->selectedIndexes().at(0).row();
    if (SelRows == 0 && SelCells > 0) ToolOffsets_tv->selectRow(CurrRowindex);
    //if (SelRows != 1) { MessageBox::Show("Select only one ToolOffset"); return 1; }
    short ToolOffsetNo = ToolOffsets_tv->model()->index(CurrRowindex, 0).data().toInt();
    IODBPSD Param;
    //float ToolOffsetUnit = 0;
    short DecimalsNum = 0;
    short ret = cnc_rdparam(FHndl, 5042, ALL_AXES, 4 + MAX_AXIS, &Param); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,QString("Error returned by cnc_rdparam (Param 5042): ") + QString::number(ret)); return ret; }
    switch (Param.u.cdata)
    {
    case 0:
        //ToolOffsetUnit = 0.001;
        DecimalsNum = 3;
        break;
    case 1:
        //ToolOffsetUnit = 0.01;
        DecimalsNum = 2;
        break;
    case 2:
        //ToolOffsetUnit = 0.0001;
        DecimalsNum = 4;
        break;
    case 4:
        //ToolOffsetUnit = 0.00001;
        DecimalsNum = 5;
        break;
    case 8:
        //ToolOffsetUnit = 0.000001;
        DecimalsNum = 6;
        break;
    }
    if ((ToolLength_tb->text().length()- ToolLength_tb->text().indexOf('.') - 1) != DecimalsNum) {QMessageBox::warning(NULL,NULL,"ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    if ((ToolLengthWear_tb->text().length() - ToolLengthWear_tb->text().indexOf('.') - 1) != DecimalsNum) {QMessageBox::warning(NULL,NULL,"ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    if ((ToolRadius_tb->text().length() - ToolRadius_tb->text().indexOf('.') - 1) != DecimalsNum) {QMessageBox::warning(NULL,NULL,"ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    if ((ToolRadiusWear_tb->text().length() - ToolRadiusWear_tb->text().indexOf('.') - 1) != DecimalsNum) {QMessageBox::warning(NULL,NULL,"ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    //ret = cnc_wrtofs(FHndl, ToolOffsetNo, 3, 8, Convert::ToInt32(Convert::ToSingle(ToolLength_tb->Text) * Math::Pow(10, DecimalsNum))); if (ret != EW_OK) goto finish; // Write Tool Length offset value
    ret = cnc_wrtofs(FHndl, ToolOffsetNo, 3, 8, (long)(ToolLength_tb->text().toDouble() * pow(10, DecimalsNum))); if (ret != EW_OK) goto finish; // Write Tool Length offset value
    ret = cnc_wrtofs(FHndl, ToolOffsetNo, 2, 8, (long)(ToolLengthWear_tb->text().toDouble() *pow(10, DecimalsNum))); if (ret != EW_OK) goto finish; // Write Tool Length Wear offset value
    ret = cnc_wrtofs(FHndl, ToolOffsetNo, 1, 8, (long)(ToolRadius_tb->text().toDouble()*pow(10, DecimalsNum))); if (ret != EW_OK) goto finish; // Write Tool Radius offset value
    ret = cnc_wrtofs(FHndl, ToolOffsetNo, 0, 8, (long)(ToolRadiusWear_tb->text().toDouble()*pow(10, DecimalsNum))); if (ret != EW_OK) goto finish; // Write Tool Radius Wear offset value
    InfoLabel_lbl->setStyleSheet("background-color: transparent");
    InfoLabel_lbl->hide();
    QMessageBox::warning(NULL,NULL,"UPDATED TOOL OFFSET IN CNC");
finish:
    if (ret != EW_OK)
    {InfoLabel_lbl->setStyleSheet("background-color: transparent");
    InfoLabel_lbl->hide();
     QMessageBox::warning(NULL,NULL,"ERROR RETURNED BY cnc_wrtofs: " + QString::number(ret));
    }
    return ret;
}
short FanucNC::FocasReadWorkZeroOffsets(FanucMTMainWnd* MainWnd)
{
    short ret = 0;
    QTableView* WorkZeroOffsets_tv = MainWnd->findChild<QTableView*>("WorkZeroOffsets_tv");
    QLineEdit* FromWorkZeroOffset_tb = MainWnd->findChild<QLineEdit*>("FromWorkZeroOffset_tb");
    QLineEdit* ToWorkZeroOffset_tb = MainWnd->findChild<QLineEdit*>("ToWorkZeroOffset_tb");
    //Label^ FromToolOffset_lbl = (Label^)MainFrm->Controls["FromToolOffset_lbl"];
    
    //Label^ ToWorkZeroOffset_lbl = (Label^)MainFrm->Controls["ToWorkZeroOffset_lbl"];
    QLabel* InfoLabel_lbl = MainWnd->findChild<QLabel*>("InfoLabel_lbl");
    QLabel* WorkZeroOffsetNo_lbl = MainWnd->findChild<QLabel*>("WorkZeroOffsetNo_lbl");
    short FirstWorkZeroOffset = FromWorkZeroOffset_tb->text().toShort();
    short LastWorkZeroOffset = ToWorkZeroOffset_tb->text().toShort();
    workZeroOffsetsModel = new WorkZeroOffsetsModel();
    if (WorkZeroOffsets_tv->model()) workZeroOffsetsModel->m_WorkZeroOffsets.clear();
    
    InfoLabel_lbl->show();
    InfoLabel_lbl->setText("READING WORK ZERO OFFSETS FROM CNC");
    InfoLabel_lbl->setStyleSheet("background-color: orange");
    InfoLabel_lbl->repaint();
    //short WorkZeroOffsets = 0;
    //short ToolOffMemType = 0;
    //String^ sToolOffMemType = "";
    IODBZOFS WorkZeroOffset;
    IODBPSD Param;
    double WorkZeroOffsetUnit = 0;
    ret = cnc_rdparam(FHndl, 1013, ALL_AXES, 4 + MAX_AXIS, &Param); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,"Error returned by cnc_rdparam (Param 1013): " + QString::number(ret)); return ret; }
    switch (Param.u.cdata)
    {
    case 0:
        WorkZeroOffsetUnit = 0.001;
        
        break;
    case 1:
        WorkZeroOffsetUnit = 0.01;
        
        break;
    case 2:
        WorkZeroOffsetUnit = 0.0001;
        
        break;
    case 4:
        WorkZeroOffsetUnit = 0.00001;
        
        break;
    case 8:
        WorkZeroOffsetUnit = 0.000001;
        
        break;
    }
    //WorkZeroOffsets = ToolOffsInfo.use_no;
    
    for (int i = FirstWorkZeroOffset; i < LastWorkZeroOffset; i++)
    {   ret = cnc_rdzofs(FHndl, i, -1, 4 + 4 * MAX_AXIS, &WorkZeroOffset); if (ret != EW_OK) {QMessageBox::warning(NULL,NULL,"Error returned by cnc_rdzofs: " + QString::number(ret) + " Work Zero Offset No: " + QString::number(i)); return ret; }
        workZeroOffsetsModel->AddWorkZeroOffset(WorkZeroOffset.datano, WorkZeroOffset.data[0] * WorkZeroOffsetUnit, WorkZeroOffset.data[1]* WorkZeroOffsetUnit,WorkZeroOffset.data[2] * WorkZeroOffsetUnit);
        WorkZeroOffsetNo_lbl->setText("Work Zero Offset No: " + QString::number(i));
        WorkZeroOffsetNo_lbl->repaint();
    }
    WorkZeroOffsets_tv->setModel(workZeroOffsetsModel);
    InfoLabel_lbl->setStyleSheet("background-color: transparent");
    InfoLabel_lbl->hide();
    return ret;
}
short FanucNC::FocasWriteWorkZeroOffset(FanucMTMainWnd* MainWnd)
{
    short ret = 0;
    
    QTableView* WorkZeroOffsets_tv = MainWnd->findChild<QTableView*>("WorkZeroOffsets_tv");
    QLineEdit* WorkZeroOffsetX_tb = MainWnd->findChild<QLineEdit*>("WorkZeroOffsetX_tb");
    QLineEdit* WorkZeroOffsetY_tb = MainWnd->findChild<QLineEdit*>("WorkZeroOffsetY_tb");
    QLineEdit* WorkZeroOffsetZ_tb = MainWnd->findChild<QLineEdit*>("WorkZeroOffsetZ_tb");
    //Label^ FromToolOffset_lbl = (Label^)MainFrm->Controls["FromToolOffset_lbl"];

    //Label^ ToToolOffset_lbl = (Label^)MainFrm->Controls["ToToolOffset_lbl"];
    QLabel* InfoLabel_lbl = MainWnd->findChild<QLabel*>("InfoLabel_lbl");
    QLabel* WorkZeroOffsetNo_lbl = MainWnd->findChild<QLabel*>("WorkZeroOffsetNo_lbl");
    //short FirstWorkZeroOffset = FromWorkZeroOffset_tb->text().toShort();
    //short LastWorkZeroOffset = ToWorkZeroOffset_tb->text().toShort();


    InfoLabel_lbl->show();
    InfoLabel_lbl->setText("WRITING WORK ZERO OFFSET TO CNC");
    InfoLabel_lbl->setStyleSheet("background-color: orange");
    InfoLabel_lbl->repaint();
    //short WorkZeroOffsets = 0;
    //short ToolOffMemType = 0;
    //String^ sToolOffMemType = "";
    int SelRows = WorkZeroOffsets_tv->selectionModel()->selectedRows().count();
    int SelCells = WorkZeroOffsets_tv->selectionModel()->selectedIndexes().count();
    int CurrRowindex = WorkZeroOffsets_tv->selectionModel()->selectedIndexes().at(0).row();
    if (SelRows == 0 && SelCells > 0) WorkZeroOffsets_tv->selectRow(CurrRowindex);
    //if (SelRows != 1) { MessageBox::Show("Select only one WorkZeroOffset"); return 1; }

    short WorkZeroOffsetNo = WorkZeroOffsets_tv->model()->index(CurrRowindex, 0).data().toInt();
    IODBZOFS WorkZeroOffset;
    IODBPSD Param;
    //double ToolOffsetUnit = 0;
    short DecimalsNum = 0;
    ret = cnc_rdparam(FHndl, 1013, ALL_AXES, 4 + MAX_AXIS, &Param); if (ret != EW_OK) { QMessageBox::warning(NULL, NULL, "Error returned by cnc_rdparam (Param 1013): " + QString::number(ret)); return ret; }
    switch (Param.u.cdata)
    {
    case 0:
        DecimalsNum = 3;
        break;
    case 1:
        DecimalsNum = 2;
        break;
    case 2:
        DecimalsNum = 4;
        break;
    case 4:
        DecimalsNum = 5;
        break;
    case 8:
        DecimalsNum = 6;
        break;
    }
    if ((WorkZeroOffsetX_tb->text().length() - WorkZeroOffsetX_tb->text().indexOf('.') - 1) != DecimalsNum) { QMessageBox::warning(NULL, NULL, "ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    if ((WorkZeroOffsetY_tb->text().length() - WorkZeroOffsetY_tb->text().indexOf('.') - 1) != DecimalsNum) { QMessageBox::warning(NULL, NULL, "ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    if ((WorkZeroOffsetZ_tb->text().length() - WorkZeroOffsetZ_tb->text().indexOf('.') - 1) != DecimalsNum) { QMessageBox::warning(NULL, NULL, "ENTER " + QString::number(DecimalsNum) + " DECIMALS"); ret = 1; goto finish; }
    WorkZeroOffset.datano = WorkZeroOffsetNo;
    WorkZeroOffset.type = -1;
    WorkZeroOffset.data[0] = (long)(WorkZeroOffsetX_tb->text().toDouble() * pow(10, DecimalsNum));
    WorkZeroOffset.data[1] = (long)(WorkZeroOffsetY_tb->text().toDouble() * pow(10, DecimalsNum));
    WorkZeroOffset.data[2] = (long)(WorkZeroOffsetZ_tb->text().toDouble() * pow(10, DecimalsNum));
    ret = cnc_wrzofs(FHndl, 4 + 4 * MAX_AXIS, &WorkZeroOffset); if (ret != EW_OK) goto finish; // Write WorkZero Offset value
    InfoLabel_lbl->setStyleSheet("background-color: transparent");
    InfoLabel_lbl->hide();
    QMessageBox::warning(NULL, NULL, "UPDATED WorkZero OFFSET IN CNC");
finish:
    if (ret != EW_OK)
    {
        InfoLabel_lbl->setStyleSheet("background-color: transparent");
        InfoLabel_lbl->hide();
        QMessageBox::warning(NULL, NULL, "ERROR RETURNED BY cnc_wrzofs: " + QString::number(ret));

        InfoLabel_lbl->setStyleSheet("background-color: transparent");
        InfoLabel_lbl->hide();

        return ret;

    }
}
short FanucNC::FocasDownloadNCFile(FanucMTMainWnd* MainWnd, QTextStream* NCFileStreamReader)
{
    short ret = 0;
    bool EndOfProgram = false;
    //bool CorrectZSurfacePoints = false;
    QCheckBox* ZSurfInspectFile_chkbox = MainWnd->findChild<QCheckBox*>("ZSurfInspectFile_chkbox");
    bool CorrectZSurfacePoints = ZSurfInspectFile_chkbox->isChecked();
    QString NCFileLine;
    QString ZSurfInspectFileLine;
    int ZIndex = 0;
    int EndOfZIndex = 0;
    float ZDeviatCoord = 0;
    float FlatZ = 0;
    int FlatZIndex = 0;
    float CorrectedZ = 0;
    int CorrectedZIndex = 0;
    bool IsPointToCorrect = false;
    int InspectedPointNumb = 0;
    QList<float> PointsZLevel;
    if (CorrectZSurfacePoints)
    {
        
    }
        long NCLineLength = 1;
        ret = cnc_dwnstart3(FHndl, 0);
        ret = cnc_download3(FHndl, &NCLineLength, (char*)"\n;");
        
        while (!NCFileStreamReader->atEnd())
        {
            NCFileStreamReader->readLineInto(&NCFileLine);
            if (NCFileLine.indexOf("M30") != -1 || NCFileLine.indexOf("M02") != -1) EndOfProgram = true;
            if (NCFileLine.indexOf("%") != -1 && EndOfProgram == false) continue;
            if (!(NCFileLine.indexOf("%") != -1 && EndOfProgram == true)) NCFileLine += "\n";
            if (CorrectZSurfacePoints)
            {
                
            }
            NCLineLength = NCFileLine.length();
            ret = cnc_download3(FHndl, &NCLineLength, (char*)NCFileLine.toStdString().c_str()); if (ret != EW_OK) { QErrorMessage msg(NULL); msg.showMessage(QString("Error returned by cnc_download3 : ") + QString::number(ret));return ret;}
        }
        
        ret = cnc_dwnend3(FHndl); if (ret != EW_OK) { QErrorMessage msg(NULL); msg.showMessage(QString("Error returned by cnc_dwnend3 : ") + QString::number(ret)); return ret; }
        QMessageBox::warning(NULL, NULL, "NC FILE REGISTERED IN CNC");
        return ret;
    }
short FanucNC::FocasReadMachinePos(FanucMTMainWnd* MainWnd)
{//DBMVINF MacroInfo;
 //long ExecProgBlockNum = 0;
 short LocalVars = 0;
 short EnumCommonVars = 0;
 short ControlledAxes = MAX_AXIS;
 QLabel* CurrentTime_lbl = MainWnd->findChild<QLabel*>("CurrentTime_lbl");
 QLabel* EllapsedTime_lbl = MainWnd->findChild<QLabel*>("EllapsedTime_lbl");
 QLabel* ExecProgName_lbl = MainWnd->findChild<QLabel*>("ExecProgName_lbl");
 QLabel* GModalData_lbl = MainWnd->findChild<QLabel*>("GModalData_lbl");
 QLabel* NoGModalData_lbl = MainWnd->findChild<QLabel*>("NoGModalData_lbl");
 QLabel* InfoLabel_lbl= MainWnd->findChild<QLabel*>("InfoLabel_lbl");
 
//ODBEXEPRG ExecutingProgram;
 ret = cnc_rdmacroinfo(FHndl, &MacroInfo); if (ret != EW_OK) { QMessageBox::warning(MainWnd, "ERROR", "Error returned by rd_macroinfo : " + QString::number(ret)); return ret; }
 LocalVars = MacroInfo.use_no1;
 unsigned long LocalVarsReaded = LocalVars;
 unsigned long CommonVarsReaded = 900;
 unsigned long CommonVars2Readed = 500;
 EnumCommonVars = MacroInfo.use_no2;
 macroVarsModel = new MacroVarsModel();
 InfoLabel_lbl->show();
 InfoLabel_lbl->setText("READING MACRO VARIABLES");
 InfoLabel_lbl->setStyleSheet("background-color: orange;");
 InfoLabel_lbl->repaint();
 for (int i = 1; i <= LocalVars; i++)
 {
     ret = cnc_rdmacro(FHndl, i, 10, &MacroVar); if (ret != EW_OK) { QMessageBox::warning(MainWnd, "ERROR READING MACROS", "Error returned by cnc_rdmacro : " + QString::number(ret)); return ret; }
     macroVarsModel->AddMacroVar(i, MacroVar.mcr_val * pow(10, -MacroVar.dec_val));
 }
 for (int i = 100; i <= 999; i++)
 {
     ret = cnc_rdmacro(FHndl, i, 10, &MacroVar); if (ret != EW_OK) { QMessageBox::warning(MainWnd, "ERROR READING MACROS", "Error returned by cnc_rdmacro : " + QString::number(ret)); return ret; }
     macroVarsModel->AddMacroVar(i, MacroVar.mcr_val * pow(10, -MacroVar.dec_val));
 }
 InfoLabel_lbl->setText("");
 InfoLabel_lbl->hide();
 macroVarsAssignedProxyMdl = new MacroVarsAssignedProxyMdl();
 macroVarsAssignedProxyMdl->setSourceModel(macroVarsModel);
 macroVarsAssignedProxyMdl->setFilterKeyColumn(1);
 //macroVarsFilterProxyModel->setFilterRegularExpression(QString("^%1$").arg(0));
  MainWnd->MacroVars_tv->setModel(macroVarsAssignedProxyMdl);
ret = cnc_exeprgname2(FHndl, ExecProgName); if (ret != EW_OK) {QMessageBox::warning(MainWnd,"ERROR","Error returned by cnc_exeprgname2 : " + QString::number(ret)); return ret; }
bool IsEndOfProgram = false;
ret = cnc_pdf_rdactpt(FHndl, ExecProgName, &ExecProgBlockNum); if (ret != EW_OK) {QMessageBox::warning(MainWnd,"ERROR","Error returned by cnc_pdf_rdactpt: " + QString::number(ret)); return ret; }
FirstProgBlockNum = ExecProgBlockNum;
ProgLength = MaxProgLength;
csProgSliceBlocks = new char[ProgLength];
progBlocksModel = new ProgBlocksModel();
if (MaxProgBlocksToRead < 1) MaxProgBlocksToRead = 1;
if (MaxProgBlocksToRead > 120) MaxProgBlocksToRead = 120;
ProgBlocksReaded = MaxProgBlocksToRead;
ret = cnc_rdpdf_line(FHndl, ExecProgName, FirstProgBlockNum, csProgSliceBlocks, &ProgBlocksReaded, &ProgLength); if (ret != EW_OK) { QMessageBox::warning(MainWnd, "ERROR", "Error returned by cnc_rpdf_line: " + QString::number(ret)); return ret; }
ProgSlice = QString(csProgSliceBlocks);
ProgSliceBlocks = ProgSlice.split('\n');
ProgSliceBlocks.removeLast();
//if (MainWnd->ExecProg_tv->model()) ((ProgBlocksModel*)MainWnd->ExecProg_tv->model())->m_ProgBlocks.clear();
if (MainWnd->ExecProg_tv->model()) progBlocksModel->m_ProgBlocks.clear();
for (int i=0;i<ProgSliceBlocks.length();i++)
{ProgBlock = ProgSliceBlocks[i];
 MacroVarNum = 0;
 if (ProgBlock.contains(MacroVarMatch))
 {MacroVarNum = ProgBlock.mid(ProgBlock.indexOf("#") + 1, ProgBlock.indexOf("=") - 1).toInt();
 }
 if (MacroVarNum>0)
 progBlocksModel->AddProgBlock(FirstProgBlockNum + i + 1, ProgSliceBlocks[i],MacroVarNum,true);
 else
 progBlocksModel->AddProgBlock(FirstProgBlockNum + i + 1, ProgSliceBlocks[i], 0,false);
 }
progBlocksMacroVarUnreadedProxyMdl = new ProgBlocksMacroVarUnreadedProxyMdl();
progBlocksMacroVarUnreadedProxyMdl->setSourceModel(progBlocksModel);
progBlocksMacroVarUnreadedProxyMdl->setFilterKeyColumn(3);
ProgBlockNum = FirstProgBlockNum + ProgBlocksReaded;
MainWnd->ExecProg_tv->setModel(progBlocksModel);
MainWnd->ExecProg_tv->setColumnWidth(0, 75); MainWnd->ExecProg_tv->setColumnWidth(1, 500); MainWnd->ExecProg_tv->setColumnWidth(2, 100); MainWnd->ExecProg_tv->setColumnWidth(3, 0);
MainWnd->ExecProg_tv->selectRow(ExecProgBlockNum - FirstProgBlockNum);
//Read modal data   
ret = cnc_modal(FHndl, -1, 0, &GModalData); if (ret != EW_OK) return ret;
MainWnd->GModalData_lbl->setText(GetGModalData(GModalData));
//ret = cnc_modal(FHndl, -3, 0, &NoGAxisModalData);
ret = cnc_modal(FHndl, -2, 0, &NoGModalData); if (ret != EW_OK) return ret;
MainWnd->NoGModalData_lbl->setText(GetNoGModalData(NoGModalData));
//0: 100, .., 149, 500, .., 549
//1: 100, .., 199, 500, .., 999
//2: 100, .., 149, 200, .., 499, 500, .., 549
//3: 100, .., 499, 500, .., 999
//4: 100, .., 199, 500, .., 999, 98000, .., 98499
//5: 100, .., 499, 500, .., 999, 98000, .., 98499
ret = cnc_pdf_rdactpt(FHndl, ExecProgName, &ExecProgBlockNum); if (ret != EW_OK) {QMessageBox::warning(MainWnd,"ERROR","Error returned by cnc_pdf_rdactpt: " + QString::number(ret)); return ret; }
//ODBPOS MachinePos[MAX_AXIS];
ret = cnc_rdposition(FHndl, -1, &ControlledAxes, MachinePos); if (ret != EW_OK) {QMessageBox::warning(MainWnd,"ERROR","Error returned by cnc_rdposition : " + QString::number(ret)); return ret; }
ret = cnc_acts(FHndl, &ActualSpindleSpeed); if (ret != EW_OK) { QMessageBox::warning(MainWnd,"ERROR","Error returned by cnc_acts: " + QString::number(ret)); return ret; }
SpindleSpeed = ActualSpindleSpeed.data;
ret = cnc_actf(FHndl, &ActualFeedRate); if (ret != EW_OK) { QMessageBox::warning(MainWnd,"ERROR","Error returned by cnc_actf: " + QString::number(ret)); return ret; }
FeedRate = ActualFeedRate.data;
//ret = cnc_rdopnlsgnl(FHndl,6, &OutputSignalImage); if (ret != EW_OK) { MessageBox::Show("Error returned by cnc_rdopnlsgnl: " + ret.ToString()); return ret; }
for (int i = 0; i < ControlledAxes; i++)
{   if (AbsMachinePosition.size()<ControlledAxes) AbsMachinePosition.push_back(MachinePos[i].abs.data * pow(10, -MachinePos[i].abs.dec)); else AbsMachinePosition[i] = MachinePos[i].abs.data * pow(10, -MachinePos[i].abs.dec);
    if (MachinePosition.size()<ControlledAxes) MachinePosition.push_back(MachinePos[i].mach.data * pow(10, -MachinePos[i].mach.dec)); else  MachinePosition[i] = MachinePos[i].mach.data * pow(10, -MachinePos[i].mach.dec);
    if (RelMachinePosition.size()<ControlledAxes) RelMachinePosition.push_back(MachinePos[i].rel.data * pow(10, -MachinePos[i].rel.dec)); else RelMachinePosition[i] = MachinePos[i].rel.data * pow(10, -MachinePos[i].rel.dec);
    if (DTGMachinePosition.size()<ControlledAxes) DTGMachinePosition.push_back(MachinePos[i].dist.data * pow(10, -MachinePos[i].dist.dec)); else DTGMachinePosition[i] = MachinePos[i].dist.data * pow(10, -MachinePos[i].dist.dec);
}
QStringList MachinePos_tw_headers;
QStringList headers;
MachinePos_tw_headers << "Machine Axis" << "Absolute" << "Machine"<<"Relative"<<"DTG";
MainWnd->MachinePos_tw->setHorizontalHeaderLabels(MachinePos_tw_headers);
for (int i = 0; i < ControlledAxes; i++)
{if (!MainWnd->MachinePos_tw->item(i, 0)) 
  {QTableWidgetItem* MachineAxisItem = new QTableWidgetItem();
   MainWnd->MachinePos_tw->setItem(i, 0, MachineAxisItem);
  }
if (MainWnd->MachinePos_tw->item(i,1)==nullptr)
{QTableWidgetItem* AbsMachinePosItem = new QTableWidgetItem();
 MainWnd->MachinePos_tw->setItem(i, 1, AbsMachinePosItem);
}
if (MainWnd->MachinePos_tw->item(i, 2)==nullptr)
{
 QTableWidgetItem* MachinePosItem = new QTableWidgetItem();
 MainWnd->MachinePos_tw->setItem(i, 2, MachinePosItem);
}
if (MainWnd->MachinePos_tw->item(i, 3)==nullptr)
{
 QTableWidgetItem* RelMachinePosItem = new QTableWidgetItem();
MainWnd->MachinePos_tw->setItem(i, 3, RelMachinePosItem);
}
if (MainWnd->MachinePos_tw->item(i, 4)==nullptr)
{
 QTableWidgetItem* DTGMachinePosItem = new QTableWidgetItem();
 MainWnd->MachinePos_tw->setItem(i, 4, DTGMachinePosItem);
}
 if (MachinePos[i].dist.data >= 0) {MainWnd->MachinePos_tw->item(i,0)->setText("+M" + QString(MachinePos[i].dist.name));}
 else {MainWnd->MachinePos_tw->item(i,0)->setText("-M" + QString(MachinePos[i].dist.name));}
 MainWnd->MachinePos_tw->item(i,1)->setData(Qt::DisplayRole, AbsMachinePosition[i]);
 MainWnd->MachinePos_tw->item(i,2)->setData(Qt::DisplayRole, MachinePosition[i]);
 MainWnd->MachinePos_tw->item(i,3)->setData(Qt::DisplayRole, RelMachinePosition[i]);
 MainWnd->MachinePos_tw->item(i,4)->setData(Qt::DisplayRole, DTGMachinePosition[i]);
}
MainWnd->SpindleSpeed_lbl->setText("S " + QString::number(SpindleSpeed) + " rpm");
MainWnd->FeedRate_lbl->setText("F " + QString::number(FeedRate) + " mm/min");
ExecProgName_lbl->setText(QString(ExecProgName));

return ret;
}


short FanucNC::FocasMonitorMachinePos(FanucMTMainWnd* MainWnd)
{ MainWnd->ui->InfoLabel_lbl->setText(QString::number(MainWnd->ActPtsReaded)); //MainWnd->ui->InfoLabel_lbl->repaint();
  ret = cnc_statinfo2(FHndl, &MachineStatusInfo); if (ret != EW_OK)  return ret;
  MainWnd->CncStatusInfo_lbl->setText(GetCNCStatusInfo(MachineStatusInfo));
  MachineRunMode = MachineStatusInfo.run;
  if (MachineRunMode != 2 && MachineRunMode != 3)
  {//MainWnd->MachinePos_timer->disconnect();
   //MainWnd->MonitorMachinePos_btn->setStyleSheet("background-color: transparent;");
   
   //MainWnd->MachinePos_timer->stop();
   //return ret;
  }
    if (MachineRunMode == 2 || MachineRunMode == 3)
    {//connect(MainWnd->MachinePos_timer, &QTimer::timeout, MainWnd, &FanucMTMainWnd::OnMachinePos_timer_timeout);
     //MainWnd->MonitorMachinePos_btn->setStyleSheet("background-color: rgb(0,150,0);");
     //MainWnd->MachinePos_timer->start();
    }
    MachineIsMoving = MachineStatusInfo.motion;
    PreviousBlockNumb = ExecProgBlockNum;
    ret = cnc_pdf_rdactpt(FHndl, ExecProgName, &ExecProgBlockNum); if (ret != EW_OK) {return ret; }
    if (ExecProgBlockNum > PreviousBlockNumb)
    {for (int i = 0; i < progBlocksMacroVarUnreadedProxyMdl->rowCount(); i++)
      {ProgBlockIndex = progBlocksMacroVarUnreadedProxyMdl->index(i, 0);
       MacroVarIndex= progBlocksMacroVarUnreadedProxyMdl->index(i, 2);
       if (ExecProgBlockNum > ProgBlockIndex.data().toInt())
       {MacroVarUnreadedIndex= progBlocksMacroVarUnreadedProxyMdl->index(i, 3);
        MacroVarNum = MacroVarIndex.data().toInt();
        //MacroVarNum = ProgBlocksMacroVarUnreadedProxyMdl->data(MacroVarIndex, 0).toInt();
        ret=cnc_rdmacro(FHndl, MacroVarNum, 10, &MacroVar); if (ret != EW_OK) { QMessageBox::warning(MainWnd, "ERROR READING MACROS", "Error returned by cnc_rdmacro3 : " + QString::number(ret)); return ret; }
        macroVarsModel->UpdateMacroVar(MacroVarNum, MacroVar.mcr_val * pow(10, -MacroVar.dec_val));
        progBlocksMacroVarUnreadedProxyMdl->data(MacroVarUnreadedIndex, 10).setValue(false);
      }
     }
     if (ExecProgBlockNum >= ProgBlockNum)
    {ProgLength = MaxProgLength;
     ProgBlocksReaded = MaxProgBlocksToRead;
     ret = cnc_rdpdf_line(FHndl, ExecProgName, ProgBlockNum, csProgSliceBlocks, &ProgBlocksReaded, &ProgLength); if (ret != EW_OK) return ret;
      //ProgSlice.clear();
      ProgSlice = QString(csProgSliceBlocks);
      ProgSliceBlocks = ProgSlice.split('\n');
      ProgSliceBlocks.removeLast();
      for (int i = 0; i < ProgBlocksReaded; i++)
       {ProgBlock = ProgSliceBlocks[i];
       //ProgBlock.size();
       
       
       MacroVarNum = 0;
       if (ProgBlock.contains(MacroVarMatch))
       {MacroVarNum = ProgBlock.mid(ProgBlock.indexOf("#") + 1, ProgBlock.indexOf("=") - 1).toInt();
        
       
       }
       if (MacroVarNum>0)
       progBlocksModel->AddProgBlock(ProgBlockNum + i + 1, ProgSliceBlocks[i],MacroVarNum,1);
       else
       progBlocksModel->AddProgBlock(ProgBlockNum + i + 1, ProgSliceBlocks[i], 0, 0);
       //if ((ProgBlock->IndexOf("%") != -1) && ProgBlockNumb > 0) IsEndOfProgram = true;
       }
      ProgBlockNum = ProgBlockNum + ProgBlocksReaded;
      //MainWnd->InfoLabel_lbl->setText(QString("Prog Blocks Readed in Advance: ") + QString::number(ProgBlocksReaded) + QString("Next ProgBlock Num: ") + QString::number(ProgBlockNum));
      //MainWnd->InfoLabel_lbl->repaint();
      //ProgBlockIndex = MainWnd->ExecProg_tv->model()->index(ExecProgBlockNum - FirstProgBlockNum, 0);
      
      }

        MainWnd->ExecProg_tv->selectRow(ExecProgBlockNum-FirstProgBlockNum);
        //Read modal data
        ret = cnc_modal(FHndl, -1, 0, &GModalData); if (ret != EW_OK) return ret;
        MainWnd->GModalData_lbl->setText(GetGModalData(GModalData));
        //ret = cnc_modal(FHndl, -3, 0, &NoGAxisModalData);
        ret = cnc_modal(FHndl, -2, 0, &NoGModalData); if (ret != EW_OK) return ret;
        MainWnd->NoGModalData_lbl->setText(GetNoGModalData(NoGModalData));
        
        

    }
    if (MachineIsMoving == 1)
    {   ret = cnc_rdposition(FHndl, -1, &MachineAxes, MachinePos); 
        if (ret != EW_OK) { return ret; }
        ret = cnc_acts(FHndl, &ActualSpindleSpeed); if (ret != EW_OK) { return ret; }
        SpindleSpeed = ActualSpindleSpeed.data;
        ret = cnc_actf(FHndl, &ActualFeedRate); if (ret != EW_OK) { return ret; }
        FeedRate = ActualFeedRate.data;
        //IODBSGNL OutputSignalImage;
        //ret = cnc_rdopnlsgnl(FHndl, 6, &OutputSignalImage);  if (ret != EW_OK) { stopwatch->Stop(); MessageBox::Show("Error returned by cnc_rdopnlsgnl: " + ret.ToString()); stopwatch->Stop();  return ret; }//Spindle Speed Override (Not available in Fanuc 30i-31i series)
        
        for (int i = 0; i < MachineAxes; i++)
        {AbsMachinePosition[i] = MachinePos[i].abs.data * pow(10, -MachinePos[i].abs.dec);
         MachinePosition[i] = MachinePos[i].mach.data * pow(10, -MachinePos[i].mach.dec);
         RelMachinePosition[i] = MachinePos[i].rel.data * pow(10, -MachinePos[i].rel.dec);
         DTGMachinePosition[i] = MachinePos[i].dist.data * pow(10, -MachinePos[i].dist.dec);
         if (MachinePos[i].dist.data >= 0) {MainWnd->MachinePos_tw->item(i, 0)->setText("+M" + QString(MachinePos[i].dist.name));}
         else {MainWnd->MachinePos_tw->item(i, 0)->setText("-M" + QString(MachinePos[i].dist.name));}
         MainWnd->MachinePos_tw->item(i, 1)->setData(Qt::DisplayRole, AbsMachinePosition[i]);
         MainWnd->MachinePos_tw->item(i, 2)->setData(Qt::DisplayRole, MachinePosition[i]);
         MainWnd->MachinePos_tw->item(i, 3)->setData(Qt::DisplayRole, RelMachinePosition[i]);
         MainWnd->MachinePos_tw->item(i, 4)->setData(Qt::DisplayRole, DTGMachinePosition[i]);
        }
        
        

        //MainFrm->Controls["XMachinePos_lbl"]->ForeColor = Color::Green;
        //MainFrm->Controls["YMachinePos_lbl"]->ForeColor = Color::Green;
        //MainFrm->Controls["ZMachinePos_lbl"]->ForeColor = Color::Green;
        
        


        MainWnd->SpindleSpeed_lbl->setText("S " + QString::number(SpindleSpeed) + " rpm");
        MainWnd->FeedRate_lbl->setText("F " + QString::number(FeedRate) + " mm/min");
        
        //MainWnd->EllapsedTime_lbl->setText(((float)stopwatch->ElapsedMilliseconds / 1000).ToString());


        

    }
    else
    {
        //MainFrm->Controls["XMachinePos_lbl"]->ForeColor = Color::Black;
        //MainFrm->Controls["YMachinePos_lbl"]->ForeColor = Color::Black;
        //MainFrm->Controls["ZMachinePos_lbl"]->ForeColor = Color::Black;

    }
 
 return ret;
 
}
QString FanucNC::GetCNCStatusInfo(ODBST2 CncStatusInfo)
{
    CNCStatInfo.clear();
    CNCStatInfo += "AUT: ";
    switch (CncStatusInfo.aut)
    {case 0:
        //CNCStatInfo += "MDI ";
        CNCStatInfo+= "<font color=orange>MDI </font>";
        break;
    case 1:
        CNCStatInfo += "<font color=green>MEMory </font>";
        break;
    case 2:
        //CNCStatInfo += "**** ";
        CNCStatInfo+= "<font color=orange>****  </font>";
        break;
    case 3:
        //CNCStatInfo += "EDIT ";
        CNCStatInfo+= "<font color=orange>EDIT </font>";
        break;
    case 4:
        CNCStatInfo += "HaNDle ";
        break;
    case 5:
        CNCStatInfo += "JOG ";
        break;
    case 6:
        CNCStatInfo += "Teach in JOG ";
        break;
    case 7:
        CNCStatInfo += "Teach in HaNDle ";
        break;
    case 8:
        CNCStatInfo += "INCfeed ";
        break;
    case 9:
        CNCStatInfo += "REFerence ";
        break;
    case 10:
        CNCStatInfo += "ReMoTe ";
        break;
    case 11:
        CNCStatInfo += "TEST ";
        break;
    default:
        break;
    }

    CNCStatInfo += "     RUN: ";
    switch (CncStatusInfo.run)
    {case 0:
        //CNCStatInfo += "****(reset) ";
        CNCStatInfo+= "<font color=orange>****(reset) </font>";
        break;
    case 1:
        //CNCStatInfo += "STOP ";
        CNCStatInfo+= "<font color=red>STOP </font>";
        break;
    case 2:
        //CNCStatInfo += "HOLD ";
        CNCStatInfo+= "<font color=orange>HOLD </font>";
        break;
    case 3:
        //CNCStatInfo += "STaRT ";
        CNCStatInfo+= "<font color=green>STaRT </font>";
        break;
    case 4:
        CNCStatInfo += "MSTR(during retraction and re-positioning of tool retraction and recovery, and operation of JOG MDI) ";
        break;

    default:
        break;
    }
    CNCStatInfo += "      MOTION: ";
    switch (CncStatusInfo.motion)
    {case 0:
        //CNCStatInfo += "*** ";
        CNCStatInfo+= "<font color=orange>*** </font>";
        break;
    case 1:
        //CNCStatInfo += "MoTioN ";
        CNCStatInfo+= "<font color=green>MoTioN </font>";
        break;
    case 2:
        //CNCStatInfo += "DWeLl ";
        CNCStatInfo+= "<font color=orange>DWeLL </font>";
        break;
    default:
        break;
    }
    CNCStatInfo += "      EMERGENCY: ";
    switch (CncStatusInfo.emergency)
    {case 0:
        CNCStatInfo += " ";
        break;
    case 1:
        //CNCStatInfo += "EMerGency ";
        CNCStatInfo+= "<font color=red>EMerGency </font>";
        break;
    case 2:
        //CNCStatInfo += "ReSET ";
        CNCStatInfo+= "<font color=orange>ReSET </font>";
        break;
    case 3:
        //CNCStatInfo += "WAIT ";
        CNCStatInfo+= "<font color=orange>WAIT </font>";
        break;
    default:
        break;
    }
    CNCStatInfo += "      ALARM: ";
    switch (CncStatusInfo.alarm)
    {case 0:
        //CNCStatInfo += "***(Others) ";
        CNCStatInfo+= "<font color=orange>***(Others) </font>";
        break;
    case 1:
        //CNCStatInfo += "ALarM ";
        CNCStatInfo += "<font color=red>ALarM </font>";
        break;
    case 2:
        CNCStatInfo += "BATtery low ";
        break;
    case 3:
        CNCStatInfo += "FAN(NC or Servo amplifier) ";
        break;
    case 4:
        CNCStatInfo += "PS Warning ";
        break;
    case 5:
        CNCStatInfo += "FSsB warning ";
        break;
    case 6:
        CNCStatInfo += "INSulate warning ";
        break;
    case 7:
        CNCStatInfo += "ENCoder warning ";
        break;
    case 8:
        CNCStatInfo += "<font color=red> PMC alarm </font>";
        break;
    default:
        break;
    }
    CNCStatInfo += "      EDIT: ";
    switch (CncStatusInfo.edit)
    {case 0:
        CNCStatInfo += "****(Not editing) ";
        break;
    case 1:
        CNCStatInfo += "EDIT(during editing) ";
        break;
    case 2:
        CNCStatInfo += "SeaRCH(during searching) ";
        break;
    case 3:
        CNCStatInfo += "OUTPUT(during output) ";
        break;
    case 4:
        CNCStatInfo += "INPUT(during input) ";
        break;
    case 5:
        CNCStatInfo += "COMPARE((during comparing) ";
        break;
    case 6:
        CNCStatInfo += "Label SKip(label skip status) ";
        break;
    case 7:
        CNCStatInfo += "ReSTaRt(during program restart) ";
        break;
    case 8:
        CNCStatInfo += "HPCC(during RISC operation) ";
        break;
    case 9:
        CNCStatInfo += "PTRR(during tool retraction and recovery mode) ";
        break;
    case 10:
        CNCStatInfo += "RVRS(during retracing) ";
        break;
    case 11:
        CNCStatInfo += "RTRY(during reprogressing) ";
        break;
    case 12:
        CNCStatInfo += "RVED(end of retracing) ";
        break;
    case 13:
        CNCStatInfo += "HANDLE(during handle overlapping) ";
        break;
    case 14:
        CNCStatInfo += "OFfSeT(during tool length measurement mode) ";
        break;
    case 15:
        CNCStatInfo += "Work OFfSet(during work zero point measurement mode) ";
        break;
    case 16:
        CNCStatInfo += "AICC(during AI coutour control) ";
        break;
    case 17:
        CNCStatInfo += "MEmory-CHecK(checking tape memory) ";
        break;
    case 18:
        CNCStatInfo += "CusToMer's BoarD(during customer's board control) ";
        break;
    case 19:
        CNCStatInfo += "SAVE(saving fine torque sensing data) ";
        break;
    case 20:
        CNCStatInfo += "AI NANO(during AI nano contour control) ";
        break;
    case 21:
        CNCStatInfo += "AI APC(during AI advanced preview control) ";
        break;
    case 22:
        CNCStatInfo += "MBL APC(during multi blocks advanced preview control) ";
        break;
    case 23:
        CNCStatInfo += "NANO HP(Running of AI High-precision Contour Control) ";
        break;
    case 24:
        CNCStatInfo += "AI HPCC(Running of AI Nano High-precision Contour Control) ";
        break;
    case 25:
        CNCStatInfo += "5-AXIS(Running of 5-axes machining) ";
        break;
    case 26:
        CNCStatInfo += "LEN(change the manual active offset value:length offset change mode) ";
        break;
    case 27:
        CNCStatInfo += "RAD(change the manual active offset value:radius offset change mode) ";
        break;
    case 28:
        CNCStatInfo += "WZR(change the manual active offset value:workpiece origin offset change mode) ";
        break;
    case 39:
        CNCStatInfo += "TCP(during tool center point control of 5-axes machining) ";
        break;
    case 40:
        CNCStatInfo += "TWP(during tilted working plane command) ";
        break;
    case 41:
        CNCStatInfo += "TCP+TWP(during tool center point control of 5-axes machining and tilted working plane command) ";
        break;
    case 42:
        CNCStatInfo += "APC(Advanced Preview Control)(16i/18i/21i,0i-C) ";
        break;
    case 43:
        CNCStatInfo += "PRG-CHK(High speed program check) ";
        break;
    case 44:
        CNCStatInfo += "APC(Advanced Preview Control)(0i-D:No.13104#0=0) ";
        break;
    case 45:
        CNCStatInfo += "S-TCP(during smooth TCP) ";
        break;
    case 59:
        CNCStatInfo += "ALLSAVE(High speed program management:the programs saving in progress) ";
        break;
    case 60:
        CNCStatInfo += "NOTSAVE(High speed program management:by the programs not saved status) ";
        break;

    default:
        break;
    }
    CNCStatInfo += "      WARNING: ";
    switch (CncStatusInfo.alarm)
    {

    case 0:
        CNCStatInfo += " ";
        break;
    case 1:
        CNCStatInfo += "WaRNing(Start from middle of program) ";
        break;

    default:
        break;
    }
    return CNCStatInfo;

}
QString FanucNC::GetGModalData(ODBMDL GModalData)
{GModal = 0;
sGModal.clear();
sGModal += "<font color=gray>MODAL(G): </font>";
    for (int i = 0; i <= 34; i++)
    {  if (GetBit((short)GModalData.modal.g_rdata[i], 7) == 1) GModal = abs(GModalData.modal.g_rdata[i]) - 128;
        else GModal = (short)GModalData.modal.g_rdata[i];
        if (i == 0)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G00 ";

                break;
            case 1:
                sGModal += "G01 ";

                break;
            case 2:
                sGModal += "G02 ";

                break;
            case 3:
                sGModal += "G03 ";

                break;
            case 4:
                sGModal += "G33 ";

                break;
            case 5:
                sGModal += "G75 ";

                break;
            case 6:
                sGModal += "G77 ";

                break;
            case 7:
                sGModal += "G78 ";

                break;
            case 8:
                sGModal += "G79 ";

                break;
            case 10:
                sGModal += "G02.2 ";

                break;
            case 11:
                sGModal += "G03.2 ";

                break;
            case 12:
                sGModal += "G02.3 ";

                break;
            case 13:
                sGModal += "G03.3 ";

                break;
            case 14:
                sGModal += "G06.2 ";

                break;
            case 15:
                sGModal += "G02.4 ";

                break;
            case 16:
                sGModal += "G03.4 ";

                break;
            case 22:
                sGModal += "G35 ";

                break;
            case 23:
                sGModal += "G36 ";

                break;
            case 24:
                sGModal += "G34 ";

                break;
            default:
                break;
            }
        }
        if (i == 1)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G17 ";

                break;
            case 4:
                sGModal += "G19 ";

                break;
            case 8:
                sGModal += "G18 ";

                break;
            case 10:
                sGModal += "G17.1 ";

                break;

            default:
                break;
            }
        }
        if (i == 2)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G90 ";

                break;
            case 1:
                sGModal += "G91 ";

                break;

            default:
                break;
            }
        }
        if (i == 3)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G23 ";

                break;
            case 1:
                sGModal += "G22 ";

                break;

            default:
                break;
            }
        }
        if (i == 4)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G94 ";

                break;
            case 1:
                sGModal += "G95 ";

                break;
            case 2:
                sGModal += "G93 ";

                break;
            case 3:
                sGModal += "G93.2 ";

                break;
            default:
                break;
            }
        }
        if (i == 5)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G20 ";

                break;
            case 1:
                sGModal += "G21 ";

                break;

            default:
                break;
            }
        }
        if (i == 6)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G40 ";

                break;
            case 1:
                sGModal += "G41 ";

                break;
            case 2:
                sGModal += "G42 ";

                break;
            case 3:
                sGModal += "G41.2 ";

                break;
            case 4:
                sGModal += "G42.2 ";

                break;
            case 5:
                sGModal += "G41.3 ";

                break;
            case 6:
                sGModal += "G41.4 ";

                break;
            case 7:
                sGModal += "G42.4 ";

                break;
            case 8:
                sGModal += "G41.5 ";

                break;
            case 9:
                sGModal += "G42.5 ";

                break;
            case 10:
                sGModal += "G41.6 ";

                break;
            case 11:
                sGModal += "G42.6 ";

                break;

            default:
                break;
            }
        }
        if (i == 7)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G49(G49.1) ";

                break;
            case 1:
                sGModal += "G43 ";

                break;
            case 2:
                sGModal += "G44 ";

                break;
            case 3:
                sGModal += "G43.1 ";

                break;
            case 4:
                sGModal += "G43.4 ";

                break;
            case 5:
                sGModal += "G43.5 ";

                break;
            case 6:
                sGModal += "G43.2 ";

                break;
            case 7:
                sGModal += "G43.3 ";

                break;
            default:
                break;
            }
        }
        if (i == 8)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G80 ";

                break;
            case 1:
                sGModal += "G81 ";

                break;
            case 2:
                sGModal += "G82 ";

                break;
            case 3:
                sGModal += "G83 ";

                break;
            case 4:
                sGModal += "G84 ";

                break;
            case 5:
                sGModal += "G85 ";

                break;
            case 6:
                sGModal += "G86 ";

                break;
            case 7:
                sGModal += "G87 ";

                break;
            case 8:
                sGModal += "G88 ";

                break;
            case 9:
                sGModal += "G89 ";

                break;
            case 10:
                sGModal += "G73 ";

                break;
            case 11:
                sGModal += "G74 ";

                break;
            case 12:
                sGModal += "G76 ";

                break;
            case 13:
                sGModal += "G84.2 ";

                break;
            case 14:
                sGModal += "G84.3 ";

                break;
            case 15:
                sGModal += "G81.2 ";

                break;
            default:
                break;
            }
        }
        if (i == 9)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G98 ";

                break;
            case 1:
                sGModal += "G99 ";

                break;

            default:
                break;
            }
        }
        if (i == 10)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G50 ";

                break;
            case 1:
                sGModal += "G51 ";

                break;

            default:
                break;
            }
        }
        if (i == 11)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G67 ";

                break;
            case 1:
                sGModal += "G66 ";

                break;
            case 2:
                sGModal += "G66.1 ";

                break;
            default:
                break;
            }
        }
        if (i == 12)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G97 ";

                break;
            case 1:
                sGModal += "G96 ";

                break;

            default:
                break;
            }
        }
        if (i == 13)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G54(G54.1) ";

                break;
            case 1:
                sGModal += "G55 ";

                break;
            case 2:
                sGModal += "G56 ";

                break;
            case 3:
                sGModal += "G57 ";

                break;
            case 4:
                sGModal += "G58 ";

                break;
            case 5:
                sGModal += "G59 ";

                break;
            default:
                break;
            }
        }
        if (i == 14)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G64 ";

                break;
            case 1:
                sGModal += "G61 ";

                break;
            case 2:
                sGModal += "G62 ";

                break;
            case 3:
                sGModal += "G63 ";

                break;

            default:
                break;
            }
        }
        if (i == 15)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G69 ";

                break;
            case 1:
                sGModal += "G68 ";

                break;
            case 2:
                sGModal += "G68.2 ";

                break;
            case 3:
                sGModal += "G68.3 ";

                break;
            default:
                break;
            }
        }
        if (i == 16)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G15 ";

                break;
            case 1:
                sGModal += "G16 ";

                break;

            default:
                break;
            }
        }
        if (i == 17)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G40.1 ";

                break;
            case 1:
                sGModal += "G41.1 ";

                break;
            case 2:
                sGModal += "G42.1 ";

                break;
            default:
                break;
            }
        }
        if (i == 18)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G25 ";

                break;
            case 1:
                sGModal += "G26 ";

                break;
            default:
                break;
            }
        }
        if (i == 19)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G160 ";

                break;
            case 1:
                sGModal += "G161 ";

                break;
            default:
                break;
            }
        }
        if (i == 20)
        {
            switch (GModal)
            {
            case 0:
                sGModal += "G13.1(G113) ";

                break;
            case 1:
                sGModal += "G12.1(G112) ";

                break;
            default:
                break;
            }
        }
    }
    return sGModal;
}
QString FanucNC::GetNoGModalData(ODBMDL NoGModalData)
{NoGModalType = 0;
 sNoGModal.clear();
 sNoGModal += "<font color=gray>MODAL(NO G): </font>";
    for (int i = 0; i <= 27; i++)
    {
        NoGModal = NoGModalData.modal.raux1[i].aux_data;
        //NoGModalType = (short)NoGModalData.modal.raux1[i].flag1 >> 5;
        //if (GetBit((short)NoGModalData.modal.raux1[i].aux_data, 7) == 1) NoGModal = abs(NoGModalData.modal.raux1[i].aux_data) - 128;
        //else NoGModal = (short)GModalData.modal.raux1[i].aux_data;
        switch (i)
        {
        case 0:
            if (NoGModal > 0) sNoGModal += "B" + QString::number(NoGModal) + " ";
            break;
        case 1:
            if (NoGModal > 0) sNoGModal += "D" + QString::number(NoGModal) + " ";
            break;
        case 3:
            if (NoGModal > 0) sNoGModal += "F" + QString::number(NoGModal) + " ";
            break;
        case 4:
            if (NoGModal > 0) sNoGModal += "H" + QString::number(NoGModal) + " ";
            break;
        case 5:
            if (NoGModal > 0) sNoGModal += "L" + QString::number(NoGModal) + " ";
            break;
        case 6:
            if (NoGModal > 0) sNoGModal += "M" + QString::number(NoGModal) + " ";
            break;
        case 7:
            if (NoGModal > 0) sNoGModal += "S" + QString::number(NoGModal) + " ";
            break;
        case 8:
            if (NoGModal > 0) sNoGModal += "T" + QString::number(NoGModal) + " ";
            break;
        case 9:
            if (NoGModal > 0) sNoGModal += "R" + QString::number(NoGModal) + " ";
            break;
        case 10:
            if (NoGModal > 0)sNoGModal += "P" + QString::number(NoGModal) + " ";
            break;
        case 11:
            if (NoGModal > 0)sNoGModal += "Q" + QString::number(NoGModal) + " ";
            break;
        case 12:
            if (NoGModal > 0) sNoGModal += "A" + QString::number(NoGModal) + " ";
            break;
        case 13:
            if (NoGModal > 0) sNoGModal += "C" + QString::number(NoGModal) + " ";
            break;
        case 14:
            if (NoGModal > 0) sNoGModal += "I" + QString::number(NoGModal) + " ";
            break;
        case 15:
            if (NoGModal > 0) sNoGModal += "J" + QString::number(NoGModal) + " ";
            break;
        case 16:
            if (NoGModal > 0) sNoGModal += "K" + QString::number(NoGModal) + " ";
            break;
        case 17:
            if (NoGModal > 0) sNoGModal += "N" + QString::number(NoGModal) + " ";
            break;
        case 18:
            if (NoGModal > 0) sNoGModal += "O" + QString::number(NoGModal) + " ";
            break;
        case 19:
            if (NoGModal > 0) sNoGModal += "U" + QString::number(NoGModal) + " ";
            break;
        case 20:
            if (NoGModal > 0) sNoGModal += "V" + QString::number(NoGModal) + " ";
            break;
        case 21:
            if (NoGModal > 0) sNoGModal += "W" + QString::number(NoGModal) + " ";
            break;
        case 22:
            if (NoGModal > 0) sNoGModal += "X" + QString::number(NoGModal*0.001) + " ";
            break;
        case 23:
            if (NoGModal > 0) sNoGModal += "Y" + QString::number(NoGModal*0.001) + " ";
            break;
        case 24:
            if (NoGModal > 0) sNoGModal += "Z" + QString::number(NoGModal*0.001) + " ";
            break;
        case 25:
            if (NoGModal > 0) sNoGModal += "2M" + QString::number(NoGModal) + " ";
            break;
        case 26:
            if (NoGModal > 0) sNoGModal += "3M" + QString::number(NoGModal) + " ";
            break;
        default:
            break;
        }

    }
    return sNoGModal;
}

  