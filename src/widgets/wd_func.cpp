#include "wd_func.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../models/etablemodel.h"
#include "../module/board.h"
#include "../module/filehelper.h"
#include "../module/modules.h"
#include "edoublespinbox.h"
#include "epopup.h"
#include "etableview.h"
#include "ipctrl.h"
#include "passwordlineedit.h"

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
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

QLineEdit *WDFunc::NewLE2(QWidget *parent, const QString &lename, const QString &letext, const QString &tooltip)
{
    auto le = new QLineEdit(parent);
    le->setObjectName(lename);
    le->setText(letext);
    le->setToolTip(tooltip);
    return le;
}

PasswordLineEdit *WDFunc::NewPswLE2(QWidget *parent, const QString &lename, QLineEdit::EchoMode echostyle)
{
    auto le = new PasswordLineEdit(echostyle, parent);
    le->setObjectName(lename);
    return le;
}

QString WDFunc::LEData(QObject *parent, const QString &lename)
{
    auto le = parent->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return QString();
    return le->text();
}

bool WDFunc::SetLEData(QObject *parent, const QString &lename, const QString &levalue, const QString &restring)
{
    auto le = parent->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return false;
    le->setText(levalue);
    if (!restring.isEmpty())
    {
        // QRegExp re;
        // re.setPattern(restring);
        auto val = new QRegExpValidator(QRegExp(restring), parent);
        le->setValidator(val);
    }
    return true;
}

bool WDFunc::SetLEColor(QWidget *parent, const QString &lename, const QColor &color)
{
    auto le = parent->findChild<QLineEdit *>(lename);
    if (le == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    auto pal = le->palette();
    pal.setColor(QPalette::Text, color);
    le->setPalette(pal);
    return true;
}

QLabel *WDFunc::NewLBL(QWidget *parent, const QString &text, const QString &lblcolor, const QString &lblname,
    const QPixmap *pm, const QString &lbltip)
{
    auto lbl = new QLabel(parent);
    lbl->setText(text);
    if (!lblname.isEmpty())
        lbl->setObjectName(lblname);
    if (!lblcolor.isEmpty())
    {
        auto tmps = "QLabel {background-color: " + lblcolor + ";}";
        lbl->setStyleSheet(tmps);
    }
    if (pm != nullptr)
        lbl->setPixmap(*pm);
    lbl->setToolTip(lbltip);
    return lbl;
}

QLabel *WDFunc::NewLBL2(
    QWidget *parent, const QString &text, const QString &lblname, const QPixmap *pm, const QString &lbltip)
{
    auto lbl = new QLabel(parent);
    lbl->setText(text);
    if (!lblname.isEmpty())
        lbl->setObjectName(lblname);
    if (pm != nullptr)
        lbl->setPixmap(*pm);
    lbl->setToolTip(lbltip);
    return lbl;
}

/*! \brief Копирует содержимое из исходной области памяти в целевую область память
 *  \param w Родитель будущего виджета
 *  \param text Текст для QLabel
 *  \param lblname Имя для QLabel
 *  \param lblstyle StyleSheet для QLabel
 *  \param lbltip ToolTip для QLabel
 *  \param Fixed Фиксированного размера?
 */
QLabel *WDFunc::NewLBLT(QWidget *parent, const QString &text, const QString &lblname, const QString &lblstyle,
    const QString &lbltip, bool Fixed)
{
    auto lbl = new QLabel(parent);
    lbl->setText(text);
    lbl->setObjectName(lblname);
    lbl->setStyleSheet(lblstyle);
    lbl->setToolTip(lbltip);
    if (Fixed == true)
        lbl->setFixedSize(120, 15);
    return lbl;
}

QLabel *WDFunc::NewLBLT2(
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

bool WDFunc::SetLBLTColor(QWidget *parent, const QString &lblname, const QString &color)
{

    auto style = "QLabel {border: 1px solid green; border-radius: 4px; "
                 "padding: 1px; color: black;"
                 "background-color: "
        + QString(color) + "; font: bold 10px;}";

    auto lblt = parent->findChild<QLabel *>(lblname);
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

bool WDFunc::SetTEData(QWidget *parent, const QString &tename, const QString &tetext)
{
    auto te = parent->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    te->setText(tetext);
    return true;
}

bool WDFunc::AppendTEData(QWidget *parent, const QString &tename, const QString &tetext)
{
    auto te = parent->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    te->append(tetext);
    return true;
}

bool WDFunc::TEData(QWidget *parent, const QString &tename, QString &tevalue)
{
    auto te = parent->findChild<QTextEdit *>(tename);
    if (te == nullptr)
        return false;
    tevalue = te->toPlainText();
    return true;
}

QComboBox *WDFunc::NewCB2(QWidget *parent, const QString &cbname, const QStringList &cbsl)
{
    auto cb = NewCB2(parent, cbsl);
    cb->setObjectName(cbname);
    return cb;
}

QComboBox *WDFunc::NewCB2(QWidget *parent, const QStringList &cbsl)
{
    auto cb = new QComboBox(parent);
    auto cblm = new QStringListModel(cbsl, cb);
    // cblm->setStringList(cbsl);
    cb->setModel(cblm);
    return cb;
}

QString WDFunc::CBData(const QWidget *parent, const QString &cbname)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return QString();
    return cb->currentText();
}

int WDFunc::CBIndex(const QObject *parent, const QString &cbname)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return -1;
    return cb->currentIndex();
}

bool WDFunc::SetCBData(const QWidget *parent, const QString &cbname, const QString &cbvalue)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    cb->setCurrentText(cbvalue);
    return true;
}

bool WDFunc::SetCBIndex(const QObject *parent, const QString &cbname, int index)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    if (index < cb->count())
    {
        cb->setCurrentIndex(index);
        return true;
    }

    return false;
}

bool WDFunc::SetCBColor(QWidget *parent, const QString &cbname, const QString &color)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    auto pal = cb->palette();
    pal.setColor(QPalette::Text, QColor(color));
    cb->setPalette(pal);
    return true;
}

QDoubleSpinBox *WDFunc::NewSPB2(
    QWidget *parent, const QString &spbname, const double min, const double max, const int decimals)
{
    auto dsb = new EDoubleSpinBox(parent);
    double step = qPow(0.1f, decimals);
    dsb->setObjectName(spbname);
    dsb->setSingleStep(step);
    dsb->setDecimals(decimals);
    dsb->setMinimum(min);
    dsb->setMaximum(max);
    return dsb;
}

DoubleSpinBoxGroup *WDFunc::NewSPBG(
    QWidget *parent, const QString &spbname, int count, const double min, const double max, const int decimals)
{
    auto spinBoxGroup = new DoubleSpinBoxGroup(count, parent);
    spinBoxGroup->setObjectName(spbname);
    double step = std::pow(0.1f, decimals);
    spinBoxGroup->setSingleStep(step);
    spinBoxGroup->setDecimals(decimals);
    spinBoxGroup->setMinimum(min);
    spinBoxGroup->setMaximum(max);
    return spinBoxGroup;
}

DoubleSpinBoxGroup *WDFunc::NewSPBG(QWidget *parent, const QString &spbname, const QStringList &list, const double min,
    const double max, const int decimals)
{
    auto spinBoxGroup = new DoubleSpinBoxGroup(list, parent);
    spinBoxGroup->setObjectName(spbname);
    double step = std::pow(0.1f, decimals);
    spinBoxGroup->setSingleStep(step);
    spinBoxGroup->setDecimals(decimals);
    spinBoxGroup->setMinimum(min);
    spinBoxGroup->setMaximum(max);
    return spinBoxGroup;
}

bool WDFunc::SetSPBData(const QObject *parent, const QString &spbname, const double &spbvalue)
{
    auto spb = parent->findChild<QDoubleSpinBox *>(spbname);
    if (spb == nullptr)
    {
        qDebug() << Error::NullDataError << spbname;
        return false;
    }
    spb->setValue(spbvalue);
    return true;
}

bool WDFunc::SetLBLImage(QWidget *parent, const QString &lblname, QPixmap *pm)
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

bool WDFunc::SetLBLColor(QWidget *parent, const QString &lblname, const QString &lblcolor)
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

bool WDFunc::SetLBLText(QWidget *parent, const QString &lblname, const QString &lbltext, bool enabled)
{
    auto lbl = parent->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return false;
    if (!lbltext.isEmpty()) // if label text is empty save previous text in QLabel
        lbl->setText(lbltext);
    lbl->setEnabled(enabled);
    return true;
}

QString WDFunc::LBLText(QWidget *parent, const QString &lblname)
{
    auto lbl = parent->findChild<QLabel *>(lblname);
    if (lbl == nullptr)
        return QString();
    auto text = lbl->text();
    return text;
}

QRadioButton *WDFunc::NewRB2(QWidget *parent, const QString &rbtext, const QString &rbname)
{
    auto rb = new QRadioButton(parent);
    rb->setObjectName(rbname);
    rb->setText(rbtext);
    return rb;
}

bool WDFunc::RBData(QWidget *parent, const QString &rbname, bool &data)
{
    auto rb = parent->findChild<QRadioButton *>(rbname);
    if (rb == nullptr)
        return false;
    data = rb->isChecked();
    return true;
}

bool WDFunc::SetRBData(QWidget *parent, const QString &rbname, bool data)
{
    auto rb = parent->findChild<QRadioButton *>(rbname);
    if (rb == nullptr)
        return false;
    rb->setChecked(data);
    return true;
}

bool WDFunc::SetIPCtrlData(const QObject *parent, const QString &name, const std::array<quint8, 4> &value)
{
    auto ipControl = parent->findChild<IPCtrl *>(name);
    if (ipControl == nullptr)
        return false;
    ipControl->setIP(value);
    return true;
}

std::array<quint8, 4> WDFunc::IPCtrlData(const QObject *parent, const QString &name)
{
    auto ipControl = parent->findChild<IPCtrl *>(name);
    if (ipControl == nullptr)
        return { 0, 0, 0, 0 };
    return ipControl->getIP();
}

QString WDFunc::TVField(QWidget *parent, const QString &tvname, int column, bool isid)
{
    auto tv = parent->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return QString();
    auto tvModel = tv->model();
    auto tvIndex = tvModel->index(tv->currentIndex().row(), column);
    auto tmps = tvModel->data(tvIndex, Qt::DisplayRole).toString();
    // если поле с ИД, надо убрать первую цифру - номер таблицы и
    // разделяющую точку, если они присутствуют
    if (isid)
    {
        auto sl = tmps.split(".");
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

void WDFunc::TVAutoResize(QWidget *parent, const QString &tvname)
{
    auto tv = parent->findChild<ETableView *>(tvname);
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

QVariant WDFunc::TVData(QWidget *parent, const QString &tvname, int column)
{
    auto tv = parent->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return QVariant();
    auto m = static_cast<ETableModel *>(tv->model());
    if (m != nullptr)
        return m->index(tv->currentIndex().row(), column, QModelIndex()).data(Qt::DisplayRole);
    return QVariant();
}

QStatusBar *WDFunc::NewSB(QWidget *parent)
{
    const QMap<Board::InterfaceType, QString> images {
        { Board::InterfaceType::USB, ":/icons/usb.svg" },           //
        { Board::InterfaceType::RS485, ":/icons/rs485.svg" },       //
        { Board::InterfaceType::Ethernet, ":/icons/ethernet.svg" }, //
        { Board::InterfaceType::Unknown, ":/icons/stop.svg" }       //
    };

    auto bar = new QStatusBar(parent);
    bar->setMaximumHeight(parent->height() / 20);

    auto widget = new QWidget(parent);
    widget->setMaximumHeight(bar->height());

    auto layout = new QHBoxLayout;
    layout->setSpacing(parent->width() / 20);
    layout->setContentsMargins(1, 1, 1, 1);

    auto msgModel = new QLabel(bar);
    msgModel->setObjectName("Model");
    layout->addWidget(msgModel);

    auto msgSerialNumber = new QLabel(bar);
    msgSerialNumber->setObjectName("SerialNumber");
    layout->addWidget(msgSerialNumber);

    auto msgConnectionState = new QLabel(bar);
    msgConnectionState->setObjectName("ConnectionState");
    layout->addWidget(msgConnectionState);

    auto msgConnectionImage = new QLabel(bar);
    msgConnectionImage->setObjectName("ConnectionImage");
    layout->addWidget(msgConnectionImage);

    auto msgConnectionType = new QLabel(bar);
    msgConnectionType->setObjectName("ConnectionType");
    layout->addWidget(msgConnectionType);

    auto height = bar->height() - layout->contentsMargins().bottom();
    for (auto i = 0; i < layout->count(); ++i)
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

QFrame *WDFunc::newHLine(QWidget *parent)
{
    auto line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine); // Horizontal line
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    return line;
}

QFrame *WDFunc::newVLine(QWidget *parent)
{
    auto line = new QFrame(parent);
    line->setFrameShape(QFrame::VLine); // Vertical line
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    return line;
}

QMainWindow *WDFunc::getMainWindow()
{
    const auto widgets = qApp->topLevelWidgets();
    for (auto parent : widgets)
    {
        auto mainWin = qobject_cast<QMainWindow *>(parent);
        if (mainWin != nullptr)
            return mainWin;
    }
    return nullptr;
}

bool WDFunc::floatIsWithinLimits(const QString &varname, double var, double base, double tolerance, bool showMessage)
{
    if (StdFunc::FloatIsWithinLimits(var, base, tolerance))
        return true;
    else if (showMessage)
    {
        qCritical() << "Ошибочное значение " + varname + ": должно быть " << QString::number(base, 'f', 5) << "±"
                    << QString::number(tolerance, 'f', 5) << ", а получили: " << QString::number(var, 'f', 5);
        EMessageBox::error(nullptr,
            "Ошибочное значение " + varname + ": должно быть " + QString::number(base, 'f', 5) + "±"
                + QString::number(tolerance, 'f', 5) + ", а получили: " + QString::number(var, 'f', 5));
    }
    return false;
}

QString WDFunc::ChooseFileForOpen(QWidget *parent, QString mask)
{
    auto dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    auto filename = dlg->getOpenFileName(
        parent, "Открыть файл", StdFunc::GetHomeDir(), mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    return filename;
}

QString WDFunc::ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr)
{
    auto tmps = Files::ChooseFileForSave(FileHelper::ChooseFileForSave(ext), filenamestr);
    auto dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    auto filename = dlg->getSaveFileName(parent, "Сохранить файл", //
        tmps, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    return filename;
}

QPushButton *WDFunc::NewPBCommon(
    QWidget *parent, const QString &pbname, const QString &text, const QString &icon, const QString &pbtooltip)
{
    auto pb = new QPushButton(parent);
    pb->setObjectName(pbname);
    if (!icon.isEmpty())
        pb->setIcon(QIcon(icon));
    pb->setText(text);
    pb->setToolTip(pbtooltip);
    return pb;
}

QCheckBox *WDFunc::NewChB2(QWidget *parent, const QString &chbname, const QString &chbtext)
{
    auto chb = new QCheckBox(parent);
    chb->setObjectName(chbname);
    chb->setText(chbtext);
    return chb;
}

bool WDFunc::ChBData(const QWidget *parent, const QString &chbname, bool &data)
{
    auto chb = parent->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    data = chb->isChecked();
    return true;
}

bool WDFunc::ChBData(const QWidget *parent, const QString &chbname)
{
    auto chb = parent->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
    {
        qDebug() << Error::NullDataError;
        return false;
    }
    return chb->isChecked();
}

bool WDFunc::SetChBData(QWidget *parent, const QString &chbname, bool data)
{
    auto chb = parent->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    chb->setChecked(data);
    return true;
}

// void WDFunc::AddLabelAndLineedit(QLayout *lyout, QString caption, QString lename, bool enabled)
//{
//    QHBoxLayout *hlyout = new QHBoxLayout;
//    QLabel *lbl = new QLabel(caption);
//    hlyout->addWidget(lbl);
//    QLineEdit *le = new QLineEdit;
//    le->setObjectName(lename);
//    le->setEnabled(enabled);
//    hlyout->addWidget(le);
//    QVBoxLayout *vlyout = static_cast<QVBoxLayout *>(lyout);
//    vlyout->addLayout(hlyout);
//}

void WDFunc::AddLabelAndLineeditH(QLayout *lyout, QString caption, QString lename, bool enabled)
{
    auto hlyout = static_cast<QHBoxLayout *>(lyout);
    auto lbl = new QLabel(caption);
    hlyout->addWidget(lbl, 0);
    auto le = new QLineEdit("");
    le->setObjectName(lename);
    le->setEnabled(enabled);
    hlyout->addWidget(le, 10);
}

QWidget *WDFunc::NewLBLAndLBL(QWidget *parent, QString lblname, QString caption, bool enabled)
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

QWidget *WDFunc::NewLBLAndLE(QWidget *parent, QString caption, QString lename, bool enabled)
{
    auto widget = new QWidget(parent);
    widget->setContentsMargins(0, 0, 0, 0);
    auto hlyout = new QHBoxLayout;
    auto lbl = new QLabel(caption, widget);
    hlyout->addWidget(lbl, 0);
    auto le = new QLineEdit("", widget);
    le->setObjectName(lename);
    le->setEnabled(enabled);
    hlyout->addWidget(le, 10);
    widget->setLayout(hlyout);
    return widget;
}

void WDFunc::SetEnabled(QWidget *parent, const QString &wname, bool enabled)
{
    auto widget = parent->findChild<QWidget *>(wname);
    if (widget != nullptr)
        widget->setEnabled(enabled);
}

void WDFunc::SetVisible(QWidget *parent, const QString &wname, bool visible)
{
    auto widget = parent->findChild<QWidget *>(wname);
    if (widget != nullptr)
    {
        if (visible)
            widget->show();
        else
            widget->hide();
    }
    else
        qDebug() << "No such widget to set it visible";
}

void WDFunc::setMinimumSize(QWidget *parent, const QString &wname, int width, int height)
{
    auto widget = parent->findChild<QWidget *>(wname);
    if (widget != nullptr)
    {
        widget->setMinimumSize(width, height);
        auto pb = qobject_cast<QPushButton *>(widget);
        if (pb != nullptr)
            pb->setIconSize(QSize(width - 10, height - 10));
    }
    else
        qDebug() << "No such widget to set size";
}

QString WDFunc::StringFloatValueWithCheck(float value, int precision, bool exp)
{
    if (value >= MAXFLOAT || value <= -MAXFLOAT)
        return "***";
    else if (exp == true)
        return QString::number(value, 'e', precision);
    else
        return QString::number(value, 'f', precision);
}

QVariant WDFunc::FloatValueWithCheck(float value)
{
    QVariant tmps;
    if (value >= MAXFLOAT || value <= MAXFLOAT)
        tmps = "***";
    else
        tmps = value;
    return tmps;
}

QImage *WDFunc::TwoImages(const QString &first, const QString &second)
{
    auto image = new QImage;
    auto FirstImage = "images/" + first + ".png";
    auto SecondImage = "images/" + second + ".png";
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

ETableView *WDFunc::NewTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model)
{
    auto tv = new ETableView(parent);
    tv->setObjectName(tvname);
    tv->horizontalHeader()->setVisible(true);
    tv->verticalHeader()->setVisible(false);
    if (model != nullptr)
        tv->setModel(model);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setSelectionMode(QAbstractItemView::NoSelection);
    return tv;
}

QTableView *WDFunc::NewQTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model)
{
    auto tv = new QTableView(parent);
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

void WDFunc::SetTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    auto tv = parent->findChild<ETableView *>(tvname);
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

void WDFunc::SetQTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable)
{
    auto tv = parent->findChild<QTableView *>(tvname);
    if (tv == nullptr)
    {
        DBGMSG("Пустой tv");
        return;
    }
    auto selectModel = tv->selectionModel();
    tv->setModel(model);
    tv->resizeColumnsToContents();
    tv->setSortingEnabled(sortenable);
    delete selectModel;
}

void WDFunc::SortTV(QWidget *parent, const QString &tvname, int column, Qt::SortOrder sortorder)
{
    auto tv = parent->findChild<ETableView *>(tvname);
    if (tv == nullptr)
    {
        DBGMSG("Пустой tv");
        return;
    }
    if (column >= 0)
        tv->sortByColumn(column, sortorder);
}

QAbstractItemModel *WDFunc::TVModel(QWidget *parent, const QString &tvname)
{
    auto tv = parent->findChild<ETableView *>(tvname);
    if (tv == nullptr)
        return nullptr;
    return tv->model();
}
