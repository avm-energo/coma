#include "oscdialog.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/timefunc.h"
#include "../models/etablemodel.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"
#include "pushbuttondelegate.h"

namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "oscHash";
}

OscDialog::OscDialog(QWidget *parent) : UDialog(crypto::hash, crypto::name, parent)
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

    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setMouseTracking(true);
    PushButtonDelegate *dg = new PushButtonDelegate(tv);
    connect(dg, &PushButtonDelegate::clicked, this, &OscDialog::getOsc);
    tv->setItemDelegateForColumn(Column::download, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца

    auto *getButton = WDFunc::NewPB(this, "", "Получить данные по осциллограммам ", this, [&, tv] {
        oscMap.clear();
        tableModel = UniquePointer<ETableModel>(new ETableModel);
        tableModel->setHorizontalHeaderLabels({ "#", "Дата/Время", "ИД", "Длина", "Скачать" });

        tv->setModel(tableModel.get());
        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqOscInfo, 1);
    });

    hlyout->addWidget(getButton);
    auto *eraseButton = WDFunc::NewPB(this, "", "Стереть все осциллограммы в памяти", this, &OscDialog::eraseOsc);
    hlyout->addWidget(eraseButton);

    hlyout->addWidget(eraseButton);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv);
    setLayout(lyout);
}

void OscDialog::getOsc(const QModelIndex &idx)
{
    fileBuffer.clear();

    auto model = idx.model();
    if (!model)
    {
        qWarning() << Error::DescError;
        return;
    }
    bool ok = false;
    // номер осциллограммы
    reqOscNum = model->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toInt(&ok);
    quint32 size = idx.model()->data(idx.sibling(idx.row(), Column::size), Qt::DisplayRole).toInt(&ok);
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }
    if (!loadIfExist(size))
        BaseInterface::iface()->reqFile(
            reqOscNum, Queries::FileFormat::CustomS2, size + sizeof(S2DataTypes::DataRecHeader));
}

void OscDialog::eraseOsc()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, 1);
}

QString OscDialog::filename(quint64 time, quint32 oscNum) const
{
    const auto &board = Board::GetInstance();
    QString filename = StdFunc::GetSystemHomeDir();
    filename.push_back(board.UID());
    filename.push_back("-");
    filename.push_back(QString::number(board.type(), 16));
    filename.push_back("-");
    filename.push_back(QString::number(oscNum));
    filename.push_back("-");
    filename.push_back(QString::number(time));
    filename.push_back(".osc");
    return filename;
}

bool OscDialog::loadIfExist(quint32 size)
{
    auto time = oscMap.value(reqOscNum).unixtime;
    auto file = filename(time, reqOscNum);
    QByteArray ba;

    QFile swjFile(file);

    if (!swjFile.exists())
        return false;

    if (swjFile.size() < (size - sizeof(S2DataTypes::SwitchJourRecord)))
        return false;

    int ret = QMessageBox::question(this, "Кэширование", "Прочитать из кэша");
    if ((ret == QMessageBox::StandardButton::Ok) || (ret == QMessageBox::StandardButton::Yes))
    {
        if (Files::LoadFromFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Swj loaded from file: " << file;
            DataTypes::S2FilePack outlist;
            S2::RestoreData(ba, outlist);
            for (auto &&file : outlist)
            {
                DataTypes::FileStruct resp { DataTypes::FilesEnum(file.ID), file.data };
                DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
            }
            return true;
        }
        else
        {
            qWarning() << "Failed to load swj from file: " << file;
        }
    }
    return false;
}

void OscDialog::fillOscInfo(S2DataTypes::OscInfo info)
{
    oscMap.insert(info.typeHeader.id, info);
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
    // ignore swj here
    case AVTUK_85::SWJ_ID:
    {
        return;
    }
    default:
    {

        oscModel = manager.load(file);

        if (!oscModel)
        {
            qWarning() << Error::ReadError;
            return;
        }
        manager.loadOsc(oscModel.get());
    }
    }
    fileBuffer.push_back(file);
    // header, osc
    if (fileBuffer.size() == 2)
    {
        QByteArray ba;
        S2::StoreDataMem(ba, fileBuffer, reqOscNum);
        auto time = oscMap.value(reqOscNum).unixtime;
        QString file = filename(time, reqOscNum);
        if (Files::SaveToFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Swj saved: " << file;
        }
        else
        {
            qWarning() << "Fail to save swj: " << file;
        }
    }
}
