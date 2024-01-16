#ifndef UWIDGET_H
#define UWIDGET_H

#include <QWidget>
#include <interfaces/moduledataupdater.h>

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
    bool checkPassword();
    QString m_hash;
    Connection *m_conn;
    ModuleDataUpdater *m_dataUpdater;

private:
    QMap<int, QList<HighlightWarnAlarmStruct>> m_highlightMap;
};

#endif // UWIDGET_H
