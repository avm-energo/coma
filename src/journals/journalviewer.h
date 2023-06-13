#pragma once

#include "../module/modulesettings.h"
#include "basejournal.h"

#include <QDialog>

namespace journals
{

/// \brief Просмотрщик журналов.
/// \details Класс для отображения журналов в бинарном формате.
class JournalViewer : public QDialog
{
    Q_OBJECT
private:
    UniquePointer<BaseJournal> journal;
    ModuleTypes::WorkJourMap workSettings;
    ModuleTypes::MeasJourList measSettings;

    /// \brief Создание журнала в зависимости от его типа из файла журнала в формате S2B.
    /// \details Если получен рабочий журнал или журнал измерений, то следует распарсить
    /// настройки журналов из XML-конфигурации (узел <journals>) для соответствующего модуля.
    /// Тип базовой и мезонниной плат хранятся в файле журнала в формате S2B.
    /// \see parseSettings
    BaseJournal *createJournal(const JournalType type, const quint16 typeB, const quint16 typeM);

    /// \brief Метод для получения настроек из XML-конфигурации устройства.
    /// \details Вызывается, если требуется открыть рабочий журнал или журнал измерений.
    /// \see createJournal
    void parseSettings(const quint16 typeB, const quint16 typeM);

    /// \brief Метод для создания UI просмотрщика журналов.
    void setupUI(const DataTypes::FileStruct &file);

private slots:
    /// \brief Слот для сохранения настроек рабочего журнала.
    void workDataReceived(const quint32 id, const QString &desc);
    /// \brief Слот для сохранения настроек журнала измерений.
    void measDataReceived(const quint32 index, const QString &header, const ModuleTypes::BinaryType type, bool visib);

public:
    explicit JournalViewer(const QString &filepath, QWidget *parent = nullptr);
};

}
