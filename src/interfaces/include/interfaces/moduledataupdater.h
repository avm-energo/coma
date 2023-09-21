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

    explicit ModuleDataUpdater(BaseConnection *iface, QObject *parent = nullptr);
    void requestUpdates();
    bool updatesEnabled();
    void setUpdatesEnabled(bool enabled = true);
    void addFloat(const BdQuery &query);
    void addSp(const BdQuery &query);
    void addBs(const BdQuery &query);

private:
    BaseConnection *m_iface;
    UniquePointer<DataTypesProxy> proxyFS, proxySP, proxyBS;

    void setFloatQuery(const QList<BdQuery> &list);
    void setSpQuery(const QList<BdQuery> &list);
    void setBsQuery(const QList<BdQuery> &list);

    /// float
    QList<BdQuery> m_floatQueryList;
    /// single-point
    QList<BdQuery> m_spQueryList;
    /// bit strings
    QList<BdQuery> m_bsQueryList;

    //    BaseInterface *m_iface;
    bool m_updatesEnabled;

signals:
    void itsTimeToUpdateFloatSignal(DataTypes::FloatStruct &);
    void itsTimeToUpdateSinglePointSignal(DataTypes::SinglePointWithTimeStruct &);
    void itsTimeToUpdateBitStringSignal(DataTypes::BitStringStruct &);
    void ifaceChanged();

private slots:
    void updateSinglePointData(const QVariant &msg);
    void updateBitStringData(const QVariant &msg);
    void updateFloatData(const QVariant &msg);
};

#endif // MODULEDATAUPDATER_H
