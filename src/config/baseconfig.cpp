#include "baseconfig.h"

#include "../widgets/checkboxgroup.h"
#include "../widgets/ipctrl.h"

BaseConfig::BaseConfig()
{
}

widgetMap WidgetFactory::widgetMap;
QWidget *WidgetFactory::createWidget(BciNumber key, QWidget *parent)
{
    QWidget *widget = nullptr;
    auto search = widgetMap.find(key);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << key;
        return widget;
    }

    const auto var = search->second;
    std::visit(overloaded {
                   [&](const auto &arg) {
                       qDebug("DefaultWidget");
                       using namespace delegate;
                       switch (arg.type)
                       {
                       case delegate::widgetType::IpControl:
                       {
                           widget = new IPCtrl(parent);
                           break;
                       }
                       case delegate::widgetType::QCheckBox:
                       {
                           widget = new QCheckBox(arg.desc, parent);
                           break;
                       }
                       case delegate::widgetType::QComboBox:
                       {
                           widget = new QComboBox(parent);
                           break;
                       }
                       case delegate::widgetType::QLineEdit:
                       {
                           widget = new QLineEdit(parent);
                           break;
                       }
                       default:
                           break;
                           //  Q_ASSERT(false && "False type");
                       }
                   },
                   [&](const delegate::DoubleSpinBoxGroup &arg) {
                       qDebug("DoubleSpinBoxGroupWidget");
                       widget
                           = WDFunc::NewSPBG(parent, QString::number(key), arg.count, arg.min, arg.max, arg.decimals);
                   },
                   [&](const delegate::DoubleSpinBoxWidget &arg) {
                       qDebug("DoubleSpinBoxWidget");
                       widget = WDFunc::NewSPB2(parent, QString::number(key), arg.min, arg.max, arg.decimals);
                   },
                   [&](const delegate::CheckBoxGroup &arg) {
                       qDebug("CheckBoxGroupWidget");
                       const QStringList desc = arg.desc.split(',');
                       // Q_ASSERT(desc.count() == arg.count);
                       widget = new CheckBoxGroup(arg.desc.split(','), parent);
                   },
               },
        var);
    return widget;
}
