#include "trendviewmodel85.h"

#include <gen/colors.h>

TrendViewModel85::TrendViewModel85(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModel85::analogColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
}

QStringList TrendViewModel85::digitalColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::BLUCOLOR, //
        Colors::BLDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
}

QStringList TrendViewModel85::analogDescriptions() const
{
    return {
        "Напряжение сети ф.А",        //
        "Напряжение сети ф.В",        //
        "Напряжение сети ф.С",        //
        "Ток в выключателе ф.А",      //
        "Ток в выключателе ф.В",      //
        "Ток в выключателе ф.С",      //
        "Напряжение на нагрузке ф.А", //
        "Напряжение на нагрузке ф.В", //
        "Напряжение на нагрузке ф.С"  //
    };
}

QStringList TrendViewModel85::digitalDescriptions() const
{
    return {
        "Оперативное управление включением полюса А",  //
        "Оперативное управление включением полюса B",  //
        "Оперативное управление включением полюса C",  //
        "Оперативное управление отключением полюса А", //
        "Оперативное управление отключением полюса B", //
        "Оперативное управление отключением полюса C", //
        "НЗ блок-контакт полюса А",                    //
        "НЗ блок-контакт полюса B",                    //
        "НЗ блок-контакт полюса C",                    //
        "НО блок-контакт полюса А",                    //
        "НО блок-контакт полюса В",                    //
        "НО блок-контакт полюса С",                    //
        "Контроль цепи соленоида отключения",          //
        "Контроль цепи соленоида включения",           //
        "Выходная команда на включение полюса А",      //
        "Выходная команда на включение полюса В",      //
        "Выходная команда на включение полюса С",      //
        "Выходная команда на отключение полюса А",     //
        "Выходная команда на отключение полюса В",     //
        "Выходная команда на отключение полюса С",     //
        "Включение полюса А нефильтр.",                //
        "Включение полюса В нефильтр.",                //
        "Включение полюса С нефильтр.",                //
        "Отключение полюса А нефильтр.",               //
        "Отключение полюса В нефильтр.",               //
        "Отключение полюса С нефильтр.",               //
        "НЗ блок-контакт полюса А нефильтр.",          //
        "НЗ блок-контакт полюса B нефильтр.",          //
        "НЗ блок-контакт полюса C нефильтр.",          //
        "НО блок-контакт полюса А нефильтр.",          //
        "НО блок-контакт полюса В нефильтр.",          //
        "НО блок-контакт полюса С нефильтр."           //
    };
}

QStringList TrendViewModel85::analogValues() const
{
    return { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };
}

QStringList TrendViewModel85::digitalValues() const
{
    return digitalDescriptions();
    //    { "OCNA", "OCNB", "OCNC", "OCFA", "OCFB", "OCFC", "BKCA", "BKCB", "BKCC", "BKOA", "BKOB", "BKOC", "CSC",
    //        "CSO", "CNA", "CNB", "CNC", "CFA", "CFB", "CFC", "NNA", "NNB", "NNC", "NFA", "NFB", "NFC", "NCA", "NCB",
    //        "NCC", "NOA", "NOB", "NOC" };
}

std::bitset<32> TrendViewModel85::digitalEnableFlags() const
{
    return { 0b11111111111100111000000000000000 };
}
