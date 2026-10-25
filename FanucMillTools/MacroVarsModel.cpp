#include "MacroVarsModel.h"
#include <QtGui\qcolor.h>
//using namespace std;
// 

//Macro Variables
MacroVarsModel::MacroVarsModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_row = 0;
}

// Returns the number of rows in the data source
int MacroVarsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0; // Flat list/table structure
    return static_cast<int>(m_MacroVars.size());
}

// Returns the number of fields/columns to display
int MacroVarsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 2;
}

// Provides data for individual cells based on index and display role
QVariant MacroVarsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    
    // Out-of-bounds safety check
    if (index.row() >= static_cast<int>(m_MacroVars.size()) || index.row() < 0)
    {
        return QVariant();
    }
    // Handle text rendering requests
    if (role == Qt::BackgroundRole)
    {
        return QVariant(QColor(53, 53, 53));; //100,100,25
    }
    
    if (role == Qt::ForegroundRole)
    {if (m_MacroVars.at(index.row())->MacroVarValue==0) return QVariant(QColor(Qt::gray));
        else return QVariant(QColor(Qt::green));
    }
    
    if (role == Qt::DisplayRole)
    {  const auto& MacroVar = m_MacroVars.at(index.row());
    
       switch (index.column())
        {
        case 0:
            return QVariant::fromValue(MacroVar->MacroVarNo);
            break;
        case 1:
            return MacroVar->MacroVarValue;
            break;
        
        default:
            return QVariant();
            
            break;
        }
    }
    if (role == Qt::CheckStateRole)
    { const auto& Parameter = m_MacroVars.at(index.row());
      switch (index.column())
        {
      
        }
    }
    return QVariant();
}

// Configures the horizontal table headers

QVariant MacroVarsModel::headerData(int section, Qt::Orientation orientation, int role) const
{   if (role != Qt::DisplayRole && (role != Qt::CheckStateRole)) return QVariant();
    if (orientation == Qt::Horizontal)
    {switch (section)
      {case 0: return QString("Macro Variable");
       case 1: return QString("Value");
       default: return QVariant();
      }
    }
    return QVariant();
}
 // Populates data and notifies the view to redraw safely
 //void MacroVarsModel::AddMacroVar(const std::vector<MacroVar>& newData) 
 //void MacroVarsModel::AddMacroVar(MacroVar* &newMacroVar) 
void MacroVarsModel::AddMacroVar(short MacroVarNo,double MacroVarValue)
{beginResetModel();
 m_MacroVars.insert({ m_row, new MacroVar(MacroVarNo,MacroVarValue)});
 m_row++;
 endResetModel();
}
void MacroVarsModel::UpdateMacroVar(short MacroVarNo, double MacroVarValue)
{beginResetModel();
 //if (MacroVarNo <= 33) {m_MacroVars.at(MacroVarNo-1)->MacroVarNo = MacroVarNo; m_MacroVars.at(MacroVarNo - 1)->MacroVarValue = MacroVarValue; }
 //if (MacroVarNo >= 100){m_MacroVars.at(MacroVarNo - 67)->MacroVarNo = MacroVarNo; m_MacroVars.at(MacroVarNo - 67)->MacroVarValue = MacroVarValue; }
 if (MacroVarNo <= 33) { m_MacroVars.at(MacroVarNo-1) = new MacroVar(MacroVarNo, MacroVarValue); }
 if (MacroVarNo >= 100) { m_MacroVars.at(MacroVarNo - 67) = new MacroVar(MacroVarNo, MacroVarValue);}
 endResetModel();
}
bool MacroVarsAssignedProxyMdl::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{QModelIndex index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
 double value = sourceModel()->data(index, Qt::DisplayRole).toDouble();
 return value > 0.0;
}