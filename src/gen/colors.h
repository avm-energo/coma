#pragma once

#include <QMap>

namespace Colors
{
constexpr char TABCOLOR[] = "#AFFFAF";
constexpr char TABCOLORA1[] = "#F3CC8F";
constexpr char ERPROTCLR[] = "rgba(255,234,255,255)";
constexpr char MAINWINCLR[] = "#E0FFE0";
constexpr char MAINWINCLRA1[] = "#FFFFE0";
// constexpr char DCONFCLR[] = "#DDDDAA";
// constexpr char DCONFWCLR[] = "#EEEECC";
// constexpr char DCONFGCLR[] = "#CCCC99";
// constexpr char DCONFRCLR[] = "#FFCCCC";
// constexpr char DCONFYCLR[] = "#FFFFCC";
// constexpr char DCONFOCLR[] = "#FFCC99";
constexpr char ACONFCLR[] = "#D0FFD0";
constexpr char ACONFWCLR[] = "#C0FFC0";
// constexpr char ACONFWCLRO[] = "#E0FFE0";
constexpr char ACONFGCLR[] = "#B0FFB0";
// constexpr char ACONFRCLR[] = "#FFFFDD";
// constexpr char ACONFYCLR[] = "#FFFFDD";
constexpr char ACONFOCLR[] = "#FFDDAA";
constexpr char ACONFYCLR[] = "#FFFA54";
constexpr char UCONFCLR[] = "#FFFFD0";
// constexpr char UCONFWCLR[] = "#FFFFC0";
// constexpr char ERRCLR[] = "#FF0000";
constexpr char REDCOLOR[] = "#FF2222";
constexpr char RDLCOLOR[] = "#FF4444";
constexpr char RDDCOLOR[] = "#FF0000";
constexpr char GRNCOLOR[] = "#22FF22";
constexpr char NFTCOLOR[] = "#9FC79F";
constexpr char GRLCOLOR[] = "#44FF44";
constexpr char GRDCOLOR[] = "#00FF00";
constexpr char YLWCOLOR[] = "#CCCC22";
constexpr char YLLCOLOR[] = "#CCCC44";
constexpr char YLDCOLOR[] = "#CCCC00";
constexpr char BLDCOLOR[] = "#0000FF";
constexpr char BLUCOLOR[] = "#2222FF";
enum ColorsNumbers
{
    Tab,
    TabA1,
    ErProt,
    MainWin,
    MainWinA1,
    AConf,
    AConfW,
    AConfG,
    AConfO,
    AConfY,
    UConf,
    Red,
    Nft
};

const QMap<ColorsNumbers, QString> ColorsMap {

    { Tab, TABCOLOR },           //
    { TabA1, TABCOLORA1 },       //
    { ErProt, ERPROTCLR },       //
    { MainWin, MAINWINCLR },     //
    { MainWinA1, MAINWINCLRA1 }, //
    { AConf, ACONFCLR },         //
    { AConfW, ACONFWCLR },       //
    { AConfG, ACONFGCLR },       //
    { AConfO, ACONFOCLR },       //
    { AConfY, ACONFYCLR },       //
    { UConf, UCONFCLR },         //
    { Red, REDCOLOR },           //
    { Nft, NFTCOLOR }            //

};
}
