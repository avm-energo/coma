#pragma once

#include "xmldialog.h"

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::SectionTabs).
class XmlSectionTabDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlSectionTabDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
