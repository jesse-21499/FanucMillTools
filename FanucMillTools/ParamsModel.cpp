#include "ParamsModel.h"
#include <QtGui\qcolor.h>
//using namespace std;
//Parameters
ParamsModel::ParamsModel(QObject* parent) : QAbstractTableModel(parent) 
{
    m_row = 0;
}

// Returns the number of rows in the data source
int ParamsModel::rowCount(const QModelIndex& parent) const 
{
    if (parent.isValid()) return 0; // Flat list/table structure
    return static_cast<int>(m_Parameters.size());
}

// Returns the number of fields/columns to display
int ParamsModel::columnCount(const QModelIndex& parent) const 
{
    if (parent.isValid()) return 0;
    return 35;
}

// Provides data for individual cells based on index and display role
QVariant ParamsModel::data(const QModelIndex& index, int role) const 
{   if (!index.isValid()) return QVariant();
    // Out-of-bounds safety check
    if (index.row() >= static_cast<int>(m_Parameters.size()) || index.row() < 0) 
    {
        return QVariant();
    }
    // Handle text rendering requests
    if (role == Qt::BackgroundRole)
    {
        return QVariant(QColor(53, 53, 53));;
    }
    if (role == Qt::ForegroundRole)
    {
        return QVariant(QColor(Qt::green));
    }
    if (role == Qt::DisplayRole)  
    {
        const auto& Parameter = m_Parameters.at(index.row());

        switch (index.column()) 
        {
        case 0: 
            return Parameter->CNCIP;
            break;
        case 1:
            return Parameter->ParamNo;
            break;
        case 2:
            return Parameter->ParamType;
            break;
        case 3:
            return Parameter->strParamType;
            break;
        case 4:
            return Parameter->ParamAxis;
            break;
        case 5:
            return Parameter->s_cdata;
            break;
        case 6:
            return Parameter->s_idata;
            break;
        case 7:
            return Parameter->s_ldata;
            break;
        case 8:
            return Parameter->s_rdata;
            break;
        case 9:
            return Parameter->s_cdata_x;
            break;
        case 10:
            return Parameter->s_cdata_y;
            break;
        case 11:
            return Parameter->s_cdata_z;
            break;
        case 12:
            return Parameter->s_idata_x;
            break;
        case 13:
            return Parameter->s_idata_y;
            break;
        case 14:
            return Parameter->s_idata_z;
            break;
        case 15:
            return Parameter->s_ldata_x;
            break;
        case 16:
            return Parameter->s_ldata_y;
            break;
        case 17:
            return Parameter->s_ldata_z;
            break;
        case 18:
            return Parameter->s_rdata_x;
            break;
        case 19:
            return Parameter->s_rdata_y;
            break;
        case 20:
            return Parameter->s_rdata_z;
            break;
        
        case 28:
            return Parameter->ParamArrangeType;
            break;
        case 29:
            return Parameter->ParamUnitType;
            break;
        case 30:
            return Parameter->ParamUnitDimens;
            break;
        case 31:
            return Parameter->ParamInputType;
            break;
        case 32:
            return Parameter->ParamDisplay;
            break;
        case 33:
            return Parameter->ParamOthers;
            break;
        case 34:
            return Parameter->ActualDate;
            break;
         default: 
             return QVariant();
             break;
        }
    }
    if (role == Qt::CheckStateRole)
    {
        const auto& Parameter = m_Parameters.at(index.row());

        switch (index.column())
        {
case 21:
    return Parameter->IsAxisParam_attr;
    break;
case 22:
    return Parameter->Param_Spindle_attr;
    break;
case 23:
    return Parameter->IsSignedParam_attr;
    break;
case 24:
    return Parameter->Param_SettingsInput_attr;
    break;
case 25:
    return Parameter->Param_WriteProtect_attr;
    break;
case 26:
    return Parameter->Param_PowerMBeOff_attr;
    break;
case 27:
    return Parameter->Param_ReadProtect_attr;
    break;
        }
    }
    return QVariant();
}

// Configures the horizontal table headers

QVariant ParamsModel::headerData(int section, Qt::Orientation orientation, int role) const
{if (role != Qt::DisplayRole && (role!=Qt::CheckStateRole)) return QVariant();
 if (orientation == Qt::Horizontal) 
  {switch (section)
 {
   case 0: return QString("CNC IP");
   case 1: return QString("Param Num");
   case 2: return QString("Param Type");
   case 3: return QString("ParType_Ubyte");
   case 4: return QString("ParAxis_Lbyte");
   case 5: return QString("cdata");
   case 6: return QString("idata");
   case 7: return QString("ldata");
   case 8: return QString("rdata");
   case 9: return QString("cdataX");
   case 10: return QString("cdataY");
   case 11: return QString("cdataZ");
   case 12: return QString("idataX");
   case 13: return QString("idataY");
   case 14: return QString("idataZ");
   case 15: return QString("ldataX");
   case 16: return QString("ldataY");
   case 17: return QString("ldataZ");
   case 18: return QString("rdataX");
   case 19: return QString("rdataY");
   case 20: return QString("rdataZ");
   case 21: return QString("AxisParam");
   case 22: return QString("SpindleParam");
   case 23: return QString("SignedParam");
   case 24: return QString("SettingsInput");
   case 25: return QString("WriteProtect");
   case 26: return QString("PowerMustBeOff");
   case 27: return QString("ReadProtect");
   case 28: return QString("ArrangeType");
   case 29: return QString("ParamUnitType");
   case 30: return QString("ParamUnitDimens");
   case 31: return QString("ParamInputType");
   case 32: return QString("ParamDisplay");
   case 33: return QString("ParamOthers");
   case 34: return QString("ParamReadDate");
   default: return QVariant();
  }
 }
  return QVariant();
}
// Populates data and notifies the view to redraw safely
//void ParamsModel::AddParameter(const std::vector<Contact>& newData) 
 //void ParamsModel::AddParameter(Parameter* &newParam) 
void ParamsModel::AddParameter(QString& CNCIP, short ParamNo, short ParamType, QString& strParamType, QString& ParamAxis, QString& s_cdata, QString& s_idata, QString& s_ldata, QString& s_rdata, QString& s_cdata_x, QString& s_cdata_y, QString& s_cdata_z, QString& s_idata_x, QString& s_idata_y, QString& s_idata_z, QString& s_ldata_x, QString& s_ldata_y, QString& s_ldata_z, QString& s_rdata_x, QString& s_rdata_y, QString& s_rdata_z, bool IsAxisParam_attr, bool Param_Spindle_attr, bool IsSignedParam_attr, bool Param_SettingsInput_attr, bool Param_WriteProtect_attr, bool Param_PowerMBeOff_attr, bool Param_ReadProtect_attr, QString& ParamArrangeType, QString& ParamUnitType, QString& ParamUnitDimens, QString& ParamInputType, QString& ParamDisplay, QString& ParamOthers, QString& ActualDate)
 {beginResetModel();
  //m_Parameters = newData;
  //m_Parameters.insert({ m_row,newParam });
  m_Parameters.insert({ m_row, new Parameter(CNCIP, ParamNo, ParamType, strParamType, ParamAxis, s_cdata, s_idata, s_ldata, s_rdata, s_cdata_x, s_cdata_y, s_cdata_z, s_idata_x, s_idata_y, s_idata_z, s_ldata_x, s_ldata_y, s_ldata_z, s_rdata_x, s_rdata_y, s_rdata_z, IsAxisParam_attr, Param_Spindle_attr, IsSignedParam_attr, Param_SettingsInput_attr, Param_WriteProtect_attr, Param_PowerMBeOff_attr, Param_ReadProtect_attr, ParamArrangeType, ParamUnitType, ParamUnitDimens, ParamInputType, ParamDisplay, ParamOthers, ActualDate) });
  m_row++;
  endResetModel();
 }

