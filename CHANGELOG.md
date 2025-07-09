# Changelog

## Version 2.6.3 (in Entwicklung)
- Neu: Statistik in der Brauübersicht.
- Neu: Menüleiste ersetzt durch Toolbar.
- Neu: Einstellungen neu in einem Dialog. 
- Neu: Reiter Bewertung ersetzt durch Dialog. 
- Neu: Reiter Ausdruck ersetzt durch Dialog. 
- Neu: Reiter Etikett ersetzt durch Dialog. 
- Neu: Hefe- und Zusätzegaben können einzeln zurückgesetzt werden.
- Neu: Knopf ersetzt die Filteroptionen in der Sudauswahl und Brauübersicht.
- Neu: Ein zentrales Hilfefeld.

## Version 2.6.2 (30.12.2023)
- Neu: Undofunktion über INI Datei ausschaltbar ([General] UndoEnabled=false).
- Neu: Unterstützt [MMuM](https://www.maischemalzundmehr.de) Rezepte Version 2.
- Fix: Absturz "neuer Rohstoff nach Speichern wieder löschen".

## Version 2.6.1 (30.09.2023)
- Neu: Änderungen in Eingabefelder lassen sich rückgängig machen (nicht durchgängig implementiert)
- Fix: Absturz bei leeren Diagrammen (Gärverlauf)
- Fix: Verdünnungswerte beim Sud kopieren zurücksetzen
- Fix: Dekoktion überarbeitet

## Version 2.6.0 (05.08.2023)
- Neu: Umstellung auf **Qt v6.5**
- Neu: Datenbankversion **2009**
- Neu: Knopf, um Sudhausausbeute anhand der Malzgaben anzupassen
- Neu: Diagramme überarbeitet (neu mit QCustomPlot statt Qt Charts)
- Neu: Eingabemöglichkeit einer Stammwürze für das Verschnittwasser
- Neu: Maischplan überarbeitet:
  - Zuschütten ermöglicht Schüttungsgabe zu einem späteren Zeitpunkt
  - Gesamtwassermenge (Einmaischen+Zubrühen) muss Hauptgussmenge entsprechen
  - Möglichkeit Wassermengen auszugleichen oder Hauptgussmenge anzupassen
  - Möglichkeit Schüttungsmengen auszugleichen
- Neu: Hefestarter lässt sich im Reiter Braudaten verändern.
- Fix: Gaben & Rasten im Rezeptreiter nicht immer aktualisiert 
- Fix: Anzeige der Hopfengaben mit Kochdauer von 0 Minuten im Spickzettel

## Version 2.5.0 (27.02.2023)
- Neu: Datenbankversion **2008**
- Neu: Niederländische Übersetzung der Tooltips (vielen Dank dachouffe)
- Neu: Knopf, um Hopfengaben auszugleichen (analog wie Malzgaben)
- Neu: Knopf, um Extraktwerte bei Malz und Zusätze auf 100% zu korrigieren
- Neu: Direkte Druck- und Kopiermöglichkeit für Tabellen.
- Neu: Bemerkungsfelder für Maischen, Kochen, Gärung, Maischplan und Wasseraufbereiung
- Neu: Linke Seite in Brau- und Abfülldaten ersetzt mit Diagrammen
- Neu: "Spickzettel/Zusammenfassung" ersetzt durch "Ausdruck" mit frei konfigurierbare Ansicht
- Neu: Spickzettel überarbeitet (nicht alles rückwärtskompatibel!)
- Neu: Unvergärbare Zusätze
- Neu: Menge der Zusätze für die Gärung inklusive Stopfhofen können sich auf die Anstellmenge beziehen
- Neu: Exportformat Brautomat
- Neu: Tooltips aktualisiert
- Fix: Übermäßige Zeilenumbrüche für die Bemerkungsfelder im Spickzettel
- Fix: Absturz beim Ändern eines Eintrags in der Hauptgärung und Schnellgärprobe

## Version 2.4.0 (26.05.2022)
- Neu: Datenbankversion **2007**
- Neu: Negative SRE erlaubt
- Neu: Eigenes Fenster für Verwaltung der Wasserprofile
- Neu: Überprüfung der verstecken Eingabefelder bei ausgeschalteten Modulen
- Neu: Eingabefeld für Wassermenge für Verdünnung der Anstellwürze
- Neu: Hintergrundfarbe des Etiketts für jeden Sud einzeln einstellbar
- Neu: Umrechnungsformel für Brix nach Dichte von Petr Novotný (jens-brand)
- Fix: Reiter Ausrüstung erste ausgewählte Anlage
- Fix: diverse Fehler in der Brauübersicht korrigiert

## Version 2.3.2 (28.01.2022)
- Neu: Neue Webseite mit Dokumentation http://kleiner-brauhelfer.de/ (noch in Arbeit) (vielen Dank Pitastic und alle, die daran gearbeitet haben)
- Neu: Reiter "Rohstoffe", "Brauübersicht", "Ausrüstung" und "Datenbank" neu als Fenster (vielen Dank psytrap)
- Neu: Obergrenze der Zutaten auf 9999 erhöht
- Fix: Kombination Spunden und Zuckerlösung
- Fix: Leerzeichen und Zeilenumbrüche in den Bemerkungsfelder

## Version 2.3.1 (27.06.2021)
- Neu: Offizielle version kompiliert mit **Qt 5.15.2**
- Neu: Die meisten Eingabefelder für Bemerkungen unterstützen HTML (inklusive Editor)
- Neu: Modul Schnellgärprobe ein- und ausschaltbar
- Neu: Menüeintrag für Hilfe (Hilfe noch in Arbeit)
- Fix: Modul Speiseberechnung

## Version 2.3.0 (24.03.2021)
- Neu: Datenbankversion **2006**
- Neu: Verschiedene Module lassen sich ein- und ausschalten (Menu Einstellungen/Module)
- Neu: Weitere Eingabemöglichkeiten für Bemerkungen (Brauen, Abfüllen, Gärung)
- Neu: Eingabemöglichkeit für Beginn der Reifung (nicht mehr an Nachgäverlauf gebunden)
- Neu: Farbige Erkennung, welche Felder geändert wurden
- Neu: Optionen bei Export im kleiner-brauhelfer Format
- Neu: Hintergrundfarbe der Etiketten einstellbar
- Neu: Negative Gärtemperaturen bis -20°C erlaubt
- Neu: Skalierungsfaktor für Benutzeroberfläche lässt sich über Argument "QT_SCALE_FACTOR=1.5" einstellen (kleiner-brauhelfer-2.exe QT_SCALE_FACTOR=1.5)
- Fix: Zeitpunkt der Hopfengaben nicht mehr in Abhängigkeit der Kochdauer
- Fix: Sortierung der Gärverläufe nach Datum
- Fix: Abfülldaten zeigt neu den scheinbaren Vergärungsgrad
- Fix: Negative Speisemenge bei eingeschalteter Schnellgärprobe

## Version 2.2.3 (23.01.2021)
- Neu: Niederländische Übersetzung (vielen Dank dachouffe)
- Neu: Tooltips komplett überarbeitet (vielen Dank rakader)
- Neu: Übernahme des Vergärungsgrades der Hefe
- Neu: Stil der Benutzeroberfläche fix auf "Fusion"
- Neu: Eingabemöglichkeit des Extrakts (Anteil an Stammwürze) für Malzgaben und weitere Zutaten
- Neu: Malzmengen prozentual anpassen (für Übernahme von Rezepten mit Angaben in Gewicht statt Prozent)
- Neu: High DPI Einstellung lässt sich über Argument "QT_AUTO_SCREEN_SCALE_FACTOR=0" deaktivieren (kleiner-brauhelfer-2.exe QT_AUTO_SCREEN_SCALE_FACTOR=0)
- Neu: Dialog für Log (Protokoll)
- Neu: Importfunktion für CSV Datei mit Gärdaten (auch per Drag & Drop)
- Neu: Separates Temperatureingabefeld für die Karbonisierung
- Neu: Bei "Sud gebraut zurücksetzen", "Hefe Zugabe zurücksetzen" und "Weitere Zutaten Zugabe zurücksetzen" lassen sich die Zutaten ins Lager zurücksetzen
- Neu: Neues Dialog für import & export mit Importfunktion aus URL oder Freitext
- Neu: Verschiedene Icons für die Buttons
- Neu: Begriff "weitere Zutat" wird durch "Zusatz" ersetzt
- Fix: Angaben Braumeister 10L
- Fix: Anpassen der Menge beim Ändern der Korrekturmenge in den Ausrüstungsreiter

## Version 2.2.2 (26.09.2020)
- Neu: Vorlagen für diverse Wasserprofile (vielen Dank rakader)
- Neu: Dialog "Restextrakt" überarbeitet
- Neu: Einstellbare Malztemperatur beim Einmaischen
- Fix: Im Reiter "Ausrüstung" Liste der Sude bei Anlage mit Sonderzeichen wird nicht angezeigt
- Fix: Dialog "Zutaten vom Lager abziehen" zeigt die Hopfenmenge nicht immer an
- Fix: Berechnung der Ist-Farbe und Ist-Bittere
- Fix: Übernahme der Verdampfungsrate vom Dialog im Reiter "Ausrüstung"

## Version 2.2.1 (19.07.2020)
- Fix: Sudinformation & Spickzettel werden nur angezeigt, nachdem Sprache eingestellt wird

## Version 2.2.0 (17.07.2020)
- Neu: Datenbankversion **2005**
- Neu: Programm als Beerware lizenziert (siehe Menü Hilfe/Bierspende)
- Neu: Englische & schwedische Übersetzung (Vielen Dank Thomas!)
- Neu: Direkte Auswahl bei Wasseraufbereiung
- Neu: Eingabemöglichkeit für gewünschter pH-Wert
- Fix: Sudkategorien nach oben/unten verschieben

## Version 2.2.0 Beta (27.06.2020)
- Neu: Offizielle version kompiliert mit **Qt 5.15.0**
- Neu: Datenbankversion **2004**
- Neu: Neue Spalte "Alternativen" bei Rohstofftabellen
- Neu: Rohstofftabellen etwas überarbeitet
- Neu: Eingabemöglichkeit für Bemerkungen bei den Anlagen
- Neu: Bei den Etiketten werden Papiergrösse und Ausrichtung gespeichert
- Neu: Eingabemöglichkeit einer Wassermenge bei Karbonisierung mit Zucker (Zuckerlösung)
- Neu: Verdampfungsrate (in l/h) ersetzt die Verdampfungsziffer (in %/h)
- Neu: Berechnung der Bierfarbe nach [brau!magazin](https://braumagazin.de/article/bierfarbe/)
- Neu: "Maischplan" ersetzt "Rasten"
- Neu: Unterstüzt Dekoktion als Maischverfahren
- Neu: Malz-, Hopfen-, Hefe-, weitere Zutatengaben und Maischplan lassen sich aus anderes Rezept übernehmen
- Neu: pH-Wert der Maische kann berechnet werden
- Neu: Sude lassen sich in Kategorien einteilen
- Neu: Neue Eingabefelder im Wasserprofil (Restalkalitätkorrektur und Bemerkung)
- Neu: Reiter Gäverlauf überarbeitet
- Neu: Aninmationen (ausschaltbar)
- Neu: Anlageprofil Brauheld Pro
- Neu: Wasserprofil Eingabemöglichkeit für Sulfat, Chlorid und Natrium
- Neu: Wasseraufbereiung für Zugabe von Säuren und andere Chemikalien

## Version 2.1.3 (02.05.2020)
- Neu: Suche in Rohstoffvorlage durchsucht alle Spalten
- Neu: Neue Import- & Exportfunktion (JSON Format), welche alle Suddaten enthält
- Neu: Datenbankbereinigung sucht ungültige Werte
- Neu: Verschiedene Reiter lassen sich verstecken
- Neu: Neue Anlageeinstellung: Korrektur der Sollmenge, um Verluste zu kompensieren
- Neu: Hopfengaben werden nach Tinseth berechnet und weitere Einflüsse (Pellets, Eiweissbruch, Nachisomerisierung) gemäss [MMuM](https://www.maischemalzundmehr.de/index.php?inhaltmitte=toolsiburechner)
- Fix: "Sud löschen" bei nicht-gespeichertem Sud löscht auch Einträge aus anderen Tabellen

## Version 2.1.2 (12.04.2020)
- Neu: Unterstützung der [oBraMa](https://obrama.mueggelland.de/) Datenbank fürs Einfügen von neuen Rohstoffe (erfordert Internetverbindung)
- Neu: Sudhausausbeute und Vergärungsgrad wird beim Import aus MMuM übernommen
- Neu: Neue Kategorien für "Weitere Zutaten": "Kraut", "Wasseraufbereiung" und "Klärmittel"
- Neu: Neue Einheiten für "Weitere Zutaten": "l" und "ml"
- Neu: "Weitere Zutaten" in HTML Templates überarbeitet (Listen zusammengeführt)
       **Eigene Templates müssen auch überarbeitet werden!**
- Neu: Anlagetyp kann gesetzt werden (Grainfather & Braumeister)
       **Daten sind nicht vollständig!**
- Neu: Direkter PDF Druck wieder möglich
- Fix: Nachisomerisierungszeit bis 90 Minuten
- Fix: Sud kopieren/teilen mit Hopfengaben ohne IBU Berechnung

**Wichtig Linux 64bit:** OpenSSL 1.1.1b oder höher wird für die Überprüfung auf Updates und die oBraMa Anbindung benötigt.
https://askubuntu.com/questions/1102803/how-to-upgrade-openssl-1-1-0-to-1-1-1-in-ubuntu-18-04

## Version 2.1.1 (01.03.2020)
- Neu: Neue Menufunktionen (Sud anlegen, kopieren, teilen, löschen, Rezept importieren, exportieren)
- Neu: Eingabemöglichkeit für Hopfengaben in g/l
- Neu: Hopfenberechnung lässt sich deaktivieren (Prozentanteil enstpricht: ignorieren) für freie Hopfengaben
- Neu: Direktes Drucken des Spickzettels und Zusammenfassung
- Fix: Diverse Felder wurden nicht auf schreibgeschützt gesetzt
- Fix: Hopfengaben beim Ausschlagen haben jetzt eine Ausbeute von 0.0%
- Fix: Brix Berechnung im Spickzettel
- Fix: Minimale Fenstergrösse

## Version 2.1.0 (25.01.2020)
- Neu: Datenbankversion **2003**
- Neu: Dialog für Zutatauswahl
- Neu: Zutatenliste im Spickzettel
- Neu: Sudspezifische Sudhausausbeute und Verdampfungsrate
- Neu: Eingabemöglichkeit des Endvergärungsgrads für die Abschätzung des Alkoholgehalts
- Neu: Gärverlaufeinträge lassen sich über das Clipboard importieren (Copy&Paste)
- Neu: Eingabemöglichkeit für die Stammwürze bei Kochbeginn (nur fürs Protokoll)
- Neu: Eingabemöglichkeit für die Würzemenge vor/nach Hopfenseihen, statt nur bei Kochende
- Neu: Anhänge lassen sich über einen Knopf öffnen
- Neu: Sollstammwürze neu auch in °Brix
- Neu: Update Spickzettel
- Fix: Update der Ressourcen verbessert (erst bei nächstem Update wirksam)
- Fix: Farben für HTML Editiermodus beim dunklen Design
- Fix: Verdünnungsmenge bezieht sich neu auf die Würzemenge beim Anstellen
- Fix: Gebrauter Sud teilen teilt auch Gesamtschüttung und Wassermenge

## Version 2.0.0 (22.12.2019)
- Neu: Datenbankversion **2001**
- Neu: Datenbankdatei wird nur bei erfolgreichem Update verändert
- Neu: Ressourcen wurden in Unterverzeichnisse verschoben
- Neu: Gärverlauf zeigt auch scheinbarer Endvergärungsgrad
- Neu: Neue Spalte "Bemerkung" für Einträge in Gärverläufen
- Neu: Reiter Etiketten überarbeitet
- Fix: Berechnung der Karbonisierung (Speise & Zucker) nach [MMuM](https://www.maischemalzundmehr.de/index.php?inhaltmitte=toolsspeiserechner)
- Fix: Sortierung der Rohstoffe nach Zahlen

## Version 2.0.0beta5 (29.11.2019)
- Neu: Tooltips ein- & ausschaltbar
- Neu: Dialog, um Zutaten vom Rohstofflager abzuziehen
- Neu: Reihenfolge der Rasten änderbar
- Neu: Weitere Hefegaben lassen sich im Reiter Gärverlauf zugeben
- Fix: Grösse der Dialogue
- Fix: Tags bei den Etiketten verwendbar

## Version 2.0.0beta4 (16.11.2019)
- Neu: Offizielle version kompiliert mit Qt 5.13.2
- Neu: Tooltips für die meisten Eingabefelder (Danke rakader)
- Neu: Spickzettel zeigt auch Kochdauer von Hopfengaben und weitere Zutaten
- Neu: Logdatei, wenn Einstellung LogLevel (1-4) in kleiner-brauhelfer-2.ini gesetzt wird
- Neu: Tabellen können in den Clipboard kopiert werden
- Neu: Knopf, um Rohstoff "aufzubrauchen"
- Fix: Eingabe über Buttons für "Stammwürze Kochende", "Stammwürze Anstellen", "Restextrakt Schnellgärprobe" und "Restextrakt Jungbier"
- Fix: Bei BeerXml Export werden alle Pflichtfelder exportiert
- Fix: Keine Rohstoffangabe im Sudinfo bei Rohstoffen mit Klammern
- Fix: Hopfenmengenangaben gehen bei Umstellung der Berechnungsart nicht verloren

## Version 2.0.0beta3 (12.10.2019)
- Neu: Funktion "Eingabefelder entsperren"
- Neu: Umrechnung zwischen Palto und spezifische Dichte nach [Brewer's Friend](http://www.brewersfriend.com/plato-to-sg-conversion-chart/)
- Neu: Milchsäurangabe in 0.1 ml Schritte
- Neu: HTML Templates werden nur aktualisiert, wenn das entsprechende Reiter aktiv ist
- Neu: Zielstammwürze bei Kochbeginn mit/ohne weiteren Zutaten "Kochen" separat aufgeführt
- Neu: Weitere Zutaten können in Milligramm und Stücke angegeben werden
- Fix: Aktualisierung der Hopfenmenge nach Änderung des Hopfens
- Fix: Dialog Rohstoffaustausch beim Löschen eines Hopfens, welches als Stopfhopfen verwendet wird
- Fix: "Sud löschen" löscht auch die Hefegaben

## Version 2.0.0beta2 (28.07.2019)
- Fix: Woche war immer auf 0
- Fix: Abstürze wegen "race condition" in Model Sud
- Fix: Dialog für Stammwürze/Restextrakt zeigt Formel und Korrekturfaktor für Refraktometer
- Fix: Korrektur in der Berechnung der Malzschüttung (Faktor 0.96)
- Neu: keine automatische Sortierung der Rasten
- Neu: Reihenfolge in der Tabelle *Brauübersicht* gedreht
- Neu: Kochzeit bei Honig & Zucker einstellbar
- Neu: Filter für Sudauswahl sucht auch in Tagnamen und Tagwerte
- Neu: Filter für Sudauswahl Checkboxes statt Radiobuttons
- Neu: zusätzliche Spalten in der Sudauswahl auswählbar
- Neu: Zeitpunkt der Hopfengabe auswählbar (Vorderwürze, Kochen, Nachisomerisierung, beim Ausschlagen)
- Neu: Dialog, um ungültige Zutatengaben zu löschen
- Neu: Spickzettel & Zusammenfassung fertiggestellt
- Neu: Dunkles Design fertiggestellt
- Neu: Import & Export für MaischeMalzundMehr und BeerXml
- Neu: Funktion, um Datenbank zu bereinigen
- Neu: Reiter Navigation mit F1 - F12

## Version 2.0.0beta1 (28.05.2019)
- Neu: Datenbankversion **2000**
- Neu: kompletter Neuaufbau des Programms
- Neu: mehrere Hefegaben pro Sud möglich
- Neu: mehrere Wasserprofile möglich
- Neu: Filterfunktion für Rohstofftabellen 
- Neu: mehr Gestaltungsmöglichkeiten bei den HTML Templates
- Neu: Filter im Reiter *Sudauswahl* bestimmt die angezeigten Sude im Reiter *Brauübersicht*
