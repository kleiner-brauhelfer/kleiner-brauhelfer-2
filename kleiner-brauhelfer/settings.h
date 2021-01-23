#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QPalette>
#include <QFont>

class Settings : public QSettings
{
public:

    enum Theme
    {
        Unused,
        Bright,
        Dark
    };

    enum Module {
        ModuleKein                  = 0x00000000,
        ModuleSudauswahl            = 0x00010000,
        ModuleRezept                = 0x00020000,
        ModuleBraudaten             = 0x00040000,
        ModuleAbfuellen             = 0x00080000,
        ModuleGaerverlauf           = 0x00100000,
        ModuleZusammenfassung       = 0x00200000,
        ModuleEtikette              = 0x00400000,
        ModuleBewertung             = 0x00800000,
        ModuleBrauuebersicht        = 0x01000000,
        ModuleAusruestung           = 0x02000000,
        ModuleRohstoffe             = 0x04000000,
        ModuleDatenbank             = 0x08000000,
        ModuleDefault               = 0xffffffff,
        ModuleAll                   = 0xffffffff,
    };
    Q_DECLARE_FLAGS(Modules, Module)

public:
    explicit Settings(QObject *parent = nullptr);
    Settings(const QString& dir, QObject *parent = nullptr);
    virtual ~Settings();

    int logLevel();
    void setLogLevel(int level);
    void initLogLevel(int level);

    Theme theme() const;
    void setTheme(Theme theme);

    QString style();
    void setStyle(const QString &style);

    bool useSystemFont();
    void setUseSystemFont(bool system);
    void setFont(const QFont &font);

    bool animationsEnabled();
    void setAnimationsEnabled(bool enabled);

    QString language();
    void setLanguage(QString lang);

    QString settingsDir() const;

    QString databasePath();
    void setDatabasePath(const QString& path);
    QString databaseDir();

    QString dataDir(int type) const;

    Modules modules() const;
    void enableModule(Module module, bool enabled);
    bool module(Module module) const;

    QString lastProgramVersion();
    bool isNewProgramVersion();

private:
    void initTheme();

public:
    QFont font;

    QPalette palette;
    QPalette paletteInput;
    QPalette paletteError;
    QPalette paletteErrorButton;
    QPalette paletteErrorLabel;

    QColor ErrorBase;
    QColor ErrorText;

    QColor colorMalz;
    QColor colorHopfen;
    QColor colorHefe;
    QColor colorZusatz;
    QColor colorWasser;
    QColor colorRast;
    QColor colorAnhang;
    QColor colorKommentar;

    QColor NichtGebrautBackground;
    QColor GebrautBackground;
    QColor AbgefuelltBackground;
    QColor VerbrauchtBackground;
    QColor MekrlisteBackground;

    QList<QColor> HopfenTypBackgrounds;
    QList<QColor> HefeTypOgUgBackgrounds;
    QList<QColor> HefeTypTrFlBackgrounds;
    QList<QColor> WZTypBackgrounds;

    QColor DiagramLinie1;
    QColor DiagramLinie2;
    QColor DiagramLinie3;
    QColor DiagramLinie1Light;
    QColor DiagramLinie2Light;
    QColor DiagramLinie3Light;

    // run-time settings
    bool ForceEnabled = false;

private:
    Theme mTheme;
    Modules mModules;
    QFont defaultFont;
    QPalette defaultPalette;
};

#endif // SETTINGS_H
