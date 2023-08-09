#ifndef UWIDGET_H
#define UWIDGET_H

#include "../interfaces/moduledataupdater.h"

#include <QWidget>

class UWidget : public QWidget
{
    Q_OBJECT
public:
    struct HighlightWarnAlarmStruct
    {
        quint32 fieldnum;
        QString color;
    };

    explicit UWidget(QWidget *parent = nullptr);
    bool updatesEnabled();
    const QString getCaption();
    void setCaption(const QString &caption);
    virtual void uponInterfaceSetting();
    virtual void reqUpdate();
    virtual void updateFloatData(const DataTypes::FloatStruct &fl);
    virtual void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp);
    virtual void updateBitStringData(const DataTypes::BitStringStruct &bs);
    ModuleDataUpdater *engine();

protected:
    QString m_hash;
    bool checkPassword();

private:
    QMap<int, QList<HighlightWarnAlarmStruct>> m_highlightMap;
    ModuleDataUpdater *m_dataUpdater;
};

#endif // UWIDGET_H
