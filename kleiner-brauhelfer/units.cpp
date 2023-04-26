#include "units.h"
#include "biercalc.h"
#include "settings.h"
#include <QObject>
#include <QLocale>

extern Settings* gSettings;

QString Units::text(Unit unit)
{
    switch (unit)
    {
    case Plato:
        return QObject::tr("°P");
    case SG:
        return QObject::tr("SG");
    case Brix:
        return QObject::tr("°B");
    default:
        return "";
    }
}

int Units::decimals(Unit unit)
{
    switch (unit)
    {
    case Plato:
        return 1;
    case SG:
        return 3;
    case Brix:
        return 1;
    default:
        return 2;
    }
}

double Units::minimum(Unit unit)
{
    switch (unit)
    {
    case Plato:
        return 0;
    case SG:
        return 0;
    case Brix:
        return 0;
    default:
        return 0;
    }
}

double Units::maximum(Unit unit)
{
    switch (unit)
    {
    case Plato:
        return 50;
    case SG:
        return 2;
    case Brix:
        return 50;
    default:
        return 99;
    }
}

double Units::singleStep(Unit unit)
{
    switch (unit)
    {
    case Plato:
        return 0.1;
    case SG:
        return 0.001;
    case Brix:
        return 0.1;
    default:
        return 0.01;
    }
}

double Units::convert(Unit srcunit, Unit destunit, double value)
{
    if (srcunit == destunit)
        return value;
    switch (srcunit)
    {
    case Plato:
        switch (destunit)
        {
        case SG:
            return BierCalc::platoToDichte(value);
        case Brix:
            return BierCalc::platoToBrix(value);
        default:
            break;
        }
        break;
    case Brix:
        switch (destunit)
        {
        case SG:
            return BierCalc::platoToDichte(BierCalc::brixToPlato(value));
        case Plato:
            return BierCalc::brixToPlato(value);
        default:
            break;
        }
        break;
    case SG:
        switch (destunit)
        {
        case Plato:
            return BierCalc::dichteToPlato(value);
        case Brix:
            return BierCalc::platoToBrix(BierCalc::dichteToPlato(value));
        default:
            break;
        }
        break;
    default:
        break;
    }
    return value;
}

QString Units::convertStr(Unit srcunit, Unit destunit, double value, const QLocale &locale)
{
    return toString(destunit, convert(srcunit, destunit, value), false, locale);
}

QString Units::toString(Unit unit, double value, bool withUnit, const QLocale &locale)
{
    QString str = locale.toString(value, 'f', decimals(unit));
    if (withUnit)
        str += " " + text(unit);
    return str;
}

void Units::setGravityUnit(Unit unit)
{
    gSettings->setValueInGroup("Units", "gravity", unit);
}

Units::Unit Units::GravityUnit()
{
    return static_cast<Unit>(gSettings->valueInGroup("Units", "gravity", Plato).toInt());
}
