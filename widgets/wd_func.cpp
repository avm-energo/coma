#include "wd_func.h"
#include "s_tqcombobox.h"
#include "s_tqspinbox.h"
#include "s_tqcheckbox.h"
#include "s_tqtableview.h"
#include <QPalette>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExp>
#include <QLineEdit>

bool WDFunc::SetLEData(QWidget *w, const QString &lename, const QString &levalue, const QString &restring)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == 0)
        return false;
    le->setText(levalue);
    if (!restring.isEmpty())
    {
        QRegExp re;
        re.setPattern(restring);
        QValidator *val = new QRegExpValidator(re);
        le->setValidator(val);
    }
    return true;
}

bool WDFunc::SetLEColor(QWidget *w, const QString &lename, const QColor &color)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
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
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == 0)
        return false;
    levalue = le->text();
    return true;
}

bool WDFunc::SetTEData(QWidget *w, const QString &tename, const QString &tetext)
{
    QTextEdit *te = w->findChild<QTextEdit *>(tename);
    if (te == 0)
        return false;
    te->setText(tetext);
    return true;
}

bool WDFunc::AppendTEData(QWidget *w, const QString &tename, const QString &tetext)
{
    QTextEdit *te = w->findChild<QTextEdit *>(tename);
    if (te == 0)
        return false;
    te->append(tetext);
    return true;
}

bool WDFunc::TEData(QWidget *w, const QString &tename, QString &tevalue)
{
    QTextEdit *te = w->findChild<QTextEdit *>(tename);
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
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == 0)
        return false;
    lbl->setPixmap(*pm);
    return true;
}

bool WDFunc::SetLBLText(QWidget *w, const QString &lblname, const QString &lbltext, bool enabled)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == 0)
        return false;
    if (!lbltext.isEmpty()) // if label text is empty save previous text in QLabel
        lbl->setText(lbltext);
    lbl->setEnabled(enabled);
    return true;
}

QString WDFunc::TVField(QWidget *w, const QString &tvname, int column, bool isid)
{
    s_tqTableView *tv = w->findChild<s_tqTableView *>(tvname);
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
    s_tqTableView *tv = w->findChild<s_tqTableView *>(tvname);
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

void WDFunc::AddLabelAndLineedit(QLayout *lyout, QString caption, QString lename, bool enabled)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(caption);
    hlyout->addWidget(lbl);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName(lename);
    le->setEnabled(enabled);
    hlyout->addWidget(le);
    QVBoxLayout *vlyout = static_cast<QVBoxLayout *>(lyout);
    vlyout->addLayout(hlyout);
}

void WDFunc::AddLabelAndLineeditH(QLayout *lyout, QString caption, QString lename, bool enabled)
{
    QHBoxLayout *hlyout = static_cast<QHBoxLayout *>(lyout);
    QLabel *lbl = new QLabel(caption);
    hlyout->addWidget(lbl,1);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName(lename);
    le->setEnabled(enabled);
    hlyout->addWidget(le,1);
}
