#ifndef UWIDGET_H
#define UWIDGET_H

// simple widget class to populate from
// has virtual methods setUpdatesDisabled & setUpdatesEnabled

#include "../gen/datatypes.h"
#include "../interfaces/baseinterface.h"

#include <QWidget>
#include <list>

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
    //    virtual void setConnections() = 0;
    const QString getCaption();
    void setCaption(const QString &caption);
    void setUpdateTimerPeriod(quint32 period);
    void setHighlightMap(const QMap<int, QList<HighlightWarnAlarmStruct>> &map);
    void setFloatBdQuery(const QList<BdQuery> &list);
    void setSpBdQuery(const QList<BdQuery> &list);
    void setInterface(BaseInterface *iface);
    void uponInterfaceSetting();
    BaseInterface *iface();

signals:

public slots:
    virtual void reqUpdate();
    void updateFloatData(const DataTypes::FloatStruct &fl);
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp);

protected:
    bool m_updatesEnabled;

private:
    QString m_caption;
    quint32 m_timerCounter;
    quint32 m_timerMax;
    BaseInterface *m_iface;

    QList<BdQuery> m_floatBdQueryList;
    QList<BdQuery> m_spBdQueryList;
    QMap<int, QList<HighlightWarnAlarmStruct>> m_highlightMap;
};

#endif // UWIDGET_H
