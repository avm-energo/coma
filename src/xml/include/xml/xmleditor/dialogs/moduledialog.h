#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Класс диалога для создания и редактирования элементов из MasterModel.
class ModuleDialog final : public XmlDialog
{
    Q_OBJECT
private:
    QList<QPair<QString, QString>> m_dlgSettings;

public:
    explicit ModuleDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
    /// \brief Собирает данные с виджетов для отправки в модель.
    [[nodiscard]] QStringList collectData() override;

public slots:
    /// \brief Сохраняет полученные из модели данные для отображения в UI.
    void loadModelData(const QStringList &response) override;
};
