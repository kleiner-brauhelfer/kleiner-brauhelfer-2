#ifndef WEBVIEW_H
#define WEBVIEW_H

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

    static void erstelleTagListe(QVariantMap& ctx, int sudRow = -1);
private:
    QString mTemplateFile;
};

#endif // WEBVIEW_H
