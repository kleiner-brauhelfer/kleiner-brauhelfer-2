#ifndef WEBVIEW_H
#define WEBVIEW_H

#define QWEBENGINE_SUPPORT_EN 1

#if QWEBENGINE_SUPPORT_EN

#include <QWebEngineView>
#include <QWebEnginePage>

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = nullptr);
    void setLinksExternal(bool external);
    bool acceptNavigationRequest(const QUrl & url, QWebEnginePage::NavigationType type, bool) Q_DECL_OVERRIDE;
private:
    bool mExternal;
};

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget* parent = nullptr);
    ~WebView();
    void setLinksExternal(bool external);
    void printToPdf(const QString& filePath);

    QString templateFile() const;
    void setTemplateFile(const QString& file);

    void renderTemplate();
    void renderTemplate(QVariantMap& contextVariables);

    void renderText(const QString &html);
    void renderText(const QString &html, QVariantMap& contextVariables);

private:
    QString mTemplateFile;
};

#else

#include <QLabel>

class WebView : public QLabel
{
    Q_OBJECT

public:
    WebView(QWidget* parent = nullptr) : QLabel(parent)
    {
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setText("QWebEngine not supported.");
    }

    void setLinksExternal(bool external) { Q_UNUSED(external) }
    void printToPdf(const QString& filePath) { Q_UNUSED(filePath) }

    QString templateFile() const { return QString(); }
    void setTemplateFile(const QString& file) { Q_UNUSED(file) }

    void renderTemplate() { }
    void renderTemplate(QVariantMap& contextVariables) { Q_UNUSED(contextVariables) }

    void renderText(const QString &html) { Q_UNUSED(html) }
    void renderText(const QString &html, QVariantMap& contextVariables) { Q_UNUSED(html) Q_UNUSED(contextVariables) }
};

#endif

#endif // WEBVIEW_H
