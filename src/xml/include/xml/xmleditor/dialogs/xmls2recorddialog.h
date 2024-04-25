#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlHideDataModel (ModelType::S2Records).
class XmlS2RecordDialog final : public XmlDialog
{
    Q_OBJECT
private:
    static const QStringList s_dataTypes;
    static const QStringList s_widgetTypes;

    QGroupBox *m_widgetEdit;

    /// \brief Сохраняет полученные из модели данные для отображения в UI.
    void loadModelData(const QStringList &response) override;

    void createWidgetEditBox();

public:
    explicit XmlS2RecordDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
