#ifndef GENERALTUNEDIALOG_H
#define GENERALTUNEDIALOG_H

#include "../widgets/udialog.h"
#include "abstracttunedialog.h"

#include <QDomDocument>
#include <QWidget>

class GeneralTuneDialog : public UDialog
{
    Q_OBJECT
public:
    struct TuneDialogStruct
    {
        QString caption;
        AbstractTuneDialog *dialog;
    };

    int m_calibrSteps;
    QList<TuneDialogStruct> m_dialogList;
    LimeReport::ReportEngine *m_Report;
    QWidget *m_BacWidget;

    GeneralTuneDialog(ConfigV *config, QWidget *parent = nullptr);
    void SetupUI();
    virtual void prepareReport();

protected:
    ConfigV *configV;

private:
    void setIconProcessed(const QString &name);
    void setIconRestricted(const QString &name);
    void setIconNormal(const QString &name);
    void setHexIcon(const QString &name, const QStringList &hexattrs, const QStringList &hexvalues,
        const QStringList &mainattrs, const QStringList &mainvalues);
    void replaceDomWithNewAttrRecursively(QDomElement &root, const QString &name, const QString &attrname,
        const QStringList &attrs, const QString &attrvalue, const QStringList &newvalues);
    QString getAttrValue(const QString &string, const QString &attrname);

public slots:
    void setCalibrButtons();
    void generateReport();
};

#endif // GENERALTUNEDIALOG_H
