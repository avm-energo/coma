#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::AlarmStateAll).
class XmlAlarmStateAllDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlAlarmStateAllDialog(QWidget *parent);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
