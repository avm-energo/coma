#ifndef MODULEDATAUPDATER_H
#define MODULEDATAUPDATER_H

#include <QObject>
#include <gen/datatypes.h>
#include <interfaces/connection.h>

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

    explicit ModuleDataUpdater(Connection *connection, QObject *parent = nullptr);

    void updateConnection(Connection *connection) noexcept;
    Connection *currentConnection() noexcept;

    void requestUpdates();
    bool updatesEnabled();
    void setUpdatesEnabled(bool enabled = true);

    void enableFloatDataUpdates() noexcept;
    void enableSinglePointDataUpdates() noexcept;
    void enableBitStringDataUpdates() noexcept;
    void disableUpdates() noexcept;

    void addFloat(const BdQuery &query);
    void addSp(const BdQuery &query);
    void addBs(const BdQuery &query);

private:
    Connection *m_conn;
    QList<BdQuery> m_floatQueryList; ///< float
    QList<BdQuery> m_spQueryList;    ///< single-point
    QList<BdQuery> m_bsQueryList;    ///< bit strings
    QMetaObject::Connection m_floatDataConn, m_singlePointDataConn, m_bitStringDataConn;
    bool m_updatesEnabled;

signals:
    void itsTimeToUpdateFloatSignal(const DataTypes::FloatStruct &fl);
    void itsTimeToUpdateSinglePointSignal(const DataTypes::SinglePointWithTimeStruct &sp);
    void itsTimeToUpdateBitStringSignal(const DataTypes::BitStringStruct &bs);

private slots:
    void updateSinglePointData(const DataTypes::SinglePointWithTimeStruct &sp);
    void updateBitStringData(const DataTypes::BitStringStruct &bs);
    void updateFloatData(const DataTypes::FloatStruct &fl);
};

#endif // MODULEDATAUPDATER_H
