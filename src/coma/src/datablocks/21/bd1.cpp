#include "datablocks/21/bd1.h"

Bd21_1::Bd21_1(QObject *parent) : DataBlock(parent)
{
    m_blockData = new BlockData;
    setBlock({ 1, "Все", DataTypes::DataBlockTypes::BdBlock, m_blockData, sizeof(BlockData), false });
}

void Bd21_1::setupValuesDesc()
{
    addNewGroup("Тек. измерения, приведённые ко входу (ток)", "bdinI", 0, 8, &m_blockData->inI[0], 5);
    addNewGroup("Тек. измерения, приведённые ко входу (напряжения)", "bdinU", 0, 8, &m_blockData->inU[0], 5);
    addNewGroup("Тек. измерения в инженерных единицах", "bdain", 0, 8, &m_blockData->ain[0], 5);
    addNewGroup("Текущие минимумы по каналам", "bdainn", 0, 8, &m_blockData->ainn[0], 5);
    addNewGroup("Текущие максимумы по каналам", "bdainx", 0, 8, &m_blockData->ainx[0], 5);
}
