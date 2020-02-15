#ifndef DATABASE_DEFS_H
#define DATABASE_DEFS_H

// Status
#define Sud_Status_Rezept                   0
#define Sud_Status_Gebraut                  1
#define Sud_Status_Abgefuellt               2
#define Sud_Status_Verbraucht               3

// Berechnungsart fuer Hopfen
#define Hopfen_Berechnung_Keine             -1
#define Hopfen_Berechnung_Gewicht           0
#define Hopfen_Berechnung_IBU               1

// Hopfen Art
#define Hopfen_Unbekannt                    0
#define Hopfen_Aroma                        1
#define Hopfen_Bitter                       2
#define Hopfen_Universal                    3

// Hefe Art
#define Hefe_Unbekannt                      0
#define Hefe_Trocken                        1
#define Hefe_Fluessig                       2

// Weitere Zutaten Typen
#define EWZ_Typ_Honig                       0
#define EWZ_Typ_Zucker                      1
#define EWZ_Typ_Gewuerz                     2
#define EWZ_Typ_Frucht                      3
#define EWZ_Typ_Sonstiges                   4
#define EWZ_Typ_Hopfen                      100

// Weitere Zutaten Zugabezeitpunkt
#define EWZ_Zeitpunkt_Gaerung               0
#define EWZ_Zeitpunkt_Kochen                1
#define EWZ_Zeitpunkt_Maischen              2

// Weitere Zutaten zugabestatus
#define EWZ_Zugabestatus_nichtZugegeben     0
#define EWZ_Zugabestatus_Zugegeben          1
#define EWZ_Zugabestatus_Entnommen          2

// Entnahmeindex
#define EWZ_Entnahmeindex_MitEntnahme       0
#define EWZ_Entnahmeindex_KeineEntnahme     1

// Weitere Zutaten Einheit
#define EWZ_Einheit_Kg                      0
#define EWZ_Einheit_g                       1
#define EWZ_Einheit_mg                      2
#define EWZ_Einheit_Stk                     3

// Maximum Anzahl Sterne
#define Bewertung_MaxSterne                 5

#endif // DATABASE_DEFS_H
