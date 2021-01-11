#include "../../interfaces/modbussettings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QTextCodec>
void parseInt32(QDomElement domElement)
{
    qDebug() << domElement.attribute("name", "") << domElement.text();
}

void parseStringList(QDomElement domElement)
{
    const auto &nodes = domElement.childNodes();
    QStringList description;
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
    while (i != nodes.count())
        qDebug() << nodes.item(i++).toElement().text();
}

void parseModbus(QDomElement domElement)
{

    qDebug() << domElement.text();
    qDebug() << "TagName: " << domElement.tagName();
    const auto &nodes = domElement.childNodes();
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
    // ModbusSettings settings;
    InterfaceInfo<CommandsMBS::ModbusGroup> settings;
    while (i != nodes.count())
    {
        const auto &group = nodes.item(i++).toElement();
        auto test = CommandsMBS::ModbusGroup(group);
        settings.addGroup(test);
        //    qDebug() << test;
        qDebug() << group.attribute("id", "") << group.text();
    }
    qDebug() << settings.groups().count();
}

bool parseModule(const QString &typea, const QString &typeb)
{
    qDebug() << typea << typeb;
    return true;
}
void traverseNode(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "quint32")
                {
                    //      qDebug() << "Attr: " << domElement.attribute("name", "")
                    //               << "\tValue: " << /*qPrintable*/ (domElement.text());
                    parseInt32(domElement);
                    // domNode = domNode.nextSibling();
                    // break;
                }
                if (domElement.tagName() == "string-array")
                {

                    qDebug() << "Attr: " << domElement.attribute("name", "");
                    parseStringList(domElement);
                    domNode = domNode.nextSibling();
                    continue;
                    // domNode = domNode.nextSibling();
                }
                if (domElement.tagName() == "module")
                {
                    if (!parseModule(domElement.attribute("mtypea", ""), domElement.attribute("mtypeb", "")))
                    {
                        domNode = domNode.nextSibling();
                        continue;
                    }

                    // qDebug() << "Attr: " << domElement.attribute("mtypea", "") << domElement.attribute("mtypeb", "");
                }
                if (domElement.tagName() == "modbus")
                {

                    parseModbus(domElement);

                    domNode = domNode.nextSibling();
                    continue;
                    // domNode = domNode.nextSibling();
                }
                //                else
                //                {
                //                    qDebug() << "TagName: " << domElement.tagName() << "\tText: " << /*qPrintable*/
                //                    (domElement.text());
                //                }
            }
        }
        traverseNode(domNode);
        domNode = domNode.nextSibling();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#ifdef Q_OS_WIN32
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
#endif

#ifdef Q_OS_LINUX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

    auto directory = QDir::current();
    directory.cdUp();
    directory.cdUp();
    directory.cd("module");
    qDebug() << directory;
    auto allFiles = directory.entryList(QDir::Files);
    auto xmlFiles = allFiles.filter(".xml");
    qDebug() << xmlFiles;
    QDomDocument domDoc;
    QFile file(directory.filePath(xmlFiles.first()));
    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            traverseNode(domElement);
        }
        file.close();
    }
    else
        qDebug("Невозможно открыть XML-конфиг");

    return a.exec();
}
