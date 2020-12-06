#ifndef HTMLHIGHLIGHTER_H
#define HTMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextCharFormat;
QT_END_NAMESPACE

class HtmlHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit HtmlHighLighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    enum States {
        None,
        Tag,
        Comment,
        Quote,
        CustomTag
    };

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> startTagRules;
    QVector<HighlightingRule> endTagRules;

    QRegularExpression openTag;
    QRegularExpression closeTag;
    QTextCharFormat edgeTagFormat;
    QTextCharFormat insideTagFormat;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QTextCharFormat multiLineCommentFormat;

    QRegularExpression quotes;
    QTextCharFormat quotationFormat;
    QTextCharFormat tagsFormat;

    QRegularExpression customTagStartExpression;
    QRegularExpression customTagEndExpression;
    QTextCharFormat customTagFormat;
};

#endif // HTMLHIGHLIGHTER_H
