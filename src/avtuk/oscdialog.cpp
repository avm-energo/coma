#include "oscdialog.h"

#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "../models/etablemodel.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"
#include "pushbuttondelegate.h"

OscDialog::OscDialog(QWidget *parent) : UDialog(parent)
{
    connect(&DataManager::GetInstance(), &DataManager::oscInfoReceived, this, &OscDialog::fillOscInfo);
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, this, &OscDialog::fillOsc);
    setupUI();
}

void OscDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    ETableView *tv = new ETableView;

    tableModel = new ETableModel(this);
    tableModel->setHorizontalHeaderLabels({ "#", "Дата/Время", "ИД", "Длина", "Скачать" });

    tv->setModel(tableModel);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setMouseTracking(true);
    PushButtonDelegate *dg = new PushButtonDelegate(tv);
    connect(dg, &PushButtonDelegate::clicked, this, &OscDialog::getOsc);
    tv->setItemDelegateForColumn(Column::download, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца

    auto *getButton = WDFunc::NewPB(this, "", "Получить данные по осциллограммам ", this, [=] {
        counter = 0;
        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqOscInfo, 1);
    });

    hlyout->addWidget(getButton);
    auto *eraseButton = WDFunc::NewPB(this, "", "Стереть все осциллограммы в памяти", this, &OscDialog::eraseOsc);
    hlyout->addWidget(eraseButton);

    if (StdFunc::IsInEmulateMode())
        eraseButton->setEnabled(false);
    hlyout->addWidget(eraseButton);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv);
    setLayout(lyout);
}

void OscDialog::getOsc(const QModelIndex &idx)
{

    auto model = idx.model();
    if (!model)
    {
        qWarning() << Error::DescError;
        return;
    }
    bool ok = false;
    int oscnum = model->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    quint32 oscsize = idx.model()->data(idx.sibling(idx.row(), Column::size), Qt::DisplayRole).toInt(&ok);
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }

    BaseInterface::iface()->reqFile(
        oscnum, Queries::FileFormat::CustomS2, oscsize + sizeof(S2DataTypes::DataRecHeader));
}

void OscDialog::eraseOsc()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, 1);
}

void OscDialog::fillOscInfo(S2DataTypes::OscInfo info)
{
    counter++;
    QVector<QVariant> lsl {
        QString::number(info.typeHeader.id),         //
        TimeFunc::UnixTime64ToString(info.unixtime), //
        info.idOsc0,                                 //
        info.typeHeader.numByte,                     //
        "Скачать",
    };

    tableModel->addRowWithData(lsl);
}

void OscDialog::fillOsc(const DataTypes::FileStruct file)
{
    if (!updatesEnabled())
        return;

    switch (file.ID)
    {
    case MT_HEAD_ID:
    {
        auto header = manager.loadCommon(file);
        manager.setHeader(header);
        break;
    }
    case AVTUK_85::SWJ_ID:
    {
        break;
    }
    default:
    {

        auto model = manager.load(file);

        if (!model)
        {
            qWarning() << Error::ReadError;
            return;
        }
        manager.loadOsc(model.get());
    }
    }
}
