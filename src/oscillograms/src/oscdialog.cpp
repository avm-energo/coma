#include "oscillograms/oscdialog.h"

#include <device/current_device.h>
#include <gen/datatypes.h>
#include <gen/files.h>
#include <gen/timefunc.h>
#include <models/etablemodel.h>
#include <s2/s2util.h>
#include <widgets/etableview.h>
#include <widgets/pushbuttondelegate.h>
#include <widgets/wd_func.h>

OscDialog::OscDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    m_dataUpdater->currentConnection()->connection(this, &OscDialog::fillOscInfo);
    m_dataUpdater->currentConnection()->connection(this, &OscDialog::fillOsc);
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
        engine()->currentConnection()->writeCommand(Commands::C_ReqOscInfo, 1);
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
        engine()->currentConnection()->reqFile(
            reqOscNum, DataTypes::FileFormat::CustomS2, size + sizeof(S2::DataRecHeader));
}

void OscDialog::eraseOsc()
{
    if (checkPassword())
        engine()->currentConnection()->writeCommand(Commands::C_EraseTechBlock, 1);
}

QString OscDialog::filename(quint64 time, quint32 oscNum) const
{
    QString filename = StdFunc::GetSystemHomeDir();
    filename.push_back(m_device->getUID());
    filename.push_back("-");
    filename.push_back(QString::number(m_device->getDeviceType(), 16));
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

    if (swjFile.size() < (size - sizeof(S2::SwitchJourRecord)))
        return false;

    int ret = QMessageBox::question(this, "Кэширование", "Прочитать из кэша");
    if ((ret == QMessageBox::StandardButton::Ok) || (ret == QMessageBox::StandardButton::Yes))
    {
        if (Files::LoadFromFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Swj loaded from file: " << file;
            DataTypes::S2FilePack outlist;
            S2Util::RestoreData(ba, outlist);
            for (auto &&s2file : outlist)
            {
                S2::FileStruct resp { S2::FilesEnum(s2file.ID), s2file.data };
                fillOsc(resp);
            }
            return true;
        }
        else
            qWarning() << "Failed to load swj from file: " << file;
    }
    return false;
}

void OscDialog::fillOscInfo(const S2::OscInfo &info)
{
    oscMap.insert(info.typeHeader.id, info);
    QVector<QVariant> lsl {
        QString::number(info.typeHeader.id),                      //
        TimeFunc::UnixTime64ToInvStringFractional(info.unixtime), //
        info.idOsc0,                                              //
        info.typeHeader.numByte,                                  //
        "Скачать",
    };
    tableModel->addRowWithData(lsl);
}

void OscDialog::fillOsc(const S2::FileStruct &file)
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
        S2Util::StoreDataMem(ba, fileBuffer, reqOscNum);
        auto time = oscMap.value(reqOscNum).unixtime;
        QString sfile = filename(time, reqOscNum);
        if (Files::SaveToFile(sfile, ba) == Error::Msg::NoError)
            qInfo() << "Swj saved: " << sfile;
        else
            qWarning() << "Fail to save swj: " << sfile;
    }
}
