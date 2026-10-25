#pragma once
#include <qabstractitemmodel.h>
#include <QtCore/qsortfilterproxymodel.h>
#include <qstring.h>
#include <map>
#include <vector>
struct MacroVar
{
    short MacroVarNo;
    double MacroVarValue;
    MacroVar(short MacroVarNo,double MacroVarValue) : MacroVarNo(MacroVarNo),MacroVarValue(MacroVarValue)
    {
    }

};
class MacroVarsModel : public QAbstractTableModel//,QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MacroVarsModel(QObject* parent = nullptr);
    // Required overrides for a read-only model
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    // Optional override to display custom column headers
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Method to add data to the model
    //void populateData(const std::map<unsigned int,MacroVar>& newData);
    //void AddParameter(MacroVar* &newMacroVar);
    void AddMacroVar(short MacroVarNo,double MacroVarValue);
    void UpdateMacroVar(short MacroVarNo,double MacroVarValue);
public:
    std::map<unsigned int, MacroVar*> m_MacroVars;
    unsigned int m_row;
};

class MacroVarsAssignedProxyMdl : public QSortFilterProxyModel
{
    Q_OBJECT

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    
};