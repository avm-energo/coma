#include "datamanager.h"

QMutex DataManager::s_inQueueMutex;
std::queue<QVariant> DataManager::s_inputQueue;

DataManager::DataManager(Singleton::token, QObject *parent) : QObject(parent)
{
}
