#pragma once
#include <qabstractitemmodel.h>
#include <qstring.h>
#include <map>
#include <vector>

struct ToolOffset
{
    short ToolOffsetNo;
    double ToolLength;
    double ToolLengthWear;
    double ToolRadius;
    double ToolRadiusWear;
    ToolOffset(short ToolOffsetNo, double  ToolLength, double ToolLengthWear, double ToolRadius, double ToolRadiusWear)
        : ToolOffsetNo(ToolOffsetNo), ToolLength(ToolLength), ToolLengthWear(ToolLengthWear), ToolRadius(ToolRadius), ToolRadiusWear(ToolRadiusWear)
    {
    }
};
class ToolOffsetsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ToolOffsetsModel(QObject* parent = nullptr);
    // Required overrides for a read-only model
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    // Optional override to display custom column headers
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Method to add data to the model
    //void populateData(const std::map<unsigned int,Parameter>& newData);
    //void AddParameter(Parameter* &newParam);
    void AddToolOffset(short ToolOffsetNo, double  ToolLength, double ToolLengthWear, double ToolRadius, double ToolRadiusWear);
public:
    std::map<unsigned int, ToolOffset*> m_ToolOffsets;
    unsigned int m_row;
};
