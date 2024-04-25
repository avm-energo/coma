#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlHideDataModel (ModelType::S2Records).
class XmlS2RecordDialog final : public XmlDialog
{
    Q_OBJECT
private:
    static const QStringList s_dataTypes;

public:
    explicit XmlS2RecordDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
