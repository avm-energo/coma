#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

/// \brief Диалог для создания и редактирования записей оверлея (ModelType::OverlayRecords).
/// \details Позволяет выборочно переопределять поля записи: имя, тип данных,
/// флаг наладки и виджет. Не переопределённые поля не записываются в XML.
class XmlOverlayRecordDialog final : public XmlDialog
{
    Q_OBJECT
private:
    static const QStringList s_dataTypes;
    static const QStringList s_widgetTypes;

    QGroupBox *m_nameEdit;
    QGroupBox *m_dtypeEdit;
    QGroupBox *m_typeEdit;
    QGroupBox *m_widgetEdit;

    QMap<QString, int> m_s2TabsMap;

    /// \brief Сохраняет полученные из модели данные для отображения в UI.
    void loadModelData(const QStringList &response) override;

    /// \brief Получает данные по вкладкам S2Tabs.
    bool loadS2TabsData();

    /// \brief Парсинг узла <config-tabs>.
    void parseConfigTab(const QDomNode &tabNode);

    /// \brief Сбор данных виджетов диалога в QStringList (20 элементов).
    QStringList collectData() override;

    /// \brief Создание группы редактирования виджета записи.
    void createWidgetEditBox(QVBoxLayout *mainLayout);

public:
    explicit XmlOverlayRecordDialog(QWidget *parent = nullptr);

    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;
};
