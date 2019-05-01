#include "webview.h"
#include <QDesktopServices>
#include <QEventLoop>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include "helper/mustache.h"

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
