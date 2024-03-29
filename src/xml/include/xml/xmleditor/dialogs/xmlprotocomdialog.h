#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

class XmlProtocomDialog final : public XmlDialog
{
    Q_OBJECT
public:
    XmlProtocomDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};
