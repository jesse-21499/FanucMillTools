#pragma once
#include <qabstractitemmodel.h>
#include <QtCore/qsortfilterproxymodel.h>
#include <qstring.h>
#include <map>
#include <vector>
struct ProgBlock
{long ProgBlockNo;
 QString ProgBlockLine;
 int MacroVarNum;
 bool VariableIsNotReaded;
 ProgBlock(long ProgBlockNo,QString& ProgBlockLine, int MacroVarNum,bool VariableIsNotReaded) : ProgBlockNo(ProgBlockNo),ProgBlockLine(ProgBlockLine),MacroVarNum(MacroVarNum),VariableIsNotReaded(VariableIsNotReaded)
 {
 }
};
class ProgBlocksModel : public QAbstractTableModel
{ Q_OBJECT
public:
    explicit ProgBlocksModel(QObject* parent = nullptr);
    // Required overrides for a read-only model
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    // Optional override to display custom column headers
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Method to add data to the model
    //void populateData(const std::map<unsigned int,ProgBlock>& newData);
    //void AddParameter(ProgBlock* &newProgBlock);
    void AddProgBlock(long ProgBlockNo,QString& ProgBlockLine, int MacroVarNum,bool VariableIsNotReaded);
public:
    std::map<unsigned int, ProgBlock*> m_ProgBlocks;
    unsigned int m_row;
};
class ProgBlocksMacroVarUnreadedProxyMdl : public QSortFilterProxyModel
{
    Q_OBJECT
        //Q_PROPERTY(bool MacroVarIsNotReaded READ ....)
public:
    explicit ProgBlocksMacroVarUnreadedProxyMdl(QObject* parent = nullptr);
private:
    //long BlockNum;
    //short MacroVarNum;
    //bool MacroVarIsNotReaded;
    //const QModelIndex BlockNum_index;
    //const QModelIndex MacroVarNum_index;
    //const QModelIndex MacroVarIsNotReaded_index;
    
protected:
    
    
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

};