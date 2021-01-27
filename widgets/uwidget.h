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
    void setSpBdQuery(const QList<BdQuery> &list);
    //    void setInterface(BaseInterface *iface);
    //    BaseInterface *iface();
    virtual void uponInterfaceSetting();

signals:
    // //     signals to set "ProgressBar2" in main Coma
    //    void setGeneralProgressBarSize(quint32 size);
    //    void setGeneralProgressBarCount(quint32 count);
    //    void setGeneralProgressBarMessage(const QString &msg);

public slots:
    virtual void reqUpdate();
    virtual void updateFloatData(const DataTypes::FloatStruct &fl);
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp);

protected:
    QString m_password;

    bool checkPassword();

private:
    bool m_updatesEnabled;
    // QString m_caption;
    quint32 m_timerCounter;
    quint32 m_timerMax;
    BaseInterface *m_iface;
    int m_regLeast, m_regCount;

    QList<BdQuery> m_floatBdQueryList;
    QList<BdQuery> m_spBdQueryList;
    QMap<int, QList<HighlightWarnAlarmStruct>> m_highlightMap;

private slots:
};

#endif // UWIDGET_H
