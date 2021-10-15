#include "wd_func.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/modules.h"
#include "../gen/stdfunc.h"
#include "../models/etablemodel.h"
#include "edoublespinbox.h"
#include "etableview.h"
#include "ipctrl.h"
#include "passwordlineedit.h"

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QRegExp>
#include <QStatusBar>
#include <QStringListModel>
#include <QTextEdit>
#include <QtDebug>
#include <QtMath>

#ifdef __GNUC__
#include <cfloat>
#endif

QLineEdit *WDFunc::NewLE2(QWidget *w, const QString &lename, const QString &letext, const QString &tooltip)
{
    QLineEdit *le = new QLineEdit(w);
    le->setObjectName(lename);
    le->setText(letext);
    le->setToolTip(tooltip);
    return le;
}

PasswordLineEdit *WDFunc::NewPswLE2(QWidget *w, const QString &lename, QLineEdit::EchoMode echostyle)
{
    PasswordLineEdit *le = new PasswordLineEdit(echostyle, w);
    le->setObjectName(lename);
    return le;
}

QString WDFunc::LEData(QObject *w, const QString &lename)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return QString();
    return le->text();
}

bool WDFunc::SetLEData(QObject *w, const QString &lename, const QString &levalue, const QString &restring)
{
    QLineEdit *le = w->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return false;
    le->setText(levalue);
    if (!restring.isEmpty())
    {
        QRegExp re;
        re.setPattern(restring);
        QValidator *val = new QRegExpValidator(re, w);
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

QLabel *WDFunc::NewLBL(QWidget *w, const QString &text, const QString &lblcolor, const QString &lblname,
    const QPixmap *pm, const QString &lbltip)
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
    lbl->setToolTip(lbltip);
    return lbl;
}

QLabel *WDFunc::NewLBL2(
    QWidget *w, const QString &text, const QString &lblname, const QPixmap *pm, const QString &lbltip)
{
    QLabel *lbl = new QLabel(w);
    lbl->setText(text);
    if (!lblname.isEmpty())
        lbl->setObjectName(lblname);
    if (pm != Q_NULLPTR)
        lbl->setPixmap(*pm);
    lbl->setToolTip(lbltip);
    return lbl;
}

/*!
Копирует содержимое из исходной области памяти в целевую область память
\param w Родитель будущего виджета
\param text Текст для QLabel
\param lblname Имя для QLabel
\param lblstyle StyleSheet для QLabel
\param lbltip ToolTip для QLabel
\param Fixed Фиксированного размера?
*/
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

QLabel *WDFunc::NewLBLT2(QWidget *w, const QString &text, const QString &lblname, const QString &lbltip, bool fixed)
{
    QLabel *lbl = new QLabel(w);
    lbl->setText(text);
    lbl->setObjectName(lblname);
    lbl->setToolTip(lbltip);
    lbl->setStyleSheet("QLabel {background-color: " + QString(Colors::ColorsMap[Colors::AConfO]) + ";}");
    if (fixed == true)
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

QComboBox *WDFunc::NewCB2(QWidget *parent, const QString &cbname, const QStringList &cbsl)
{
    QComboBox *cb = NewCB2(parent, cbsl);
    cb->setObjectName(cbname);
    return cb;
}

QComboBox *WDFunc::NewCB2(QWidget *parent, const QStringList &cbsl)
{
    QComboBox *cb = new QComboBox(parent);
    QStringListModel *cblm = new QStringListModel(cb);
    cblm->setStringList(cbsl);
    cb->setModel(cblm);
    return cb;
}

QString WDFunc::CBData(const QWidget *w, const QString &cbname)
{
    QComboBox *cb = w->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return QString();
    return cb->currentText();
}

bool WDFunc::SetCBData(const QWidget *w, const QString &cbname, const QString &cbvalue)
{
    QComboBox *cb = w->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    cb->setCurrentText(cbvalue);
    return true;
}

bool WDFunc::SetCBIndex(const QObject *w, const QString &cbname, int index)
{
    QComboBox *cb = w->findChild<QComboBox *>(cbname);
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
    QComboBox *cb = w->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    QPalette pal = cb->palette();
    pal.setColor(QPalette::Text, QColor(color));
    cb->setPalette(pal);
    return true;
}

QDoubleSpinBox *WDFunc::NewSPB2(
    QWidget *parent, const QString &spbname, const double min, const double max, const int decimals)
{
    EDoubleSpinBox *dsb = new EDoubleSpinBox(parent);

    double step = qPow(0.1f, decimals);
    dsb->setObjectName(spbname);
    dsb->setSingleStep(step);
    dsb->setDecimals(decimals);
    dsb->setMinimum(min);
    dsb->setMaximum(max);
    return dsb;
}

bool WDFunc::SetSPBData(const QObject *w, const QString &spbname, const double &spbvalue)
{
    QDoubleSpinBox *spb = w->findChild<QDoubleSpinBox *>(spbname);
    if (spb == nullptr)
    {
        qDebug() << Error::NullDataError << spbname;
        return false;
    }
    spb->setValue(spbvalue);
    return true;
}

bool WDFunc::SetLBLImage(QWidget *w, const QString &lblname, QPixmap *pm)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    Q_ASSERT(lbl != nullptr);
    if (lbl == nullptr)
    {
        qDebug() << Error::DescError << lblname;
        return false;
    }
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

QString WDFunc::LBLText(QWidget *w, const QString &lblname)
{
    QLabel *lbl = w->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return QString();
    QString text = lbl->text();
    return text;
}

QRadioButton *WDFunc::NewRB2(QWidget *parent, const QString &rbtext, const QString &rbname)
{
    QRadioButton *rb = new QRadioButton(parent);
    rb->setObjectName(rbname);
    rb->setText(rbtext);
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

bool WDFunc::SetIPCtrlData(const QObject *w, const QString &name, const std::array<quint8, 4> &value)
{
    auto *ipControl = w->findChild<IPCtrl *>(name);
    if (ipControl == nullptr)
        return false;
    ipControl->setIP(value);
    return true;
}

std::array<quint8, 4> WDFunc::IPCtrlData(const QObject *w, const QString &name)
{
    auto *ipControl = w->findChild<IPCtrl *>(name);
    if (ipControl == nullptr)
        return { 0, 0, 0, 0 };
    return ipControl->getIP();
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
        DBGMSG("Пустой tv");
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
    const QMap<Board::InterfaceType, QString> images {
        { Board::InterfaceType::USB, ":/icons/usb.svg" },           //
        { Board::InterfaceType::RS485, ":/icons/rs485.svg" },       //
        { Board::InterfaceType::Ethernet, ":/icons/ethernet.svg" }, //
        { Board::InterfaceType::Unknown, ":/icons/stop.svg" }       //
    };

    QStatusBar *bar = new QStatusBar(w);
    bar->setMaximumHeight(w->height() / 20);

    QWidget *widget = new QWidget(w);
    widget->setMaximumHeight(bar->height());
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(w->width() / 20);
    layout->setContentsMargins(1, 1, 1, 1);

    QLabel *msgModel = new QLabel(bar);
    msgModel->setObjectName("Model");
    layout->addWidget(msgModel);

    QLabel *msgSerialNumber = new QLabel(bar);
    msgSerialNumber->setObjectName("SerialNumber");
    layout->addWidget(msgSerialNumber);

    QLabel *msgConnectionState = new QLabel(bar);
    msgConnectionState->setObjectName("ConnectionState");
    layout->addWidget(msgConnectionState);

    QLabel *msgConnectionImage = new QLabel(bar);
    msgConnectionImage->setObjectName("ConnectionImage");
    layout->addWidget(msgConnectionImage);

    QLabel *msgConnectionType = new QLabel(bar);
    msgConnectionType->setObjectName("ConnectionType");
    layout->addWidget(msgConnectionType);

    int height = bar->height() - layout->contentsMargins().bottom();
    for (int i = 0; i < layout->count(); ++i)
    {
        layout->itemAt(i)->widget()->setFixedHeight(height);
    }
    auto board = &Board::GetInstance();
    QObject::connect(
        board, qOverload<>(&Board::typeChanged), msgModel, [=]() { msgModel->setText(board->moduleName()); });

    QObject::connect(
        board, &Board::connectionStateChanged, msgConnectionState,
        [=](Board::ConnectionState state) {
            QString connState = QVariant::fromValue(Board::ConnectionState(state)).toString();
            msgConnectionState->setText(connState);
            msgConnectionState->setForegroundRole(QPalette::Highlight);
            msgConnectionState->setBackgroundRole(QPalette::HighlightedText);
        },
        Qt::DirectConnection);
    QObject::connect(
        board, &Board::interfaceTypeChanged, msgConnectionType, [=](const Board::InterfaceType &interfaceType) {
            QString connName = QVariant::fromValue(Board::InterfaceType(interfaceType)).toString();
            msgConnectionType->setText(connName);
        });
    QObject::connect(
        board, &Board::interfaceTypeChanged, msgConnectionImage, [=](const Board::InterfaceType &interfaceType) {
            QPixmap pixmap = QIcon(QString(images.value(interfaceType))).pixmap(QSize(height, height));
            msgConnectionImage->setPixmap(pixmap);
        });
    widget->setLayout(layout);
    bar->addPermanentWidget(widget);
    return bar;
}

QPixmap WDFunc::NewCircle(QColor color, int radius)
{
    QPixmap myPix(QSize(radius, radius));
    myPix.fill(Qt::transparent);
    QPainter painter(&myPix); // Create object of QPainter
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
    QRadialGradient gradient(radius / 2, radius / 2, radius);
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
    /// Same expression on both sides of '-'
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

QMainWindow *WDFunc::getMainWindow()
{
    const auto widgets = qApp->topLevelWidgets();
    for (QWidget *w : widgets)
        if (QMainWindow *mainWin = qobject_cast<QMainWindow *>(w))
            return mainWin;
    return nullptr;
}

bool WDFunc::floatIsWithinLimits(
    QWidget *w, const QString &varname, double var, double base, double tolerance, bool showMessage)
{
    if (StdFunc::floatIsWithinLimits(var, base, tolerance))
        return true;
    else if (showMessage)
    {
        qCritical() << "Ошибочное значение " + varname + ": должно быть " << QString::number(base, 'f', 5) << "±"
                    << QString::number(tolerance, 'f', 5) << ", а получили: " << QString::number(var, 'f', 5);
        QMessageBox::critical(w, "Ошибка",
            "Ошибочное значение " + varname + ": должно быть " + QString::number(base, 'f', 5) + "±"
                + QString::number(tolerance, 'f', 5) + ", а получили: " + QString::number(var, 'f', 5));
    }
    return false;
}

QString WDFunc::ChooseFileForOpen(QWidget *parent, QString mask)
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(
        parent, "Открыть файл", StdFunc::GetHomeDir(), mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    return filename;
}

QString WDFunc::ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr)
{
    QString tmps = Files::ChooseFileForSave(ext, filenamestr);
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename
        = dlg->getSaveFileName(parent, "Сохранить файл", tmps, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    return filename;
}

QPushButton *WDFunc::NewPBCommon(
    QWidget *parent, const QString &pbname, const QString &text, const QString &icon, const QString &pbtooltip)
{
    QPushButton *pb = new QPushButton(parent);
    pb->setObjectName(pbname);
    if (!icon.isEmpty())
        pb->setIcon(QIcon(icon));
    pb->setText(text);
    pb->setToolTip(pbtooltip);
    return pb;
}

QCheckBox *WDFunc::NewChB2(QWidget *parent, const QString &chbname, const QString &chbtext)
{
    QCheckBox *chb = new QCheckBox(parent);
    chb->setObjectName(chbname);
    chb->setText(chbtext);
    return chb;
}

bool WDFunc::ChBData(const QWidget *w, const QString &chbname, bool &data)
{
    QCheckBox *chb = w->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    data = chb->isChecked();
    return true;
}

bool WDFunc::ChBData(const QWidget *w, const QString &chbname)
{
    QCheckBox *chb = w->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
    {
        qDebug() << Error::NullDataError;
        return false;
    }
    return chb->isChecked();
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
    else
        qDebug() << "No such widget to set it visible";
}

void WDFunc::setMinimumSize(QWidget *w, const QString &wname, int width, int height)
{
    QWidget *wdgt = w->findChild<QWidget *>(wname);
    if (wdgt != nullptr)
    {
        wdgt->setMinimumSize(width, height);
        QPushButton *pb = qobject_cast<QPushButton *>(wdgt);
        if (pb != nullptr)
            pb->setIconSize(QSize(width - 10, height - 10));
    }
    else
        qDebug() << "No such widget to set size";
}

QString WDFunc::StringValueWithCheck(float value, int precision, bool exp)
{
    if (value >= FLT_MAX || value <= -FLT_MAX)
        return "***";
    else if (exp == true)
        return QString::number(value, 'e', precision);
    else
        return QString::number(value, 'f', precision);
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

ETableView *WDFunc::NewTV(QWidget *w, const QString &tvname, QAbstractItemModel *model)
{
    ETableView *tv = new ETableView(w);
    tv->setObjectName(tvname);
    tv->horizontalHeader()->setVisible(true);
    tv->verticalHeader()->setVisible(false);
    if (model != nullptr)
        tv->setModel(model);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setSelectionMode(QAbstractItemView::NoSelection);
    return tv;
}

QTableView *WDFunc::NewQTV(QWidget *w, const QString &tvname, QAbstractItemModel *model)
{
    QTableView *tv = new QTableView(w);
    if (model != nullptr)
        tv->setModel(model);
    tv->setObjectName(tvname);
    // tv->horizontalHeader()->setVisible(true);
    // tv->verticalHeader()->setVisible(false);
    //  tv->setAttribute(Qt::WA_TranslucentBackground, true);
    //  tv->setAttribute(Qt::WA_NoSystemBackground, true);
    // tv->setFrameStyle(QFrame::NoFrame);
    //  tv->setShowGrid(false);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    return tv;
}

void WDFunc::SetTVModel(QWidget *w, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
    {
        qDebug("Empty tv");
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
        DBGMSG("Пустой tv");
        return;
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(model);
    tv->resizeColumnsToContents();
    tv->setSortingEnabled(sortenable);
    delete m;
}

void WDFunc::SortTV(QWidget *w, const QString &tvname, int column, Qt::SortOrder sortorder)
{
    ETableView *tv = w->findChild<ETableView *>(tvname);
    if (tv == nullptr)
    {
        DBGMSG("Пустой tv");
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
