#pragma once
#include <Fwlib64.h>
#include "FanucMTMainWnd.h"
#include <QtCore/qsettings.h>
#include "ProgBlocksModel.h"
#include "ParamsModel.h"
#include "ToolOffsetsModel.h"
#include "WorkZeroOffsetsModel.h"
#include "MacroVarsModel.h"
#include <QtCore/qregularexpression.h>

using namespace Fwlib64;

class FanucNC : public QObject
{Q_OBJECT
 public:
 explicit FanucNC(QObject* parent=nullptr);
 ~FanucNC();
 //////////////////
 public:
 unsigned short FHndl;
 bool Connected;
 QString CNCIP;
 unsigned short CNCPort;
 long ConnectTimeOut;
 short MaxParamNo;
 short ret;
 QSettings* FanucMTSettings;
 short MachineAxes;
 short ControlledAxes;
 short MaxAxes;
 ODBPOS MachinePos[MAX_AXIS];
 std::vector<double>MachinePosition;
 std::vector<double>RelMachinePosition;
 std::vector<double>AbsMachinePosition;
 std::vector<double>DTGMachinePosition;
 ODBACT ActualSpindleSpeed;
 ODBACT ActualFeedRate;
 long SpindleSpeed;
 long FeedRate;
 ODBST2 MachineStatusInfo;
 QString  CNCStatInfo;
 short MachineRunMode;
 short MachineIsMoving;
 long PreviousBlockNumb;
 long ExecProgBlockNum;
 long FirstProgBlockNum;
 
 long ProgBlockNum;
 char ExecProgName[256];
 unsigned long ProgBlockLength;
 unsigned long ProgBlocksReaded;
 unsigned long MaxProgBlocksToRead;
 unsigned long ProgLength;
 unsigned long MaxProgLength;
 char* csProgSliceBlocks; //C-string ProgSlice Blocks
 QString ProgBlock;
 QString ProgSlice;
 QStringList ProgSliceBlocks;
 ODBMDL GModalData;
 short GModal;
 QString sGModal;
 ODBMDL NoGModalData;
 short NoGModalType;
 long NoGModal;
 QString sNoGModal;
 ODBMDL NoGAxisModalData;
 QString sNoGAxisModal;
 ParamsModel* paramsModel;
 ToolOffsetsModel* toolOffsetsModel;
 WorkZeroOffsetsModel* workZeroOffsetsModel;
 ProgBlocksModel* progBlocksModel;
 MacroVarsModel* macroVarsModel;
 MacroVarsAssignedProxyMdl* macroVarsAssignedProxyMdl;
 ProgBlocksMacroVarUnreadedProxyMdl* progBlocksMacroVarUnreadedProxyMdl;
 ODBMVINF MacroInfo;
 ODBM MacroVar;
 short MacroVarNum;
 QRegularExpression MacroVarMatch;
 QModelIndex ProgBlockIndex;
 QModelIndex MacroVarIndex;
 QModelIndex MacroVarUnreadedIndex;
 //////////////////////////////
 public:
 short FocasConnect(FanucMTMainWnd* MainWnd);
 short FocasDisconnect(FanucMTMainWnd* MainWnd);
 short FocasReadCNCInfo(FanucMTMainWnd* MainWnd);
 short FocasReadParams(FanucMTMainWnd* MainWnd);
 short FocasReadNumParams(FanucMTMainWnd* MainWnd);
 short FocasWriteParam(FanucMTMainWnd* MainWnd);
 short FocasReadNumToolOffsets(FanucMTMainWnd* MainWnd, short& ToolOffsets);
 short FocasReadNumWorkOffsets(FanucMTMainWnd* MainWnd, short& WorkOffsets);
 short FocasReadToolOffsets(FanucMTMainWnd* MainWnd);
 short FocasWriteToolOffset(FanucMTMainWnd* MainWnd);
 short FocasReadWorkZeroOffsets(FanucMTMainWnd* MainWnd);
 short FocasWriteWorkZeroOffset(FanucMTMainWnd* MainWnd);
 short FocasDownloadNCFile(FanucMTMainWnd* MainWnd,QTextStream* NCFileStreamReader);
 short FocasReadMachinePos(FanucMTMainWnd* MainWnd);
 short FocasMonitorMachinePos(FanucMTMainWnd* MainWnd);
 QString GetCNCStatusInfo(ODBST2 CncStatusInfo);
 QString GetGModalData(ODBMDL GModalData);
 QString GetNoGModalData(ODBMDL NoGModalData);
 
 
 

};
