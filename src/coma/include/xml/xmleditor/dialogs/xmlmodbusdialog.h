#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::Modbus).
class XmlModbusDialog final : public XmlDialog
{
    Q_OBJECT
private:
    /// \brief Конвертация строки в тип, хранимый внутри XML.
    QString convertStringToType(const QString &namedTransType);
    /// \brief Конвертация типа в индекс комбо-бокса.
    int convertTypeToIndex(const QString &transTypeNum);

    /// \brief Сохраняет полученные из модели данные для отображения в UI.
    void loadModelData(const QStringList &response) override;

public:
    explicit XmlModbusDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
    /// \brief Собирает данные с виджетов для отправки в модель.
    [[nodiscard]] QStringList collectData() override;
};
