#ifndef DLGRICHTEXTEDITOR_H
#define DLGRICHTEXTEDITOR_H

#include "dlgabstract.h"
#include <QTextCharFormat>
#include "helper/htmlhighlighter.h"

namespace Ui {
class DlgRichTextEditor;
}

class DlgRichTextEditor : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgRichTextEditor(QWidget *parent = nullptr);
    ~DlgRichTextEditor();
    void setHtml(const QString& html);
    QString toHtml() const;

private:
    QString extractBody(const QString& html) const;
    void changeList(bool checked, QTextListFormat::Style style);
    void changeIndent(int delta);

private slots:
    void on_tbRichText_textChanged();
    void on_tbSource_textChanged();
    void on_tbRichText_cursorPositionChanged();
    void on_tbRichText_currentCharFormatChanged(const QTextCharFormat &format);
    void on_cbStyle_currentIndexChanged(int index);
    void on_cbFont_currentFontChanged(const QFont &font);
    void on_cbSize_textActivated(const QString &size);
    void on_btnColorText_clicked();
    void on_btnBold_clicked(bool checked);
    void on_btnItalic_clicked(bool checked);
    void on_btnUnderline_clicked(bool checked);
    void on_btnLeft_clicked(bool checked);
    void on_btnCenter_clicked(bool checked);
    void on_btnRight_clicked(bool checked);
    void on_btnLink_clicked(bool checked);
    void on_btnListBullet_clicked(bool checked);
    void on_btnListOrdered_clicked(bool checked);
    void on_btnIdentInc_clicked();
    void on_btnIndDecr_clicked();
    void on_btnImg_clicked();

private:
    Ui::DlgRichTextEditor *ui;
    HtmlHighLighter *mHtmlHightLighter;
};

#endif // DLGRICHTEXTEDITOR_H
