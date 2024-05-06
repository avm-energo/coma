#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::HiddenTab).
class XmlHiddenWidgetDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlHiddenWidgetDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
