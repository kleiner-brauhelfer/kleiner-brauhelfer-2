#ifndef WEBVIEW_H
#define WEBVIEW_H

#ifdef QT_WEBENGINECORE_LIB
  #include <QWebEngineView>
  #include <QWebEnginePage>
#else
  #include <QTextBrowser>
#endif

#ifdef QT_WEBENGINECORE_LIB
class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = nullptr);
    void setLinksExternal(bool external);
    bool acceptNavigationRequest(const QUrl & url, QWebEnginePage::NavigationType type, bool) Q_DECL_OVERRIDE;
    QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;

private:
    bool mExternal;
};
#endif

#ifdef QT_WEBENGINECORE_LIB
class WebView : public QWebEngineView
#else
class WebView : public QTextBrowser
#endif
{
    Q_OBJECT

public:
    static void setSupported(bool isSupported);

public:
    WebView(QWidget* parent = nullptr);
    ~WebView();
    void setLinksExternal(bool external);
    void printToPdf(const QString& filePath, const QMarginsF& margins);

    QString templateFile() const;
    void setTemplateFile(const QString& file);

    void renderTemplate();
    void renderTemplate(QVariantMap& contextVariables);

    void renderText(const QString &html);
    void renderText(const QString &html, QVariantMap& contextVariables);

private:
    QString mTemplateFile;

private:
    static bool gIsSupported;
};

#endif // WEBVIEW_H
