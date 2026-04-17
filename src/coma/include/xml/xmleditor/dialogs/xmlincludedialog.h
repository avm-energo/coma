#pragma once

#include "xmldialog.h"

/// \brief Класс диалога для создания и редактирования элементов
/// из XmlDataModel (ModelType::Includes) - узлы <include src="..."/>.
class XmlIncludeDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlIncludeDialog(QWidget *parent = nullptr);

protected:
    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;

    /// \brief Переопределение для нормализации выбранного пути относительно configDir.
    [[nodiscard]] QStringList collectData() override;
};
