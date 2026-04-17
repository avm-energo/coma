#include "xml/xmleditor/dialogs/xmlresmembercreatedialog.h"

#include <avm-widgets/cbfunc.h>
#include <xml/xmleditor/dialogs/dialogfabric.h>
#include <xml/xmleditor/models/xmlmodel.h>

#include <algorithm>

namespace
{
constexpr auto kComboName = "xmlResMemberTypeCombo";
} // namespace

XmlResMemberCreateDialog::XmlResMemberCreateDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlResMemberCreateDialog::setupUI(QVBoxLayout *mainLayout)
{
    setupSizePos(450, 120);

    QStringList titles;
    titles.reserve(XmlDialogFabric::s_resourceCreatableTypes.size());
    for (const auto type : XmlDialogFabric::s_resourceCreatableTypes)
    {
        const auto it = XmlDialogFabric::s_dialogTitles.find(type);
        if (it != XmlDialogFabric::s_dialogTitles.cend())
        {
            titles.append(it->second);
            m_types.append(type);
        }
    }

    mainLayout->addWidget(new QLabel("Создание:", this));
    auto combo = CBFunc::New(this, kComboName, titles);
    QObject::connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlResMemberCreateDialog::dataChanged));
    m_dlgItems.append(combo);
    mainLayout->addWidget(combo);
}

QStringList XmlResMemberCreateDialog::collectData()
{
    const auto idx = CBFunc::index(this, kComboName);
    if (idx >= 0 && idx < m_types.size())
    {
        const auto type = m_types.at(idx);
        const auto tagIt = std::find_if(XmlModel::s_types.cbegin(), XmlModel::s_types.cend(),
            [type](const auto &kv) { return kv.second == type; });
        if (tagIt != XmlModel::s_types.cend())
            return { tagIt->first, QString() }; // col 0 - имя тега, col 1 - описание
    }
    return {};
}
