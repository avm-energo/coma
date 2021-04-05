#include "console.h"

#include "../../gen/datamanager.h"

#include <QDebug>
#include <QMetaMethod>

template <typename T> void print(const T &data)
{
    qDebug() << data;
}

Console::Console(QObject *parent) : QObject(parent)
{
    const auto &manager = DataManager::GetInstance();

    connect(&manager, &DataManager::bitStringReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::dataReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::singlePointReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::floatReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::fileReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::responseReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::blockReceived, [this](const auto &st) { print(st); });
    connect(&manager, &DataManager::oscInfoReceived, [this](const auto &st) { print(st); });
}
