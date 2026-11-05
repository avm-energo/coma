#pragma once

#include "xml/xmlparser/xmlparser.h"

namespace Device
{
class CurrentDevice;
} // namespace Device

namespace Xml
{
/// \brief Class for parsing "s2files.xml" file.
class XmlS2Parser final : public XmlParser
{
    Q_OBJECT
public:
    explicit XmlS2Parser(QObject *parent = nullptr);
    /// \brief Выбирает файл s2files под версию ВПО устройства.
    /// \details В узле <firmwares> файла s2files.xml тег fw_<модуль> (например, <fw_a243>) хранит минимальную версию ВПО,
    /// с которой работает этот файл. Если ВПО устройства старее, берётся последний (по имени) файл s2files*.xml
    /// из каталога old, у которого версия в этом теге не выше версии ВПО. Если тега нет (версия не ограничена)
    /// или подходящего файла нет, используется s2files.xml.
    explicit XmlS2Parser(const Device::CurrentDevice *device, QObject *parent = nullptr);
    /// \brief Парсинг всех нод <record> файла s2files.xml.
    void parse();

signals:

private:
    /// \brief Возвращает версию из тега fw_<tagName> узла <firmwares> (0 и false, если тега нет).
    static bool readMinVersion(const QDomElement &root, const QString &tagName, quint32 &version);

    QDomElement m_content;
};

}
