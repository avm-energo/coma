#include "xml/xmleditor/models/xmlmodel.h"

#include <xml/xmleditor/models/modelfabric.h>
#include <xml/xmltags.h>

const std::map<QString, ModelType> XmlModel::s_types {
    { tags::res, ModelType::Resources },           //
    { tags::sigs, ModelType::Signals },            //
    { tags::tabs, ModelType::SectionTabs },        //
    { tags::sections, ModelType::Sections },       //
    { tags::section, ModelType::Section },         //
    { tags::sgroup, ModelType::SGroup },           //
    { tags::alarms, ModelType::Alarms },           //
    { tags::state_all, ModelType::AlarmStateAll }, //
    { tags::crit, ModelType::AlarmsCrit },         //
    { tags::warn, ModelType::AlarmsWarn },         //
    { tags::info, ModelType::AlarmsInfo },         //
    { tags::journals, ModelType::Journals },       //
    { tags::work, ModelType::WorkJours },          //
    { tags::meas, ModelType::MeasJours },          //
    { tags::modbus, ModelType::Modbus },           //
    { tags::protocom, ModelType::Protocom },       //
    { tags::iec, ModelType::IEC60870 },            //
    { tags::config, ModelType::Config },           //
    { tags::hidden, ModelType::Hidden },           //
    { tags::hidden_tab, ModelType::HiddenTab },    //
    { tags::bsi_ext, ModelType::BsiExt },          //
};

const std::map<ModelType, QStringList> XmlModel::s_headers {
    { ModelType::Resources, { "XML", "Описание" } },                                                     //
    { ModelType::Signals, { "Стартовый адрес", "Количество", "ID сигнала", "Тип" } },                    //
    { ModelType::SectionTabs, { "ID вкладки", "Название" } },                                            //
    { ModelType::Sections, { "Название" } },                                                             //
    { ModelType::Section, { "Название", "ID вкладки" } },                                                //
    { ModelType::SGroup, { "Адрес", "Имя" } },                                                           //
    { ModelType::Alarms, { "XML", "Описание" } },                                                        //
    { ModelType::AlarmStateAll, { "Адрес", "Описание", "Тип" } },                                        //
    { ModelType::AlarmsCrit, { "Адрес", "Описание", "Подсветка" } },                                     //
    { ModelType::AlarmsWarn, { "Адрес", "Описание", "Подсветка" } },                                     //
    { ModelType::AlarmsInfo, { "Адрес", "Описание" } },                                                  //
    { ModelType::Journals, { "XML", "Описание" } },                                                      //
    { ModelType::WorkJours, { "Адрес", "Описание" } },                                                   //
    { ModelType::MeasJours, { "Индекс", "Заголовок", "Тип", "Видимость" } },                             //
    { ModelType::Modbus, { "ID сигнала", "Тип регистра", "Возвращаемый тип", "Описание" } },             //
    { ModelType::Protocom, { "Блок", "ID сигнала" } },                                                   //
    { ModelType::IEC60870, { "ID сигнала", "Тип сигнала", "Тип передачи", "Группа" } },                  //
    { ModelType::Config, { "ID виджета", "Значение по умолчанию", "Изм. количество", "Видимость" } },    //
    { ModelType::Hidden, { "Название", "Префикс", "Флаг", "Задний фон" } },                              //
    { ModelType::HiddenTab, { "Индекс", "Название", "Виджет", "Тип", "Данные", "Адрес", "Видимость" } }, //
    { ModelType::BsiExt, { "Адрес", "Описание", "Тип", "Видимость" } },                                  //
};

XmlModel::XmlModel(int rows, int cols, ModelType type, QObject *parent) : BaseEditorModel(rows, cols, type, parent)
{
}

QVariant XmlModel::data(const QModelIndex &index, int nRole) const
{
    if (index.isValid())
        return QStandardItemModel::data(index, nRole);
    else
        return QVariant();
}

bool XmlModel::setData(const QModelIndex &index, const QVariant &val, int nRole)
{
    if (index.isValid() && val.isValid())
    {
        auto column = index.column();
        auto row = index.row();
        if ((column >= 0 && column < columnCount()) && (row >= 0 && row < rowCount()))
            return QStandardItemModel::setData(index, val, nRole);
    }
    return false;
}

void XmlModel::create(const QStringList &saved, int *row)
{
    BaseEditorModel::create(saved, row);
    emit modelChanged();
}

void XmlModel::update(const QStringList &saved, const int row)
{
    BaseEditorModel::update(saved, row);
    emit modelChanged();
}

void XmlModel::remove(const int row)
{
    BaseEditorModel::remove(row);
    emit modelChanged();
}

void XmlModel::setDataNode(bool isChildModel, QDomNode &root)
{
    int row = 0;
    if (isChildModel)
    {
        auto itemIndex = index(row, 0);
        setData(itemIndex, QString(".."));
        row++;
    }
    auto child = root.firstChild();
    while (!child.isNull())
    {
        if (!child.isComment())
            parseDataNode(child, row);
        child = child.nextSibling();
    }
}

void XmlModel::parseDataNode(QDomNode &child, int &row)
{
    auto childNodeName = child.nodeName();
    auto type = s_types.find(childNodeName);
    if (type != s_types.cend())
    {
        auto modelNode = ChildModelNode { nullptr, type->second };
        ModelFabric::createChildModel(modelNode, child, this);
        auto itemIndex = index(row, 0);
        setData(itemIndex, QVariant::fromValue(modelNode), ModelNodeRole);
    }
    parseNode(child, row);
    row++;
}

void XmlModel::parseTag(QDomNode &node, const QString &tagName, int row, int col, const QString &defValue, bool isInt)
{
    auto namedNode = node.firstChildElement(tagName);
    auto tagIndex = index(row, col);
    if (!namedNode.isNull())
    {
        auto tag = namedNode.firstChild().toText().data();
        if (isInt)
        {
            auto status = true;
            auto intValue = tag.toInt(&status);
            if (status)
                setData(tagIndex, intValue);
            else
                setData(tagIndex, tag);
        }
        else
            setData(tagIndex, tag);
    }
    else
    {
        if (!defValue.isEmpty())
        {
            if (isInt)
            {
                auto status = true;
                auto intValue = defValue.toInt(&status);
                if (status)
                    setData(tagIndex, intValue);
                else
                    setData(tagIndex, defValue);
            }
            else
                setData(tagIndex, defValue);
        }
    }
}

void XmlModel::parseAttribute(QDomNode &node, const QString &attrName, int row, int col, const QString &defValue)
{
    auto attr = node.toElement().attribute(attrName, defValue);
    if (!attr.isNull())
    {
        auto attrIndex = index(row, col);
        setData(attrIndex, attr);
    }
}
