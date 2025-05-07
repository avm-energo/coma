#pragma once

#include <widgets/udialog.h>

#include <QString>
#include <QWidget>

class FileFunc
{
public:
    FileFunc();

    [[nodiscard]] static QString ChooseFileForOpen(QWidget *parent, QString mask);
    /// Input: QString mask: описание файлов, например: "Файлы журналов (*.swj)";
    /// QString ext - расширение по умолчанию Output: QString filename
    [[nodiscard]] static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filename);
    [[nodiscard]] static QString ChooseFileForSave(UDialog *parent, const QString &mask, const QString &ext);
    [[nodiscard]] static QString ChooseDirectoryForOpen(QWidget *parent);
    static void saveWorkDir(const QString &filename);
};
