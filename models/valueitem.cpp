#include "valueitem.h"

ValueItem::ValueItem(const ValueItem &item)
    : m_data(item.m_data)
    , m_name(item.m_name)
    , m_tooltip(item.m_tooltip)
    , m_style(item.m_style)
    , m_format(item.m_format)
    , m_addData(item.m_addData)
{
    //    switch (m_style)
    //    {
    //    case DataStyles::ComboBox:
    //    {
    //        m_addData.setValue(item.m_addData);
    //        /*        QStringList *sl = static_cast<QStringList *>(item.adddata);
    //                QStringList *newsl = new QStringList;
    //                *newsl = *sl;
    //                adddata = newsl; */
    //        break;
    //    }
    //    case DataStyles::SpinBox:
    //    {
    //        m_addData = newVar<SpbDataStyleAdditionalDataClass>(item.m_addData);
    //        /*                SpbDataStyleAdditionalDataClass *spb = static_cast<SpbDataStyleAdditionalDataClass
    //         *>(item.adddata); SpbDataStyleAdditionalDataClass *newspb = new SpbDataStyleAdditionalDataClass; *newspb
    //         = *spb; adddata = newspb; */
    //        break;
    //    }
    //    case DataStyles::Label:
    //    case DataStyles::LineEdit:
    //    default:
    //        break;
    //    }
    //    switch (m_format)
    //    {
    //    case DataFormat::OUTVALUEDOUBLE:
    //        m_data = newVar<double>(item.m_dataPtr);
    //        break;
    //    case DataFormat::OUTVALUEFLOAT0:
    //    case DataFormat::OUTVALUEFLOAT1:
    //    case DataFormat::OUTVALUEFLOAT2:
    //    case DataFormat::OUTVALUEFLOAT3:
    //    case DataFormat::OUTVALUEFLOAT4:
    //    case DataFormat::OUTVALUEFLOAT5:
    //    case DataFormat::OUTVALUEHEX:
    //    case DataFormat::OUTVALUEINT:
    //    case DataFormat::SIMPLE_CELL:
    //    default:
    //        m_dataPtr = newVar<float>(item.m_dataPtr);
    //        break;
    //    case DataFormat::OUTVALUESTRING:
    //        m_dataPtr = newVar<QString>(item.m_dataPtr);
    //        break;
    //    }
}

ValueItem &ValueItem::operator=(const ValueItem &item)
{
    m_addData = item.m_addData;
    m_data = item.m_data;
    //    switch (m_style)
    //    {
    //    case DataStyles::ComboBox:
    //    {
    //        setVar<QStringList>(m_addData, item.m_addData);
    //        break;
    //    }
    //    case DataStyles::SpinBox:
    //    {
    //        setVar<SpbDataStyleAdditionalDataClass>(m_addData, item.m_addData);
    //        break;
    //    }
    //    case DataStyles::Label:
    //    case DataStyles::LineEdit:
    //    default:
    //        break;
    //    }
    //    switch (m_format)
    //    {
    //    case DataFormat::OUTVALUEDOUBLE:
    //        setVar<double>(m_dataPtr, item.m_dataPtr);
    //        break;
    //    case DataFormat::OUTVALUEFLOAT0:
    //    case DataFormat::OUTVALUEFLOAT1:
    //    case DataFormat::OUTVALUEFLOAT2:
    //    case DataFormat::OUTVALUEFLOAT3:
    //    case DataFormat::OUTVALUEFLOAT4:
    //    case DataFormat::OUTVALUEFLOAT5:
    //    case DataFormat::OUTVALUEHEX:
    //    case DataFormat::OUTVALUEINT:
    //    case DataFormat::SIMPLE_CELL:
    //    default:
    //        setVar<float>(m_dataPtr, item.m_dataPtr);
    //        break;
    //    case DataFormat::OUTVALUESTRING:
    //        setVar<QString>(m_dataPtr, item.m_dataPtr);
    //        break;
    //    }
    return *this;
}

QVariant ValueItem::data()
{
    return m_data;
}

void ValueItem::setData(QVariant data)
{
    m_data = data;
}

// void *ValueItem::dataPtr()
//{
//    return m_dataPtr;
//}

// void ValueItem::setDataPtr(void *ptr)
//{
//    m_dataPtr = ptr;
//}

QString ValueItem::name()
{
    return m_name;
}

void ValueItem::setName(const QString &str)
{
    m_name = str;
}

QString ValueItem::tooltip()
{
    return m_tooltip;
}

void ValueItem::setTooltip(const QString &str)
{
    m_tooltip = str;
}

ValueItem::DataStyles ValueItem::style()
{
    return m_style;
}

void ValueItem::setStyle(ValueItem::DataStyles stl)
{
    m_style = stl;
}

ValueItem::DataFormat ValueItem::format()
{
    return m_format;
}

void ValueItem::setFormat(ValueItem::DataFormat fmt)
{
    m_format = fmt;
}

QVariant ValueItem::addData()
{
    return m_addData;
}

void ValueItem::setAddData(QVariant adddata)
{
    m_addData = adddata;
}

// SpbDataStyleAdditionalDataClass::SpbDataStyleAdditionalDataClass(const SpbDataStyleAdditionalDataClass &cpy)
//    : min(cpy.min), max(cpy.max), decimals(cpy.decimals)
//{
//}

// SpbDataStyleAdditionalDataClass &SpbDataStyleAdditionalDataClass::operator=(const SpbDataStyleAdditionalDataClass
// &cpy)
//{
//    min = cpy.min;
//    max = cpy.max;
//    decimals = cpy.decimals;
//    return *this;
//}
