#include <gen/colors.h>
#include <gen/error.h>
#include <widgets/lblfunc.h>

#include <QHBoxLayout>

LBLFunc::LBLFunc() { }

QWidget *LBLFunc::NewLBLAndLBL(QWidget *parent, QString lblname, QString caption, bool enabled)
{
    static constexpr char valuesFormat[]
        = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
    auto widget = new QWidget(parent);
    widget->setContentsMargins(0, 0, 0, 0);
    auto hlyout = new QHBoxLayout;
    auto lbl = new QLabel(caption, widget);
    hlyout->addWidget(lbl, 0);
    lbl = new QLabel("", widget);
    lbl->setObjectName(lblname);
    lbl->setEnabled(enabled);
    lbl->setStyleSheet(valuesFormat);
    hlyout->addWidget(lbl, 10);
    widget->setLayout(hlyout);
    return widget;
}

QLabel *LBLFunc::NewLBL(
    QWidget *parent, const QString &text, const QString &lblname, const QPixmap *pm, const QString &lbltip)
{
    auto lbl = new QLabel(parent);
    lbl->setStyleSheet("QLabel {border: none;}");
    lbl->setWordWrap(true);
    lbl->setText(text);
    if (!lblname.isEmpty())
        lbl->setObjectName(lblname);
    if (pm != nullptr)
        lbl->setPixmap(*pm);
    lbl->setToolTip(lbltip);
    return lbl;
}

QLabel *LBLFunc::NewLBLT(
    QWidget *parent, const QString &text, const QString &lblname, const QString &lbltip, bool fixed)
{
    auto lbl = new QLabel(parent);
    lbl->setText(text);
    lbl->setObjectName(lblname);
    lbl->setToolTip(lbltip);
    lbl->setStyleSheet("QLabel {background-color: " + QString(Colors::ColorsMap[Colors::AConfO]) + ";}");
    if (fixed == true)
        lbl->setFixedSize(120, 15);
    return lbl;
}

bool LBLFunc::SetLBLTColor(QWidget *parent, const QString &lblname, const QString &color)
{

    auto style = "QLabel {border: 1px solid green; border-radius: 4px; "
                 "padding: 1px; color: black;"
                 "background-color: "
        + QString(color) + "; font: bold 10px;}";

    auto lblt = parent->findChild<QLabel *>(lblname);
    if (lblt == nullptr)
        return false;

    lblt->setStyleSheet(style);

    return true;
}

bool LBLFunc::SetLBLImage(QWidget *parent, const QString &lblname, QPixmap *pm)
{
    auto lbl = parent->findChild<QLabel *>(lblname);
    Q_ASSERT(lbl != nullptr);
    if (lbl == nullptr)
    {
        qDebug() << Error::DescError << lblname;
        return false;
    }
    else
    {
        lbl->setPixmap(*pm);
        return true;
    }
}

bool LBLFunc::SetLBLColor(QWidget *parent, const QString &lblname, const QString &lblcolor)
{
    auto lbl = parent->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    auto pal = lbl->palette();
    pal.setColor(QPalette::Text, QColor(lblcolor));
    lbl->setPalette(pal);
    return true;
}

bool LBLFunc::SetLBLText(QWidget *parent, const QString &lblname, const QString &lbltext, bool enabled)
{
    auto lbl = parent->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return false;
    if (!lbltext.isEmpty()) // if label text is empty save previous text in QLabel
        lbl->setText(lbltext);
    lbl->setEnabled(enabled);
    return true;
}

QString LBLFunc::LBLText(QWidget *parent, const QString &lblname)
{
    auto lbl = parent->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return QString();
    auto text = lbl->text();
    return text;
}
