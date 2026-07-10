#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

#include <QDialog>
#include <QObject>

class XmlOverlayDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlOverlayDialog(QWidget *parent = nullptr);
    /// \brief Создаёт и настраивает UI диалога.
    void setupUI(QVBoxLayout *mainLayout) override;

private:
    static const QStringList s_dataTypes;
    static const QStringList s_widgetTypes;
    QMap<QString, int> m_s2TabsMap;
    QMap<QString, QVariant> m_values;

    QGroupBox *m_widgetEdit;

    /// \brief Сохраняет полученные из модели данные для отображения в UI.
    void loadModelData(const QStringList &response) override;

    /// \brief Получает данные по вкладкам S2Tabs
    bool loadS2TabsData();

    /// \brief Создание редактирования виджета для записи
    void createWidgetEditBox();

    /// \brief Парсинг узла <config-tabs>
    void parseConfigTab(const QDomNode &tabNode);

private slots:
    void idChanged(double value);
    void nameChanged(const QString &name);
    void datatypeChanged(const int &index);
    void dtypeChanged(Qt::CheckState state);
    void classnameChanged(const QString &classname);
    void widgettypeChanged(int index);
    void tabidChanged(const int &id);
    void stringChanged(const QString &string);
    void tooltipChanged(const QString &string);
    void parentidChanged(const int &id);
    void countChanged(const int &count);
    void minvalueChanged(const int &value);
    void maxvalueChanged(const int &value);
    void decimalsChanged(const int &value);
    void fieldChanged(const QString &string);
    void strarrayChanged(const QString &string);
};
