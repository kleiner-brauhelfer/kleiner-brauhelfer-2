#ifndef HTMLHIGHLIGHTER_H
#define HTMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

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
    // Состояние подсветки, в которой находится текстовый блок на момент его закрытия
    enum States {
        None,       // Без подсветки
        Tag,        // Подсветка внутри тега
        Comment,    // Внутри комментария
        Quote       // Внутри кавычек, которые внутри тега
    };

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> startTagRules;    // Правила форматирования для открывающих тегов
    QVector<HighlightingRule> endTagRules;      // Правила форматирования для закрывающих тегов

    QRegExp openTag;                            // Символ открыватия тега - "<"
    QRegExp closeTag;                           // Символ закрытия тег    - ">"
    QTextCharFormat edgeTagFormat;              // Форматирование символов openTag и closeTag
    QTextCharFormat insideTagFormat;            // Форматирование текста внутри тега

    QRegExp commentStartExpression;             // Регулярка начала комментария
    QRegExp commentEndExpression;               // Регулярка закрытия комментария
    QTextCharFormat multiLineCommentFormat;     // Форматирование текста внутри комментария

    QRegExp quotes;                             // Регулярное выражение для текста в кавычках внутри тега
    QTextCharFormat quotationFormat;            // Форматирование текста в кавычках внутри тега
    QTextCharFormat tagsFormat;                 // Форматирование самих тегов
};

#endif // HTMLHIGHLIGHTER_H
