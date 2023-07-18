#ifndef BIERCALC_H
#define BIERCALC_H

// auskommentieren falls nicht als teil von kleiner-brauhelfer-core kompiliert
#include "kleiner-brauhelfer-core_global.h"

#ifdef QT_CORE_LIB
  #include <QObject>
#else
  #define Q_OBJECT
  #define Q_ENUM(x)
  #define Q_INVOKABLE
#endif

#ifndef LIB_EXPORT
  #define LIB_EXPORT
#endif

/**
 * @brief Berechnungen rund um die Bierherstellung
 * @note Wird von QObject abgeleitet, damit die Klasse in QML aufgerufen werden kann
 */
#ifdef QT_CORE_LIB
class LIB_EXPORT BierCalc : public QObject
#else
class LIB_EXPORT BierCalc
#endif
{
    Q_OBJECT

public:

    /**
     * @brief Formeln für die Umrechnung von Brix [°brix] nach Plato [°P] bei vergärter Würze.
     */
    enum FormulaBrixToPlato
    {
        Terrill = 0,
        TerrillLinear = 1,
        Standard = 2,
        Novotny = 3
    };
    Q_ENUM(FormulaBrixToPlato)

    /**
     * @brief Umrechnungsfaktor von Plato [°P] nach Brix [°brix], da Bierwürze
     * keine reine Saccharoselösung ist
     * @note Zwischen 1.02 und 1.06 je nach Literaturangabe, meist 1.03
     */
    static double faktorPlatoToBrix;

    /**
     * @brief Ein kg Malzschrot verdrängt eingemaischt je nach
     * Feinheitsgrad der Schrotung ein Volumen von 0,65 bis 0,8 l Wasser.
     * Näherungsweise rechnet man mit 0.75 l Wasserverdrängung pro kg Malzschrot.
     */
    static constexpr double MalzVerdraengung = 0.75;

    /**
     * @brief Ballingkonstante
     */
    static constexpr double Balling = 2.0665;

    /**
     * @brief Spezifische Wärmekapazität von Wasser [kJ/(kg*°C)]
     */
    static constexpr double cWasser = 4.2;

    /**
     * @brief Spezifische Wärmekapazität von Malz [kJ/(kg*°C)]
     */
    static constexpr double cMalz = 1.7;

public:

    /**
     * @brief Umrechnung von Refraktometerwert [°brix] nach plato [°P]
     * @note 1°Plato = 1g Saccharose / 100g Saccharoselösung
     * @param brix Refraktometerwert [°brix]
     * @return Plato [°P]
     */
    Q_INVOKABLE static double brixToPlato(double brix);

    /**
     * @brief Umrechnung von Refraktometerwert [°brix] nach plato [°P]
     * @note 1°Plato = 1g Saccharose / 100g Saccharoselösung
     * @param plato Plato [°P]
     * @return Refraktometerwert [°brix]
     */
    Q_INVOKABLE static double platoToBrix(double plato);

    /**
     * @brief Umrechnung von Refraktometerwert [°brix] nach spezifische Dichte [g/ml]
     * @param sw Stammwürze [°P]
     * @param brix Refraktometerwert [°brix]
     * @param formel Benutze Umrechnungsformel
     * @return Spezifische Dichte [g/ml]
     */
    Q_INVOKABLE static double brixToDichte(double sw, double brix, BierCalc::FormulaBrixToPlato formel);

    /**
     * @brief Umrechnung spezifische Dichte bei 20°C [g/ml] nach plato [°P]
     * @note 1°Plato = 1g Saccharose / 100g Saccharoselösung
     * @param sg Spezifische Dichte [g/ml]
     * @return Plato [°P]
     */
    Q_INVOKABLE static double dichteToPlato(double sg);

    /**
     * @brief Umrechnung plato [°P] nach spezifische Dichte bei 20°C [g/ml]
     * @note 1°Plato = 1g Saccharose / 100g Saccharoselösung
     * @param plato Plato [°P]
     * @return Spezifische Dichte [g/ml]
     */
    Q_INVOKABLE static double platoToDichte(double plato);

    /**
     * @brief Spezifische Dichte bei temperatur T
     * @param plato
     * @param T
     * @param Tcalib
     * @return
     */
    Q_INVOKABLE static double spindelKorrektur(double plato, double T, double Tcalib = 20.0);

    /**
     * @brief Tatsächlicher Restextrakt (Alkohol-korrigiert) [°P]
     * @param sw Stammwürze [°P]
     * @param sre Scheinbarer Restextrakt [°P]
     * @return Tatsächlicher Restextrakt [°P]
     */
    Q_INVOKABLE static double toTRE(double sw, double sre);

    /**
     * @brief Scheinbarer Restextrakt (Alkohol-verfälscht) [°P]
     * @param sw Stammwürze [°P]
     * @param tre TatsächlicherScheinbarer Restextrakt [°P]
     * @return Scheinbarer Restextrakt [°P]
     */
    Q_INVOKABLE static double toSRE(double sw, double tre);

    /**
     * @brief Vergärungsgrad [%]
     * @param sw Stammwuerze [°P]
     * @param re Restextrakt [°P]
     * @return Vergärungsgrad [%]
     */
    Q_INVOKABLE static double vergaerungsgrad(double sw, double re);

    /**
     * @brief sreAusVergaerungsgrad
     * @param sw
     * @param vg
     * @return
     */
    Q_INVOKABLE static double sreAusVergaerungsgrad(double sw, double vg);

    /**
     * @brief Alkohol [vol%]
     * @param sw Stammwürze [°P]
     * @param sre Scheinbarer Restextrakt [°P]
     * @return Alkohol [vol%]
     */
    Q_INVOKABLE static double alkohol(double sw, double sre, double alcZusatz = 0);

    /**
     * @brief alkoholVonZucker
     * @param m
     * @return
     */
    Q_INVOKABLE static double alkoholVonZucker(double m);

    /**
     * @brief CO2 Gehalt bei bestimmentem Druck und bestimmter Temperatur [g/l]
     * @param p Druck [bar]
     * @param T Temperatur [°C]
     * @return CO2 Gehalt [g/l]
     */
    Q_INVOKABLE static double co2(double p, double T);

    /**
     * @brief Druck bei bestimmentem CO2 Gehalt und bestimmter Temperatur [bar]
     * @param co2 CO2 Gehalt [g/l]
     * @param T Temperatur [°C]
     * @return Druck [bar]
     */
    Q_INVOKABLE static double p(double co2, double T);

    /**
     * @brief Gruenschlauchzeitpunkt [°P]
     * @param co2Soll Soll CO2 Gehalt [g/l]
     * @param sw Stammwuerze [°P]
     * @param sreSchnellgaerprobe Scheinbarer Restextrakt Schnellgaerprobe [°P]
     * @param T Temperatur Jungbier [°C]
     * @return Gruenschlauchzeitpunkt [°P]
     */
    Q_INVOKABLE static double gruenschlauchzeitpunkt(double co2Soll, double sw, double sreSchnellgaerprobe, double T);

    /**
     * @brief spundungsdruck Spundungsdruck [bar]
     * @param co2Soll Soll CO2 Gehalt [g/l]
     * @param T Temperatur Jungbier [°C]
     * @return Spundungsdruck [bar]
     */
    Q_INVOKABLE static double spundungsdruck(double co2Soll, double T);

    /**
     * @brief co2Noetig
     * @param co2Soll
     * @param sw
     * @param sreSchnellgaerprobe
     * @param sreJungbier
     * @param T
     * @return
     */
    Q_INVOKABLE static double co2Noetig(double co2Soll, double sw, double sreSchnellgaerprobe, double sreJungbier, double T);

    /**
     * @brief wuerzeCO2Potential
     * @param sw
     * @param tre
     * @return
     */
    Q_INVOKABLE static double co2Vergaerung(double sw, double tre);

    /**
     * @brief zuckerCO2Potential
     * @return
     */
    Q_INVOKABLE static double co2Zucker();

    /**
     * @brief Benoetigte Speise fuer Karbonisierung [L/L]
     * @param co2Soll Soll CO2 Gehalt [g/l]
     * @param sw Stammwuerze [°P]
     * @param sreSchnellgaerprobe Scheinbarer Restextrakt Schnellgaerprobe [°P]
     * @param sreJungbier Scheinbarer Restextrakt Jungbier [°P]
     * @param T Temperatur Jungbier [°C]
     * @return Speisemenge [L/L]
     */
    Q_INVOKABLE static double speise(double co2Soll, double sw, double sreSchnellgaerprobe, double sreJungbier, double T);

    /**
     * @brief zucker
     * @param co2Soll
     * @param sw
     * @param sreSchnellgaerprobe
     * @param sreJungbier
     * @param T
     * @return
     */
    Q_INVOKABLE static double zucker(double co2Soll, double sw, double sreSchnellgaerprobe, double sreJungbier, double T);

    /**
     * @brief Dichte von Wasser bei gegebenen Temperatur
     * @param T Temperatur [°C]
     * @return Dichte [kg/l]
     */
    Q_INVOKABLE static double dichteWasser(double T);

    /**
     * @brief Berechnet das Volumen von Wasser bei einer andere Temperatur
     * @param T1 Temperatur 1 [°C]
     * @param T2 Temperatur 2 [°C]
     * @param V1 Volumen bei Temperatur 1 [L]
     * @return Volumen bei Temperatur 2 [L]
     */
    Q_INVOKABLE static double volumenWasser(double T1, double T2, double V1);

    /**
     * @brief Berechnet die Verdampfungsziffer
     * @param V1 Anfangsvolumen [L]
     * @param V2 Endvolumen [L]
     * @param t Kochzeit [min]
     * @return Verdampfungsziffer [%]
     */
    Q_INVOKABLE static double verdampfungsziffer(double V1, double V2, double t);

    /**
     * @brief Berechnet die Verdampfungsrate
     * @param V1 Anfangsvolumen [L]
     * @param V2 Endvolumen [L]
     * @param t Kochzeit [min]
     * @return Verdampfungsrate [l/h]
     */
    Q_INVOKABLE static double verdampfungsrate(double V1, double V2, double t);

    /**
     * @brief Berechnet die Sudhausausbeute
     * @param sw Stammwürze [°P]
     * @param V Volumen bei 20°C [L]
     * @param schuettung Schüttung [kg]
     * @return Sudhausausbeute [%]
     */
    Q_INVOKABLE static double sudhausausbeute(double sw, double sw_dichte, double V, double schuettung);

    /**
     * @brief Berechnet die nötige Schüttung
     * @param sw Stammwürze [°P]
     * @param V Volumen bei 20°C [L]
     * @param sudhausausbeute Sudhausausbeute [%]
     * @return Schüttung [kg]
     */
    Q_INVOKABLE static double schuettung(double sw, double sw_dichte, double V, double sudhausausbeute);

    /**
     * @brief Benötigte Wassermenge, um Sollstammwürze zu erreichen
     * @param swIst Iststammwürze [°P]
     * @param swSoll Sollstammwürze [°P]
     * @param swWasser Glattwasserstammwürze [°P]
     * @param menge Volumen [L]
     * @return Verschneidung [L]
     */
    Q_INVOKABLE static double verschneidung(double swIst, double swSoll, double swWasser, double menge);

    /**
     * @brief BierCalc::tinseth
     * @param t
     * @param sw
     * @return
     */
    Q_INVOKABLE static double tinseth(double t, double sw);

    /**
     * @brief mischungstemperaturTm
     * @param m1
     * @param c1
     * @param T1
     * @param m2
     * @param c2
     * @param T2
     * @return
     */
    Q_INVOKABLE static double mischungstemperaturTm(double m1, double c1, double T1, double m2, double c2, double T2);

    /**
     * @brief mischungstemperaturT2
     * @param Tm
     * @param m1
     * @param c1
     * @param T1
     * @param m2
     * @param c2
     * @return
     */
    Q_INVOKABLE static double mischungstemperaturT2(double Tm, double m1, double c1, double T1, double m2, double c2);

    /**
     * @brief mischungstemperaturM2
     * @param Tm
     * @param m1
     * @param c1
     * @param T1
     * @param c2
     * @param T2
     * @return
     */
    Q_INVOKABLE static double mischungstemperaturM2(double Tm, double m1, double c1, double T1, double c2, double T2);

    /**
     * @brief cMaische
     * @param m_malz
     * @param V_wasser
     * @return
     */
    Q_INVOKABLE static double cMaische(double m_malz, double V_wasser);

    /**
     * @brief phMalz
     * @param farbe
     * @return
     */
    Q_INVOKABLE static double phMalz(double farbe);

    /**
     * @brief phMalzCarafa
     * @param farbe
     * @return
     */
    Q_INVOKABLE static double phMalzCarafa(double farbe);

    /**
     * @brief phMalzRoest
     * @param farbe
     * @return
     */
    Q_INVOKABLE static double phMalzRoest(double farbe);

    /**
     * @brief hauptfussFaktor
     * @param ebc
     * @return
     */
    Q_INVOKABLE static double hauptgussFaktor(double ebc);

    /**
     * @brief Farbe im RGB Raum
     * @param ebc EBC Farbwert [EBC]
     * @return Farbwert im RGB Raum
     */
    Q_INVOKABLE static unsigned int ebcToColor(double ebc);
};

#endif // BIERCALC_H
