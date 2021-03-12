#pragma once

#include "../gen/datarecv.h"

#include <QMap>
#include <QObject>
/// Another view of DataBlock, maybe will be useless not with conf
/// ConfigBlock can contain N levels of children ConfigBlocks like a B-tree.
/// For example, if current config block connected with array<N>,
/// config block contains another config block, which contains one widget per element of array,
/// internally any element of array can be displayed as another widget (but its not recommended),
/// one array should displayed as one kind of widgets.
///
///
/// Here is forward declared compatible widgets,
/// if you want to add more, just declare it here,
/// then include it in cpp
class QLabel;
class QLineEdit;
class QDoubleSpinBox;
class QComboBox;

class ConfigBlock : QObject
{
    Q_OBJECT
    // Caption of current Config Block
    Q_PROPERTY(QString caption READ caption WRITE setCaption NOTIFY captionChanged)
    // List of connected to current block IDs
    Q_PROPERTY(QList<unsigned> items READ items WRITE setItems NOTIFY itemsChanged)

public:
    explicit ConfigBlock(QObject *parent = nullptr);
    ~ConfigBlock();

    QString caption() const;

    /// Return all connected to current ConfigBlock (or its children) IDs
    QList<unsigned> items() const
    {
        return m_items;
    }

    /// Call base when reimplement
    /// Update all children from default values
    virtual void setDefBlock()
    {
        const auto children = findChildren<ConfigBlock *>(QString(), Qt::FindDirectChildrenOnly);
        for (const auto &child : children)
            child->setDefBlock();
    }
    /// Call base when reimplement
    /// Update all children from connected DataRecV
    virtual void updateWidget()
    {
        const auto children = findChildren<ConfigBlock *>(QString(), Qt::FindDirectChildrenOnly);
        for (const auto &child : children)
            child->updateWidget();
    }
    /// Call base when reimplement
    /// Update all DataRecV from connected children
    /// Its just a concept, maybe it should another logic
    /// maybe this func should return DataRecV from
    /// current configBlock
    virtual void updateFromWidget()
    {
        const auto children = findChildren<ConfigBlock *>(QString(), Qt::FindDirectChildrenOnly);
        for (const auto &child : children)
            child->updateFromWidget();
    }
    void setCaption(const QString &caption);
    /// Handle DataRecV, then try to update ConfigBlock or its children
    /// Update current config block should be simple as possbile,
    /// user just call this function with appropriate DataRecV, Config block
    /// internally select appropriate widget by key and set its value to DataRecV::value;
    void handleData(const DataTypes::DataRecV &record);
    /// Add child to current Config block, child will have object name from pos
    void addChild(unsigned pos, ConfigBlock *child);
    void setItems(const QList<unsigned> &items)
    {
        if (m_items == items)
            return;

        m_items = items;
        emit itemsChanged(m_items);
    }
    /// Add item to current Config block
    void addItem(unsigned item)
    {
        assert(!m_items.contains(item));

        m_items.push_back(item);
        emit itemsChanged(m_items);
    }

protected:
    /// Update ConfigBlock or its children
    /// Config block internally select appropriate
    ///  widget by key and set its value to value;
    template <typename T, std::enable_if_t<DataTypes::DataRecV::true_type<T>::value, bool> = true>
    void update(const unsigned key, const T &value)
    {
        if constexpr (std::is_container<T>())
        {
            const auto children = findChildren<ConfigBlock *>(QString(), Qt::FindDirectChildrenOnly);
            for (auto i = 0; i != value.size(); ++i)
                children.value(i)->update(i, value);
        }
        else
            selfUpdate(key, value);
    }
    /// Self update current ConfigBlock
    template <typename T, std::enable_if_t<DataTypes::DataRecV::true_type<T>::value, bool> = true>
    void selfUpdate(const unsigned key, const T &value)
    {
        auto label = findChild<QLabel *>(QString::number(key), Qt::FindDirectChildrenOnly);
        if (label)
        {
            update(label, QString::number(value));
            return;
        }
        auto lineEdit = findChild<QLineEdit *>(QString::number(key), Qt::FindDirectChildrenOnly);
        if (lineEdit)
        {
            update(lineEdit, QString::number(value));
            return;
        }
        auto spinBox = findChild<QDoubleSpinBox *>(QString::number(key), Qt::FindDirectChildrenOnly);
        if (spinBox)
        {
            update(spinBox, double(value));
            return;
        }
        auto comboBox = findChild<QComboBox *>(QString::number(key), Qt::FindDirectChildrenOnly);
        if (comboBox)
        {
            update(comboBox, int(value));
            return;
        }
    }
    void update(QLabel *label, const QString &text);
    void update(QLineEdit *lineEdit, const QString &text);
    void update(QDoubleSpinBox *spinBox, double value);
    void update(QComboBox *comboBox, int index);

signals:
    void captionChanged(QString caption);
    void childrenChanged(QMap<unsigned, ConfigBlock *> children);
    void itemsChanged(QList<unsigned> items);

private:
    QString m_caption;
    //   QMap<unsigned, ConfigBlock *> m_children;
    QList<unsigned> m_items;
};
