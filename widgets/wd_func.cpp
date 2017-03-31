#include "wd_func.h"
#include "s_tqlineedit.h"
#include "s_tqlabel.h"
#include "s_tqchoosewidget.h"
#include "s_tqcombobox.h"
#include "s_tqspinbox.h"
#include "treeview.h"
#include "s_tqcheckbox.h"
#include "s_tqtextedit.h"
#include <QPalette>

bool WDFunc::SetCWData(QWidget *w, const QString &cwname, const QString &cwvalue)
{
    s_tqChooseWidget *cw = w->findChild<s_tqChooseWidget *>(cwname);
    if (cw == 0)
        return false;
    cw->SetValue(cwvalue);
    return true;
}

bool WDFunc::SetCWColor(QWidget *w, const QString &cwname, const QColor &color)
{
    s_tqChooseWidget *cw = w->findChild<s_tqChooseWidget *>(cwname);
    if (cw == 0)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = cw->palette();
    pal.setColor(QPalette::Window, color);
    cw->setPalette(pal);
    return true;
}

QString WDFunc::CWData(QWidget *w, const QString &cwname)
{
    s_tqChooseWidget *cw = w->findChild<s_tqChooseWidget *>(cwname);
    if (cw == 0)
        return QString();
    return cw->Value();
}

bool WDFunc::SetLEData(QWidget *w, const QString &lename, const QString &levalue)
{
    s_tqLineEdit *le = w->findChild<s_tqLineEdit *>(lename);
    if (le == 0)
        return false;
    le->setText(levalue);
    return true;
}

bool WDFunc::SetLEColor(QWidget *w, const QString &lename, const QColor &color)
{
    s_tqLineEdit *le = w->findChild<s_tqLineEdit *>(lename);
    if (le == 0)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = le->palette();
    pal.setColor(QPalette::Text, color);
    le->setPalette(pal);
    return true;
}

bool WDFunc::LEData(QWidget *w, const QString &lename, QString &levalue)
{
    s_tqLineEdit *le = w->findChild<s_tqLineEdit *>(lename);
    if (le == 0)
        return false;
    levalue = le->text();
    return true;
}

bool WDFunc::SetTEData(QWidget *w, const QString &tename, const QString &tetext)
{
    s_tqTextEdit *te = w->findChild<s_tqTextEdit *>(tename);
    if (te == 0)
        return false;
    te->setText(tetext);
    return true;
}

bool WDFunc::TEData(QWidget *w, const QString &tename, QString &tevalue)
{
    s_tqTextEdit *te = w->findChild<s_tqTextEdit *>(tename);
    if (te == 0)
        return false;
    tevalue = te->toPlainText();
    return true;
}

bool WDFunc::CBData(QWidget *w, const QString &cbname, QString &cbvalue)
{
    s_tqComboBox *cb = w->findChild<s_tqComboBox *>(cbname);
    if (cb == 0)
        return false;
    cbvalue = cb->currentText();
    return true;
}

bool WDFunc::SetCBData(QWidget *w, const QString &cbname, const QString &cbvalue)
{
    s_tqComboBox *cb = w->findChild<s_tqComboBox *>(cbname);
    if (cb == 0)
        return false;
    cb->setCurrentText(cbvalue);
    return true;
}

bool WDFunc::SPBData(QWidget *w, const QString &spbname, double &spbvalue)
{
    s_tqSpinBox *spb = w->findChild<s_tqSpinBox *>(spbname);
    if (spb == 0)
        return false;
    spbvalue = spb->value();
    return true;
}

bool WDFunc::SetSPBData(QWidget *w, const QString &spbname, const double &spbvalue)
{
    s_tqSpinBox *spb = w->findChild<s_tqSpinBox *>(spbname);
    if (spb == 0)
        return false;
    spb->setValue(spbvalue);
    return true;
}

bool WDFunc::SetLBLImage(QWidget *w, const QString &lblname, QPixmap *pm)
{
    s_tqLabel *lbl = w->findChild<s_tqLabel *>(lblname);
    if (lbl == 0)
        return false;
    lbl->setPixmap(*pm);
    return true;
}

bool WDFunc::SetLBLText(QWidget *w, const QString &lblname, const QString &lbltext)
{
    s_tqLabel *lbl = w->findChild<s_tqLabel *>(lblname);
    if (lbl == 0)
        return false;
    lbl->setText(lbltext);
    return true;
}

QString WDFunc::TVField(QWidget *w, const QString &tvname, int column, bool isid)
{
    TreeView *tv = w->findChild<TreeView *>(tvname);
    if (tv == 0)
        return QString();
    QString tmps = tv->model()->data(tv->model()->index(tv->currentIndex().row(),column,QModelIndex()),Qt::DisplayRole).toString();
    if (isid) // если поле с ИД, надо убрать первую цифру - номер таблицы и разделяющую точку, если они присутствуют
    {
        QStringList sl = tmps.split(".");
        if (sl.size() > 1) // есть номер таблицы
            tmps = sl.at(1);
        bool ok;
        int tmpi = tmps.toInt(&ok);
        if (!ok)
            return QString();
        tmps = QString::number(tmpi); // убираем старшие незначащие нули
    }
    return tmps;
}

void WDFunc::TVAutoResize(QWidget *w, const QString &tvname)
{
    TreeView *tv = w->findChild<TreeView *>(tvname);
    if (tv == 0)
        return;
    tv->resizeColumnsToContents();
    tv->resizeRowsToContents();
}

bool WDFunc::ChBData(QWidget *w, const QString &chbname, bool &data)
{
    s_tqCheckBox *chb = w->findChild<s_tqCheckBox *>(chbname);
    if (chb == 0)
        return false;
    data = chb->isChecked();
    return true;
}

bool WDFunc::SetChBData(QWidget *w, const QString &chbname, bool data)
{
    s_tqCheckBox *chb = w->findChild<s_tqCheckBox *>(chbname);
    if (chb == 0)
        return false;
    chb->setChecked(data);
    return true;
}
