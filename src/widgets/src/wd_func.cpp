#include <gen/colors.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <models/etablemodel.h>
#include <widgets/epopup.h>
#include <widgets/udialog.h>
#include <widgets/wd_func.h>

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QStatusBar>
#include <QSvgRenderer>
#include <QTextEdit>
#include <QtDebug>
#include <QtMath>

QLabel *WDFunc::NewIcon(QWidget *parent, const QString &iconpath)
{
    auto label = new QLabel(parent);
    label->setStyleSheet("QLabel {border: none;}");
    if (!iconpath.isEmpty())
        label->setPixmap(QPixmap(iconpath));
    return label;
}

QStatusBar *WDFunc::NewSB(QWidget *parent) { }

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
    auto workPath = StdFunc::GetHomeDir();
    auto dlg = new QFileDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    auto filename = dlg->getOpenFileName(parent, "Открыть файл", workPath, mask, Q_NULLPTR);
    if (!filename.isEmpty())
    {
        QFileInfo info(filename);
        StdFunc::SetHomeDir(info.absolutePath());
    }
    dlg->close();
    return filename;
}

QString WDFunc::ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext, const QString &filename)
{
    auto workPath = StdFunc::GetHomeDir();
    auto dlg = new QFileDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    auto fullpath = workPath + "/" + filename + "." + ext;
    auto filepath = dlg->getSaveFileName(parent, "Сохранить файл", fullpath, mask, Q_NULLPTR);
    if (!filepath.isEmpty())
    {
        QFileInfo info(filepath);
        StdFunc::SetHomeDir(info.absolutePath());
    }
    dlg->close();
    return filepath;
}

QString WDFunc::ChooseFileForSave(UDialog *parent, const QString &mask, const QString &ext)
{
    return WDFunc::ChooseFileForSave(parent, mask, ext, parent->getFilenameForDevice());
}

QString WDFunc::ChooseDirectoryForOpen(QWidget *parent)
{
    auto workPath = StdFunc::GetHomeDir();
    auto dlg = new QFileDialog(parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::Directory);
    dlg->setViewMode(QFileDialog::Detail);
    auto dirPath = dlg->getExistingDirectory(parent, "Выбрать папку с файлами", workPath);
    if (!dirPath.isEmpty())
    {
        QFileInfo info(dirPath);
        StdFunc::SetHomeDir(info.absolutePath());
    }
    dlg->close();
    return dirPath;
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
    if (value >= MAXFLOAT || value <= -MAXFLOAT)
        tmps = "***";
    else
        tmps = value;
    return tmps;
}
