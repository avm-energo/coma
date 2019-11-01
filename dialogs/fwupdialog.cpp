#include "fwupdialog.h"
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include <QFileDialog>
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/files.h"
#include "../widgets/etableview.h"
#include "../gen/s2.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

fwupdialog::fwupdialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
   // fwupdialog* fwup = new fwupdialog;
    SetupUI();
}

void fwupdialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("fwp");

    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Записать ПО в память "+tmps);
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFW()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, 1,1,1,1);

    //hlyout->addWidget(glyout,Qt::AlignTop);
    lyout->addLayout(glyout,Qt::AlignTop);
    lyout->addWidget(tv, 89);
    setLayout(lyout);

}

int fwupdialog::LoadFW()
{
    QByteArray ba;
    File_struct PV_file;
    //quint32 crc=0xFFFFFFFF;
    quint32 i;
    quint32 tmpi = 0;
    //void *Rptr = static_cast<void *>(&PV_file.Type);
    //tmpi = sizeof(PV_file.Type)+sizeof(PV_file.File.FileDatHeader);
    QVector<S2::DataRec> S2DR;

    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Program Version (*.hex)"), ba);
    if (res != Files::ER_NOERROR)
    {
        WARNMSG("Ошибка файла ПО");
        return Error::ER_GENERALERROR;
    }

    ParseHexToS2(ba);

    //PV_file.File_xxx_header.size=sizeof(PV_file.Type) + sizeof(PV_file.File.FileDatHeader) + sizeof(ba) + sizeof(PV_file.void_recHeader);

    PV_file.Type.TypeHeader.id = 8000;
    PV_file.Type.TypeHeader.NumByte = 8;

    PV_file.File.FileDatHeader.id = 8001;
    PV_file.File.FileDatHeader.NumByte = sizeof(ba);
    PV_file.File.Data = ba;

    PV_file.void_recHeader.id = 0xFFFFFFFF;
    PV_file.void_recHeader.NumByte = 0;

    S2DR.append({PV_file.Type.TypeHeader.id, PV_file.Type.TypeHeader.NumByte, nullptr});
    S2DR.append({PV_file.File.FileDatHeader.id, PV_file.File.FileDatHeader.NumByte, &PV_file.File.Data});
    S2DR.append({PV_file.void_recHeader.id, PV_file.void_recHeader.NumByte, nullptr});

    /*for(i=0;i<PV_file.File_xxx_header.size;i++)
    S2::updCRC32((static_cast<char *>(Rptr))[i],&crc);

    PV_file.File_xxx_header.crc32=crc;
    PV_file.File_xxx_header.thetime=S2::getTime32();
    PV_file.File_xxx_header.service=0xFFFF;
    PV_file.File_xxx_header.fname=0x003;*/

    #if PROGSIZE != PROGSIZE_EMUL
    if (Commands::WriteFile(&(BaForSend.data()[0]), 3, &S2DR) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Ошибка", "Ошибка записи в модуль!");
        return Error::ER_GENERALERROR;
    }
    EMessageBox::information(this, "Успешно", "Загрузка прошла успешно!");
    return Error::ER_NOERROR;
    #endif


}

int fwupdialog::ParseHexToS2(QByteArray ba)
{

    int i,j;
    QString str;
    QStringList sl;
    bool ok = false;

    str = ba;
    str.split(":");

    sl.append(str.split("\r\n:"));

    //str = str.toLocal8Bit();

    for(i = 0; i<sl.size(); i++)
    {
        str = sl.at(i);

        if(i == 0)
        {
          //ba.data()[0] = QString("%1").arg(str.toULongLong(&ok, 16), 16, 2, QChar('0'));
         // i++;
          str = QString("%1").arg(str.toULongLong(&ok, 2), 16, 2, QChar('0'));
          memcpy(&ba.data()[0], &str, 16);
          i++;
        }
    }

   return Error::ER_NOERROR;
}
