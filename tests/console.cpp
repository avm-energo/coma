#include "console.h"

#include "../gen/datamanager.h"

template <typename T> void print(const T &data)
{
    qDebug() << data;
}

Console::Console(QObject *parent) : QObject(parent)
{
    const auto &manager = DataManager::GetInstance();
    connect(&manager, &DataManager::bitStringReceived, [this](const DataTypes::BitStringStruct &st) { print(st); });
    connect(&manager, &DataManager::dataReceived, [this](const DataTypes::SignalsStruct &st) { print(st); });
    connect(&manager, &DataManager::singlePointReceived,
        [this](const DataTypes::SinglePointWithTimeStruct &st) { print(st); });
    connect(&manager, &DataManager::floatReceived, [this](const DataTypes::FloatStruct &st) { print(st); });
    connect(&manager, &DataManager::fileReceived, [this](const DataTypes::FileStruct &st) { print(st); });
    connect(&manager, &DataManager::confParametersListReceived,
        [this](const DataTypes::ConfParametersListStruct &st) { print(st); });
    connect(
        &manager, &DataManager::responseReceived, [this](const DataTypes::GeneralResponseStruct &st) { print(st); });
    connect(&manager, &DataManager::blockReceived, [this](const DataTypes::BlockStruct &st) { print(st); });
}
