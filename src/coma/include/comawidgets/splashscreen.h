#pragma once

#include <QSplashScreen>

// Сделать translatable
namespace Prog
{
constexpr char name_ru[] = "АВМ-Сервис";
constexpr char department[] = "ООО \"АВМ-Энерго\", 2016-2025";
constexpr char modules[] = "Modules used:";
constexpr auto libavmgen = "AVM-Gen library, version 1.10.8 (https://git.avmenergo.ru/avm-energo/avm-gen.git)";
constexpr auto libavmwidgets
    = "AVM-Widgets library, version 2.2.6 (https://git.avmenergo.ru/avm-energo/avm-widgets.git)";
constexpr auto hidapi = "HIDAPI library, version 0.15.1 (https://github.com/libusb/hidapi)";
constexpr auto qxlsx = "QXlsx library, version 1.4.9 (https://qtexcel.github.io/QXlsx/)";
constexpr auto limereport = "LimeReport library, version 1.7.14 (https://limereport.ru/)";
constexpr auto qcustomplot = "QCustomPlot library, version 2.1.1 (https://www.qcustomplot.com/)";
constexpr auto icons = "Icons are based on the iconset by 0melapics / Freepik";
constexpr auto fonts = "Font used: FlorenceSans by ShyFoundry (https://shyfoundry.com/home)";
constexpr char prepare[] = "Подготовка \nокружения...";
}

class SplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    SplashScreen(const QPixmap &pixmap = QPixmap(":images/avtuk.png"), Qt::WindowFlags f = Qt::WindowFlags());

protected:
    void drawContents(QPainter *painter) override;
};
