#include "settings.h"
#include <QGuiApplication>
#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>

Settings::Settings(QObject *parent) :
    QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName(), parent)
{
    defaultFont = QGuiApplication::font();
    defaultPalette = QGuiApplication::palette();
    beginGroup("General");
    mModules = static_cast<Modules>(value("Modules", int(ModuleDefault)).toUInt());
    endGroup();
    initTheme();
}

Settings::Settings(const QString& dir, QObject *parent) :
    QSettings(dir + "/" + QCoreApplication::applicationName() + ".ini", QSettings::IniFormat, parent)
{
    defaultFont = QGuiApplication::font();
    defaultPalette = QGuiApplication::palette();
    beginGroup("General");
    mModules = static_cast<Modules>(value("Modules", int(ModuleDefault)).toUInt());
    endGroup();
    initTheme();
}

Settings::~Settings()
{
    beginGroup("General");
    setValue("version", QCoreApplication::applicationVersion());
    endGroup();
}

void Settings::initTheme()
{
    beginGroup("Style");

    // font
    if (value("UseSystemFont", true).toBool())
        font = defaultFont;
    else
        font = value("Font", QFont()).value<QFont>();

    // palette
    mTheme = static_cast<Theme>(value("Theme", Bright).toInt());
    if (mTheme == Unused)
        mTheme = Bright;
    switch (mTheme)
    {
    default:
    case Bright:
        palette.setColorGroup(QPalette::Disabled,
            QColor(128,128,128),//windowText
            QColor(226,226,226),//button
            QColor(240,240,240),//light
            QColor(200,200,200),//dark
            QColor(210,210,210),//mid
            QColor(128,128,128),//text
            QColor(240,240,240),//bright_text
            QColor(255,255,255),//base
            QColor(226,226,226));//window
        palette.setColorGroup(QPalette::Active,
            QColor(40,40,40),//windowText
            QColor(226,226,226),//button
            QColor(240,240,240),//light
            QColor(200,200,200),//dark
            QColor(210,210,210),//mid
            QColor(40,40,40),//text
            QColor(240,240,240),//bright_text
            QColor(255,255,255),//base
            QColor(226,226,226));//window
        palette.setColorGroup(QPalette::Inactive,
            QColor(40,40,40),//windowText
            QColor(226,226,226),//button
            QColor(240,240,240),//light
            QColor(200,200,200),//dark
            QColor(210,210,210),//mid
            QColor(40,40,40),//text
            QColor(240,240,240),//bright_text
            QColor(255,255,255),//base
            QColor(226,226,226));//window
        palette.setColor(QPalette::Highlight, QColor(0,120,215));
        palette.setColor(QPalette::HighlightedText, QColor(255,255,255));
        palette.setColor(QPalette::Link, QColor(0,120,215));
        palette.setColor(QPalette::LinkVisited, QColor(0,120,215));
        break;

    case Dark:
        palette.setColorGroup(QPalette::Disabled,
            QColor(128,128,128),//windowText
            QColor(53,53,53),//button
            QColor(120,120,120),//light
            QColor(25,25,25),//dark
            QColor(32,32,32),//mid
            QColor(128,128,128),//text
            QColor(120,120,120),//bright_text
            QColor(25,25,25),//base
            QColor(53,53,53));//window
        palette.setColorGroup(QPalette::Active,
            QColor(255,255,255),//windowText
            QColor(53,53,53),//button
            QColor(120,120,120),//light
            QColor(25,25,25),//dark
            QColor(32,32,32),//mid
            QColor(255,255,255),//text
            QColor(120,120,120),//bright_text
            QColor(25,25,25),//base
            QColor(53,53,53));//window
        palette.setColorGroup(QPalette::Inactive,
            QColor(255,255,255),//windowText
            QColor(53,53,53),//button
            QColor(120,120,120),//light
            QColor(25,25,25),//dark
            QColor(32,32,32),//mid
            QColor(255,255,255),//text
            QColor(120,120,120),//bright_text
            QColor(25,25,25),//base
            QColor(53,53,53));//window
        palette.setColor(QPalette::Highlight, QColor(42,130,218));
        palette.setColor(QPalette::HighlightedText, QColor(0,0,0));
        palette.setColor(QPalette::Link, QColor(42,130,218));
        palette.setColor(QPalette::LinkVisited, QColor(42,130,218));
        break;
    }

    if (palette.color(QPalette::Active, QPalette::Text).lightness() < 127)
    {
        ErrorBase = QColor(252,171,171);
        ErrorText = QColor(180,10,10);

        colorMalz = QColor(252,247,224);
        colorHopfen = QColor(222,253,224);
        colorHefe = QColor(253,237,222);
        colorZusatz = QColor(247,240,253);
        colorWasser = QColor(232,247,253);
        colorRast = QColor(253,232,232);
        colorAnhang = QColor(235,253,245);
        colorKommentar = QColor(249,249,249);

        NichtGebrautBackground = QColor(242,242,242);
        GebrautBackground = QColor(242,231,201);
        AbgefuelltBackground = QColor(196,229,165);
        VerbrauchtBackground = QColor(206,206,206);
        MekrlisteBackground = QColor(128,172,242);

        HopfenTypBackgrounds = {QColor(), QColor(255,200,170), QColor(200,255,200), QColor(200,200,255)};
        HefeTypOgUgBackgrounds = {QColor(), QColor(255,200,170), QColor(200,200,255)};
        HefeTypTrFlBackgrounds = {QColor(), QColor(255,200,170), QColor(200,200,255)};
        WZTypBackgrounds = {QColor(253,199,197),
                            QColor(239,207,175),
                            QColor(241,216,167),
                            QColor(184,223,178),
                            QColor(159,226,201),
                            QColor(154,225,227),
                            QColor(176,218,246),
                            QColor(210,208,252)};

        DiagramLinie1 = QColor(123,33,178);
        DiagramLinie2 = QColor(56,104,2);
        DiagramLinie3 = QColor(120,30,30);
        DiagramLinie1Light = QColor(153,98,188);
        DiagramLinie2Light = QColor(102,150,46);
        DiagramLinie3Light = QColor(211,97,97);
    }
    else
    {
        ErrorBase = QColor(165,84,84);
        ErrorText = QColor(237,118,118);

        colorMalz = QColor(89,86,79);
        colorHopfen = QColor(77,89,65);
        colorHefe = QColor(89,82,75);
        colorZusatz = QColor(83,76,86);
        colorWasser = QColor(78,85,89);
        colorRast = QColor(89,79,74);
        colorAnhang = QColor(89,89,89);
        colorKommentar = QColor(76,76,76);

        NichtGebrautBackground = QColor(50, 50, 50);
        GebrautBackground = QColor(96, 50, 49);
        AbgefuelltBackground = QColor(78, 78, 38);
        VerbrauchtBackground = QColor(80, 80, 80);
        MekrlisteBackground = QColor(52, 58, 92);

        HopfenTypBackgrounds = {QColor(), QColor(127,72,47), QColor(75,127,75), QColor(89,89,127)};
        HefeTypOgUgBackgrounds = {QColor(), QColor(75,127,75), QColor(89,89,127)};
        HefeTypTrFlBackgrounds = {QColor(), QColor(75,127,75), QColor(89,89,127)};
        WZTypBackgrounds = {QColor(127,65,62),
                            QColor(127,87,0),
                            QColor(127,127,0),
                            QColor(56,127,28),
                            QColor(0,127,87),
                            QColor(0,171,176),
                            QColor(0,125,127),
                            QColor(76,75,127)};

        DiagramLinie1 = QColor(105,139,87);
        DiagramLinie2 = QColor(18,90,136);
        DiagramLinie3 = QColor(138,108,77);

        DiagramLinie1Light = QColor(105,139,87);
        DiagramLinie2Light = QColor(18,90,136);
        DiagramLinie3Light = QColor(138,108,77);
    }

    paletteInput = palette;

    paletteError = palette;
    paletteError.setColor(QPalette::Base, ErrorBase);

    paletteErrorButton = palette;
    paletteErrorButton.setColor(QPalette::Button, ErrorBase);

    paletteErrorLabel = palette;
    paletteErrorLabel.setColor(QPalette::WindowText, ErrorText);

    endGroup();
}

int Settings::logLevel()
{
    beginGroup("General");
    int level = value("LogLevel", "0").toInt();
    endGroup();
    return level;
}

void Settings::setLogLevel(int level)
{
    beginGroup("General");
    setValue("LogLevel", level);
    endGroup();
    initLogLevel(level);
}

void Settings::initLogLevel(int level)
{
    QString rules;
    while (level > 100)
        level -= 100;
    if (level <= 0)
        rules = "*.info=false\n*.debug=false";
    else if (level == 1)
        rules = "*.debug=false";
    else if (level == 2)
        rules = "";
    else if (level == 3)
        rules = "SqlTableModel.info=true";
    else
        rules = "SqlTableModel.info=true\nSqlTableModel.debug=true";
    QLoggingCategory::setFilterRules(rules);
}

Settings::Theme Settings::theme() const
{
    return mTheme;
}

void Settings::setTheme(Theme theme)
{
    beginGroup("Style");
    setValue("Theme", theme);
    endGroup();
    initTheme();
}

QString Settings::style()
{
  #if 0
    beginGroup("Style");
    QString style = value("Style", "Fusion").toString();
    endGroup();
    return style;
  #else
    return "Fusion";
  #endif
}

void Settings::setStyle(const QString &style)
{
    beginGroup("Style");
    setValue("Style", style);
    endGroup();
}

bool Settings::useSystemFont()
{
    beginGroup("Style");
    bool system = value("UseSystemFont", true).toBool();
    endGroup();
    return system;
}

void Settings::setUseSystemFont(bool system)
{
    beginGroup("Style");
    setValue("UseSystemFont", system);
    endGroup();
    this->font = defaultFont;
}

void Settings::setFont(const QFont &font)
{
    beginGroup("Style");
    setValue("Font", font);
    endGroup();
    this->font = font;
}

bool Settings::animationsEnabled()
{
    beginGroup("General");
    int enabled = value("Animations", true).toBool();
    endGroup();
    return enabled;
}

void Settings::setAnimationsEnabled(bool enabled)
{
    beginGroup("General");
    setValue("Animations", enabled);
    endGroup();
}

QString Settings::language()
{
    beginGroup("General");
    QString lang = value("language", "de").toString();
    endGroup();
    return lang;
}

void Settings::setLanguage(QString lang)
{
    beginGroup("General");
    setValue("language", lang);
    endGroup();
}

QString Settings::settingsDir() const
{
    return QFileInfo(fileName()).absolutePath() + "/";
}

QString Settings::databasePath()
{
    QString path;
    beginGroup("General");
    path = value("database").toString();
    endGroup();
    if (!path.isEmpty() && QDir::isRelativePath(path))
    {
        QDir dir(settingsDir());
        path = QDir::cleanPath(dir.filePath(path));
    }

    return path;
}

void Settings::setDatabasePath(const QString& path)
{
    QDir dir(settingsDir());
    QString pathRel = dir.relativeFilePath(path);
    beginGroup("General");
    if (pathRel.startsWith(".."))
        setValue("database", path);
    else
        setValue("database", pathRel);
    endGroup();
}

QString Settings::databaseDir()
{
    return QFileInfo(databasePath()).absolutePath() + "/";
}

QString Settings::dataDir(int type) const
{
    switch (type)
    {
    case 1:
        return settingsDir() + "data/Webview/";
    case 2:
        return settingsDir() + "data/Rohstoffe/";
    case 3:
        return settingsDir() + "data/Etiketten/";
    default:
        return settingsDir() + "data/";
    }
}

Settings::Modules Settings::modules() const
{
    return mModules;
}

void Settings::enableModule(Settings::Module module, bool enabled)
{
    mModules.setFlag(module, enabled);
    beginGroup("General");
    setValue("Modules", uint(mModules));
    endGroup();
    emit(modulesChanged(module));
}

bool Settings::module(Settings::Module module) const
{
    return mModules.testFlag(module);
}

QString Settings::lastProgramVersion()
{
    QString version;
    beginGroup("General");
    version = value("version").toString();
    endGroup();
    return version;
}

bool Settings::isNewProgramVersion()
{
    return lastProgramVersion() != QCoreApplication::applicationVersion();
}
