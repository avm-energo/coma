#ifndef GENERALTUNEDIALOG_H
#define GENERALTUNEDIALOG_H

#include "../widgets/udialog.h"

#include <QDomDocument>
#include <QWidget>

class GeneralTuneDialog : public UDialog
{
    Q_OBJECT
public:
    int m_calibrSteps;

    GeneralTuneDialog(QWidget *parent = nullptr);

private:
    void setIconProcessed(const QString &name);
    void setIconRestricted(const QString &name);
    void setHexIcon(const QString &name, const QStringList &hexattrs, const QStringList &hexvalues,
        const QStringList &mainattrs, const QStringList &mainvalues);
    void replaceDomWithNewAttrRecursively(QDomElement &root, const QString &name, const QString &attrname,
        const QStringList &attrs, const QString &attrvalue, const QStringList &newvalues);
    QString getAttrValue(const QString &string, const QString &attrname);

public slots:
    void setCalibrButtons();
};

#endif // GENERALTUNEDIALOG_H
