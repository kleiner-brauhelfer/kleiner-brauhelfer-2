#include "htmlhighlighter.h"
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>
#include "settings.h"

extern Settings* gSettings;

HtmlHighLighter::HtmlHighLighter(QTextDocument *parent):
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    if (gSettings->theme() != Qt::ColorScheme::Dark)
    {
        edgeTagFormat.setForeground(QBrush(QColor(0x32a9dd)));
        insideTagFormat.setForeground(Qt::blue);
        tagsFormat.setForeground(Qt::darkBlue);
        multiLineCommentFormat.setForeground(Qt::darkGray);
        quotationFormat.setForeground(Qt::darkGreen);
        customTagFormat.setForeground(QColor(219, 137, 9));
    }
    else
    {
        edgeTagFormat.setForeground(QBrush(QColor(0x2a82da)));
        insideTagFormat.setForeground(QBrush(QColor(0x32a9dd)));
        tagsFormat.setForeground(QBrush(QColor(0x0044FF)));
        multiLineCommentFormat.setForeground(Qt::darkGray);
        quotationFormat.setForeground(QBrush(QColor(0x82da2a)));
        customTagFormat.setForeground(QColor(219, 137, 9));
    }
    insideTagFormat.setFontWeight(QFont::Bold);
    tagsFormat.setFontWeight(QFont::Bold);

    openTag = QRegularExpression(QStringLiteral("<"));
    closeTag = QRegularExpression(QStringLiteral(">"));
    commentStartExpression = QRegularExpression(QStringLiteral("<!--"));
    commentEndExpression = QRegularExpression(QStringLiteral("-->"));
    quotes = QRegularExpression(QStringLiteral("\""));
    customTagStartExpression = QRegularExpression(QStringLiteral("\\{\\{"));
    customTagEndExpression = QRegularExpression(QStringLiteral("\\}\\}"));

    QStringList keywordPatterns;
    keywordPatterns << QStringLiteral("<\\ba\\b") << QStringLiteral("<\\babbr\\b")
                    << QStringLiteral("<\\bacronym\\b") << QStringLiteral("<\\baddress\\b")
                    << QStringLiteral("<\\bapplet\\b") << QStringLiteral("<\\barea\\b")
                    << QStringLiteral("<\\barticle\\b") << QStringLiteral("<\\baside\\b")
                    << QStringLiteral("<\\baudio\\b") << QStringLiteral("<\\bb\\b")
                    << QStringLiteral("<\\bbase\\b") << QStringLiteral("<\\bbasefont\\b")
                    << QStringLiteral("<\\bbdi\\b") << QStringLiteral("<\\bbdo\\b")
                    << QStringLiteral("<\\bbgsound\\b") << QStringLiteral("<\\bblockquote\\b")
                    << QStringLiteral("<\\bbig\\b") << QStringLiteral("<\\bbody\\b")
                    << QStringLiteral("<\\bblink\\b") << QStringLiteral("<\\bbr\\b")
                    << QStringLiteral("<\\bbutton\\b") << QStringLiteral("<\\bcanvas\\b")
                    << QStringLiteral("<\\bcaption\\b") << QStringLiteral("<\\bcenter\\b")
                    << QStringLiteral("<\\bcite\\b") << QStringLiteral("<\\bcode\\b")
                    << QStringLiteral("<\\bcol\\b") << QStringLiteral("<\\bcolgroup\\b")
                    << QStringLiteral("<\\bcommand\\b") << QStringLiteral("<\\bcomment\\b")
                    << QStringLiteral("<\\bdata\\b") << QStringLiteral("<\\bdatalist\\b")
                    << QStringLiteral("<\\bdd\\b") << QStringLiteral("<\\bdel\\b")
                    << QStringLiteral("<\\bdetails\\b") << QStringLiteral("<\\bdfn\\b")
                    << QStringLiteral("<\\bdialog\\b") << QStringLiteral("<\\bdir\\b")
                    << QStringLiteral("<\\bdiv\\b") << QStringLiteral("<\\bdl\\b")
                    << QStringLiteral("<\\bdt\\b") << QStringLiteral("<\\bem\\b")
                    << QStringLiteral("<\\bembed\\b") << QStringLiteral("<\\bfieldset\\b")
                    << QStringLiteral("<\\bfigcaption\\b") << QStringLiteral("<\\bfigure\\b")
                    << QStringLiteral("<\\bfont\\b") << QStringLiteral("<\\bfooter\\b")
                    << QStringLiteral("<\\bform\\b") << QStringLiteral("<\\bframe\\b")
                    << QStringLiteral("<\\bframeset\\b") << QStringLiteral("<\\bh1\\b")
                    << QStringLiteral("<\\bh2\\b") << QStringLiteral("<\\bh3\\b")
                    << QStringLiteral("<\\bh4\\b") << QStringLiteral("<\\bh5\\b")
                    << QStringLiteral("<\\bh6\\b") << QStringLiteral("<\\bhead\\b")
                    << QStringLiteral("<\\bheader\\b") << QStringLiteral("<\\bhgroup\\b")
                    << QStringLiteral("<\\bhr\\b") << QStringLiteral("<\\bhtml\\b")
                    << QStringLiteral("<\\bi\\b") << QStringLiteral("<\\biframe\\b")
                    << QStringLiteral("<\\bimg\\b") << QStringLiteral("<\\binput\\b")
                    << QStringLiteral("<\\bins\\b") << QStringLiteral("<\\bisindex\\b")
                    << QStringLiteral("<\\bkbd\\b") << QStringLiteral("<\\bkeygen\\b")
                    << QStringLiteral("<\\blabel\\b") << QStringLiteral("<\\blegend\\b")
                    << QStringLiteral("<\\bli\\b") << QStringLiteral("<\\blink\\b")
                    << QStringLiteral("<\\blisting\\b") << QStringLiteral("<\\bmain\\b")
                    << QStringLiteral("<\\bmap\\b") << QStringLiteral("<\\bmarquee\\b")
                    << QStringLiteral("<\\bmark\\b") << QStringLiteral("<\\bmenu\\b")
                    << QStringLiteral("<\\bamenuitem\\b") << QStringLiteral("<\\bmeta\\b")
                    << QStringLiteral("<\\bmeter\\b") << QStringLiteral("<\\bmulticol\\b")
                    << QStringLiteral("<\\bnav\\b") << QStringLiteral("<\\bnobr\\b")
                    << QStringLiteral("<\\bnoembed\\b") << QStringLiteral("<\\bnoindex\\b")
                    << QStringLiteral("<\\bnoframes\\b") << QStringLiteral("<\\bnoscript\\b")
                    << QStringLiteral("<\\bobject\\b") << QStringLiteral("<\\bol\\b")
                    << QStringLiteral("<\\boptgroup\\b") << QStringLiteral("<\\boption\\b")
                    << QStringLiteral("<\\boutput\\b") << QStringLiteral("<\\bp\\b")
                    << QStringLiteral("<\\bparam\\b") << QStringLiteral("<\\bpicture\\b")
                    << QStringLiteral("<\\bplaintext\\b") << QStringLiteral("<\\bpre\\b")
                    << QStringLiteral("<\\bprogress\\b") << QStringLiteral("<\\bq\\b")
                    << QStringLiteral("<\\brp\\b") << QStringLiteral("<\\brt\\b")
                    << QStringLiteral("<\\brtc\\b") << QStringLiteral("<\\bruby\\b")
                    << QStringLiteral("<\\bs\\b") << QStringLiteral("<\\bsamp\\b")
                    << QStringLiteral("<\\bscript\\b") << QStringLiteral("<\\bsection\\b")
                    << QStringLiteral("<\\bselect\\b") << QStringLiteral("<\\bsmall\\b")
                    << QStringLiteral("<\\bsource\\b") << QStringLiteral("<\\bspacer\\b")
                    << QStringLiteral("<\\bspan\\b") << QStringLiteral("<\\bstrike\\b")
                    << QStringLiteral("<\\bstrong\\b") << QStringLiteral("<\\bstyle\\b")
                    << QStringLiteral("<\\bsub\\b") << QStringLiteral("<\\bsummary\\b")
                    << QStringLiteral("<\\bsup\\b") << QStringLiteral("<\\btable\\b")
                    << QStringLiteral("<\\btbody\\b") << QStringLiteral("<\\btd\\b")
                    << QStringLiteral("<\\btemplate\\b") << QStringLiteral("<\\btextarea\\b")
                    << QStringLiteral("<\\btfoot\\b") << QStringLiteral("<\\bth\\b")
                    << QStringLiteral("<\\bthead\\b") << QStringLiteral("<\\btime\\b")
                    << QStringLiteral("<\\btitle\\b") << QStringLiteral("<\\btr\\b")
                    << QStringLiteral("<\\btrack\\b") << QStringLiteral("<\\btt\\b")
                    << QStringLiteral("<\\bu\\b") << QStringLiteral("<\\bul\\b")
                    << QStringLiteral("<\\bvar\\b") << QStringLiteral("<\\bvideo\\b")
                    << QStringLiteral("<\\bwbr\\b") << QStringLiteral("<\\bxmp\\b");

    startTagRules.reserve(keywordPatterns.size());
    for (int i = 0; i < keywordPatterns.size(); ++i)
    {
        rule.pattern = QRegularExpression(keywordPatterns[i]);
        rule.format = tagsFormat;
        startTagRules.append(rule);
    }

    QStringList keywordPatterns_end;
    keywordPatterns_end << QStringLiteral("<!\\bDOCTYPE\\b") << QStringLiteral("</\\ba\\b")
                        << QStringLiteral("</\\babbr\\b") << QStringLiteral("</\\bacronym\\b")
                        << QStringLiteral("</\\baddress\\b") << QStringLiteral("</\\bapplet\\b")
                        << QStringLiteral("</\\barea\\b") << QStringLiteral("</\\barticle\\b")
                        << QStringLiteral("</\\baside\\b") << QStringLiteral("</\\baudio\\b")
                        << QStringLiteral("</\\bb\\b") << QStringLiteral("</\\bbase\\b")
                        << QStringLiteral("</\\bbasefont\\b") << QStringLiteral("</\\bbdi\\b")
                        << QStringLiteral("</\\bbdo\\b") << QStringLiteral("</\\bbgsound\\b")
                        << QStringLiteral("</\\bblockquote\\b") << QStringLiteral("</\\bbig\\b")
                        << QStringLiteral("</\\bbody\\b") << QStringLiteral("</\\bblink\\b")
                        << QStringLiteral("</\\bbr\\b") << QStringLiteral("</\\bbutton\\b")
                        << QStringLiteral("</\\bcanvas\\b") << QStringLiteral("</\\bcaption\\b")
                        << QStringLiteral("</\\bcenter\\b") << QStringLiteral("</\\bcite\\b")
                        << QStringLiteral("</\\bcode\\b") << QStringLiteral("</\\bcol\\b")
                        << QStringLiteral("</\\bcolgroup\\b") << QStringLiteral("</\\bcommand\\b")
                        << QStringLiteral("</\\bcomment\\b") << QStringLiteral("</\\bdata\\b")
                        << QStringLiteral("</\\bdatalist\\b") << QStringLiteral("</\\bdd\\b")
                        << QStringLiteral("</\\bdel\\b") << QStringLiteral("</\\bdetails\\b")
                        << QStringLiteral("</\\bdfn\\b") << QStringLiteral("</\\bdialog\\b")
                        << QStringLiteral("</\\bdir\\b") << QStringLiteral("</\\bdiv\\b")
                        << QStringLiteral("</\\bdl\\b") << QStringLiteral("</\\bdt\\b")
                        << QStringLiteral("</\\bem\\b") << QStringLiteral("</\\bembed\\b")
                        << QStringLiteral("</\\bfieldset\\b")
                        << QStringLiteral("</\\bfigcaption\\b") << QStringLiteral("</\\bfigure\\b")
                        << QStringLiteral("</\\bfont\\b") << QStringLiteral("</\\bfooter\\b")
                        << QStringLiteral("</\\bform\\b") << QStringLiteral("</\\bframe\\b")
                        << QStringLiteral("</\\bframeset\\b") << QStringLiteral("</\\bh1\\b")
                        << QStringLiteral("</\\bh2\\b") << QStringLiteral("</\\bh3\\b")
                        << QStringLiteral("</\\bh4\\b") << QStringLiteral("</\\bh5\\b")
                        << QStringLiteral("</\\bh6\\b") << QStringLiteral("</\\bhead\\b")
                        << QStringLiteral("</\\bheader\\b") << QStringLiteral("</\\bhgroup\\b")
                        << QStringLiteral("</\\bhr\\b") << QStringLiteral("</\\bhtml\\b")
                        << QStringLiteral("</\\bi\\b") << QStringLiteral("</\\biframe\\b")
                        << QStringLiteral("</\\bimg\\b") << QStringLiteral("</\\binput\\b")
                        << QStringLiteral("</\\bins\\b") << QStringLiteral("</\\bisindex\\b")
                        << QStringLiteral("</\\bkbd\\b") << QStringLiteral("</\\bkeygen\\b")
                        << QStringLiteral("</\\blabel\\b") << QStringLiteral("</\\blegend\\b")
                        << QStringLiteral("</\\bli\\b") << QStringLiteral("</\\blink\\b")
                        << QStringLiteral("</\\blisting\\b") << QStringLiteral("</\\bmain\\b")
                        << QStringLiteral("</\\bmap\\b") << QStringLiteral("</\\bmarquee\\b")
                        << QStringLiteral("</\\bmark\\b") << QStringLiteral("</\\bmenu\\b")
                        << QStringLiteral("</\\bamenuitem\\b") << QStringLiteral("</\\bmeta\\b")
                        << QStringLiteral("</\\bmeter\\b") << QStringLiteral("</\\bmulticol\\b")
                        << QStringLiteral("</\\bnav\\b") << QStringLiteral("</\\bnobr\\b")
                        << QStringLiteral("</\\bnoembed\\b") << QStringLiteral("</\\bnoindex\\b")
                        << QStringLiteral("</\\bnoframes\\b") << QStringLiteral("</\\bnoscript\\b")
                        << QStringLiteral("</\\bobject\\b") << QStringLiteral("</\\bol\\b")
                        << QStringLiteral("</\\boptgroup\\b") << QStringLiteral("</\\boption\\b")
                        << QStringLiteral("</\\boutput\\b") << QStringLiteral("</\\bp\\b")
                        << QStringLiteral("</\\bparam\\b") << QStringLiteral("</\\bpicture\\b")
                        << QStringLiteral("</\\bplaintext\\b") << QStringLiteral("</\\bpre\\b")
                        << QStringLiteral("</\\bprogress\\b") << QStringLiteral("</\\bq\\b")
                        << QStringLiteral("</\\brp\\b") << QStringLiteral("</\\brt\\b")
                        << QStringLiteral("</\\brtc\\b") << QStringLiteral("</\\bruby\\b")
                        << QStringLiteral("</\\bs\\b") << QStringLiteral("</\\bsamp\\b")
                        << QStringLiteral("</\\bscript\\b") << QStringLiteral("</\\bsection\\b")
                        << QStringLiteral("</\\bselect\\b") << QStringLiteral("</\\bsmall\\b")
                        << QStringLiteral("</\\bsource\\b") << QStringLiteral("</\\bspacer\\b")
                        << QStringLiteral("</\\bspan\\b") << QStringLiteral("</\\bstrike\\b")
                        << QStringLiteral("</\\bstrong\\b") << QStringLiteral("</\\bstyle\\b")
                        << QStringLiteral("</\\bsub\\b") << QStringLiteral("</\\bsummary\\b")
                        << QStringLiteral("</\\bsup\\b") << QStringLiteral("</\\btable\\b")
                        << QStringLiteral("</\\btbody\\b") << QStringLiteral("</\\btd\\b")
                        << QStringLiteral("</\\btemplate\\b") << QStringLiteral("</\\btextarea\\b")
                        << QStringLiteral("</\\btfoot\\b") << QStringLiteral("</\\bth\\b")
                        << QStringLiteral("</\\bthead\\b") << QStringLiteral("</\\btime\\b")
                        << QStringLiteral("</\\btitle\\b") << QStringLiteral("</\\btr\\b")
                        << QStringLiteral("</\\btrack\\b") << QStringLiteral("</\\btt\\b")
                        << QStringLiteral("</\\bu\\b") << QStringLiteral("</\\bul\\b")
                        << QStringLiteral("</\\bvar\\b") << QStringLiteral("</\\bvideo\\b")
                        << QStringLiteral("</\\bwbr\\b") << QStringLiteral("</\\bxmp\\b");

    endTagRules.reserve(keywordPatterns_end.size());
    for (int i = 0; i < keywordPatterns_end.size(); ++i)
    {
        rule.pattern = QRegularExpression(keywordPatterns_end[i]);
        rule.format = tagsFormat;
        endTagRules.append(rule);
    }
}

void HtmlHighLighter::highlightBlock(const QString &text)
{
    setCurrentBlockState(HtmlHighLighter::None);

    // TAG
    int startIndex = 0;
    // Если не находимся внутри тега,
    if (previousBlockState() != HtmlHighLighter::Tag && previousBlockState() != HtmlHighLighter::Quote)
    {
        // То пытаемся найти начало следующего тега
        startIndex = text.indexOf(openTag);
    }

    // Забираем состояние предыдущего текстового блока
    int subPreviousTag = previousBlockState();
    while (startIndex >= 0)
    {
        // ищем символ конца тега
        int endIndex = text.indexOf(closeTag, startIndex);

        int tagLength;
        // если конец тега не найден, то устанавливаем состояние блока
        if (endIndex == -1)
        {
            setCurrentBlockState(HtmlHighLighter::Tag);
            tagLength = text.length() - startIndex;
        }
        else
        {
            tagLength = endIndex - startIndex + 1;
        }

        // Устанавливаем форматирования для тега
        if (subPreviousTag != HtmlHighLighter::Tag)
        {
            // с начала тега и до конца, если предыдущее состояние не равнялось Tag
            setFormat(startIndex, 1, edgeTagFormat);
            setFormat(startIndex + 1, tagLength - 1, insideTagFormat);
        }
        else
        {
            // Если же находимся уже внутри тега с самого начала блока
            // и до конца тега
            setFormat(startIndex, tagLength, insideTagFormat);
            subPreviousTag = HtmlHighLighter::None;
        }
        // Форматируем символ конца тега
        setFormat(endIndex, 1, edgeTagFormat);

        /// QUOTES ///////////////////////////////////////
        int startQuoteIndex = 0;
        // Если не находимся в кавычках с предыдущего блока
        if (previousBlockState() != HtmlHighLighter::Quote)
        {
            // То пытаемся найти начало кавычек
            startQuoteIndex = text.indexOf(quotes, startIndex);
        }

        // Подсвечиваем все кавычки внутри тега
        while (startQuoteIndex >= 0 && ((startQuoteIndex < endIndex) || (endIndex == -1)))
        {
            int endQuoteIndex = text.indexOf(quotes,startQuoteIndex + 1);
            int quoteLength;
            if (endQuoteIndex == -1)
            {
                // Если закрывающая кавычка не найдена, то устанавливаем состояние Quote для блока
                setCurrentBlockState(HtmlHighLighter::Quote);
                quoteLength = text.length() - startQuoteIndex;
            }
            else
            {
                quoteLength = endQuoteIndex - startQuoteIndex + 1;
            }

            if ((endIndex > endQuoteIndex) || endIndex == -1)
            {
                setFormat(startQuoteIndex, quoteLength, quotationFormat);
                startQuoteIndex = text.indexOf(quotes, startQuoteIndex + quoteLength);
            }
            else
            {
                break;
            }
        }
        //////////////////////////////////////////////////
        // Снова ищем начало тега
        startIndex = text.indexOf(openTag, startIndex + tagLength);
    }

    // EDGES OF TAGS
    // Обработка цвета саимх тегов, то есть подсветка слов div, p, strong и т.д.
    for (int i = 0; i < startTagRules.size(); ++i)
    {
        QRegularExpression expression(startTagRules[i].pattern);
        int index = text.indexOf(expression);
        while (index >= 0)
        {
            int length = expression.pattern().length() - 3;
            setFormat(index + 1, length - 1, startTagRules[i].format);
            index = text.indexOf(expression, index + length);
        }
    }

    for (int i = 0; i < endTagRules.size(); ++i)
    {
        QRegularExpression expression(endTagRules[i].pattern);
        int index = text.indexOf(expression);
        while (index >= 0) {
            int length = expression.pattern().length() - 3;
            setFormat(index + 1, 1, edgeTagFormat);
            setFormat(index + 2, length - 2, endTagRules[i].format);
            index = text.indexOf(expression, index + length);
        }
    }

    // COMMENT
    int startCommentIndex = 0;
    // Если предыдущее состояние тега не является комментарием
    if (previousBlockState() != HtmlHighLighter::Comment)
    {
        // то пытаемся найти начало комментария
        startCommentIndex = text.indexOf(commentStartExpression);
    }

    // Если комментарий найден
    while (startCommentIndex >= 0)
    {
        // Ищем конец комментария
        int endCommentIndex = text.indexOf(commentEndExpression, startCommentIndex);
        int commentLength;

        // Если конец не найден
        if (endCommentIndex == -1)
        {
            // То устанавливаем состяоние Comment
            // Принцип аналогичен, что и для обычных тегов
            setCurrentBlockState(HtmlHighLighter::Comment);
            commentLength = text.length() - startCommentIndex;
        }
        else
        {
            commentLength = endCommentIndex - startCommentIndex + 3;
        }

        setFormat(startCommentIndex, commentLength, multiLineCommentFormat);
        startCommentIndex = text.indexOf(commentStartExpression, startCommentIndex + commentLength);
    }

    // CUSTOM TAG
    int startCustomTagIndex = 0;
    if (previousBlockState() != HtmlHighLighter::CustomTag)
    {
        startCustomTagIndex = text.indexOf(customTagStartExpression);
    }
    while (startCustomTagIndex >= 0)
    {
        int endIndex = text.indexOf(customTagEndExpression, startCustomTagIndex);
        int length;
        if (endIndex == -1)
        {
            setCurrentBlockState(HtmlHighLighter::CustomTag);
            length = text.length() - startCustomTagIndex;
        }
        else
        {
            length = endIndex - startCustomTagIndex + 2;
        }
        setFormat(startCustomTagIndex, length, customTagFormat);
        startCustomTagIndex = text.indexOf(customTagStartExpression, startCustomTagIndex + length);
    }
}
