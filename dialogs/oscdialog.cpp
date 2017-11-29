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
#include "../gen/commands.h"
#include "../config/config.h"
#include "../widgets/s_tqtableview.h"
#include "../widgets/etablemodel.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

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
    s_tqTableView *tv = new s_tqTableView;
    tv->setObjectName("osctv");
    tm = new ETableModel;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Получить данные по осциллограммам в памяти "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(GetAndProcessOscInfo()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть все осциллограммы в памяти "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseOsc()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv, 89);
    setLayout(lyout);
}

/*int OscDialog::InputFileType()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Вывод осциллограмм"), Qt::AlignCenter);
    lyout->addWidget(WDFunc::NewChB(this, "xlsxchb", "В файл .xlsx"));
    lyout->addWidget(WDFunc::NewChB(this, "comtradechb", "В файлы COMTRADE"));
    lyout->addWidget(WDFunc::NewChB(this, "nonechb", "На экран"));
    QPushButton *pb = new QPushButton("Продолжить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Accept()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    AcceptedOscFileType = false;
    pc.Cancelled = false;
    dlg->show();
    while (!AcceptedOscFileType && !pc.Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    dlg->close();
    if (pc.Cancelled)
        return GENERALERROR;
    return NOERROR;
} */

void OscDialog::GetAndProcessOscInfo()
{
    QVector<QVector<QVariant> > lsl;
    QByteArray OscInfo;
    quint32 OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    quint32 RecordSize = sizeof(EOscillogram::GBoStruct); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    if ((Commands::GetBt(TECH_Bo, &(OscInfo.data()[0]), OscInfoSize)) == NOERROR)
    {
        if (OscInfoSize < RecordSize)
        {
            MessageBox2::information(this, "Информация", "Присланное количество байт слишком мало");
            return;
        }
        tm->ClearModel();
        tm->addColumn("#");
        tm->addColumn("ИД");
        tm->addColumn("Дата/Время");
        tm->addColumn("Длина");
        tm->addColumn("Скачать");
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QVector<QVariant> Num, IDs, Tim, Lngth, Dwld;
        int counter = 0;
        for (quint32 i = 0; i < OscInfoSize; i+= RecordSize)
        {
            EOscillogram::GBoStruct gbos;
            memcpy(&gbos, &(OscInfo.data()[i]), RecordSize);
            Num << QString::number(gbos.FileNum);
            Tim << pc.UnixTime64ToString(gbos.UnixTime);
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
        QApplication::restoreOverrideCursor();
        s_tqTableView *tv = this->findChild<s_tqTableView *>("osctv");
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
        tv->setItemDelegateForColumn(4,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
        tv->resizeRowsToContents();
        tv->resizeColumnsToContents();
    }
}

void OscDialog::GetOsc(QModelIndex idx)
{
    bool ok;
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
    OscFunc->BA.resize(basize);
    if (Commands::GetOsc(oscnum, &(OscFunc->BA.data()[0])) == NOERROR)
    {
        QString tmps = pc.SystemHomeDir+"/temporary.osc";
        pc.SaveToFile(tmps, OscFunc->BA, basize);
//        OscFunc->ProcessOsc(oscid, oscnum, OscDateTime);
        OscFunc->ProcessOsc();
/*        if (InputFileType() == GENERALERROR)
        {
            MessageBox2::error(this, "Ошибка", "Ошибка при задании типа файла осциллограмм");
            return;
        } */
    }
    else
        WARNMSG("");
}

/*void OscDialog::EndExtractOsc(quint32 id, OscHeader_Data &OHD, const QString &fn)
{
}*/

void OscDialog::EraseOsc()
{
    pc.PrbMessage = "Стёрто записей: ";
    if (Commands::EraseTechBlock(TECH_Bo) == NOERROR)
        MessageBox2::information(this, "Внимание", "Стёрто успешно");
    else
        MessageBox2::information(this, "Внимание", "Ошибка при стирании");
}

/*void OscDialog::Accept()
{
    OscFileType = 0;
    bool isChecked;
    WDFunc::ChBData(this, "xlsxchb", isChecked);
    if (isChecked)
        OscFileType |= MT_FT_XLSX;
    WDFunc::ChBData(this, "comtradechb", isChecked);
    if (isChecked)
        OscFileType |= MT_FT_COMTRADE;
    WDFunc::ChBData(this, "nonechb", isChecked);
    if (isChecked)
        OscFileType |= MT_FT_NONE;
    AcceptedOscFileType = true;
}

void OscDialog::Cancel()
{
    pc.Cancelled = true;
} */
