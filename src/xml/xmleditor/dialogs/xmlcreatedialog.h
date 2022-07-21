#ifndef XMLCREATEDIALOG_H
#define XMLCREATEDIALOG_H

#include <QDialog>

class XmlCreateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit XmlCreateDialog() = delete;
    explicit XmlCreateDialog(QWidget *parent);

protected:
    int a;
};

#endif // XMLCREATEDIALOG_H
