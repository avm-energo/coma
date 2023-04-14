#include "measjournal.h"

namespace journals
{

MeasJournal::MeasJournal(QObject *parent) : BaseJournal(parent)
{
    jourName = "Журнал измерений";
    viewName = "meas";
}

}
