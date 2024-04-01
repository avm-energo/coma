#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::Section).
class XmlSGroupDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlSGroupDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
