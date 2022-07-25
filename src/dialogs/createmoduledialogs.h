#ifndef CREATEMODULEDIALOGS_H
#define CREATEMODULEDIALOGS_H

#include "../widgets/udialog.h"

#include <QObject>

class CreateModuleDialogs
{
public:
    CreateModuleDialogs(quint16 typeB, quint16 typeM);

    QList<UDialog *> CreateDialogs();

private:
    quint16 m_typeB, m_typeM;
    QList<UDialog *> m_dialogs;

    void DeleteDialogs();
    void CreateConfigDialogs();
};

#endif // CREATEMODULEDIALOGS_H
