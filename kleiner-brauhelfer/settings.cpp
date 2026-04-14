#include "settings.h"
#include <QGuiApplication>
#include <QDir>
#include <QFileInfo>
#ifdef QT_PRINTSUPPORT_LIB
#include <QPrinterInfo>
#endif
#include <QLoggingCategory>
#include <QWidget>
#include <QStyleHints>

Settings::Settings(QObject *parent) :
    QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName(), parent)
{
    defaultFont = QGuiApplication::font();
    initTheme();
}

Settings::Settings(const QString& dir, QObject *parent) :
    QSettings(dir + "/" + QCoreApplication::applicationName() + ".ini", QSettings::IniFormat, parent)
{
    defaultFont = QGuiApplication::font();
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
        QGuiApplication::setFont(defaultFont);
    else
        QGuiApplication::setFont(value("Font", QFont()).value<QFont>());

    // theme
    mTheme = static_cast<Qt::ColorScheme>(value("Theme", (int)Qt::ColorScheme::Unknown).toInt());
    if (mTheme == Qt::ColorScheme::Unknown)
        qApp->styleHints()->unsetColorScheme();

    // colors
    QColor colorErrorWindow, colorErrorButton;
    QColor colorChangedBase, colorChangedWindow, colorChangedButton;
    QPalette palette = qApp->palette();
    switch (theme())
    {
    default:
    case Qt::ColorScheme::Light:
        colorErrorText = QColor(180, 10, 10);
        colorErrorBase = QColor(252, 171, 171);
        colorErrorWindow = QColor(245, 185, 185);
        colorErrorButton = QColor(230, 135, 135);

        colorChangedBase = QColor(255, 240, 175);
        colorChangedWindow = QColor(250, 235, 190);
        colorChangedButton = QColor(240, 215, 140);

        colorMalz = QColor(248, 242, 210);
        colorHopfen = QColor(232, 248, 225);
        colorHefe = QColor(248, 235, 215);
        colorZusatz = QColor(238, 232, 248);
        colorRast = QColor(248, 228, 228);
        colorWasser = QColor(228, 242, 248);

        colorRezept = QColor(195, 190, 165);
        colorGebraut = QColor(222, 150, 135);
        colorAbgefuellt = QColor(165, 205, 125);
        colorAusgetrunken = QColor(115, 140, 135);
        colorMerkliste = QColor(105, 150, 210);

        colorsHopfenTyp = {
            QColor(),
            QColor(235, 195, 165),
            QColor(185, 225, 185),
            QColor(185, 195, 235)
        };
        colorsHefeTyp = {
            QColor(),
            QColor(235, 195, 165),
            QColor(185, 195, 235)
        };
        colorsZusatzTyp = {
            QColor(235, 185, 185),
            QColor(230, 200, 170),
            QColor(230, 210, 170),
            QColor(185, 220, 180),
            QColor(170, 225, 200),
            QColor(170, 220, 225),
            QColor(180, 210, 235),
            QColor(205, 200, 235)
        };

        colorLine1 = QColor(120, 65, 175);
        colorBackground1 = QColor(200, 160, 225);
        colorLine2 = QColor(70, 120, 45);
        colorBackground2 = QColor(170, 210, 150);
        colorLine3 = QColor(155, 70, 70);
        colorBackground3 = QColor(225, 165, 165);

        palette.setColor(QPalette::Window, QColor(245, 245, 245));
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
        palette.setColor(QPalette::AlternateBase, QColor(235, 235, 235));
        palette.setColor(QPalette::WindowText, QColor(40, 40, 40));
        palette.setColor(QPalette::Text, QColor(40, 40, 40));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(140, 140, 140));
        palette.setColor(QPalette::Button, QColor(235, 235, 235));
        palette.setColor(QPalette::ButtonText, QColor(40, 40, 40));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 150));
        palette.setColor(QPalette::Highlight, QColor(250, 235, 170));
        palette.setColor(QPalette::HighlightedText, QColor(40, 40, 40));
        palette.setColor(QPalette::Accent, QColor(110, 150, 210));
        palette.setColor(QPalette::Link, QColor(150, 110, 200));
        palette.setColor(QPalette::LinkVisited, QColor(150, 110, 200));
        break;

    case Qt::ColorScheme::Dark:
        colorErrorText = QColor(220, 80, 80);
        colorErrorBase = QColor(80, 30, 30);
        colorErrorWindow = QColor(160, 70, 70);
        colorErrorButton = QColor(110, 45, 45);

        colorChangedBase = QColor(70, 65, 40);
        colorChangedWindow = QColor(85, 80, 50);
        colorChangedButton = QColor(110, 102, 65);

        colorMalz = QColor(55, 52, 40);
        colorHopfen = QColor(45, 60, 50);
        colorHefe = QColor(60, 48, 45);
        colorZusatz = QColor(55, 50, 65);
        colorRast = QColor(65, 45, 45);
        colorWasser = QColor(40, 55, 65);

        colorRezept = QColor(70, 68, 55);
        colorGebraut = QColor(95, 60, 55);
        colorAbgefuellt = QColor(55, 80, 50);
        colorAusgetrunken = QColor(45, 70, 65);
        colorMerkliste = QColor(55, 75, 105);

        colorsHopfenTyp = {
            QColor(),
            QColor(70, 50, 38),
            QColor(40, 65, 45),
            QColor(45, 50, 75)
        };
        colorsHefeTyp = {
            QColor(),
            QColor(70, 50, 38),
            QColor(45, 50, 75)
        };
        colorsZusatzTyp = {
            QColor(95, 55, 55),
            QColor(90, 70, 55),
            QColor(90, 80, 55),
            QColor(55, 85, 60),
            QColor(50, 90, 80),
            QColor(50, 85, 90),
            QColor(60, 80, 100),
            QColor(75, 70, 100)
        };

        colorLine1 = QColor(115, 85, 155);
        colorBackground1 = QColor(70, 55, 85);
        colorLine2 = QColor(85, 130, 70);
        colorBackground2 = QColor(60, 80, 55);
        colorLine3 = QColor(145, 85, 85);
        colorBackground3 = QColor(85, 55, 55);

        palette.setColor(QPalette::Window, QColor(30, 30, 35));
        palette.setColor(QPalette::Base, QColor(20, 20, 25));
        palette.setColor(QPalette::AlternateBase, QColor(40, 40, 45));
        palette.setColor(QPalette::WindowText, QColor(220, 220, 220));
        palette.setColor(QPalette::Text, QColor(220, 220, 220));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
        palette.setColor(QPalette::Button, QColor(45, 45, 50));
        palette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
        palette.setColor(QPalette::Highlight, QColor(100, 90, 35));
        palette.setColor(QPalette::HighlightedText, QColor(220, 220, 220));
        palette.setColor(QPalette::Accent, QColor(140, 180, 240));
        palette.setColor(QPalette::Link, QColor(190, 150, 240));
        palette.setColor(QPalette::LinkVisited, QColor(190, 150, 240));
        break;
    }
    qApp->setPalette(palette);

    paletteChanged = palette;
    paletteChanged.setColor(QPalette::Base, colorChangedBase);
    paletteChanged.setColor(QPalette::Window, colorChangedWindow);
    paletteChanged.setColor(QPalette::Button, colorChangedButton);
    paletteError = palette;
    paletteError.setColor(QPalette::Base, colorErrorBase);
    paletteError.setColor(QPalette::Window, colorErrorWindow);
    paletteError.setColor(QPalette::Button, colorErrorButton);
    paletteErrorLabel = palette;
    paletteErrorLabel.setColor(QPalette::WindowText, colorErrorText);

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
    if (level < 0)
        rules = QStringLiteral("*.info=false\n*.debug=false");
    else if (level == 0)
        rules = QStringLiteral("*.debug=false");
    else if (level == 1)
        rules = QStringLiteral("");
    if (level >= 2)
        rules += QStringLiteral("kleiner-brauhelfer-core.info=true\n");
    if (level >= 3)
        rules += QStringLiteral("kleiner-brauhelfer-core.debug=true\n");
    if (level >= 4)
        rules += QStringLiteral("SqlTableModel.info=true\n");
    if (level >= 5)
        rules += QStringLiteral("SqlTableModel.debug=true\n");
    QLoggingCategory::setFilterRules(rules);
}

Qt::ColorScheme Settings::theme() const
{
    if (mTheme == Qt::ColorScheme::Unknown)
        return qApp->styleHints()->colorScheme();
    return mTheme;
}

Qt::ColorScheme Settings::theme_set() const
{
    return mTheme;
}

void Settings::setTheme(Qt::ColorScheme theme)
{
    if (theme != mTheme)
    {
        setValueInGroup("Style", "Theme", (int)theme);
        initTheme();
        emit themeChanged(theme);
    }
}

bool Settings::useDefaultFont()
{
    return valueInGroup("Style", "UseSystemFont", true).toBool();
}

void Settings::setFont(const QFont &font)
{
    setValueInGroup("Style", "UseSystemFont", false);
    setValueInGroup("Style", "Font", font);
    QGuiApplication::setFont(font);
}

void Settings::setDefaultFont()
{
    setValueInGroup("Style", "UseSystemFont", true);
    QGuiApplication::setFont(defaultFont);
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

void Settings::setLanguage(const QString& language)
{
    if (language != this->language())
    {
        setValueInGroup("General", "language", language);
        emit languageChanged(language);
    }
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
    emit databasePathChanged(databasePath());
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
    {
        if (printer->pageLayout().orientation() == QPageLayout::Orientation::Landscape)
            pageSize = QPageSize(printer->pageLayout().pageSize().sizePoints().transposed());
        else
            pageSize = QPageSize(printer->pageLayout().pageSize().sizePoints());
    }
    setValue("Size", pageSize.id());
    QMarginsF margins = printer->pageLayout().margins(QPageLayout::Millimeter);
    QRectF rect(margins.left(), margins.top(), margins.right(), margins.bottom());
    setValue("Margins", rect);
    endGroup();
}

#endif

void Settings::setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets)
{
    bool visible = isModuleEnabled(module);
    for (const auto& it : widgets)
        it->setVisible(visible);
}
