#pragma once

#include <QString>
#include <QtXml>

namespace FileHelper
{

/// \brief Function for helping Files::ChooseFileForSave calling
/// \details This function wraps a class method calling
/// Board::GetInstance and getting its members typeM and typeB
/// for using result string in Files::ChooseFileForSave
QString ChooseFileForSave(const QString &ext);

/// \brief Checking whether a file with the specified name exists in the user's local folder.
bool isFileExist(const QString &filename);

/// \brief Returns QDomDocument for a file with the specified filename from user's local folder.
QDomDocument getFileContent(const QString &filename);

}
