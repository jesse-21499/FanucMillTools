#include "ToolOffsetsModel.h"
#include <QtGui\qcolor.h>
//using namespace std;

//Tool Offsets
ToolOffsetsModel::ToolOffsetsModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_row = 0;
}

// Returns the number of rows in the data source
int ToolOffsetsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0; // Flat list/table structure
    return static_cast<int>(m_ToolOffsets.size());
}

// Returns the number of fields/columns to display
int ToolOffsetsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 5;
}

// Provides data for individual cells based on index and display role
QVariant ToolOffsetsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    // Out-of-bounds safety check
    if (index.row() >= static_cast<int>(m_ToolOffsets.size()) || index.row() < 0)
    {
        return QVariant();
    }
    // Handle text rendering requests
    if (role == Qt::DisplayRole)
    {
        const auto& ToolOffset = m_ToolOffsets.at(index.row());
        switch (index.column())
        {
        case 0:
            return ToolOffset->ToolOffsetNo;
            break;
        case 1:
            return ToolOffset->ToolLength;
            break;
        case 2:
            return ToolOffset->ToolLengthWear;
            break;
        case 3:
            return ToolOffset->ToolRadius;
            break;
        case 4:
            return ToolOffset->ToolRadiusWear;
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

QVariant ToolOffsetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return QString("Tool Offset No");
        case 1: return QString("Tool Length");
        case 2: return QString("Tool Length Wear");
        case 3: return QString("Tool Radius");
        case 4: return QString("Tool Radius Wear");
        default: return QVariant();
        }
    }
    return QVariant();
}
// Populates data and notifies the view to redraw safely
//void ParamsModel::AddToolOffset(const std::vector<Contact>& newData) 
 //void ParamsModel::AddToolOffset(ToolOffset* &newParam) 
void ToolOffsetsModel::AddToolOffset(short ToolOffsetNo, double  ToolLength, double ToolLengthWear, double ToolRadius, double ToolRadiusWear)
{
    beginResetModel();
    //m_ToolOffsets = newData;
    //m_ToolOffsets.insert({ m_row,newParam });
    m_ToolOffsets.insert({ m_row, new ToolOffset(ToolOffsetNo,ToolLength, ToolLengthWear, ToolRadius,ToolRadiusWear) });
    m_row++;
    endResetModel();
}
