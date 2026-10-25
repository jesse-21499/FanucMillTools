#pragma once
#include <qabstractitemmodel.h>
#include <qstring.h>
#include <map>
#include <vector>
struct Parameter
{
    QString CNCIP;
    short ParamNo;
    short ParamType;
    QString strParamType;
    QString ParamAxis;
    QString s_cdata;
    QString s_idata;
    QString s_ldata;
    QString s_rdata;
    QString s_cdata_x;
    QString s_cdata_y;
    QString s_cdata_z;
    QString s_idata_x;
    QString s_idata_y;
    QString s_idata_z;
    QString s_ldata_x;
    QString s_ldata_y;
    QString s_ldata_z;
    QString s_rdata_x;
    QString s_rdata_y;
    QString s_rdata_z;
    bool IsAxisParam_attr;
    bool Param_Spindle_attr;
    bool IsSignedParam_attr;
    bool Param_SettingsInput_attr;
    bool Param_WriteProtect_attr;
    bool Param_PowerMBeOff_attr;
    bool Param_ReadProtect_attr;
    QString ParamArrangeType;
    QString ParamUnitType;
    QString ParamUnitDimens;
    QString ParamInputType;
    QString ParamDisplay;
    QString ParamOthers;
    QString ActualDate;
    Parameter(QString& CNCIP, short ParamNo, short ParamType, QString& strParamType, QString& ParamAxis, QString& s_cdata, QString& s_idata, QString& s_ldata, QString& s_rdata, QString& s_cdata_x, QString& s_cdata_y, QString& s_cdata_z, QString& s_idata_x, QString& s_idata_y, QString& s_idata_z, QString& s_ldata_x, QString& s_ldata_y, QString& s_ldata_z, QString& s_rdata_x, QString& s_rdata_y, QString& s_rdata_z, bool IsAxisParam_attr, bool Param_Spindle_attr, bool IsSignedParam_attr, bool Param_SettingsInput_attr, bool Param_WriteProtect_attr, bool Param_PowerMBeOff_attr, bool Param_ReadProtect_attr, QString& ParamArrangeType, QString& ParamUnitType, QString& ParamUnitDimens, QString& ParamInputType, QString& ParamDisplay, QString& ParamOthers, QString& ActualDate)
        : CNCIP(CNCIP), ParamNo(ParamNo), ParamType(ParamType), strParamType(strParamType), ParamAxis(ParamAxis), s_cdata(s_cdata), s_idata(s_idata), s_ldata(s_ldata), s_rdata(s_rdata), s_cdata_x(s_cdata_x), s_cdata_y(s_cdata_y), s_cdata_z(s_cdata_z), s_idata_x(s_idata_x), s_idata_y(s_idata_y), s_idata_z(s_idata_z), s_ldata_x(s_ldata_x), s_ldata_y(s_ldata_y), s_ldata_z(s_ldata_z), s_rdata_x(s_rdata_x), s_rdata_y(s_rdata_y), s_rdata_z(s_rdata_z), IsAxisParam_attr(IsAxisParam_attr), Param_Spindle_attr(Param_Spindle_attr), IsSignedParam_attr(IsSignedParam_attr), Param_SettingsInput_attr(Param_SettingsInput_attr), Param_WriteProtect_attr(Param_WriteProtect_attr), Param_PowerMBeOff_attr(Param_PowerMBeOff_attr), Param_ReadProtect_attr(Param_ReadProtect_attr), ParamArrangeType(ParamArrangeType), ParamUnitType(ParamUnitType), ParamUnitDimens(ParamUnitDimens), ParamInputType(ParamInputType), ParamDisplay(ParamDisplay), ParamOthers(ParamOthers), ActualDate(ActualDate)
    {
    }

};
class ParamsModel : public QAbstractTableModel
{Q_OBJECT
 public:
 explicit ParamsModel(QObject* parent = nullptr);
 // Required overrides for a read-only model
 int rowCount(const QModelIndex& parent = QModelIndex()) const override;
 int columnCount(const QModelIndex& parent = QModelIndex()) const override;
 QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
 // Optional override to display custom column headers
 QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
 // Method to add data to the model
 //void populateData(const std::map<unsigned int,Parameter>& newData);
 //void AddParameter(Parameter* &newParam);
 void AddParameter(QString& CNCIP, short ParamNo, short ParamType, QString& strParamType, QString& ParamAxis, QString& s_cdata, QString& s_idata, QString& s_ldata, QString& s_rdata, QString& s_cdata_x, QString& s_cdata_y, QString& s_cdata_z, QString& s_idata_x, QString& s_idata_y, QString& s_idata_z, QString& s_ldata_x, QString& s_ldata_y, QString& s_ldata_z, QString& s_rdata_x, QString& s_rdata_y, QString& s_rdata_z, bool IsAxisParam_attr, bool Param_Spindle_attr, bool IsSignedParam_attr, bool Param_SettingsInput_attr, bool Param_WriteProtect_attr, bool Param_PowerMBeOff_attr, bool Param_ReadProtect_attr, QString& ParamArrangeType, QString& ParamUnitType, QString& ParamUnitDimens, QString& ParamInputType, QString& ParamDisplay, QString& ParamOthers, QString& ActualDate);
public:
    std::map<unsigned int,Parameter*> m_Parameters;
    unsigned int m_row;
};