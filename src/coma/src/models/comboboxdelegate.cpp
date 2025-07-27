#include "models/comboboxdelegate.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QPainter>
#include <QSpinBox>
#include <QStringListModel>

ComboBoxDelegate::ComboBoxDelegate(const QStringList &list, QObject *parent) : QStyledItemDelegate(parent), m_list(list)
{
}

QWidget *ComboBoxDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox *editor = new QComboBox(parent);
    editor->setStyleSheet("QComboBox"
                          "{"
                          "padding-right: 4px;"
                          "}");
    QStringListModel *model = new QStringListModel(editor);
    model->setStringList(m_list);
    editor->setModel(model);
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto value = index.model()->data(index, Qt::EditRole).toInt() - offset();
    QComboBox *comboBoxEdit = qobject_cast<QComboBox *>(editor);

    if (value < 0)
        value = 0;
    comboBoxEdit->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBoxEdit = qobject_cast<QComboBox *>(editor);

    const auto value = comboBoxEdit->currentIndex() + offset();
    model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

int ComboBoxDelegate::offset() const
{
    return m_offset;
}

void ComboBoxDelegate::setOffset(int offset)
{
    m_offset = offset;
}

QStringList ComboBoxDelegate::list() const
{
    return m_list;
}

void ComboBoxDelegate::setList(const QStringList &list)
{
    m_list = list;
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QWidget *const widget = option.widget;
    QStyle *const style = widget ? widget->style() : QApplication::style();
    QStyleOptionComboBox styleOption;
    styleOption.rect = option.rect;
    styleOption.direction = option.direction;
    int indexNumber = index.data(Qt::DisplayRole).toInt() - offset();
    if (indexNumber >= m_list.size() || indexNumber < 0)
        indexNumber = 0;
    styleOption.currentText = m_list.at(indexNumber);
    styleOption.fontMetrics = option.fontMetrics;
    const int iconWidth = style->pixelMetric(QStyle::PM_SmallIconSize, nullptr, widget);
    styleOption.iconSize = QSize(iconWidth, iconWidth);
    styleOption.palette = option.palette;
    styleOption.state = option.state;
    styleOption.styleObject = option.styleObject;

    style->drawControl(QStyle::CE_ComboBoxLabel, &styleOption, painter, widget);
}

SpinBoxDelegate::SpinBoxDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

SpinBoxDelegate::SpinBoxDelegate(const int min, const int max, QObject *parent)
    : QStyledItemDelegate(parent), m_minValue(min), m_maxValue(max), m_default(false)
{
}

QWidget *SpinBoxDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QSpinBox *editor = new QSpinBox(parent);
    if (!m_default)
    {
        editor->setMinimum(m_minValue);
        editor->setMaximum(m_maxValue);
    }
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const auto value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor);

    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor);

    spinBox->interpretText();
    int value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
