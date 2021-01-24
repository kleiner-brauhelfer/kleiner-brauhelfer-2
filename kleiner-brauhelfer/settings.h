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
        //
        ModuleSudauswahl            = 0x00000001,
        ModuleRezept                = 0x00000002,
        ModuleBraudaten             = 0x00000004,
        ModuleAbfuellen             = 0x00000008,
        ModuleGaerverlauf           = 0x00000010,
        ModuleZusammenfassung       = 0x00000020,
        ModuleEtikette              = 0x00000040,
        ModuleBewertung             = 0x00000080,
        ModuleBrauuebersicht        = 0x00000100,
        ModuleAusruestung           = 0x00000200,
        ModuleRohstoffe             = 0x00000400,
        ModuleDatenbank             = 0x00000800,
        //
        //ModuleWasseraufbereitung    = 0x00010000,
        //ModulePreiskalkulation      = 0x00020000,
        //
        ModuleDefault               = 0xfffff73f,
        ModuleAlle                  = 0xffffffff,
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
