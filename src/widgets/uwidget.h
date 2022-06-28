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
    enum WidgetType
    {
        NORMAL = 0,
        WITHOUT_GUI = 1
    };

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

    //    bool m_withGUI;

    explicit UWidget(QWidget *parent = nullptr);

    void setUpdatesEnabled();
    void setUpdatesDisabled();
    bool updatesEnabled();
    //    virtual void setConnections() = 0;
    const QString getCaption();
    void setCaption(const QString &caption);
    void setHighlightMap(const QMap<int, QList<HighlightWarnAlarmStruct>> &map);
    void setFloatBdQuery(const QList<BdQuery> &list);
    void addFloatBd(const BdQuery &query);
    void setSpBdQuery(const QList<BdQuery> &list);
    void addSpBd(const BdQuery &query);
    void setBsBdQuery(const QList<BdQuery> &list);
    void addBsBd(const BdQuery &query);
    virtual void uponInterfaceSetting();
    virtual void reqUpdate();
    virtual void updateFloatData(const QVariant &msg);
    virtual void updateSPData(const QVariant &msg);
    virtual void updateBitStringData(const QVariant &msg);

protected:
    QString m_hash;
    UniquePointer<DataTypesProxy> proxyFS, proxySP, proxyBS;

    bool checkPassword();

private:
    bool m_updatesEnabled;
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
