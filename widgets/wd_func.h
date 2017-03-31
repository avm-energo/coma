#ifndef WD_FUNC
#define WD_FUNC

#include <QWidget>

class WDFunc
{
public:
    WDFunc();
    static bool SetCWData(QWidget *w, const QString &cwname, const QString &cwvalue);
    static QString CWData(QWidget *w, const QString &cwname);
    static bool SetCWColor(QWidget *w, const QString &cwname, const QColor &color);
    static bool SetLEData(QWidget *w, const QString &lename, const QString &levalue, const QString &restring="");
    static bool LEData(QWidget *w, const QString &lename, QString &levalue);
    static bool AppendTEData(QWidget *w, const QString &tename, const QString &tetext);
    static bool SetTEData(QWidget *w, const QString &tename, const QString &tetext);
    static bool TEData(QWidget *w, const QString &tename, QString &tevalue);
    static bool SetCBData(QWidget *w, const QString &cbname, const QString &cbvalue);
    static bool CBData(QWidget *w, const QString &cbname, QString &cbvalue);
    static bool SetSPBData(QWidget *w, const QString &spbname, const double &spbvalue);
    static bool SPBData(QWidget *w, const QString &spbname, double &spbvalue);
    static bool SetLEColor(QWidget *w, const QString &lename, const QColor &color);
    static bool SetLBLImage(QWidget *w, const QString &lblname, QPixmap *pm);
    static bool SetLBLText(QWidget *w, const QString &lblname, const QString &lbltext="", bool enabled=true);
    static QString TVField(QWidget *w, const QString &tvname, int column, bool isid=false);
    static void TVAutoResize(QWidget *w, const QString &tvname);
    static bool ChBData(QWidget *w, const QString &chbname, bool &data);
    static bool SetChBData(QWidget *w, const QString &chbname, bool data);
    static void AddLabelAndLineedit(QLayout *lyout, QString caption, QString lename, bool enabled=false);
};

#endif // WD_FUNC

