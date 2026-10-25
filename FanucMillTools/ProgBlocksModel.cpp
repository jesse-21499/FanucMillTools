#include "ProgBlocksModel.h"
#include <QtGui\qcolor.h>
//using namespace std;
//Parameters
ProgBlocksModel::ProgBlocksModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_row = 0;
}

// Returns the number of rows in the data source
int ProgBlocksModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0; // Flat list/table structure
    return static_cast<int>(m_ProgBlocks.size());
}

// Returns the number of fields/columns to display
int ProgBlocksModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 4;
}

// Provides data for individual cells based on index and display role
QVariant ProgBlocksModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    // Out-of-bounds safety check
    if (index.row() >= static_cast<int>(m_ProgBlocks.size()) || index.row() < 0)
    {
        return QVariant();
    }
    // Handle text rendering requests
    if (role == Qt::BackgroundRole)
    {
        return QVariant(QColor(53, 53, 53));; //100,100,25
    }
    
    if (role == Qt::ForegroundRole)
    {
        if ((index.column() == 1) && (index.data(0).toString().contains("(")) && (index.data(0).toString().contains(")"))) return QVariant(QColor(Qt::gray));
        if ((index.column() == 1) && (index.data(0).toString().contains("G"))) return QVariant(QColor(Qt::darkCyan));
        if ((index.column() == 1) && (index.data(0).toString().contains("M"))) return QVariant(QColor(Qt::darkGreen));
        if ((index.column() == 1) && ((index.data(0).toString().contains("X"))|| (index.data(0).toString().contains("Y")) || (index.data(0).toString().contains("Z")))) return QVariant(QColor(Qt::green));
        if ((index.column() == 2) && (index.data(0).toInt() != 0)) return QVariant(QColor(Qt::magenta));
        
        else return QVariant();
    }
    if (role == Qt::DisplayRole)
    {  const auto& ProgBlock = m_ProgBlocks.at(index.row());
       switch (index.column())
        {case 0:
            return QVariant::fromValue(ProgBlock->ProgBlockNo);
            break;
         case 1:
            return ProgBlock->ProgBlockLine;
            break;
         case 2:
             if (ProgBlock->MacroVarNum!=0)
            return ProgBlock->MacroVarNum;
            break;
         
         default:
            return QVariant();
            break;
        }
    }
    if (role == Qt::CheckStateRole)
    { const auto& ProgBlock = m_ProgBlocks.at(index.row());
      switch (index.column())
        {
      case 3:
          if (ProgBlock->MacroVarNum!=0)
          return ProgBlock->VariableIsNotReaded;
          break;
      
        }
    }
    return QVariant();
}

// Configures the horizontal table headers

QVariant ProgBlocksModel::headerData(int section, Qt::Orientation orientation, int role) const
{if (role != Qt::DisplayRole && (role != Qt::CheckStateRole)) return QVariant();
 if (orientation == Qt::Horizontal)
    {switch (section)
        {case 0: return QString("BlockNo");
         case 1: return QString("ProgBlock");
         case 2: return QString("MacroVarNo");
         case 3: return QString("UnReaded");
         default: return QVariant();
        }
    }
    return QVariant();
}
// Populates data and notifies the view to redraw safely
//void ProgBlocksModel::AddProgBlock(const std::vector<Contact>& newData) 
 //void ProgBlocksModel::AddProgBlock(ProgBlock* &newProgBlock) 

void ProgBlocksModel::AddProgBlock(long ProgBlockNo,QString& ProgBlockLine,int MacroVarNum,bool VariableIsNotReaded)
{beginResetModel();
 //m_ProgBlocks = newData;
 //m_ProgBlocks.insert({ m_row,newProgBlock });
 m_ProgBlocks.insert({m_row, new ProgBlock(ProgBlockNo,ProgBlockLine,MacroVarNum,VariableIsNotReaded)});
 m_row++;
 endResetModel();
}
ProgBlocksMacroVarUnreadedProxyMdl::ProgBlocksMacroVarUnreadedProxyMdl(QObject* parent) : QSortFilterProxyModel(parent)
{

}
bool ProgBlocksMacroVarUnreadedProxyMdl::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const 
{const QModelIndex BlockNum_index = sourceModel()->index(source_row, 0, source_parent);
  const QModelIndex MacroVarNum_index = sourceModel()->index(source_row, 2, source_parent);
  const QModelIndex MacroVarUnReaded_index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
  const long BlockNum = BlockNum_index.data().toInt();
  const short MacroVarNum = MacroVarNum_index.data().toInt();
  const bool MacroVarIsNotReaded = MacroVarUnReaded_index.data(10).toBool();
  //value=sourceModel()->data(model_index, Qt::DisplayRole).to...();
  return (MacroVarIsNotReaded == true);
}