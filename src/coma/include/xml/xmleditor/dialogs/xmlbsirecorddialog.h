#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Диалог создания и редактирования записей BSI/BsiExt (ModelType::BsiRecords и BsiExtRecords).
class XmlBsiRecordDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlBsiRecordDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
