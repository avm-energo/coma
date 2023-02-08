#pragma once

#include <QDialog>

namespace AboutText
{
constexpr auto department = "ООО \"АВМ-Энерго\", 2016-2023";
constexpr auto modules = "Modules used:";
constexpr auto liblzma = "LZMA library from XZ Utils, version 5.2.5 (https://tukaani.org/xz/)";
constexpr auto hidapi = "HIDAPI library, version 0.9.0 (https://github.com/libusb/hidapi)";
constexpr auto qxlsx = "QXlsx library, version 1.3.56 (https://qtexcel.github.io/QXlsx/)";
constexpr auto limereport = "LimeReport library, version 1.5.1 (https://limereport.ru/)";
constexpr auto qcustomplot = "QCustomPlot library, version 2.1.0 (https://www.qcustomplot.com/)";
constexpr auto icons = "Icons are based on the iconset by 0melapics / Freepik";
constexpr auto fonts = "Font used: FlorenceSans by ShyFoundry (https://shyfoundry.com/home)";
}

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    void setupUI();
    void prependLine(const QString &str);
    void appendLine(const QString &str);

private:
    QStringList m_lines { //
        AboutText::department, AboutText::modules, AboutText::liblzma, AboutText::hidapi, AboutText::qxlsx,
        AboutText::limereport, AboutText::qcustomplot, AboutText::icons, AboutText::fonts
    };
};
