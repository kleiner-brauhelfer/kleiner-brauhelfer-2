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
    void renderTemplate(QVariantHash& contextVariables);

    void renderText(const QString &html);
    void renderText(const QString &html, QVariantHash& contextVariables);

    static void erstelleTagListe(QVariantHash& contextVariables, bool sudDaten);
private:
    QString mTemplateFile;
};

#endif // WEBVIEW_H
