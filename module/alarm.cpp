#include "alarm.h"

#include "../gen/board.h"

Alarm::Alarm(QWidget *parent) : QWidget(parent)
{
}

void Alarm::Update(QList<bool> states)
{
    if (states.isEmpty())
        return;
    auto max_range = states.length();
    auto const &board = Board::GetInstance();
    if (Alarm->MapAlarm.value(board.type()).avarCounts != states.length())
    {
        qDebug() << Alarm->MapAlarm.value(board.type()).avarCounts << ":" << states.length();
        max_range = std::min(Alarm->MapAlarm.value(board.type()).avarCounts, states.length());
    }
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i);
    }
}
