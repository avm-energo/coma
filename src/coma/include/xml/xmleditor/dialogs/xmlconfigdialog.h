#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

class QComboBox;

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::Config).
class XmlConfigDialog final : public XmlDialog
{
    Q_OBJECT
private:
    /// \brief Populates \a combo with names from s2files.xml records.
    void loadS2RecordsData(QComboBox *combo);

public:
    explicit XmlConfigDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
