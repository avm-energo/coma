#ifndef UWIDGET_H
#define UWIDGET_H

// simple widget class to populate from
// has virtual methods setUpdatesDisabled & setUpdatesEnabled

#include "../gen/datatypes.h"
#include "../interfaces/baseinterface.h"

#include <QWidget>

class UWidget : public QWidget
{
    Q_OBJECT
public:
    struct BdQuery
    {
        quint32 sigAdr;
        quint32 sigQuantity;
    };

    struct HighlightWarnAlarmStruct
    {
        quint32 fieldnum;
        QString color;
    };

    explicit UWidget(QWidget *parent = nullptr);

    void setUpdatesEnabled();
    void setUpdatesDisabled();
    bool updatesEnabled();
    //    virtual void setConnections() = 0;
    const QString getCaption();
    void setCaption(const QString &caption);
    void setUpdateTimerPeriod(quint32 period);
    void setHighlightMap(const QMap<int, QList<HighlightWarnAlarmStruct>> &map);
    void setFloatBdQuery(const QList<BdQuery> &list);
    void addFloatBd(const BdQuery &query);
    void setSpBdQuery(const QList<BdQuery> &list);
    void addSpBd(const BdQuery &query);
    void setBsBdQuery(const QList<BdQuery> &list);
    void addBsBd(const BdQuery &query);
    //    void setInterface(BaseInterface *iface);
    //    BaseInterface *iface();
    virtual void uponInterfaceSetting();
    virtual void reqUpdate();
    virtual void updateFloatData(const DataTypes::FloatStruct &fl);
    virtual void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp);
    virtual void updateBitStringData(const DataTypes::BitStringStruct &bs) {};
signals:
    // //     signals to set "ProgressBar2" in main Coma
    //    void setGeneralProgressBarSize(quint32 size);
    //    void setGeneralProgressBarCount(quint32 count);
    //    void setGeneralProgressBarMessage(const QString &msg);

public slots:

protected:
    QString m_hash;

    bool checkPassword();

private:
    bool m_updatesEnabled;
    quint32 m_timerCounter;
    quint32 m_timerMax;
    BaseInterface *m_iface;
    int m_regLeast, m_regCount;

    /// float
    QList<BdQuery> m_floatBdQueryList;
    /// single-point
    QList<BdQuery> m_spBdQueryList;
    /// bit strings
    QList<BdQuery> m_bsBdQueryList;
    QMap<int, QList<HighlightWarnAlarmStruct>> m_highlightMap;

private slots:
};

inline void UWidget::addFloatBd(const BdQuery &query)
{
    m_floatBdQueryList.push_back(query);
}

inline void UWidget::addSpBd(const BdQuery &query)
{
    m_spBdQueryList.push_back(query);
}

inline void UWidget::addBsBd(const BdQuery &query)
{
    m_bsBdQueryList.push_back(query);
}

#endif // UWIDGET_H
