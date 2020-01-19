# kleiner-brauhelfer v2
Der kleine-brauhelfer ist ein Hilfsprogramm für Hobbybrauer zum Erstellen und Verwalten von Biersuden. Die Version 2 ist die Weiterentwicklung vom [kleinen-brauhelfer](http://github.com/Gremmel/kleiner-brauhelfer).

**Diskussionsthread:**

https://hobbybrauer.de/forum/viewtopic.php?f=3&t=21510

## Download letzte Version
- [Version 2.0.0](https://github.com/kleiner-brauhelfer/kleiner-brauhelfer-2/releases/tag/v2.0.0)

### Windows
- **kbh2_v2.x.x_setup_win_x64.exe**: Installationsdatei für 64bit System
- **kbh2_v2.x.x_portable_win_x64.zip**: Portable Version für 64bit System
- **kbh2_v2.x.x_setup_win_x86.exe**: Installationsdatei für 32bit System
- **kbh2_v2.x.x_portable_win_x86.zip**: Portable Version für 32bit System

### Linux
- **kbh2_v2.x.x_linux_x64.deb**: Installationsdatei für 64bit System
  - Benötigt zusätzlich Installation von [Qt 5.13.2](https://www.qt.io/download-qt-installer) mit folgenden Komponenten
    - Desktop gcc 64-bit, Qt Charts, Qt WebEngine
- **kleiner-brauhelfer-2-x86_64.AppImage**: Portable Version (AppImage) für 64bit System
- **kbh2_v2.x.x_linux_x32.deb**: Installationsdatei für 32bit System
  - Neuere Qt Versionen werden für Linux 32bit nicht mehr angeboten. Deshalb werden nicht alle Features unterstüzt.
  - Benötigt zusätzlich Installation von [Qt 5.5](https://www.qt.io/download-qt-installer) mit folgenden Komponenten
    - Desktop gcc, Qt WebEngine

### Mac OS
- **kbh2_v2.x.x_macos.zip**: Installationsdatei für Mac OS

## Änderungen & Erweiterungen

### Version 2.1.0 (in Entwicklung)
- Neu: Datenbankversion **2003**
- Neu: Dialog für Zutatauswahl
- Neu: Zutatenliste im Spickzettel
- Neu: Sudspezifische Sudhausausbeute und Verdampfungsrate
- Neu: Eingabemöglichkeit des Endvergärungsgrads für die Abschätzung des Alkoholgehalts
- Neu: Gärverlaufeinträge lassen sich über das Clipboard importieren (Copy&Paste)
- Neu: Eingabemöglichkeit für die Stammwürze bei Kochbeginn (nur fürs Protokoll)
- Neu: Eingabemöglichkeit für die Würzemenge vor/nach Hopfenseihen, statt nur bei Kochende
- Neu: Anhänge lassen sich über einen Knopf öffnen
- Fix: Update der Ressourcen verbessert (erst bei nächstem Update wirksam)
- Fix: Farben für HTML Editiermodus beim dunklen Design
- Fix: Verdünnungsmenge bezieht sich neu auf die Würzemenge beim Anstellen
- Fix: Gebrauter Sud teilen teilt auch Gesamtschüttung und Wassermenge

### Version 2.0.0 (22.12.2019)
- Neu: Datenbankversion **2001**
- Neu: Datenbankdatei wird nur bei erfolgreichem Update verändert
- Neu: Ressourcen wurden in Unterverzeichnisse verschoben
- Neu: Gärverlauf zeigt auch scheinbarer Endvergärungsgrad
- Neu: Neue Spalte "Bemerkung" für Einträge in Gärverläufen
- Neu: Reiter Etiketten überarbeitet
- Fix: Berechnung der Karbonisierung (Speise & Zucker) nach [MMuM](https://www.maischemalzundmehr.de/index.php?inhaltmitte=toolsspeiserechner)
- Fix: Sortierung der Rohstoffe nach Zahlen

### Version 2.0.0beta5 (29.11.2019)
- Neu: Tooltips ein- & ausschaltbar
- Neu: Dialog, um Zutaten vom Rohstofflager abzuziehen
- Neu: Reihenfolge der Rasten änderbar
- Neu: Weitere Hefegaben lassen sich im Reiter Gärverlauf zugeben
- Fix: Grösse der Dialogue
- Fix: Tags bei den Etiketten verwendbar

### Version 2.0.0beta4 (16.11.2019)
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

### Version 2.0.0beta3 (12.10.2019)
- Neu: Funktion "Eingabefelder entsperren"
- Neu: Umrechnung zwischen Palto und spezifische Dichte nach [Brewer's Friend](http://www.brewersfriend.com/plato-to-sg-conversion-chart/)
- Neu: Milchsäurangabe in 0.1 ml Schritte
- Neu: HTML Templates werden nur aktualisiert, wenn das entsprechende Reiter aktiv ist
- Neu: Zielstammwürze bei Kochbeginn mit/ohne weiteren Zutaten "Kochen" separat aufgeführt
- Neu: Weitere Zutaten können in Milligramm und Stücke angegeben werden
- Fix: Aktualisierung der Hopfenmenge nach Änderung des Hopfens
- Fix: Dialog Rohstoffaustausch beim Löschen eines Hopfens, welches als Stopfhopfen verwendet wird
- Fix: "Sud löschen" löscht auch die Hefegaben

### Version 2.0.0beta2 (28.07.2019)
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

### Version 2.0.0beta1 (28.05.2019)
- Neu: Datenbankversion **2000**
- Neu: kompletter Neuaufbau des Programms
- Neu: mehrere Hefegaben pro Sud möglich
- Neu: mehrere Wasserprofile möglich
- Neu: Filterfunktion für Rohstofftabellen 
- Neu: mehr Gestaltungsmöglichkeiten bei den HTML Templates
- Neu: Filter im Reiter *Sudauswahl* bestimmt die angezeigten Sude im Reiter *Brauübersicht*
