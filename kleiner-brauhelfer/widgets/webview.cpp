#include "webview.h"
#include <QDesktopServices>
#include <QEventLoop>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include "brauhelfer.h"
#include "settings.h"
#include "helper/mustache.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WebPage::WebPage(QObject* parent) :
    QWebEnginePage(parent),
    mExternal(false)
{
}

void WebPage::setLinksExternal(bool external)
{
    mExternal = external;
}

bool WebPage::acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    Q_UNUSED(isMainFrame)
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        if (url.isLocalFile() || mExternal)
        {
            QDesktopServices::openUrl(url);
            return false;
        }
    }
    return true;
}

WebView::WebView(QWidget* parent) :
    QWebEngineView(parent)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setPage(new WebPage());
}

WebView::~WebView()
{
    delete page();
}

void WebView::setLinksExternal(bool external)
{
    static_cast<WebPage*>(page())->setLinksExternal(external);
}

void WebView::printToPdf(const QString& filePath)
{
    QEventLoop loop;
    connect(page(), SIGNAL(pdfPrintingFinished(const QString&, bool)), &loop, SLOT(quit()));
    page()->printToPdf(filePath, QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF(20, 20, 20, 20)));
    loop.exec();
}

QString WebView::templateFile() const
{
    return mTemplateFile;
}

void WebView::setTemplateFile(const QString& file)
{
    mTemplateFile = file;
}

void WebView::renderTemplate()
{
    QFile file(mTemplateFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setHtml("");
        return;
    }
    renderText(file.readAll());
    file.close();
}

void WebView::renderTemplate(QVariantHash& contextVariables)
{
    QFile file(mTemplateFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setHtml("");
        return;
    }
    renderText(file.readAll(), contextVariables);
    file.close();
}

void WebView::renderText(const QString &html)
{
    setHtml(html, QUrl::fromLocalFile(QFileInfo(mTemplateFile).absolutePath() + "/"));
}

void WebView::renderText(const QString &html, QVariantHash& contextVariables)
{
    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(contextVariables);
    renderText(renderer.render(html, &context));
}

void WebView::erstelleTagListe(QVariantHash& contextVariables, bool sudDaten)
{
    QLocale locale = QLocale();
    contextVariables["AppName"] = QCoreApplication::applicationName();
    contextVariables["AppVersion"] = QCoreApplication::applicationVersion();
    contextVariables["Style"] = gSettings->theme() == Settings::Dark ? "style_dunkel.css" : "style_hell.css";
    if (sudDaten && bh->sud()->isLoaded())
    {
        contextVariables["Sudname"] = bh->sud()->getSudname();
        contextVariables["Stammwuerze"] = locale.toString(bh->sud()->getSW(), 'f', 1);
        contextVariables["AlcVol"] = locale.toString(bh->sud()->geterg_Alkohol(), 'f', 1);
        contextVariables["IBU"] = QString::number(bh->sud()->getIBU());
        contextVariables["CO2"] = locale.toString(bh->sud()->getCO2(), 'f', 1);
        contextVariables["EBC"] = locale.toString(bh->sud()->getFarbeIst(), 'f', 1);
        contextVariables["Braudatum"] = locale.toString(bh->sud()->getBraudatum().date(), QLocale::ShortFormat);
        contextVariables["Anstelldatum"] = locale.toString(bh->sud()->getAnstelldatum().date(), QLocale::ShortFormat);
        contextVariables["Abfuelldatum"] = locale.toString(bh->sud()->getAbfuelldatum().date(), QLocale::ShortFormat);
        contextVariables["Nr"] = QString::number(bh->sud()->getSudnummer());
        for (int i = 0; i < bh->modelFlaschenlabelTags()->rowCount(); ++i)
        {
            int sudId = bh->modelFlaschenlabelTags()->data(i, "SudID").toInt();
            if (sudId < 0 || sudId == bh->sud()->id())
            {
                QString t = bh->modelFlaschenlabelTags()->data(i, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(i, "Value").toString();
                if (!t.isEmpty() && !v.isEmpty())
                    contextVariables[t] = v;
            }
        }
    }
    else
    {
        for (int i = 0; i < bh->modelFlaschenlabelTags()->rowCount(); ++i)
        {
            int sudId = bh->modelFlaschenlabelTags()->data(i, "SudID").toInt();
            if (sudId < 0)
            {
                QString t = bh->modelFlaschenlabelTags()->data(i, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(i, "Value").toString();
                if (!t.isEmpty() && !v.isEmpty())
                    contextVariables[t] = v;
            }
        }
    }
}
