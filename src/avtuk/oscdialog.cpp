#include "oscdialog.h"

#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"
#include "QMessageBox"
#include "pushbuttondelegate.h"

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
constexpr int MT_FT_XLSX = 0x01;
constexpr int MT_FT_COMTRADE = 0x02;
constexpr int MT_FT_NONE = 0x04;

OscDialog::OscDialog(QWidget *parent) : UDialog(parent)
{
    connect(&DataManager::GetInstance(), &DataManager::oscInfoReceived, this, &OscDialog::fillOscInfo);
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, this, &OscDialog::fillOsc);
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

    tm = new ETableModel(this);
    tm->setHorizontalHeaderLabels({ "#", "Дата/Время", "ИД", "Длина", "Скачать" });

    tv->setModel(tm);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setMouseTracking(true);
    PushButtonDelegate *dg = new PushButtonDelegate(tv);
    connect(dg, &PushButtonDelegate::clicked, this, &OscDialog::GetOsc);
    tv->setItemDelegateForColumn(4, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца

    auto *getButton = WDFunc::NewPB(this, "", "Получить данные по осциллограммам ", this, [=] {
        counter = 0;
        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqOscInfo, 1);
    });

    if (StdFunc::IsInEmulateMode())
        getButton->setEnabled(false);
    hlyout->addWidget(getButton);
    auto *eraseButton = WDFunc::NewPB(this, "", "Стереть все осциллограммы в памяти", this, &OscDialog::EraseOsc);
    hlyout->addWidget(eraseButton);

    if (StdFunc::IsInEmulateMode())
        eraseButton->setEnabled(false);
    hlyout->addWidget(eraseButton);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv);
    setLayout(lyout);
}

void OscDialog::GetOsc(const QModelIndex &idx)
{
    emit StopCheckTimer();
    bool ok = false;
    int oscnum
        = idx.model()->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    quint32 oscsize = idx.model()->data(idx.sibling(idx.row(), Column::size), Qt::DisplayRole).toInt(&ok);
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }

    BaseInterface::iface()->reqFile(
        oscnum, Queries::FileFormat::CustomS2, oscsize + sizeof(S2DataTypes::DataRecHeader));
}

void OscDialog::EraseOsc()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, 1);
}

void OscDialog::fillOscInfo(DataTypes::OscInfo info)
{
    counter++;
    QVector<QVariant> lsl {
        QString::number(info.fileNum),               //
        TimeFunc::UnixTime64ToString(info.unixtime), //
        info.id,                                     //
        info.fileLength,                             //
        "Скачать",
    };

    tm->addRowWithData(lsl);
}

void OscDialog::fillOsc(const DataTypes::FileStruct file)
{
    quint16 curFileNum = quint16(file.filenum);
    quint16 minFileNum = quint16(DataTypes::FilesEnum::FileOscMin);
    quint16 maxFileNum = quint16(DataTypes::FilesEnum::FileOscMax);

    if ((curFileNum < minFileNum) || (curFileNum > maxFileNum))
        return;
    manager.loadOsc(file.filedata);
}
