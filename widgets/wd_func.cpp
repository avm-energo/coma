#include <QPalette>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExp>
#include <QLineEdit>
#include <QStringListModel>
#include "wd_func.h"
#include "s_tqtableview.h"

QLineEdit *WDFunc::NewLE(QWidget *w, const QString &lename, const QString &letext, const QString &lestyle)
{
    QLineEdit *le = new QLineEdit(w);
    le->setObjectName(lename);
    le->setText(letext);
    if (!lestyle.isEmpty())
        le->setStyleSheet(lestyle);
    return le;
}

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

QLabel *WDFunc::NewLBL(QWidget *w, const QString &text, const QString &lblcolor, const QString &lblname, const QPixmap *pm)
{
    QLabel *lbl = new QLabel(w);
    lbl->setText(text);
    if (!lblname.isEmpty())
        lbl->setObjectName(lblname);
    if (!lblcolor.isEmpty())
    {
        QString tmps = "QLabel {background-color: " + lblcolor + ";}";
        lbl->setStyleSheet(tmps);
    }
    if (pm != Q_NULLPTR)
        lbl->setPixmap(*pm);
    return lbl;
}

QLabel *WDFunc::NewLBLT(QWidget *w, const QString &text, const QString &lblname, const QString &lblstyle, const QString &lbltip)
{
    QLabel *lbl = new QLabel(w);
    lbl->setText(text);
    lbl->setObjectName(lblname);
    lbl->setStyleSheet(lblstyle);
    lbl->setToolTip(lbltip);
    return lbl;

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

s_tqComboBox *WDFunc::NewCB(QWidget *parent, const QString &cbname, QStringList &cbsl, const QString &cbcolor)
{
    s_tqComboBox *cb = new s_tqComboBox(parent);
    cb->setObjectName(cbname);
    QStringListModel *cblm = new QStringListModel;
    cblm->setStringList(cbsl);
    cb->setModel(cblm);
    if (!cbcolor.isEmpty())
    {
        QString tmps = "QComboBox {background-color: " + cbcolor + ";}";
        cb->setStyleSheet(tmps);
    }
    return cb;
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

bool WDFunc::SetCBIndex(QWidget *w, const QString &cbname, int index)
{
    s_tqComboBox *cb = w->findChild<s_tqComboBox *>(cbname);
    if (cb == 0)
        return false;
    if (index < cb->count())
    {
        cb->setCurrentIndex(index);
        return true;
    }
    return false;
}

bool WDFunc::SetCBColor(QWidget *w, const QString &cbname, const QString &color)
{
    s_tqComboBox *cb = w->findChild<s_tqComboBox *>(cbname);
    if (cb == 0)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = cb->palette();
    pal.setColor(QPalette::Text, QColor(color));
    cb->setPalette(pal);
    return true;
}

s_tqSpinBox *WDFunc::NewSPB(QWidget *parent, const QString &spbname, double min, double max, double step, int decimals, const QString &spbcolor)
{
    s_tqSpinBox *dspbls = new s_tqSpinBox(parent);
    dspbls->setObjectName(spbname);
    dspbls->setSingleStep(step);
    dspbls->setDecimals(decimals);
    dspbls->setMinimum(min);
    dspbls->setMaximum(max);
    if (!spbcolor.isEmpty())
    {
        QString tmps = "QDoubleSpinBox {background-color: " + spbcolor + ";}";
        dspbls->setStyleSheet(tmps);
    }
    return dspbls;
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

bool WDFunc::SetLBLColor(QWidget *w, const QString &lblname, const QString &lblcolor)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == 0)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = lbl->palette();
    pal.setColor(QPalette::Text, QColor(lblcolor));
    lbl->setPalette(pal);
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

bool WDFunc::LBLText(QWidget *w, const QString &lblname, QString &text)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == 0)
        return false;
    text = lbl->text();
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

s_tqCheckBox *WDFunc::NewChB(QWidget *parent, const QString &chbname, const QString &chbtext, const QString &chbcolor)
{
    s_tqCheckBox *chb = new s_tqCheckBox(parent);
    chb->setObjectName(chbname);
    chb->setText(chbtext);
    if (!chbcolor.isEmpty())
    {
        QString tmps = "QCheckBox {background-color: "+chbcolor+";}";
        chb->setStyleSheet(tmps);
    }
    return chb;
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

void WDFunc::SetEnabled(QWidget *w, const QString &wname, bool enabled)
{
    QWidget *wdgt = w->findChild<QWidget *>(wname);
    if (wdgt != 0)
        wdgt->setEnabled(enabled);
}

void WDFunc::SetVisible(QWidget *w, const QString &wname, bool visible)
{
    QWidget *wdgt = w->findChild<QWidget *>(wname);
    if (wdgt != 0)
    {
        if (visible)
            wdgt->show();
        else
            wdgt->hide();
    }
}

QString WDFunc::StringValueWithCheck(float value)
{
    if (value )
}
