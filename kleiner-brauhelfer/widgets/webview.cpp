#include "webview.h"
#include <QDesktopServices>
#include <QEventLoop>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QLoggingCategory>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#include <QOperatingSystemVersion>
#endif
#include "helper/mustache.h"

bool WebView::gIsSupported = true;

#ifdef QT_WEBENGINECORE_LIB

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
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        if (url.isLocalFile() || mExternal)
        {
            QDesktopServices::openUrl(url);
            return false;
        }
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

QWebEnginePage* WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type)
    return this;
}

#endif

void WebView::setSupported(bool isSupported)
{
    gIsSupported = isSupported;
}

#ifdef QT_WEBENGINECORE_LIB

WebView::WebView(QWidget* parent) :
    QWebEngineView(parent)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setPage(new WebPage(this));
}

WebView::~WebView()
{
    delete page();
}

#else

WebView::WebView(QWidget* parent) :
    QTextBrowser(parent)
{
    setContextMenuPolicy(Qt::NoContextMenu);
}

WebView::WebView::~WebView()
{
}

#endif

void WebView::setLinksExternal(bool external)
{
  #ifdef QT_WEBENGINECORE_LIB
    static_cast<WebPage*>(page())->setLinksExternal(external);
  #else
    setOpenExternalLinks(external);
  #endif
}

void WebView::printToPdf(const QString& filePath, const QMarginsF& margins)
{
  #if defined(QT_WEBENGINECORE_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QEventLoop loop;
    connect(page(), SIGNAL(pdfPrintingFinished(QString,bool)), &loop, SLOT(quit()));
    page()->printToPdf(filePath, QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, margins));
    loop.exec();
  #else
    Q_UNUSED(filePath)
    Q_UNUSED(margins)
  #endif
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
    if (!gIsSupported)
        return;
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
    if (!gIsSupported)
        return;
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
    if (!gIsSupported)
        return;
  #ifdef QT_WEBENGINECORE_LIB
    setHtml(html, QUrl::fromLocalFile(QFileInfo(mTemplateFile).absolutePath() + "/"));
  #else
    setHtml(html);
  #endif
}

void WebView::renderText(const QString &html, QVariantMap &contextVariables)
{
    if (!gIsSupported)
        return;
    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(contextVariables);
    QString output = renderer.render(html, &context);
    if (renderer.errorPos() != -1)
        qWarning() << "Mustache:" << renderer.error();
    renderText(output);
}
