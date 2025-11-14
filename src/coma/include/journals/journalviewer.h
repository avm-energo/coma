#pragma once

#include <device/xml_settings.h>
#include <journals/basejournal.h>
#include <s2/s2datatypes.h>

#include <QDialog>

namespace journals
{

using MeasSettings = Device::XmlDataTypes::MeasJourList;
using WorkSettings = Device::XmlDataTypes::WorkJourMap;
using BinaryType = Device::XmlDataTypes::BinaryType;

/// \brief Просмотрщик журналов.
/// \details Класс для отображения журналов в бинарном формате.
class JournalViewer : public QDialog
{
    Q_OBJECT
private:
    UniquePointer<BaseJournal> m_journal;
    WorkSettings m_workSettings;
    MeasSettings m_measSettings;
    u16 m_typeB, m_typeM, m_fname;

    /// \brief Парсинг и отображение полученного файла журнала.
    void showJournal(const S2::S2BFile &file);

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
    void setupUI(const S2::S2BFile &file);

private slots:
    /// \brief Слот для сохранения настроек рабочего журнала.
    void workDataReceived(const u32 id, const QString &desc);
    /// \brief Слот для сохранения настроек журнала измерений.
    void measDataReceived(const u32 index, const QString &header, const journals::BinaryType type, bool visib);
    /// \brief Слот для сохранения журнала в Excel-таблицу.
    void saveExcelJournal();
    /// \bried Get journal type and module type slot
    void jnAndModuleTypesReceived(u16 mTypeB, u16 mTypeM, u16 fname);

public:
    explicit JournalViewer(const QString &filepath, JournalType type, QWidget *parent = nullptr);
};

}
