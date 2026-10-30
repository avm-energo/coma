#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::Firmwares).
class XmlFirmwareDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlFirmwareDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;

protected:
    /// \brief Проверяет имя модуля и формат версии.
    bool checkDataBeforeSaving(const QStringList &savedData) override;
};
