#ifndef MODULEDIALOG_H
#define MODULEDIALOG_H

#include "xmleditor/dialogs/xmldialog.h"

#include <QDialog>

class ModuleDialog : public XmlDialog
{
    Q_OBJECT
private:
    QList<std::tuple<QString, QString, bool>> dlgSettings;

public:
    explicit ModuleDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // MODULEDIALOG_H
