#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QProgressBar>
#include <QApplication>
#include <QDateTime>
#include "QtXlsx/xlsxdocument.h"
#include <QFileDialog>
#include <QMessageBox>
#include "oscdialog.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../config/config.h"
#include "../widgets/etableview.h"
#include "../widgets/etablemodel.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/emessagebox.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../widgets/wd_func.h"
#include "../gen/timefunc.h"

OscDialog::OscDialog(EOscillogram *osc, QWidget *parent) :
    QDialog(parent), OscFunc(osc)
{
    SetupUI();
}

OscDialog::~OscDialog()
{
}

void OscDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("osctv");
    tm = new ETableModel;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Получить данные по осциллограммам в памяти "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(GetAndProcessOscInfo()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть все осциллограммы в памяти "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseOsc()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

void OscDialog::UpdateModel()
{
    ETableView *tv = this->findChild<ETableView *>("osctv");
    if (tv == 0)
    {
        DBGMSG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(tm);
    delete m;
    GetOscPBDelegate *dg = new GetOscPBDelegate;
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    tv->setItemDelegateForColumn(4, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

#if PROGSIZE != PROGSIZE_EMUL
void OscDialog::GetAndProcessOscInfo()
{
    emit StopCheckTimer();
    QVector<QVector<QVariant> > lsl;
    QByteArray OscInfo;
    quint32 OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    quint32 RecordSize = sizeof(EOscillogram::GBoStruct); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    if ((Commands::GetBt(TECH_Bo, &(OscInfo.data()[0]), OscInfoSize)) == Error::ER_NOERROR)
    {
        tm->ClearModel();
        tm->addColumn("#");
        tm->addColumn("ИД");
        tm->addColumn("Дата/Время");
        tm->addColumn("Длина");
        tm->addColumn("Скачать");
        if (OscInfoSize < RecordSize)
        {
            EMessageBox::information(this, "Информация", "В памяти модуля нет осциллограмм");
            UpdateModel();
            return;
        }
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QVector<QVariant> Num, IDs, Tim, Lngth, Dwld;
        int counter = 0;
        for (quint32 i = 0; i < OscInfoSize; i+= RecordSize)
        {
            EOscillogram::GBoStruct gbos;
            memcpy(&gbos, &(OscInfo.data()[i]), RecordSize);
            Num << QString::number(gbos.FileNum);
            Tim << TimeFunc::UnixTime64ToString(gbos.UnixTime);
            IDs << gbos.ID;
            Lngth << gbos.FileLength;
            ++counter;
        }
        Dwld.fill("Скачать", counter);
        lsl.append(Num);
        lsl.append(IDs);
        lsl.append(Tim);
        lsl.append(Lngth);
        lsl.append(Dwld);
        tm->fillModel(lsl);
        UpdateModel();
        QApplication::restoreOverrideCursor();
    }
}

void OscDialog::GetOsc(QModelIndex idx)
{
    emit StopCheckTimer();
    bool ok;
    StdFunc::SetPrbMessage("Загружено, байт: ");
    quint32 oscnum = tm->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    int basize = tm->data(idx.sibling(idx.row(),3),Qt::DisplayRole).toInt(&ok);
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    basize += sizeof(S2::FileHeader);
    OscFunc->BA.resize(basize);
    if (Commands::GetOsc(oscnum, &(OscFunc->BA.data()[0])) == Error::ER_NOERROR)
    {
        QString tmps = StdFunc::GetSystemHomeDir()+"/temporary.osc";
        Files::SaveToFile(tmps, OscFunc->BA, basize);
        OscFunc->ProcessOsc();
    }
    else
        WARNMSG("Номер файла не соответствует диапазону, принятому для осциллограмм: "+QString::number(oscnum));
}

void OscDialog::EraseOsc()
{
    StdFunc::SetPrbMessage("Стёрто записей: ");
    if (Commands::EraseTechBlock(TECH_Bo) == Error::ER_NOERROR)
        EMessageBox::information(this, "Внимание", "Стёрто успешно");
    else
        EMessageBox::information(this, "Внимание", "Ошибка при стирании");
}
#endif
