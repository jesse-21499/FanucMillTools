#pragma once
#include <qabstractitemmodel.h>
#include <qstring.h>
#include <map>
#include <vector>

struct WorkZeroOffset
{
    short WorkZeroOffsetNo;
    double WorkZeroOffsetX;
    double WorkZeroOffsetY;
    double WorkZeroOffsetZ;


    WorkZeroOffset(short WorkZeroOffsetNo, double  WorkZeroOffsetX, double WorkZeroOffsetY, double WorkZeroOffsetZ)
        : WorkZeroOffsetNo(WorkZeroOffsetNo), WorkZeroOffsetX(WorkZeroOffsetX), WorkZeroOffsetY(WorkZeroOffsetY), WorkZeroOffsetZ(WorkZeroOffsetZ)
    {
    }

};
class WorkZeroOffsetsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit WorkZeroOffsetsModel(QObject* parent = nullptr);
    // Required overrides for a read-only model
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    // Optional override to display custom column headers
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Method to add data to the model
    //void populateData(const std::map<unsigned int,Parameter>& newData);
    //void AddParameter(Parameter* &newParam);
    void AddWorkZeroOffset(short WorkZeroOffsetNo, double  WorkZeroOffsetX, double WorkZeroOffsetY, double WorkZeroOffsetZ);
public:
    std::map<unsigned int, WorkZeroOffset*> m_WorkZeroOffsets;
    unsigned int m_row;
};


