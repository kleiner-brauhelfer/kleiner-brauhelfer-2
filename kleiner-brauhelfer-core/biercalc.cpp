#include "biercalc.h"
#include <math.h>
#include <cmath>
#include <limits>


double BierCalc::faktorPlatoToBrix = 1.03;

double BierCalc::brixToPlato(double brix)
{
    return brix / faktorPlatoToBrix;
}

double BierCalc::platoToBrix(double plato)
{
    return plato * faktorPlatoToBrix;
}

double BierCalc::brixToDichte(double sw, double brix, FormulaBrixToPlato formel)
{
    double b = brixToPlato(brix);
    switch (formel)
    {
    case Terrill:
        // http://seanterrill.com/2011/04/07/refractometer-fg-results/
        return 1 - 0.0044993*sw + 0.0117741*b + 0.000275806*sw*sw - 0.00127169*b*b - 0.00000727999*sw*sw*sw + 0.0000632929*b*b*b;
    case TerrillLinear:
        // http://seanterrill.com/2011/04/07/refractometer-fg-results/
        return 1.0000 - 0.00085683*sw + 0.0034941*b;
    case Standard:
        return 1.001843-0.002318474*sw - 0.000007775*sw*sw - 0.000000034*sw*sw*sw + 0.00574*brix + 0.00003344*brix*brix + 0.000000086*brix*brix*brix;
    case Novotny:
        // http://www.diversity.beer/2017/01/pocitame-nova-korekce-refraktometru.html
        return (-1 * 0.002349)*sw + 0.006276*b + 1;

        // quadratische Formel, laut Novotny geringfügig genauer, damit aber genauer als die Messungenauigkeit, daher reicht die lineare Formel aus.
        //return 1.335*pow(10,-5)*pow(sw,2) - 3.239*pow(10,-5)*sw*b + 2.961*pow(10,-5)*pow(b,2) - 2.421*pow(10,-3)*sw + 6.219*pow(10, -3)*b + 1;
    default:
        return 0.0;
    }
}

double BierCalc::dichteToPlato(double sg)
{
  #if 0
    // deClerk: http://www.realbeer.com/spencer/attenuation.html
    return 668.72 * sg - 463.37 - 205.347 * sg * sg;
  #else
    // http://www.brewersfriend.com/plato-to-sg-conversion-chart/
    return (-1 * 616.868) + (1111.14 * sg) - (630.272 * sg * sg) + (135.997 * sg * sg * sg);
  #endif
}

double BierCalc::platoToDichte(double plato)
{
  #if 0 // Umkehrformel zu dichteToPlato()
    // deClerk: http://www.realbeer.com/spencer/attenuation.html
    double a = 205.347;
    double b = 668.72;
    double c = 463.37 + plato;
    double d = 4 * a * c;
    return (-b + sqrt(b * b - d)) / (2 * a) * -1;
  #elif 0 // Formel vom Internet
    return 261.11 / (261.53 - plato);
  #else
    // http://www.brewersfriend.com/plato-to-sg-conversion-chart/
    return 1 + (plato / (258.6 - ((plato / 258.2) * 227.1)));
  #endif
}

static double interpolate3(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x)
{
    // Lagrange Interpolation k=3 (y=ax^3+bx^2+cx+d)
    double y =
        y1 * (((x - x2) * (x - x3) * (x - x4)) / ((x1 - x2) * (x1 - x3) * (x1 - x4))) +
        y2 * (((x - x1) * (x - x3) * (x - x4)) / ((x2 - x1) * (x2 - x3) * (x2 - x4))) +
        y3 * (((x - x1) * (x - x2) * (x - x4)) / ((x3 - x1) * (x3 - x2) * (x3 - x4))) +
        y4 * (((x - x1) * (x - x2) * (x - x3)) / ((x4 - x1) * (x4 - x2) * (x4 - x3)));
    return y;
}

static void interpolateTemperature(double* result, double T)
{
    // Dichteverteilung von Saccharose-Lösungen bei verschiedenen Temperaturen
    // Zuckertechniker-Taschenbuch, Albert Bartens Verlage, Berlin, 1966, 7. Auflage
    const double density_tbl[11][9] = {
        // 0      5     10     15     20     25     30     35     40     g/100g
        {1.0002,1.0204,1.0418,1.0632,1.0851,1.1088,1.1323,1.1574,1.1840}, //   0°C
        {0.9997,1.0196,1.0402,1.0614,1.0835,1.1064,1.1301,1.1547,1.1802}, //  10°C
        {0.9982,1.0178,1.0381,1.0591,1.0810,1.1035,1.1270,1.1513,1.1764}, //  20°C
        {0.9957,1.0151,1.0353,1.0561,1.0777,1.1000,1.1232,1.1473,1.1723}, //  30°C
        {0.9922,1.0116,1.0316,1.0522,1.0737,1.0958,1.1189,1.1428,1.1676}, //  40°C
        {0.9881,1.0072,1.0271,1.0477,1.0690,1.0910,1.1140,1.1377,1.1624}, //  50°C
        {0.9832,1.0023,1.0221,1.0424,1.0636,1.0856,1.1085,1.1321,1.1568}, //  60°C
        {0.9778,0.9968,1.0165,1.0368,1.0579,1.0798,1.1026,1.1262,1.1507}, //  70°C
        {0.9718,0.9908,1.0104,1.0306,1.0517,1.0735,1.0963,1.1198,1.1443}, //  80°C
        {0.9653,0.9842,1.0038,1.0240,1.0450,1.0669,1.0896,1.1130,1.1375}, //  90°C
        {0.9584,0.9773,0.9968,1.0169,1.0379,1.0599,1.0825,1.1058,1.1301}  // 100°C
    };
    int lowest = (int)floor(T / 10) - 1;
    if (lowest < 0)
        lowest = 0;
    if (lowest > 7)
        lowest = 7;
    int T1 = lowest;
    int T2 = lowest + 1;
    int T3 = lowest + 2;
    int T4 = lowest + 3;
    for (int i = 0; i < 9; i++)
        result[i] = interpolate3(T1 * 10.0, density_tbl[T1][i],
                                 T2 * 10.0, density_tbl[T2][i],
                                 T3 * 10.0, density_tbl[T3][i],
                                 T4 * 10.0, density_tbl[T4][i],
                                 T);
}

double BierCalc::spindelKorrektur(double plato, double T, double Tcalib)
{
    double tmp[9];

    // dichte bei plato und Tcalib
    interpolateTemperature(tmp, Tcalib);
    int lowest = (int)floor(plato / 5) - 1;
    if (lowest < 0)
        lowest = 0;
    if (lowest > 5)
        lowest = 5;
    int plato1 = lowest;
    int plato2 = lowest + 1;
    int plato3 = lowest + 2;
    int plato4 = lowest + 3;
    double dichteTcalib =  interpolate3(plato1 * 5.0, tmp[plato1],
                                        plato2 * 5.0, tmp[plato2],
                                        plato3 * 5.0, tmp[plato3],
                                        plato4 * 5.0, tmp[plato4],
                                        plato);

    // plato bei dichte und T
    interpolateTemperature(tmp, T);
    lowest = 0;
    for (int i = 0; i < 9; i++)
        if (dichteTcalib > tmp[i])
            lowest = i - 1;
    if (lowest < 0)
        lowest = 0;
    if (lowest > 5)
        lowest = 5;
    plato1 = lowest;
    plato2 = lowest + 1;
    plato3 = lowest + 2;
    plato4 = lowest + 3;
    return interpolate3(tmp[plato1], plato1 * 5.0,
                        tmp[plato2], plato2 * 5.0,
                        tmp[plato3], plato3 * 5.0,
                        tmp[plato4], plato4 * 5.0,
                        dichteTcalib);
}

double BierCalc::toTRE(double sw, double sre)
{
#if 1
    // Balling: http://realbeer.com/spencer/attenuation.html
    return 0.1808 * sw + 0.8192 * sre;
#else
    // http://hobbybrauer.de/modules.php?name=eBoard&file=viewthread&fid=1&tid=11943
    double q = 0.22 + 0.001 * sw;
    return (q * sw + sre)/(1 + q);
#endif
}

double BierCalc::toSRE(double sw, double tre)
{
#if 1
    // Balling: http://realbeer.com/spencer/attenuation.html
    return (tre - 0.1808 * sw) / 0.8192;
#else
    // http://hobbybrauer.de/modules.php?name=eBoard&file=viewthread&fid=1&tid=11943
    double q = 0.22 + 0.001 * sw;
    return tre * (1 + q) - q * sw;
#endif
}

double BierCalc::vergaerungsgrad(double sw, double re)
{
    if (sw == 0.0)
        return 0.0;
    return (1 - re / sw) * 100;
}

double BierCalc::sreAusVergaerungsgrad(double sw, double vg)
{
    return sw * (100 - vg) / 100;
}

double BierCalc::alkohol(double sw, double sre, double alcZusatz)
{
    if (sw <= 0.0)
        return 0.0;

    double tre = toTRE(sw, sre);
    double dichte = platoToDichte(tre);

    // Alkohol Gewichtsprozent
    // http://www.realbeer.com/spencer/attenuation.html
    // Balling: 2.0665g Extrakt ergibt 1g Alkohol, 0.9565g CO2, 0.11g Hefe
    double alcGewicht = (sw - tre) / (Balling - 0.010665 * sw);

    // Alkohol Volumenprozent
    double alc = (alcGewicht + alcZusatz) * dichte / 0.794;
    if (alc < 0.0)
        alc = 0.0;
    return alc;
}

double BierCalc::alkoholVonZucker(double m)
{
    return m * 0.488 / 10;
}

double BierCalc::co2(double p, double T)
{
    return (1.013 + p) * exp(-10.73797+(2617.25/(T+273.15))) * 10;
}

double BierCalc::p(double co2, double T)
{
    return co2 / (exp(-10.73797+(2617.25/(T+273.15))) * 10) - 1.013;
}

double BierCalc::gruenschlauchzeitpunkt(double co2Soll, double sw, double sreSchnellgaerprobe, double T)
{
    double tre = toTRE(sw, sreSchnellgaerprobe);
    if (tre < 0.0)
        tre = 0.0;
    double co2Noetig = co2Soll - co2(0.0, T);
    double extraktCO2 = co2Noetig / (co2Zucker() * 10);
    if (extraktCO2 < 0.0)
        extraktCO2 = 0.0;
    return toSRE(sw, tre + extraktCO2);
}

double BierCalc::spundungsdruck(double co2Soll, double T)
{
    double res = p(co2Soll, T);
    if (res < 0.0)
        res = 0.0;
    return res;
}

double BierCalc::co2Vergaerung(double sw, double tre)
{
    return co2Zucker() * 10 * (sw - tre);
}

double BierCalc::co2Zucker()
{
    return 0.468;
}

double BierCalc::co2Noetig(double co2Soll, double sw, double sreSchnellgaerprobe, double sreJungbier, double T)
{
    return co2Soll - co2(0.0, T) - co2Vergaerung(toTRE(sw, sreJungbier), toTRE(sw, sreSchnellgaerprobe));
}

double BierCalc::speise(double co2Soll, double sw, double sreSchnellgaerprobe, double sreJungbier, double T)
{
    double co2Pot = co2Vergaerung(sw, toTRE(sw, sreSchnellgaerprobe));
    if (co2Pot <= 0)
        return std::numeric_limits<double>::infinity();
    double speise = co2Noetig(co2Soll, sw, sreSchnellgaerprobe, sreJungbier, T) / co2Pot;
    if (speise < 0)
        speise = 0;
    return speise;
}

double BierCalc::zucker(double co2Soll, double sw, double sreSchnellgaerprobe, double sreJungbier, double T)
{
    double co2Pot = co2Zucker();
    if (co2Pot <= 0)
        return std::numeric_limits<double>::infinity();
    double zucker = co2Noetig(co2Soll, sw, sreSchnellgaerprobe, sreJungbier, T) / co2Pot;
    if (zucker < 0)
        zucker = 0;
    return zucker;
}

double BierCalc::dichteWasser(double T)
{
    const double a0 = 999.83952;
    const double a1 = 16.952577;
    const double a2 = -0.0079905127;
    const double a3 = -0.000046241757;
    const double a4 = 0.00000010584601;
    const double a5 = -0.00000000028103006;
    const double b = 0.016887236;
    return (a0 + T * a1 + pow(T,2) * a2 + pow(T,3) * a3 + pow(T,4) * a4 + pow(T,5) * a5) / (1 + T * b) * 1e-3;
}

double BierCalc::volumenWasser(double T1, double T2, double V1)
{
    double rho1 = dichteWasser(T1);
    double rho2 = dichteWasser(T2);
    return (rho1 * V1) / rho2;
}

double BierCalc::verdampfungsziffer(double V1, double V2, double t)
{
    if (t == 0.0 || V2 == 0.0 || V1 < V2)
        return 0.0;
    return ((V1 - V2) * 100 * 60) / (V2 * t);
}

double BierCalc::verdampfungsrate(double V1, double V2, double t)
{
    if (t == 0.0 || V2 == 0.0 || V1 < V2)
        return 0.0;
    return ((V1 - V2) * 60) / t;
}

double BierCalc::sudhausausbeute(double sw, double sw_dichte, double V, double schuettung)
{
    if (schuettung <= 0.0)
        return 0.0;
    return sw * platoToDichte(sw_dichte) * V / schuettung;
}

double BierCalc::schuettung(double sw, double sw_dichte, double V, double sudhausausbeute)
{
    if (sudhausausbeute <= 0.0)
        return 0.0;
    return sw * platoToDichte(sw_dichte) * V / sudhausausbeute;
}

double BierCalc::verschneidung(double swIst, double swSoll, double menge)
{
    if (swIst < swSoll || swSoll == 0.0)
        return 0.0;
    return menge * (swIst / swSoll - 1);
}

double BierCalc::tinseth(double t, double sw)
{
    // https://realbeer.com/hops/
    if (t < 0)
        return 0;
    double bigness  = 1.65 * pow(0.000125, 0.004 * sw);
    double boiltime = (1 - exp(-0.04 * t)) / 4.15;
    return bigness * boiltime;
}

double BierCalc::mischungstemperaturTm(double m1, double c1, double T1, double m2, double c2, double T2)
{
    return (m1*c1*T1 + m2*c2*T2) / (m1*c1 + m2*c2);
}

double BierCalc::mischungstemperaturT2(double Tm, double m1, double c1, double T1, double m2, double c2)
{
    return Tm + (m1*c1) / (m2*c2) * (Tm-T1);
}

double BierCalc::mischungstemperaturM2(double Tm, double m1, double c1, double T1, double c2, double T2)
{
    if (T2 == Tm)
        return 0;
    return (m1*c1) / c2 * (Tm-T1) / (T2-Tm);
}

double BierCalc::cMaische(double m_malz, double V_wasser)
{
    double m_wasser = V_wasser * dichteWasser(20);
    return (m_malz * cMalz + m_wasser * cWasser) / (m_malz + m_wasser);
}

double BierCalc::einmaischetemperatur(double T_rast, double m_malz, double T_malt, double V_wasser)
{
    return BierCalc::mischungstemperaturT2(T_rast, m_malz, cMalz, T_malt, V_wasser * dichteWasser(20), cWasser);
}

double BierCalc::phMalz(double farbe)
{
    return -0.255 * std::log(farbe) + 6.156;
}

double BierCalc::phMalzCarafa(double farbe)
{
    return -0.289 * std::log(farbe) + 6.069;
}

double BierCalc::phMalzRoest(double farbe)
{
    (void)farbe;
    return 4.65;
}

double BierCalc::hauptgussFaktor(double ebc)
{
    if (ebc < 50)
        return 4.0 - ebc * 0.02;
    else
        return 3.0;
}

static unsigned int toRgb(unsigned char r, unsigned char g, unsigned char b)
{
    return (0xffu << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | (b & 0xffu);
}

unsigned int BierCalc::ebcToColor(double ebc)
{
    const unsigned char farben[99][3] = {
        {255, 246, 169},
        {255, 244, 163},
        {254, 241, 154},
        {254, 237, 143},
        {254, 232, 129},
        {254, 226, 114},
        {254, 219, 97},
        {253, 212, 86},
        {252, 205, 75},
        {251, 197, 65},
        {249, 188, 55},
        {247, 178, 45},
        {245, 167, 35},
        {242, 157, 25},
        {239, 148, 16},
        {233, 139, 7},
        {225, 130, 0},
        {215, 121, 0},
        {206, 112, 0},
        {198, 103, 0},
        {192, 94, 0},
        {187, 85, 0},
        {182, 76, 0},
        {177, 67, 0},
        {173, 58, 0},
        {169, 49, 0},
        {165, 40, 0},
        {161, 31, 0},
        {157, 22, 0},
        {153, 13, 0},
        {149, 4, 0},
        {145, 0, 0},
        {141, 0, 0},
        {137, 0, 0},
        {133, 0, 0},
        {129, 0, 0},
        {125, 0, 0},
        {121, 0, 0},
        {118, 0, 0},
        {115, 0, 0},
        {112, 0, 0},
        {109, 0, 0},
        {106, 0, 0},
        {103, 0, 0},
        {100, 0, 0},
        {97, 0, 0},
        {94, 0, 0},
        {91, 0, 0},
        {88, 0, 0},
        {85, 0, 0},
        {82, 0, 0},
        {79, 0, 0},
        {76, 0, 0},
        {73, 0, 0},
        {70, 0, 0},
        {67, 0, 0},
        {64, 0, 0},
        {61, 0, 0},
        {58, 0, 0},
        {56, 0, 0},
        {54, 0, 0},
        {52, 0, 0},
        {50, 0, 0},
        {48, 0, 0},
        {46, 0, 0},
        {44, 0, 0},
        {42, 0, 0},
        {40, 0, 0},
        {38, 0, 0},
        {36, 0, 0},
        {34, 0, 0},
        {32, 0, 0},
        {30, 0, 0},
        {28, 0, 0},
        {26, 0, 0},
        {24, 0, 0},
        {22, 0, 0},
        {21, 0, 0},
        {20, 0, 0},
        {19, 0, 0},
        {18, 0, 0},
        {17, 0, 0},
        {16, 0, 0},
        {15, 0, 0},
        {14, 0, 0},
        {13, 0, 0},
        {12, 0, 0},
        {11, 0, 0},
        {10, 0, 0},
        {9, 0, 0},
        {8, 0, 0},
        {7, 0, 0},
        {6, 0, 0},
        {5, 0, 0},
        {4, 0, 0},
        {3, 0, 0},
        {2, 0, 0},
        {1, 0, 0},
        {0, 0, 0}
    };
    int idx = static_cast<int>(ebc - 2);
    if (idx < 0)
        idx = 0;
    if (idx > 98)
        idx = 98;
    return toRgb(farben[idx][0], farben[idx][1], farben[idx][2]);
}
