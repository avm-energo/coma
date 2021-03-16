#include "configblock.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
ConfigBlock::ConfigBlock(QObject *parent) : QObject(parent)
{
}

QString ConfigBlock::caption() const
{
    return m_caption;
}

void ConfigBlock::update(QLabel *label, const QString& text)
{
    label->setText(text);
}

void ConfigBlock::update(QLineEdit *lineEdit, const QString& text)
{
    lineEdit->setText(text);
}

void ConfigBlock::update(QDoubleSpinBox *spinBox, double value)
{
    spinBox->setValue(value);
}

void ConfigBlock::update(QComboBox *comboBox, int index)
{
    comboBox->setCurrentIndex(index);
}

void ConfigBlock::setCaption(const QString& caption)
{
    if (m_caption == caption)
        return;

    m_caption = caption;
    emit captionChanged(m_caption);
}

void ConfigBlock::handleData(const DataTypes::DataRecV &record)
{
    if (!m_items.contains(record.id))
        return;
    std::visit([&](auto &&arg) { update(record.id, arg); }, record.data);
}

void ConfigBlock::addChild(unsigned pos, ConfigBlock *child)
{
    child->setParent(this);
    child->setObjectName(QString::number(pos));
}
