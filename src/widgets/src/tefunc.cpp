#include <widgets/tefunc.h>

TEFunc::TEFunc() { }

bool TEFunc::SetTEData(QWidget *parent, const QString &tename, const QString &tetext)
{
    auto te = parent->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    te->setText(tetext);
    return true;
}

bool TEFunc::AppendTEData(QWidget *parent, const QString &tename, const QString &tetext)
{
    auto te = parent->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    te->append(tetext);
    return true;
}

bool TEFunc::TEData(QWidget *parent, const QString &tename, QString &tevalue)
{
    auto te = parent->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    tevalue = te->toPlainText();
    return true;
}
