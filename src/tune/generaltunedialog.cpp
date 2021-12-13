#include "generaltunedialog.h"

#include "../gen/board.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "tunesequencefile.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QSvgRenderer>
#include <limereport/LimeReport>

GeneralTuneDialog::GeneralTuneDialog(ConfigV *config, QWidget *parent) : UDialog(parent), configV(config)
{
    TuneSequenceFile::init();
    m_calibrSteps = 0;
}

void GeneralTuneDialog::SetupUI()
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addStretch(100);
    int count = 1;
    for (auto &d : m_dialogList)
    {
        QString tns = "tn" + QString::number(count++);
        lyout->addWidget(WDFunc::NewHexagonPB(
            this, tns, [&d]() { d.dialog->show(); }, ":/icons/" + tns + ".svg", d.caption));
    }
    lyout->addWidget(WDFunc::NewHexagonPB(
        this, "tnprotocol", [this]() { prepareReport(); }, ":/icons/tnprotocol.svg",
        "Генерация протокола регулировки"));
    lyout->addStretch(100);
    hlyout->addLayout(lyout);
    hlyout->addWidget(m_BacWidget, 100);
    setLayout(hlyout);
    setCalibrButtons();
}

void GeneralTuneDialog::prepareReport()
{
}

void GeneralTuneDialog::setCalibrButtons()
{
    //    QString cpuserialnum = Board::GetInstance().UID();
    //    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    //    int calibrstep = storedcalibrations.value(cpuserialnum + "/step", "1").toInt();
    int calibrstep = TuneSequenceFile::value("step", "1").toInt();
    for (int i = 1; i < calibrstep; ++i)
        setIconProcessed("tn" + QString::number(i));
    setIconNormal("tn" + QString::number(calibrstep));
    for (int i = (calibrstep + 1); i < m_calibrSteps; ++i)
        setIconRestricted("tn" + QString::number(i));
    if (calibrstep < m_calibrSteps)
        setIconRestricted("tnprotocol");
}

void GeneralTuneDialog::generateReport()
{
    m_Report = new LimeReport::ReportEngine;
    prepareReport();
    if (EMessageBox::question("Сохранить протокол поверки?"))
    {
        QString filename = WDFunc::ChooseFileForSave(this, "*.pdf", "pdf");
        if (!filename.isEmpty())
        {
            m_Report->designReport();
            m_Report->printToPDF(filename);
            //        report->previewReport();
            //  report->designReport();
            EMessageBox::information("Записано успешно!");
        }
        else
            EMessageBox::information("Действие отменено");
    }
    delete m_Report;
}

void GeneralTuneDialog::setIconProcessed(const QString &name)
{
    setHexIcon(name, { "fill", "stroke-width" }, { "#8cc800", "0.5" },
        { "stroke", "stroke-width", "fill", "fill-opacity" }, { "#0000ff", "1.5", "#8cc800", "0.3" });
}

void GeneralTuneDialog::setIconRestricted(const QString &name)
{
    setHexIcon(
        name, { "fill", "stroke-width" }, { "#FF0000", "0.1" }, { "stroke", "stroke-width" }, { "#FF0000", "0.3" });
}

void GeneralTuneDialog::setIconNormal(const QString &name)
{
    setHexIcon(name, { "fill", "stroke-width" }, { "#8cc800", "0.15" },
        { "stroke", "stroke-width", "fill", "fill-opacity" }, { "#8cc800", "1", "none", "1" });
}

void GeneralTuneDialog::setHexIcon(const QString &name, const QStringList &hexattrs, const QStringList &hexvalues,
    const QStringList &mainattrs, const QStringList &mainvalues)
{
    QPushButton *pb = this->findChild<QPushButton *>(name);
    if (pb)
    {
        // open svg resource load contents to qbytearray
        QFile file("images/" + name + ".svg");
        file.open(QIODevice::ReadOnly);
        QByteArray baData = file.readAll();
        // load svg contents to xml document and edit contents
        QDomDocument doc;
        doc.setContent(baData);
        // recurivelly change color
        QDomElement root = doc.documentElement();
        //        QStringList attrs = { "fill", "stroke-width" };
        //        QStringList values = { "#FF00ff", "1.0" };
        replaceDomWithNewAttrRecursively(root, "path", "style", mainattrs, "#8cc800", mainvalues);
        //        attrs = QStringList({ "stroke", "stroke-width" });
        //        values = QStringList({ "#FF00ff", "1.5" });
        replaceDomWithNewAttrRecursively(root, "path", "style", hexattrs, "#8cc800", hexvalues);
        // create svg renderer with edited contents
        QSvgRenderer svgRenderer(doc.toByteArray());
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

// check attrname to contain attrs.at(0) in it and if so
// replace all attrs oldvalues with newvalues

void GeneralTuneDialog::replaceDomWithNewAttrRecursively(QDomElement &root, const QString &name,
    const QString &attrname, const QStringList &attrs, const QString &attrcheckvalue, const QStringList &newvalues)
{
    assert(attrs.size() == newvalues.size());
    QDomNodeList nodes = root.childNodes();
    for (int i = 0; i < nodes.size(); ++i)
    {
        QDomNode node = nodes.at(i);
        QString tmps = node.nodeName();
        if (node.nodeName() == name) // path
        {
            QDomNode newnode = node;
            QDomElement element = newnode.toElement();
            QString tmps = element.attribute(attrname); // style
            int count = 0;
            QString attrvalue = getAttrValue(tmps, attrs.first());
            if (attrvalue == attrcheckvalue)
            {
                while (!attrvalue.isEmpty())
                {
                    if (count < newvalues.size())
                    {
                        tmps = tmps.replace(attrvalue, newvalues.at(count));
                        ++count;
                        if (count < attrs.size())
                            attrvalue = getAttrValue(tmps, attrs.at(count));
                        else
                            attrvalue.clear();
                    }
                }
                element.setAttribute(attrname, tmps);
                root.replaceChild(newnode, node);
            }
        }
        else
        {
            QDomElement element = node.toElement();
            replaceDomWithNewAttrRecursively(element, name, attrname, attrs, attrcheckvalue, newvalues);
            //            else
            //            {
            //                QDomNode newnode = node;
            //                root.replaceChild(newnode, node);
            //            }
        }
    }
}

QString GeneralTuneDialog::getAttrValue(const QString &string, const QString &attrname)
{
    QString tmps;
    int index = string.indexOf(attrname);
    if (index == -1)
        return QString();
    index += attrname.size();
    if (index >= string.size())
        return QString();
    if (string.at(index++) == ":")
    {
        while ((index < string.size()) && (string.at(index) == " "))
            index++;
        while ((index < string.size()) && (string.at(index) != ";") && (string.at(index) != "\""))
        {
            tmps += string.at(index++);
        }
        return tmps;
    }
    return QString();
}
