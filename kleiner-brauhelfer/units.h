#ifndef UNITS_H
#define UNITS_H

#include <QString>
#include <QLocale>

class Units
{
public:

    enum Unit
    {
        None = 0,
        Default = 0,

        Plato = 100,
        SG = 101,
        Brix = 102

        /*
        mL = 200,
        L = 201,
        hL = 202,

        mg = 300,
        g = 301,
        kg = 302,

        Stueck = 400,
        percent = 401

        gradC = 500,
        gradF = 501,

        percentVol = 600,

        IBU = 700,

        EBC = 800,

        mbar = 900,
        bar = 901,
        Pa = 902,

        dH = 1000,

        sec = 1100,
        min = 1101,
        hour = 1102,
        */
    };

    static QString text(Unit unit);
    static int decimals(Unit unit);
    static double minimum(Unit unit);
    static double maximum(Unit unit);
    static double singleStep(Unit unit);

    static double convert(Unit srcunit, Unit destunit, double value);
    static QString convertStr(Unit srcunit, Unit destunit, double value, const QLocale &locale = QLocale());

    static QString toString(Unit unit, double value, bool withUnit, const QLocale &locale = QLocale());

    static void setGravityUnit(Unit unit);
    static Unit GravityUnit();
};

#endif // UNITS_H
