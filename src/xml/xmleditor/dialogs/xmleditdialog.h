#ifndef XMLEDITDIALOG_H
#define XMLEDITDIALOG_H

#include <QDialog>

class XmlEditDialog : public QDialog
{
    Q_OBJECT
protected:
    bool changes;

public:
    explicit XmlEditDialog(QWidget *parent = nullptr);
    void reject() override;
};

#endif // XMLEDITDIALOG_H
