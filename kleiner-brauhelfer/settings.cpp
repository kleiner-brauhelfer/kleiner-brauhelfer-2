#include "settings.h"
#include <QGuiApplication>
#include <QDir>
#include <QFileInfo>
#ifdef QT_PRINTSUPPORT_LIB
#include <QPrinterInfo>
#endif
#include <QLoggingCategory>

Settings::Settings(QObject *parent) :
    QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName(), parent)
{
    defaultFont = QGuiApplication::font();
    defaultPalette = QGuiApplication::palette();
    initTheme();
}

Settings::Settings(const QString& dir, QObject *parent) :
    QSettings(dir + "/" + QCoreApplication::applicationName() + ".ini", QSettings::IniFormat, parent)
{
    defaultFont = QGuiApplication::font();
    defaultPalette = QGuiApplication::palette();
    initTheme();
}

Settings::~Settings()
{
    setValueInGroup("General", "version", QCoreApplication::applicationVersion());
}

void Settings::setValueInGroup(QAnyStringView group, QAnyStringView key, const QVariant &value)
{
    beginGroup(group);
    setValue(key, value);
    endGroup();
}

QVariant Settings::valueInGroup(QAnyStringView group, QAnyStringView key, const QVariant &defaultValue)
{
    QVariant val;
    beginGroup(group);
    val = value(key, defaultValue);
    endGroup();
    return val;
}

void Settings::initTheme()
{
    beginGroup("Style");

    // font
    if (value("UseSystemFont", true).toBool())
        font = defaultFont;
    else
        font = value("Font", QFont()).value<QFont>();

    // theme
    mTheme = static_cast<Qt::ColorScheme>(value("Theme", (int)Qt::ColorScheme::Light).toInt());
    if (mTheme == Qt::ColorScheme::Unknown)
        mTheme = Qt::ColorScheme::Light;

    // colors
    QColor colorChanged;
    switch (mTheme)
    {
    default:
    case Qt::ColorScheme::Light:
        ErrorBase = QColor(252,171,171);
        ErrorText = QColor(180,10,10);
        colorChanged = QColor(255,240,175);

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
        break;

    case Qt::ColorScheme::Dark:
        ErrorBase = QColor(165,84,84);
        ErrorText = QColor(237,118,118);
        colorChanged = QColor(106,73,52);

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
        break;
    }

    // palette
    switch (mTheme)
    {
    default:
    case Qt::ColorScheme::Light:
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

    case Qt::ColorScheme::Dark:
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

    paletteInput = palette;
    paletteChanged = palette;
    paletteChanged.setColor(QPalette::Base, colorChanged);
    paletteChanged.setColor(QPalette::Button, colorChanged);
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
    return valueInGroup("General", "LogLevel", 0).toInt();
}

void Settings::setLogLevel(int level)
{
    setValueInGroup("General", "LogLevel", level);
    initLogLevel(level);
}

void Settings::initLogLevel(int level)
{
    QString rules;
    while (level > 100)
        level -= 100;
    if (level <= 0)
        rules = QStringLiteral("*.info=false\n*.debug=false");
    else if (level == 1)
        rules = QStringLiteral("*.debug=false");
    else if (level == 2)
        rules = QStringLiteral("");
    else if (level < 99)
    {
        if (level >= 3)
            rules += QStringLiteral("kleiner-brauhelfer-core.info=true\n");
        if (level >= 4)
            rules += QStringLiteral("kleiner-brauhelfer-core.debug=true\n");
        if (level >= 5)
            rules += QStringLiteral("SqlTableModel.info=true\n");
        if (level >= 6)
            rules += QStringLiteral("SqlTableModel.debug=true\n");
    }
    else
        rules = QStringLiteral("*.info=true\n*.debug=true");
    QLoggingCategory::setFilterRules(rules);
}

Qt::ColorScheme Settings::theme() const
{
    return mTheme;
}

void Settings::setTheme(Qt::ColorScheme theme)
{
    setValueInGroup("Style", "Theme", (int)theme);
    initTheme();
}

QString Settings::style()
{
    return valueInGroup("Style", "Style", QStringLiteral("Fusion")).toString();
}

void Settings::setStyle(const QString &style)
{
    setValueInGroup("Style", "Style", style);
}

bool Settings::useSystemFont()
{
    return valueInGroup("Style", "UseSystemFont", true).toBool();
}

void Settings::setUseSystemFont(bool system)
{
    setValueInGroup("Style", "UseSystemFont", system);
    font = defaultFont;
}

void Settings::setFont(const QFont &font)
{
    setValueInGroup("Style", "Font", font);
    this->font = font;
}

bool Settings::animationsEnabled()
{
    return valueInGroup("General", "Animations", true).toBool();
}

void Settings::setAnimationsEnabled(bool enabled)
{
    setValueInGroup("General", "Animations", enabled);
}

QString Settings::language()
{
    return valueInGroup("General", "language", "de").toString();
}

void Settings::setLanguage(const QString& lang)
{
    setValueInGroup("General", "language", lang);
}

QString Settings::settingsDir() const
{
    return QFileInfo(fileName()).absolutePath() + "/";
}

QString Settings::databasePath()
{
    QString path = valueInGroup("General", "database").toString();
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
    if (pathRel.startsWith(QStringLiteral("..")))
        setValueInGroup("General", "database", path);
    else
        setValueInGroup("General", "database", pathRel);
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

void Settings::initModules()
{
    beginGroup("General");
    if (!contains("Modules"))
    {
        modulesFirstTime = true;
        setValue("Modules", uint(ModuleDefault));
    }
    mModules = static_cast<Modules>(value("Modules").toUInt());
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    mModules.setFlag(ModuleSudauswahl);
    mModules.setFlag(ModuleRezept);
    mModules.setFlag(ModuleBraudaten);
    mModules.setFlag(ModuleAbfuellen);
    mModules.setFlag(ModuleRohstoffe);
  #else
    mModules |= ModuleSudauswahl;
    mModules |= ModuleRezept;
    mModules |= ModuleBraudaten;
    mModules |= ModuleAbfuellen;
    mModules |= ModuleRohstoffe;
  #endif
    endGroup();
}

Settings::Modules Settings::modules() const
{
    return mModules;
}

void Settings::enableModule(Settings::Module module, bool enabled)
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    mModules.setFlag(module, enabled);
  #else
    if (enabled)
        mModules |= module;
    else
        mModules &= ~module;
  #endif
    setValueInGroup("General", "Modules", uint(mModules));
    emit modulesChanged(module);
}

bool Settings::isModuleEnabled(Settings::Module module) const
{
    return mModules.testFlag(module);
}

QString Settings::lastProgramVersion()
{
    return valueInGroup("General", "version").toString();
}

bool Settings::isNewProgramVersion()
{
    return lastProgramVersion() != QCoreApplication::applicationVersion();
}

#ifdef QT_PRINTSUPPORT_LIB

QPrinter* Settings::createPrinter()
{
    beginGroup("Printer");
    QPrinterInfo printerInfo = QPrinterInfo::printerInfo(value("Name").toString());
    QPrinter* printer = new QPrinter(printerInfo, QPrinter::HighResolution);
    printer->setColorMode(QPrinter::Color);
    QPageLayout layout;
    layout.setOrientation((QPageLayout::Orientation)value("Orientation", QPageLayout::Portrait).toInt());
    layout.setUnits(QPageLayout::Millimeter);
    QRectF rect = value("Margins", QRectF(5, 10, 5, 15)).toRectF();
    QMarginsF margins(rect.left(), rect.top(), rect.width(), rect.height());
    layout.setPageSize(QPageSize((QPageSize::PageSizeId)value("Size", QPageSize::PageSizeId::A4).toInt()), margins);
    printer->setPageLayout(layout);
    endGroup();
    return printer;
}

void Settings::savePrinter(const QPrinter* printer)
{
    beginGroup("Printer");
    setValue("Name", printer->printerName());
    setValue("Orientation", printer->pageLayout().orientation());
    QPageSize pageSize = printer->pageLayout().pageSize();
    if (pageSize.id() == QPageSize::Custom)
        pageSize = QPageSize(printer->pageLayout().pageSize().sizePoints().transposed());
    if (pageSize.id() == QPageSize::Custom)
        pageSize = QPageSize(printer->pageLayout().pageSize().sizePoints());
    setValue("Size", pageSize.id());
    QMarginsF margins = printer->pageLayout().margins(QPageLayout::Millimeter);
    QRectF rect(margins.left(), margins.top(), margins.right(), margins.bottom());
    setValue("Margins", rect);
    endGroup();
}

#endif
