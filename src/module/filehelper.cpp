#include "filehelper.h"

#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "board.h"

QString FileHelper::ChooseFileForSave(const QString &ext)
{
    const auto &board = Board::GetInstance();
    QString MTypeM = (board.typeM() == 0) ? "00" : QString::number(board.typeM(), 16);
    QString res = StdFunc::GetHomeDir() + "/" + QString::number(board.typeB(), 16) + MTypeM + "-"
        + QString("%1").arg(board.serialNumber(Board::BaseMezz), 8, 10, QChar('0')) + "." + ext;
    return res;
}
