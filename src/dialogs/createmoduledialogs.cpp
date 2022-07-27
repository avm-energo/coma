#include "createmoduledialogs.h"

#include "../dialogs/configdialog.h"

CreateModuleDialogs::CreateModuleDialogs(const ModuleSettings &settings) : m_settings(settings)
{
}

QList<UDialog *> CreateModuleDialogs::CreateDialogs()
{
    DeleteDialogs();
    CreateConfigDialogs();
    return m_dialogs;
}

void CreateModuleDialogs::DeleteDialogs()
{
    while (m_dialogs.size() != 0)
    {
        auto d = m_dialogs.takeFirst();
        d->deleteLater();
    }
}

void CreateModuleDialogs::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void CreateModuleDialogs::CreateConfigDialogs()
{
    using ConfigHash = QHash<int, ModuleSettings::ConfigList>;
    ConfigHash config = m_settings.getConfig();
    for (ConfigHash::Iterator it = config.begin(); it != config.end(); ++it)
    {
        QString indexStr = QString::number(it.key());
        addDialogToList(new ConfigDialog(&m_configV, it.value()), "Конфигурация " + indexStr, "conf" + indexStr);
    }
}

void CreateModuleDialogs::CreateCheckDialogs()
{
}
