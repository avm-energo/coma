#pragma once

#include "xmldialog.h"

#include <QList>

/// \brief Диалог-предложение создания нового дочернего узла <resources>.
/// \details Показывает комбобокс с человекочитаемыми заголовками (из
///          XmlDialogFabric::s_dialogTitles) тех типов, что разрешены как
///          дочерние узлы <resources> (s_resourceCreatableTypes). Наследуется
///          от XmlDialog, поэтому сохранение идёт через стандартный поток:
///          collectData → createData → BaseEditorModel::create.
class XmlResMemberCreateDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlResMemberCreateDialog(QWidget *parent = nullptr);

protected:
    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;

    /// \brief Возвращает данные для вставки в родительскую модель <resources>:
    ///        col 0 — имя тега (nodeName), col 1 — пустое описание.
    [[nodiscard]] QStringList collectData() override;

private:
    QList<ModelType> m_types; ///< Индексы соответствуют индексам элементов комбобокса.
};
