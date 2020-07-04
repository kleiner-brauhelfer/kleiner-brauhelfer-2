#include "webview.h"

#if QWEBENGINE_SUPPORT_EN

#include <QDesktopServices>
#include <QEventLoop>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#include <QOperatingSystemVersion>
#endif
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
    mIsSupported = true;
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows &&
        QOperatingSystemVersion::current() <= QOperatingSystemVersion::Windows7)
        mIsSupported = false;
  #endif
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

void WebView::printToPdf(const QString& filePath, const QMarginsF& margins)
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QEventLoop loop;
    connect(page(), SIGNAL(pdfPrintingFinished(const QString&, bool)), &loop, SLOT(quit()));
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
    if (!mIsSupported)
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
    if (!mIsSupported)
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
    if (!mIsSupported)
        return;
    setHtml(html, QUrl::fromLocalFile(QFileInfo(mTemplateFile).absolutePath() + "/"));
}

void WebView::renderText(const QString &html, QVariantMap &contextVariables)
{
    if (!mIsSupported)
        return;
    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(contextVariables);
    renderText(renderer.render(html, &context));
}

#endif
