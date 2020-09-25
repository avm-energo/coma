#include "wd_func.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../models/etablemodel.h"
#include "edoublespinbox.h"
#include "etableview.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QRegExp>
#include <QStringListModel>
#include <QTextEdit>
#include <QtMath>

#ifdef __GNUC__
#include <cfloat>
#endif
QLineEdit *WDFunc::NewLE(QWidget *w, const QString &lename, const QString &letext, const QString &lecolor)
{
    QLineEdit *le = new QLineEdit(w);
    le->setObjectName(lename);
    le->setText(letext);
    if (!lecolor.isEmpty())
    {
        QString tmps = "QLineEdit {background-color: " + lecolor + ";}";
        le->setStyleSheet(tmps);
    }
    return le;
}

PasswordLineEdit *WDFunc::NewPswLE(
    QWidget *w, const QString &lename, QLineEdit::EchoMode echostyle, const QString &lestyle)
{
    PasswordLineEdit *le = new PasswordLineEdit(echostyle, w);
    le->setObjectName(lename);
    if (!lestyle.isEmpty())
        le->setStyleSheet(lestyle);
    return le;
}

QString WDFunc::LEData(QWidget *w, const QString &lename)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return QString();
    return le->text();
}

bool WDFunc::SetLEData(QWidget *w, const QString &lename, const QString &levalue, const QString &restring)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == nullptr)
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
    if (le == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = le->palette();
    pal.setColor(QPalette::Text, color);
    le->setPalette(pal);
    return true;
}

QLabel *WDFunc::NewLBL(
    QWidget *w, const QString &text, const QString &lblcolor, const QString &lblname, const QPixmap *pm)
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

QLabel *WDFunc::NewLBLT(
    QWidget *w, const QString &text, const QString &lblname, const QString &lblstyle, const QString &lbltip, bool Fixed)
{
    QLabel *lbl = new QLabel(w);
    lbl->setText(text);
    lbl->setObjectName(lblname);
    lbl->setStyleSheet(lblstyle);
    lbl->setToolTip(lbltip);
    if (Fixed == true)
        lbl->setFixedSize(120, 15);
    return lbl;
}

bool WDFunc::SetLBLTColor(QWidget *w, const QString &lblname, const QString &color)
{

    QString style = "QLabel {border: 1px solid green; border-radius: 4px; "
                    "padding: 1px; color: black;"
                    "background-color: "
        + QString(color) + "; font: bold 10px;}";

    QLabel *lblt = w->findChild<QLabel *>(lblname);
    if (lblt == nullptr)
        return false;

    lblt->setStyleSheet(style);

    // QPalette pal = lblt->palette();
    // pal.setColor(QPalette::Background, QColor(color));
    // lblt->setBackgroundRole(QPalette::Background);
    // lblt->setPalette(pal);
    // lblt->setBackgroundRole(QColor(color));
    // http://forum.sources.ru/index.php?showtopic=313950
    // QPalette pal = cb->palette();
    // pal.setColor(QPalette::Text, QColor(color));
    // cb->setPalette(pal);

    /*QPalette t_palette = lblt->palette();
    t_palette.setColor(QPalette::Background, QColor(Colors::TABCOLORA1));
    lblt->setPalette(t_palette);*/
    return true;
}

bool WDFunc::SetTEData(QWidget *w, const QString &tename, const QString &tetext)
{
    QTextEdit *te = w->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    te->setText(tetext);
    return true;
}

bool WDFunc::AppendTEData(QWidget *w, const QString &tename, const QString &tetext)
{
    QTextEdit *te = w->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    te->append(tetext);
    return true;
}

bool WDFunc::TEData(QWidget *w, const QString &tename, QString &tevalue)
{
    QTextEdit *te = w->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    tevalue = te->toPlainText();
    return true;
}

EComboBox *WDFunc::NewCB(QWidget *parent, const QString &cbname, QStringList &cbsl, const QString &cbcolor)
{
    EComboBox *cb = new EComboBox(parent);
    cb->setObjectName(cbname);
    QStringListModel *cblm = new QStringListModel(cb);
    cblm->setStringList(cbsl);
    cb->setModel(cblm);
    if (!cbcolor.isEmpty())
    {
        QString tmps = "QComboBox {background-color: " + cbcolor + ";}";
        cb->setStyleSheet(tmps);
    }
    return cb;
}

QString WDFunc::CBData(QWidget *w, const QString &cbname)
{
    EComboBox *cb = w->findChild<EComboBox *>(cbname);
    if (cb == nullptr)
        return QString();
    return cb->currentText();
}

QMetaObject::Connection WDFunc::CBConnect(
    QWidget *w, const QString &cbname, int cbconnecttype, const QObject *receiver, const char *method)
{
    EComboBox *cb = w->findChild<EComboBox *>(cbname);
    if (cb == nullptr)
        return QMetaObject::Connection();
    switch (cbconnecttype)
    {
    case CT_INDEXCHANGED:
        return QObject::connect(cb, SIGNAL(currentIndexChanged(int)), receiver, method);
    case CT_TEXTCHANGED:
        return QObject::connect(cb, SIGNAL(currentTextChanged(const QString &)), receiver, method);
    default:
        break;
    }
    return QMetaObject::Connection();
}

bool WDFunc::SetCBData(QWidget *w, const QString &cbname, const QString &cbvalue)
{
    EComboBox *cb = w->findChild<EComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    cb->setCurrentText(cbvalue);
    return true;
}

bool WDFunc::SetCBIndex(QObject *w, const QString &cbname, int index)
{
    EComboBox *cb = w->findChild<EComboBox *>(cbname);
    if (cb == nullptr)
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
    EComboBox *cb = w->findChild<EComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = cb->palette();
    pal.setColor(QPalette::Text, QColor(color));
    cb->setPalette(pal);
    return true;
}

QDoubleSpinBox *WDFunc::NewSPB(
    QWidget *parent, const QString &spbname, double min, double max, int decimals, const QString &spbcolor)
{
    QDoubleSpinBox *dsb = new QDoubleSpinBox(parent);
    // QDoubleSpinBox *dspbls = new QDoubleSpinBox(parent);
    double step = qPow(0.1f, decimals);
    dsb->setObjectName(spbname);
    dsb->setSingleStep(step);
    dsb->setDecimals(decimals);
    dsb->setMinimum(min);
    dsb->setMaximum(max);

    if (!spbcolor.isEmpty())
    {
        QString tmps = "QDoubleSpinBox {background-color: " + spbcolor + ";}";
        dsb->setStyleSheet(tmps);
    }
    return dsb;
}

bool WDFunc::SetSPBData(QObject *w, const QString &spbname, const double &spbvalue)
{
    QDoubleSpinBox *spb = w->findChild<QDoubleSpinBox *>(spbname);
    if (spb == nullptr)
        return false;
    spb->setValue(spbvalue);
    return true;
}

/*bool WDFunc::SetSPBColor(QWidget *w, const QString &spbname, QString color)
{
    QDoubleSpinBox *spb = w->findChild<QDoubleSpinBox *>(spbname);
    if (spb == nullptr)
        return false;
    spb->setStyle();
    return true;
}*/

bool WDFunc::SetLBLImage(QWidget *w, const QString &lblname, QPixmap *pm)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return false;
    lbl->setPixmap(*pm);
    return true;
}

bool WDFunc::SetLBLColor(QWidget *w, const QString &lblname, const QString &lblcolor)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
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
    if (lbl == nullptr)
        return false;
    if (!lbltext.isEmpty()) // if label text is empty save previous text in QLabel
        lbl->setText(lbltext);
    lbl->setEnabled(enabled);

    return true;
}

bool WDFunc::LBLText(QWidget *w, const QString &lblname, QString &text)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return false;
    text = lbl->text();
    return true;
}

QRadioButton *WDFunc::NewRB(QWidget *parent, const QString &rbtext, const QString &rbname, const QString &rbcolor)
{
    QRadioButton *rb = new QRadioButton(parent);
    rb->setObjectName(rbname);
    rb->setText(rbtext);
    if (!rbcolor.isEmpty())
    {
        QString tmps = "QRadioButton {background-color: " + rbcolor + ";}";
        rb->setStyleSheet(tmps);
    }
    return rb;
}

bool WDFunc::RBData(QWidget *w, const QString &rbname, bool &data)
{
    QRadioButton *rb = w->findChild<QRadioButton *>(rbname);
    if (rb == nullptr)
        return false;
    data = rb->isChecked();
    return true;
}

bool WDFunc::SetRBData(QWidget *w, const QString &rbname, bool data)
{
    QRadioButton *rb = w->findChild<QRadioButton *>(rbname);
    if (rb == nullptr)
        return false;
    rb->setChecked(data);
    return true;
}

QString WDFunc::TVField(QWidget *w, const QString &tvname, int column, bool isid)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return QString();
    QString tmps = tv->model()
                       ->data(tv->model()->index(tv->currentIndex().row(), column, QModelIndex()), Qt::DisplayRole)
                       .toString();
    if (isid) // если поле с ИД, надо убрать первую цифру - номер таблицы и
              // разделяющую точку, если они присутствуют
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
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
    {
        ERMSG("Пустой tv");
        return;
    }
    tv->update();
    tv->repaint();
    tv->resizeColumnsToContents();
    tv->resizeRowsToContents();
}

QVariant WDFunc::TVData(QWidget *w, const QString &tvname, int column)
{
    QString tmps;
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return QVariant();
    ETableModel *m = static_cast<ETableModel *>(tv->model());
    if (m != nullptr)
        return m->index(tv->currentIndex().row(), column, QModelIndex()).data(Qt::DisplayRole);
    return QVariant();
}

QStatusBar *WDFunc::NewSB(QWidget *w)
{
    QStatusBar *bar = new QStatusBar(w);
    QLabel *msgModel = new QLabel(bar);
    msgModel->setObjectName("Model");
    QLabel *msgSerialNumber = new QLabel(bar);
    msgSerialNumber->setObjectName("SerialNumber");
    QLabel *msgConnectionState = new QLabel(bar);
    msgConnectionState->setObjectName("ConnectionState");
    QLabel *msgConnectionType = new QLabel(bar);
    msgConnectionType->setObjectName("ConnectionType");

    QObject::connect(Board::GetInstance(), &Board::typeChanged, [msgModel]() {
        quint16 serialNumber = Board::GetInstance()->type();
        QString deviceName = QVariant::fromValue(Board::DeviceModel(serialNumber)).toString();
        msgModel->setText(deviceName);
    });

    QObject::connect(
        Board::GetInstance(), &Board::connectionStateChanged, [msgConnectionState](Board::ConnectionState state) {
            QString connState = QVariant::fromValue(Board::ConnectionState(state)).toString();
            msgConnectionState->setText(connState);
            msgConnectionState->setForegroundRole(QPalette::Highlight);
            msgConnectionState->setBackgroundRole(QPalette::HighlightedText);
        });

    QObject::connect(Board::GetInstance(), &Board::interfaceTypeChanged,
        [msgConnectionType](const Board::InterfaceType &interfaceType) {
            QString connName = QVariant::fromValue(Board::InterfaceType(interfaceType)).toString();
            msgConnectionType->setText(connName);
        });
    bar->insertPermanentWidget(0, msgModel);
    bar->insertPermanentWidget(1, msgSerialNumber);
    bar->insertPermanentWidget(2, msgConnectionType);
    bar->insertPermanentWidget(3, msgConnectionState);
    return bar;
}

QPixmap WDFunc::NewCircle(QColor color, float radius)
{
    int intRadius = radius;
    QPixmap myPix(QSize(intRadius, intRadius));
    myPix.fill(Qt::transparent);
    QPainter painter(&myPix); // Create object of QPainter
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
    QRadialGradient gradient(intRadius / 2, intRadius / 2, intRadius);
    gradient.setColorAt(0, color);
    gradient.setColorAt(1, Qt::black);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(0 + painter.pen().width(), 0 + painter.pen().width(), myPix.width() - 2 * painter.pen().width(),
        myPix.height() - 2 * painter.pen().width());

    return myPix;
}

QPixmap WDFunc::NewLedIndicator(QColor color, float height)
{

    QColor lightColor(QColor(0xE0, 0xE0, 0xE0));
    QColor shadowColor(QColor(0x70, 0x70, 0x70));
    QColor ringShadowDarkColor(QColor(0x50, 0x50, 0x50, 0xFF));
    QColor ringShadowMedColor(QColor(0x50, 0x50, 0x50, 0x20));

    QColor ringShadowLightColor(QColor(0xEE, 0xEE, 0xEE, 0x00));
    QColor topReflexUpColor(QColor(0xFF, 0xFF, 0xFF, 0xA0));
    QColor topReflexDownColor(QColor(0xFF, 0xFF, 0xFF, 0x00));
    QColor bottomReflexCenterColor(QColor(0xFF, 0xFF, 0xFF, 0x00));
    QColor bottomReflexSideColor(QColor(0xFF, 0xFF, 0xFF, 0x70));

    int Height = height;
    int minDim = Height;
    int half = minDim / 2;
    int center = Height / 2;

    int outerBorderWidth = minDim / 10;
    int innerBorderWidth = minDim / 14;
    int outerBorderRadius = half - outerBorderWidth;
    int innerBorderRadius = half - (outerBorderWidth + innerBorderWidth);

    int topReflexY = center - (half - outerBorderWidth - innerBorderWidth) / 2;
    int bottomReflexY = center + (half - outerBorderWidth - innerBorderWidth) / 2;
    int topReflexHeight = half / 5;
    int topReflexWidth = half / 3;
    int bottomReflexHeight = half / 5;
    int bottomReflexWidth = half / 3;

    QRect drawingRect;
    drawingRect.setTop((Height - minDim) / 2);
    drawingRect.setLeft((Height - minDim) / 2);
    drawingRect.setHeight(minDim);
    drawingRect.setWidth(minDim);

    QPixmap myPix(QSize(minDim, minDim));
    myPix.fill(Qt::transparent);
    QPainter p(&myPix);
    QPen pen;
    pen.setStyle(Qt::NoPen);
    p.setPen(pen);

    QRadialGradient outerRingGradient(
        QPoint(center, center - outerBorderRadius - (outerBorderWidth / 2)), minDim - (outerBorderWidth / 2));
    outerRingGradient.setColorAt(0, lightColor);
    outerRingGradient.setColorAt(1, shadowColor);
    QBrush outerRingBrush(outerRingGradient);
    p.setBrush(outerRingBrush);
    p.drawEllipse(drawingRect);

    QRadialGradient innerRingGradient(
        QPoint(center, center + innerBorderRadius + (innerBorderWidth / 2)), minDim - (innerBorderWidth / 2));
    innerRingGradient.setColorAt(0, lightColor);
    innerRingGradient.setColorAt(1, shadowColor);
    QBrush innerRingBrush(innerRingGradient);
    p.setBrush(innerRingBrush);
    p.drawEllipse(QPoint(center, center), outerBorderRadius, outerBorderRadius);

    QColor dark(color.darker(120));
    QRadialGradient glassGradient(QPoint(center, center), innerBorderRadius);
    glassGradient.setColorAt(0, color);
    glassGradient.setColorAt(1, dark);
    QBrush glassBrush(glassGradient);
    p.setBrush(glassBrush);
    p.drawEllipse(QPoint(center, center), innerBorderRadius, innerBorderRadius);

    QRadialGradient shadowGradient(QPoint(center, center), innerBorderRadius);
    shadowGradient.setColorAt(0, ringShadowLightColor);
    shadowGradient.setColorAt(0.85, ringShadowMedColor);
    shadowGradient.setColorAt(1, ringShadowDarkColor);
    QBrush shadowBrush(shadowGradient);
    p.setBrush(shadowBrush);
    p.drawEllipse(QPoint(center, center), innerBorderRadius, innerBorderRadius);

    QLinearGradient topTeflexGradient(QPoint(center, (innerBorderWidth + outerBorderWidth)), QPoint(center, center));
    topTeflexGradient.setColorAt(0, topReflexUpColor);
    topTeflexGradient.setColorAt(1, topReflexDownColor);
    QBrush topReflexbrush(topTeflexGradient);
    p.setBrush(topReflexbrush);
    p.drawEllipse(QPoint(center, topReflexY), topReflexWidth, topReflexHeight);

    QRadialGradient bottomReflexGradient(QPoint(center, bottomReflexY + (bottomReflexHeight / 2)), bottomReflexWidth);
    bottomReflexGradient.setColorAt(0, bottomReflexSideColor);
    bottomReflexGradient.setColorAt(1, bottomReflexCenterColor);
    QBrush bottomReflexBrush(bottomReflexGradient);
    p.setBrush(bottomReflexBrush);
    p.drawEllipse(QPoint(center, bottomReflexY), bottomReflexWidth, bottomReflexHeight);

    return myPix;
}

QCheckBox *WDFunc::NewChB(QWidget *parent, const QString &chbname, const QString &chbtext, const QString &chbcolor)
{
    QCheckBox *chb = new QCheckBox(parent);
    chb->setObjectName(chbname);
    chb->setText(chbtext);
    if (!chbcolor.isEmpty())
    {
        QString tmps = "QCheckBox {background-color: " + chbcolor + ";}";
        chb->setStyleSheet(tmps);
    }
    return chb;
}

bool WDFunc::ChBData(QWidget *w, const QString &chbname, bool &data)
{
    QCheckBox *chb = w->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    data = chb->isChecked();
    return true;
}

bool WDFunc::SetChBData(QWidget *w, const QString &chbname, bool data)
{
    QCheckBox *chb = w->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    chb->setChecked(data);
    return true;
}

void WDFunc::AddLabelAndLineedit(QLayout *lyout, QString caption, QString lename, bool enabled)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(caption);
    hlyout->addWidget(lbl);
    QLineEdit *le = new QLineEdit;
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
    hlyout->addWidget(lbl, 0);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName(lename);
    le->setEnabled(enabled);
    hlyout->addWidget(le, 10);
}

QWidget *WDFunc::NewLBLAndLE(QWidget *parent, QString caption, QString lename, bool enabled)
{
    QWidget *w = new QWidget(parent);
    w->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(caption);
    hlyout->addWidget(lbl, 0);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName(lename);
    le->setEnabled(enabled);
    hlyout->addWidget(le, 10);
    w->setLayout(hlyout);
    return w;
}

void WDFunc::SetEnabled(QWidget *w, const QString &wname, bool enabled)
{
    QWidget *wdgt = w->findChild<QWidget *>(wname);
    if (wdgt != nullptr)
        wdgt->setEnabled(enabled);
}

void WDFunc::SetVisible(QWidget *w, const QString &wname, bool visible)
{
    QWidget *wdgt = w->findChild<QWidget *>(wname);
    if (wdgt != nullptr)
    {
        if (visible)
            wdgt->show();
        else
            wdgt->hide();
    }
}

QString WDFunc::StringValueWithCheck(float value, int precision, bool exp)
{
    QString tmps;
    QLocale german(QLocale::German);
    if (value >= FLT_MAX || value <= -FLT_MAX)
        tmps = "***";
    else if (exp == true)
        tmps = german.toString(value, 'e', precision);

    else
        tmps = german.toString(value, 'f', precision);

    return tmps;
}

QVariant WDFunc::FloatValueWithCheck(float value)
{
    QVariant tmps;
    if (value >= FLT_MAX || value <= FLT_MIN)
        tmps = "***";
    else
        tmps = value;
    return tmps;
}

QImage *WDFunc::TwoImages(const QString &first, const QString &second)
{
    QImage *image = new QImage;
    QString FirstImage = "images/" + first + ".png";
    QString SecondImage = "images/" + second + ".png";
    QImage FirstI(FirstImage);
    QImage SecondI(SecondImage);
    if ((first.isEmpty()) && (!SecondI.isNull()))
        *image = SecondI;
    else if ((second.isEmpty()) && (!FirstI.isNull()))
        *image = FirstI;
    else if ((FirstI.isNull()) || (SecondI.isNull()))
        image = new QImage("images/cross.png");
    else
    {
        *image = QImage((FirstI.width() + SecondI.width()), qMax(FirstI.height(), SecondI.height()), FirstI.format());
        image->fill(0);
        QPainter p(image);
        p.drawImage(0, 0, FirstI);
        p.drawImage(FirstI.width(), 0, SecondI);
        p.end();
    }
    return image;
}

QPushButton *WDFunc::NewPB(QWidget *parent, const QString &pbname, const QString &text, const QObject *receiver,
    const char *method, const QString &icon, const QString &pbtooltip)
{
    QPushButton *pb = new QPushButton(parent);
    pb->setStyleSheet("QPushButton {background-color: rgba(0,0,0,0); border: 1px solid gray; "
                      "border-radius: 5px; border-style: outset; padding: 2px 5px;}"
                      "QPushButton:pressed {border-style: inset;}"
                      "QPushButton:disabled {border: none;}");
    pb->setObjectName(pbname);
    if (!icon.isEmpty())
        pb->setIcon(QIcon(icon));
    pb->setText(text);
    pb->setToolTip(pbtooltip);
    if (receiver != nullptr)
        QObject::connect(pb, SIGNAL(clicked(bool)), receiver, method);
    return pb;
}

QMetaObject::Connection WDFunc::PBConnect(
    QWidget *w, const QString &pbname, const QObject *receiver, const char *method)
{
    QPushButton *pb = w->findChild<QPushButton *>(pbname);
    if (pb == nullptr)
        return QMetaObject::Connection();
    return QObject::connect(pb, SIGNAL(clicked(bool)), receiver, method);
}

ETableView *WDFunc::NewTV(QWidget *w, const QString &tvname, QAbstractItemModel *model)
{
    ETableView *tv = new ETableView(w);
    tv->setObjectName(tvname);
    tv->horizontalHeader()->setVisible(true);
    tv->verticalHeader()->setVisible(false);
    if (model != nullptr)
        tv->setModel(model);
    tv->setSelectionMode(QAbstractItemView::NoSelection);
    return tv;
}

QTableView *WDFunc::NewQTV(QWidget *w, const QString &tvname, QAbstractItemModel *model)
{
    QTableView *tv = new QTableView(w);
    tv->setObjectName(tvname);
    // tv->horizontalHeader()->setVisible(true);
    // tv->verticalHeader()->setVisible(false);
    if (model != nullptr)
        tv->setModel(model);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    return tv;
}

void WDFunc::SetTVModel(QWidget *w, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
    {
        ERMSG("Пустой tv");
        return;
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(model);
    tv->resizeColumnsToContents();
    tv->setSortingEnabled(sortenable);
    delete m;
}

void WDFunc::SetQTVModel(QWidget *w, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    QTableView *tv = w->findChild<QTableView *>(tvname);
    if (tv == nullptr)
    {
        ERMSG("Пустой tv");
        return;
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(model);
    tv->resizeColumnsToContents();
    tv->setSortingEnabled(sortenable);
    delete m;
}

void WDFunc::TVConnect(QWidget *w, const QString &tvname, int signaltype, const QObject *receiver, const char *method)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return;
    switch (signaltype)
    {
    case CT_DCLICKED:
        QObject::connect(tv, SIGNAL(doubleClicked(QModelIndex)), receiver, method);
        break;
    case CT_CLICKED:
        QObject::connect(tv, SIGNAL(clicked(QModelIndex)), receiver, method);
        break;
    case CT_CONTEXT:
        tv->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(tv, SIGNAL(customContextMenuRequested(QPoint)), receiver, method);
        break;
    default:
        break;
    }
}

void WDFunc::SortTV(QWidget *w, const QString &tvname, int column, Qt::SortOrder sortorder)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
    {
        ERMSG("Пустой tv");
        return;
    }
    if (column >= 0)
        tv->sortByColumn(column, sortorder);
}

QAbstractItemModel *WDFunc::TVModel(QWidget *w, const QString &tvname)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return nullptr;
    return tv->model();
}

bool WDFunc::LE_read_data(QObject *w, const QString &lename, QString &levalue)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return false;
    levalue = le->text();
    return true;
}

bool WDFunc::LE_write_data(QObject *w, const QString &levalue, const QString &lename)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return false;
    le->text() = levalue;
    le->setText(levalue);
    // SetTEData(w, lename, levalue);
    return true;
}
