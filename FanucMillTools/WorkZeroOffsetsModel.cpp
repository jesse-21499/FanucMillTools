#include "WorkZeroOffsetsModel.h"
#include <QtGui\qcolor.h>
//using namespace std;
//Work Zero Offsets
WorkZeroOffsetsModel::WorkZeroOffsetsModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_row = 0;
}

// Returns the number of rows in the data source
int WorkZeroOffsetsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0; // Flat list/table structure
    return static_cast<int>(m_WorkZeroOffsets.size());
}

// Returns the number of fields/columns to display
int WorkZeroOffsetsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 4;
}

// Provides data for individual cells based on index and display role
QVariant WorkZeroOffsetsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    // Out-of-bounds safety check
    if (index.row() >= static_cast<int>(m_WorkZeroOffsets.size()) || index.row() < 0)
    {
        return QVariant();
    }
    // Handle text rendering requests
    if (role == Qt::DisplayRole)
    {
        const auto& WorkZeroOffset = m_WorkZeroOffsets.at(index.row());
        switch (index.column())
        {
        case 0:
            return WorkZeroOffset->WorkZeroOffsetNo;
            break;
        case 1:
            return WorkZeroOffset->WorkZeroOffsetX;
            break;
        case 2:
            return WorkZeroOffset->WorkZeroOffsetY;
            break;
        case 3:
            return WorkZeroOffset->WorkZeroOffsetZ;
            break;
        default:
            return QVariant();
            break;
        }
    }
    if (role == Qt::BackgroundRole)
    {
        return QVariant(QColor(53, 53, 53));;
    }
    if (role == Qt::ForegroundRole)
    {
        return QVariant(QColor(Qt::green));
    }

    return QVariant();
}

// Configures the horizontal table headers

QVariant WorkZeroOffsetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return QString("WorkZero Offset No");
        case 1: return QString("WorkZero Offset [X]");
        case 2: return QString("WorkZero Offset [Y]");
        case 3: return QString("WorkZero Offset [Z]");
        default: return QVariant();
        }
    }
    return QVariant();
}
// Populates data and notifies the view to redraw safely
//void ParamsModel::AddWorkZeroOffset(const std::vector<Contact>& newData) 
 //void ParamsModel::AddWorkZeroOffset(WorkZeroOffset* &newParam) 
void WorkZeroOffsetsModel::AddWorkZeroOffset(short WorkZeroOffsetNo, double  WorkZeroOffsetX, double WorkZeroOffsetY, double WorkZeroOffsetZ)
{
    beginResetModel();
    //m_WorkZeroOffsets = newData;
    //m_WorkZeroOffsets.insert({ m_row,newParam });
    m_WorkZeroOffsets.insert({ m_row, new WorkZeroOffset(WorkZeroOffsetNo,WorkZeroOffsetX, WorkZeroOffsetY, WorkZeroOffsetZ) });
    m_row++;
    endResetModel();
}
