#include "createmoduledialogs.h"

CreateModuleDialogs::CreateModuleDialogs(quint16 typeB, quint16 typeM)
{
    m_typeB = typeB;
    m_typeM = typeM;
}

QList<UDialog *> CreateModuleDialogs::CreateDialogs()
{
    DeleteDialogs();
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

void CreateModuleDialogs::CreateConfigDialogs()
{
}
