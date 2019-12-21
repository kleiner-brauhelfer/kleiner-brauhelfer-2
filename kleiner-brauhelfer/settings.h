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
        System,
        Bright,
        Dark
    };

public:
    explicit Settings(QObject *parent = nullptr);
    Settings(const QString& dir, QObject *parent = nullptr);
    virtual ~Settings();

    int logLevel();
    void setLogLevel(int level);

    Theme theme() const;
    void setTheme(Theme theme);

    QString style();
    void setStyle(const QString &style);

    bool useSystemFont();
    void setUseSystemFont(bool system);
    void setFont(const QFont &font);

    QString settingsDir() const;

    QString databasePath();
    void setDatabasePath(const QString& path);
    QString databaseDir();

    QString dataDir(int type = 0) const;

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
    QColor WarningText;
    QColor ErrorText;

    QColor MekrlisteBackground;
    QColor VerbrauchtBackground;
    QColor AbgefuelltBackground;
    QColor GebrautBackground;
    QColor NichtGebrautBackground;

    QList<QColor> HopfenTypBackgrounds;
    QList<QColor> HefeTypOgUgBackgrounds;
    QList<QColor> HefeTypTrFlBackgrounds;
    QList<QColor> HefeSedBackgrounds;
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
};

#endif // SETTINGS_H
