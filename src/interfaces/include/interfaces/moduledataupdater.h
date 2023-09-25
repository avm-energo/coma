#ifndef MODULEDATAUPDATER_H
#define MODULEDATAUPDATER_H

#include <QObject>
#include <gen/datatypes.h>
#include <interfaces/conn/baseconnection.h>

using namespace Interface;
class ModuleDataUpdater : public QObject
{
    Q_OBJECT
public:
    struct BdQuery
    {
        quint32 sigAdr;
        quint32 sigQuantity;
    };

    explicit ModuleDataUpdater(BaseConnection *connection, QObject *parent = nullptr);
    void requestUpdates();
    bool updatesEnabled();
    void setUpdatesEnabled(bool enabled = true);
    void addFloat(const BdQuery &query);
    void addSp(const BdQuery &query);
    void addBs(const BdQuery &query);

private:
    BaseConnection *m_conn;
    QList<BdQuery> m_floatQueryList; ///< float
    QList<BdQuery> m_spQueryList;    ///< single-point
    QList<BdQuery> m_bsQueryList;    ///< bit strings
    bool m_updatesEnabled;
    // UniquePointer<DataTypesProxy> proxyFS, proxySP, proxyBS;

    void setFloatQuery(const QList<BdQuery> &list);
    void setSpQuery(const QList<BdQuery> &list);
    void setBsQuery(const QList<BdQuery> &list);

signals:
    void itsTimeToUpdateFloatSignal(const DataTypes::FloatStruct &fl);
    void itsTimeToUpdateSinglePointSignal(const DataTypes::SinglePointWithTimeStruct &sp);
    void itsTimeToUpdateBitStringSignal(const DataTypes::BitStringStruct &bs);
    void ifaceChanged();

private slots:
    void updateSinglePointData(const DataTypes::SinglePointWithTimeStruct &sp);
    void updateBitStringData(const DataTypes::BitStringStruct &bs);
    void updateFloatData(const DataTypes::FloatStruct &fl);
};

#endif // MODULEDATAUPDATER_H
