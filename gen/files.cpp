#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QStorageInfo>

#include "error.h"
#include "files.h"
#include "modulebsi.h"
#include "stdfunc.h"

Files::Files()
{
}

QString Files::ChooseFileForOpen(QWidget *parent, QString mask)
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(
        parent, "Открыть файл", StdFunc::GetHomeDir(), mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    return filename;
}

int Files::LoadFromFile(const QString &filename, QByteArray &ba)
{
    if (filename.isEmpty())
    {
        ERMSG("Пустое имя файла");
        return ER_FILEOPEN; // Пустое имя файла
    }
    QFile *file = new QFile;
    file->setFileName(filename);
    if (!file->open(QIODevice::ReadOnly))
    {
        ERMSG("Ошибка открытия файла");
        return ER_FILEOPEN; // Ошибка открытия файла
    }
    ba = file->readAll();
    file->close();
    return ER_NOERROR;
}

// Input: QString mask: описание файлов, например: "Файлы журналов (*.swj)"; QString ext - расширение по умолчанию
// Output: QString filename

QString Files::ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr)
{
    QString MTypeM = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == 0)
        ? "00"
        : QString::number(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN), 16);
    QString tmps;
    if (filenamestr.isEmpty())
        tmps = StdFunc::GetHomeDir() + "/" + QString::number(ModuleBSI::GetMType(BoardTypes::BT_BASE), 16) + MTypeM
            + "-" + QString("%1").arg(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 8, 10, QChar('0')) + "." + ext;
    else
        tmps = filenamestr;
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename
        = dlg->getSaveFileName(parent, "Сохранить файл", tmps, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    return filename;
}

int Files::SaveToFile(const QString &filename, QByteArray &src, unsigned int numbytes)
{
    if (filename.isEmpty())
        return ER_FILENAMEEMP; // Пустое имя файла
    QFile *file = new QFile;
    file->setFileName(filename);
    if (!file->open(QIODevice::WriteOnly))
        return ER_FILEOPEN; // Ошибка открытия файла
    int res = file->write(src, numbytes);
    file->close();
    delete file;
    if (res == GENERALERROR)
        return ER_FILEWRITE; // ошибка записи
    return ER_NOERROR; // нет ошибок
}

QStringList Files::Drives()
{
    QStringList sl;
    QFileInfoList list = QDir::drives();
    foreach (QFileInfo fi, list)
        sl << fi.path();
    return sl;
}

QStringList Files::SearchForFile(QStringList &filepaths, const QString &filename, bool subdirs)
{
    QStringList files;
    foreach (QString filepath, filepaths)
    {
        QStringList sl = QStringList() << filename;
        QDirIterator it(filepath, sl, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot,
            (subdirs) ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
        while (it.hasNext())
            files << it.next();
    }
    return files;
}

QString Files::GetFirstDriveWithLabel(QStringList &filepaths, const QString &label)
{
    QString str = "";
    int i = 0;
    while ((str.isEmpty()) && (i < filepaths.size()))
    {
        QStorageInfo si;
        si.setPath(filepaths.at(i));
        if (si.name() == label)
            str = filepaths.at(i);
        ++i;
    }
    return str;
}
