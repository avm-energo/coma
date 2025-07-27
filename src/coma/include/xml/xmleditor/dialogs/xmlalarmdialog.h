#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования элементов из XmlDataModel
/// (ModelType::AlarmsCrit, ModelType::AlarmsWarn и ModelType::AlarmsInfo).
class XmlAlarmDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlAlarmDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
