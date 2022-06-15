#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QString>

namespace FileHelper
{

/// \brief Function for helping Files::ChooseFileForSave calling
/// \details This function wraps a class method calling
/// Board::GetInstance and getting its members typeM and typeB
/// for using result string in Files::ChooseFileForSave
QString ChooseFileForSave(const QString &ext);

}

#endif // FILEHELPER_H
