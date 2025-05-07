#include <gen/settings.h>
#include <gen/stdfunc.h>
#include <widgets/filefunc.h>

#include <QFileDialog>

FileFunc::FileFunc() { }

QString FileFunc::ChooseFileForOpen(QWidget *parent, QString mask)
{
    auto workPath = Settings::workDir();
    auto dlg = new QFileDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    auto filename = dlg->getOpenFileName(parent, "Открыть файл", workPath, mask, Q_NULLPTR);
    saveWorkDir(filename);
    dlg->close();
    return filename;
}

QString FileFunc::ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext, const QString &filename)
{
    auto workPath = Settings::workDir();
    auto dlg = new QFileDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    auto fullpath = workPath + "/" + filename + "." + ext;
    auto filepath = dlg->getSaveFileName(parent, "Сохранить файл", fullpath, mask, Q_NULLPTR);
    saveWorkDir(filepath);
    dlg->close();
    return filepath;
}

QString FileFunc::ChooseFileForSave(UDialog *parent, const QString &mask, const QString &ext)
{
    return FileFunc::ChooseFileForSave(parent, mask, ext, parent->getFilenameForDevice());
}

QString FileFunc::ChooseDirectoryForOpen(QWidget *parent)
{
    auto workPath = Settings::dataDir();
    auto dlg = new QFileDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::Directory);
    dlg->setViewMode(QFileDialog::Detail);
    auto dirPath = dlg->getExistingDirectory(parent, "Выбрать папку с файлами", workPath);
    saveWorkDir(dirPath);
    dlg->close();
    return dirPath;
}

void FileFunc::saveWorkDir(const QString &name)
{
    if (!name.isEmpty())
    {
        QFileInfo info(name);
        Settings::setWorkDir(info.absolutePath());
    }
}
