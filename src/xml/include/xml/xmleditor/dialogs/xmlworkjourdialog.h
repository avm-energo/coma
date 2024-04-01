#pragma once

#include "xmldialog.h"

/// \brief Класс диалога для создания и редактирования
/// элементов из XmlDataModel (ModelType::WorkJours).
class XmlWorkJourDialog final : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlWorkJourDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
