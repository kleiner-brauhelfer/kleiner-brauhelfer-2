# kleiner-brauhelfer v2
Der kleine-brauhelfer ist ein Hilfsprogramm für Hobbybrauer zum Erstellen und Verwalten von Biersuden. Die Version 2 ist die Weiterentwicklung vom [kleinen-brauhelfer](http://github.com/Gremmel/kleiner-brauhelfer).

**Homepage:**
http://www.joerum.de/kleiner-brauhelfer/

**Diskussionsthread:**
https://hobbybrauer.de/forum/viewtopic.php?f=3&t=21510

## Download letzte Version
- [Version 2.0.0beta2](https://github.com/kleiner-brauhelfer/kleiner-brauhelfer-2/releases/tag/v2.0.0beta2)

### Windows
- **kbh2_v2.x.x_setup_win_x64.exe**
  - Installationsdatei für 64bit System
  - Qt Abhängigkeiten integriert
- **kbh2_v2.x.x_portable_win_x64.zip**
  - Portable Version für 64bit System
  - Qt Abhängigkeiten integriert
- **kbh2_v2.x.x_setup_win_x86.exe**
  - Installationsdatei für 32bit System
  - Qt Abhängigkeiten integriert
- **kbh2_v2.x.x_portable_win_x86.zip**
  - Portable Version für 32bit System
  - Qt Abhängigkeiten integriert

### Linux
- **kbh2_v2.x.x_linux_x64.deb**
  - Installationsdatei für 64bit System
  - Qt Abhängigkeiten **nicht** integriert
  - Benötigt Installation von [Qt 5.13](https://www.qt.io/download-qt-installer) mit folgenden Komponenten
    - Desktop gcc 64-bit
    - Qt Charts
    - Qt WebEngine
- **kbh2_v2.x.x_portable_linux_x64.tar.gz**
  - Portable Version (AppImage) für 64bit System
  - Qt Abhängigkeiten integriert
- **kbh2_v2.x.x_linux_x32.deb**
  - Installationsdatei für 32bit System
  - Qt Abhängigkeiten **nicht** integriert
  - Qt wird für Linux 32bit nicht mehr kompiliert. Deshalb werden nicht alle Features unterstüzt.
  - Benötigt Installation von [Qt 5.5](https://www.qt.io/download-qt-installer) mit folgenden Komponenten
    - Desktop gcc
    - Qt WebEngine

### Mac OS
- **kbh2_v2.x.x_macos.zip**: Installationsdatei für Mac OS (Qt Abhängigkeiten integriert)

## Änderungen & Erweiterungen

### Version 2.0.0beta3
- Neu: Funktion "Eingabefelder entsperren"
- Neu: Umrechnung zwischen Palto und spezifische Dichte nach [Brewer's Friend](http://www.brewersfriend.com/plato-to-sg-conversion-chart/)
- Neu: Milchsäurangabe in 0.1 ml Schritte
- Neu: HTML Templates werden nur aktualisiert, wenn das entsprechende Reiter aktiv ist
- Neu: Zielstammwürze bei Kochbeginn mit/ohne weiteren Zutaten "Kochen" separat aufgeführt
- Neu: Weitere Zutaten können in Milligramm und Stücke angegeben werden
- Fix: Aktualisierung der Hopfenmenge nach Änderung des Hopfens
- Fix: Dialog Rohstoffaustausch beim Löschen eines Hopfens, welches als Stopfhopfen verwendet wird
- Fix: "Sud löschen" löscht auch die Hefegaben

### Version 2.0.0beta2
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

### Version 2.0.0beta1
- Neu: kompletter Neuaufbau des Programms
- Neu: mehrere Hefegaben pro Sud möglich
- Neu: mehrere Wasserprofile möglich
- Neu: Filterfunktion für Rohstofftabellen 
- Neu: mehr Gestaltungsmöglichkeiten bei den HTML Templates
- Neu: Filter im Reiter *Sudauswahl* bestimmt die angezeigten Sude im Reiter *Brauübersicht*
