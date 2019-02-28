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

void WebView::renderTemplate(QVariantMap &contextVariables)
{
    if (mTemplateFile.isEmpty())
        return;
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

void WebView::renderText(const QString &html, QVariantMap &contextVariables)
{
    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(contextVariables);
    renderText(renderer.render(html, &context));
}

void WebView::erstelleTagListe(QVariantMap &ctx, int sudRow)
{
    QLocale locale = QLocale();

    QVariantMap ctxApp;
    ctxApp["Name"] = QCoreApplication::applicationName();
    ctxApp["Version"] = QCoreApplication::applicationVersion();
    ctx["App"] = ctxApp;

    ctx["Style"] = gSettings->theme() == Settings::Dark ? "style_dunkel.css" : "style_hell.css";

    if (sudRow >= 0)
    {
        QVariantMap ctxRezept;
        ctxRezept["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SW").toDouble(), 'f', 1);
        ctxRezept["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "Menge").toDouble(), 'f', 1);
        ctxRezept["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IBU").toDouble());
        ctxRezept["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2").toDouble(), 'f', 1);
        ctxRezept["Farbe"] = locale.toString(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble(), 'f', 0);
        ctxRezept["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble())).name();
        ctxRezept["Nachisomerisierung"] = QString::number(bh->modelSud()->data(sudRow, "Nachisomerisierungszeit").toInt());
        ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, "Anlage").toString();
        ctxRezept["Name"] = bh->modelSud()->data(sudRow, "Sudname").toString();
        ctxRezept["Nummer"] = QString::number(bh->modelSud()->data(sudRow, "Sudnummer").toDouble());
        ctxRezept["Kommentar"] = bh->modelSud()->data(sudRow, "Kommentar").toString().replace("\n", "<br>");
        ctx["Rezept"] = ctxRezept;

        QVariantMap ctxSud;
        ctxSud["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SWIst").toDouble(), 'f', 1);
        ctxSud["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "MengeIst").toDouble(), 'f', 1);
        ctxSud["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IbuIst").toInt());
        ctxSud["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2Ist").toDouble(), 'f', 1);
        ctxSud["Farbe"] = locale.toString(bh->modelSud()->data(sudRow, "FarbeIst").toDouble(), 'f', 0);
        ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, "Braudatum").toDate(), QLocale::ShortFormat);
        ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, "Abfuelldatum").toDate(), QLocale::ShortFormat);
        ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "FarbeIst").toDouble())).name();
        ctxSud["Bewertung"] = QString::number(bh->modelSud()->data(sudRow, "BewertungMax").toInt());
        ctxSud["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, "erg_Alkohol").toDouble(), 'f', 1);
        ctxSud["tEVG"] = locale.toString(bh->modelSud()->data(sudRow, "tEVG").toDouble(), 'f', 1);
        ctxSud["effSHA"] = locale.toString(bh->modelSud()->data(sudRow, "erg_EffektiveAusbeute").toDouble(), 'f', 1);
        ctx["Sud"] = ctxSud;

        QVariantMap ctxTag;
        int sudId = bh->modelSud()->data(sudRow, "ID").toInt();
        for (int col = 0; col < bh->modelFlaschenlabelTags()->rowCount(); ++col)
        {
            int id = bh->modelFlaschenlabelTags()->data(col, "SudID").toInt();
            if (id == sudId || id < 0)
            {
                QString t = bh->modelFlaschenlabelTags()->data(col, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(col, "Value").toString();
                if (!t.isEmpty())
                    ctxTag[t] = v;
            }
        }
        ctx["Tag"] = ctxTag;
    }
    else
    {
        QVariantMap ctxTag;
        for (int col = 0; col < bh->modelFlaschenlabelTags()->rowCount(); ++col)
        {
            int id = bh->modelFlaschenlabelTags()->data(col, "SudID").toInt();
            if (id < 0)
            {
                QString t = bh->modelFlaschenlabelTags()->data(col, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(col, "Value").toString();
                if (!t.isEmpty())
                    ctxTag[t] = v;
            }
        }
        ctx["Tag"] = ctxTag;
    }
}
