#include <gen/xml/xmlattr.h>
#include <widgets/hexpbfunc.h>

#include <QFile>
#include <QPainter>
#include <QSvgRenderer>

HexPBFunc::HexPBFunc() { }

void HexPBFunc::setHexagonPBProcessed(QWidget *parent, const QString &name)
{
    QStringList attrs = { "stroke", "stroke-width", "fill", "fill-opacity" };
    QStringList values = { "#7abe7a", "1.0", "#5ba166", "1.0" };
    setHexagonPBIcon(parent, name, attrs, values);
}

void HexPBFunc::setHexagonPBRestricted(QWidget *parent, const QString &name)
{
    QStringList attrs = { "stroke", "stroke-width" };
    QStringList values = { "#FF0000", "0.1" };
    setHexagonPBIcon(parent, name, attrs, values);
}

void HexPBFunc::setHexagonPBNormal(QWidget *parent, const QString &name)
{
    QStringList attrs = { "stroke", "stroke-width" };
    QStringList values = { "#8cc800", "0.1" };
    setHexagonPBIcon(parent, name, attrs, values);
}

void HexPBFunc::setHexagonPBIcon(QWidget *parent, const QString &name, QStringList &attrs, QStringList &values)
{
    HexagonButton *pb = parent->findChild<HexagonButton *>(name);
    if (pb)
    {
        // open svg resource load contents to qbytearray
        QFile file(pb->data());
        file.open(QIODevice::ReadOnly);
        QByteArray baData = file.readAll();
        // load svg contents to xml document and edit contents
        // XmlFunc::AttrsSearchedStruct str = { "path", "style", "stroke", "#8cc800" };
        // XmlFunc::replaceDomWithNewAttrRecursively(baData, str, attrs, values);
        XmlAttr::replaceSimpleAttr(baData, "g", attrs, values); // change group colors
        // create svg renderer with edited contents
        QSvgRenderer svgRenderer(baData);
        // create pixmap target (could be a QImage)
        QPixmap pix(svgRenderer.defaultSize());
        pix.fill(Qt::transparent);
        // create painter to act over pixmap
        QPainter pixPainter(&pix);
        // use renderer to render over painter which paints on pixmap
        svgRenderer.render(&pixPainter);
        pb->setIcon(QIcon(pix));
    }
}
