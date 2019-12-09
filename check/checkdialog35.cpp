#include <QCoreApplication>
#include <QFileDialog>
#include <QDateTime>
#include "checkdialog35.h"
#include "check35.h"
#include "../widgets/emessagebox.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

CheckDialog35::CheckDialog35(BoardTypes board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Ch35 = new Check35();
    Ch = new Check;
//    BdNum = 2; // количество блоков данных 1
    BdUINum = 2; // количество вкладок - 1
    SetBd(BD_COMMON, &Ch->Bd_block0, sizeof(Check::Bd0));
    QStringList sl = QStringList() << "Общ" << "Проверка выходных реле";
    SetupUI(sl);

}

QWidget *CheckDialog35::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        return Ch->Bd0W(this);
    case 1:
        return Ch35->Bd1W(this);
    default:
        return new QWidget;
    }
}

#if PROGSIZE != PROGSIZE_EMUL
void CheckDialog35::SetDefaultValuesToWrite()
{

}

void CheckDialog35::PrepareAnalogMeasurements()
{

}

void CheckDialog35::ChooseValuesToWrite()
{

}

void CheckDialog35::WriteToFile(int row, int bdnum)
{

    Q_UNUSED(row);
    Q_UNUSED(bdnum);
}

void CheckDialog35::PrepareHeadersForFile(int row)
{

    Q_UNUSED(row);
}

void CheckDialog35::RefreshAnalogValues(int bdnum)
{

    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
        break;
    default:
        return;
    }
}

 QWidget*CheckDialog35::CustomTab()
 {
     return nullptr;
 }
#endif

