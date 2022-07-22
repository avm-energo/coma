#ifndef TUNEDIALOGFABRIC_H
#define TUNEDIALOGFABRIC_H

#include "abstracttunedialog.h"

#include <QObject>

class TuneDialogFabric
{
    Q_OBJECT
public:
    TuneDialogFabric();

    static AbstractTuneDialog *GetTuneDialogForType(quint16 typeB, quint16 typeM);
};

#endif // TUNEDIALOGFABRIC_H
