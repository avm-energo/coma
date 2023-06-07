#pragma once

#include "../module/modulesettings.h"
#include "basejournal.h"

#include <QDialog>

namespace journals
{

class JournalViewer : public QDialog
{
    Q_OBJECT
private:
    UniquePointer<BaseJournal> journal;
    ModuleTypes::WorkJourMap workSettings;
    ModuleTypes::MeasJourList measSettings;

    BaseJournal *createJournal(const JournalType type, const quint16 typeB, const quint16 typeM);
    void parseSettings(const quint16 typeB, const quint16 typeM);
    void setupUI(const DataTypes::FileStruct &file);

private slots:
    void workDataReceived(const quint32 id, const QString &desc);
    void measDataReceived(const quint32 index, const QString &header, const ModuleTypes::BinaryType type, bool visib);

public:
    explicit JournalViewer(const QString &filepath, QWidget *parent = nullptr);
};

}
