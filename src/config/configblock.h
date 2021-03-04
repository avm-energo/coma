#pragma once

//#include "../module/modules.h"
#include "../gen/datarecv.h"

#include <QObject>
/// Another view of DataBlock, maybe will be useless not with conf

class ConfigBlock : QObject
{
public:
    explicit ConfigBlock(QObject *parent = nullptr);
    ~ConfigBlock();
    virtual void createWidget() = 0;

    QString caption() const
    {
        return m_caption;
    }

    QList<unsigned> items() const
    {
        return m_items;
    }

public slots:
    void setCaption(QString caption)
    {
        if (m_caption == caption)
            return;

        m_caption = caption;
        emit captionChanged(m_caption);
    }

    void setItems(QList<unsigned> items)
    {
        if (m_items == items)
            return;

        m_items = items;
        emit itemsChanged(m_items);
    }
    void appendItem(const unsigned item)
    {
        m_items.push_back(item);
        emit itemsChanged(m_items);
    }
    void handleData(const DataTypes::DataRecV &record);

signals:
    void captionChanged(QString caption);

    void itemsChanged(QList<unsigned> items);

protected:
    Q_PROPERTY(QString caption READ caption WRITE setCaption NOTIFY captionChanged)
    Q_PROPERTY(QList<unsigned> items READ items WRITE setItems NOTIFY itemsChanged)
private:
    QString m_caption;
    QList<unsigned> m_items;
};
