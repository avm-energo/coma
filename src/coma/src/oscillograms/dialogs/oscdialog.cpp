#include <device/current_device.h>
#include <gen/datatypes.h>
#include <gen/files.h>
#include <gen/settings.h>
#include <gen/timefunc.h>
#include <models/etablemodel.h>
#include <oscillograms/dialogs/oscdialog.h>
#include <oscillograms/dialogs/trendviewdialog.h>
#include <s2/s2util.h>
#include <avm-widgets/emessagebox.h>
#include <avm-widgets/etableview.h>
#include <avm-widgets/pbfunc.h>
#include <avm-widgets/pushbuttondelegate.h>

#include <QLayout>

OscDialog::OscDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    setupUI();
}

void OscDialog::disableResponseConnections()
{
    QObject::disconnect(m_fillOscInfoConnection);
    QObject::disconnect(m_fillOscConnection);
}

void OscDialog::enableResponseConnections()
{
    if (!m_fillOscInfoConnection)
        m_fillOscInfoConnection = m_dataUpdater->currentConnection()->connection(this, &OscDialog::fillOscInfo);
    if (!m_fillOscConnection)
        m_fillOscConnection = m_dataUpdater->currentConnection()->connection(this, &OscDialog::fillOsc);
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

    auto *getButton = PBFunc::New(this, "", "Получить данные по осциллограммам ", this,
        [&, tv]
        {
            m_oscMap.clear();
            m_tableModel = UniquePointer<ETableModel>(new ETableModel);
            m_tableModel->setHorizontalHeaderLabels({ "#", "Дата/Время", "ИД", "Длина", "Скачать" });

            tv->setModel(m_tableModel.get());
            engine()->currentConnection()->writeCommand(Commands::C_ReqOscInfo, 1);
        });

    hlyout->addWidget(getButton);
    auto *eraseButton = PBFunc::New(this, "", "Стереть все осциллограммы в памяти", this, &OscDialog::eraseOsc);
    hlyout->addWidget(eraseButton);

    hlyout->addWidget(eraseButton);
    lyout->addLayout(hlyout);
    lyout->addWidget(tv);
    setLayout(lyout);
}

void OscDialog::getOsc(const QModelIndex &idx)
{
    m_fileBuffer.clear();

    auto model = idx.model();
    if (!model)
    {
        qWarning() << Error::DescError;
        return;
    }
    bool ok = false;
    // номер осциллограммы
    m_reqOscNum = model->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toInt(&ok);
    quint32 size = idx.model()->data(idx.sibling(idx.row(), Column::size), Qt::DisplayRole).toInt(&ok);
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }
    if (!loadIfExist(size))
        engine()->currentConnection()->reqFile(
            m_reqOscNum, DataTypes::FileFormat::CustomS2, size + sizeof(S2::DataRecHeader));
}

void OscDialog::eraseOsc()
{
    if (checkPassword())
        engine()->currentConnection()->writeCommand(Commands::C_EraseTechBlock, 1);
}

QString OscDialog::filename(quint64 time, quint32 oscNum) const
{
    QString filename = Settings::workDir();
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
    auto time = m_oscMap.value(m_reqOscNum).unixtime;
    auto file = filename(time, m_reqOscNum);
    QByteArray ba;

    QFile swjFile(file);

    if (!swjFile.exists())
        return false;

    if (swjFile.size() < (size - sizeof(S2::SwitchJourRecord)))
        return false;

    if (EMessageBox::question(this, "Прочитать из кэша"))
    {
        if (Files::LoadFromFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Файл swj загружен: " << file;
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
    m_oscMap.insert(info.typeHeader.id, info);
    QVector<QVariant> lsl {
        QString::number(info.typeHeader.id),                      //
        TimeFunc::UnixTime64ToInvStringFractional(info.unixtime), //
        info.idOsc0,                                              //
        info.typeHeader.numByte,                                  //
        "Скачать",
    };
    m_tableModel->addRowWithData(lsl);
}

void OscDialog::fillOsc(const S2::FileStruct &file)
{
    if (!updatesEnabled())
        return;

    switch (file.ID)
    {
    case MT_HEAD_ID:
    {
        auto header = m_manager.loadCommon(file);
        m_manager.setHeader(header);
        break;
    }
    // ignore swj here
    case AVTUK_85::SWJ_ID:
    {
        return;
    }
    default:
    {
        TrendViewDialog *dlg = new TrendViewDialog;
        dlg->loadOsc(file);
    }
    }
    m_fileBuffer.push_back(file);
    // header, osc
    if (m_fileBuffer.size() == 2)
    {
        QByteArray ba;
        S2Util::StoreDataMem(ba, m_fileBuffer, m_reqOscNum);
        auto time = m_oscMap.value(m_reqOscNum).unixtime;
        QString sfile = filename(time, m_reqOscNum);
        if (Files::SaveToFile(sfile, ba) == Error::Msg::NoError)
            qInfo() << "Файл swj сохранён: " << sfile;
        else
            qWarning() << "Ошибка при сохранении swj-файла: " << sfile;
    }
}
