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

    if (gSettings->theme() != Settings::Dark)
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

    openTag = QRegularExpression("<");
    closeTag = QRegularExpression(">");
    commentStartExpression = QRegularExpression("<!--");
    commentEndExpression = QRegularExpression("-->");
    quotes = QRegularExpression("\"");
    customTagStartExpression = QRegularExpression("\\{\\{");
    customTagEndExpression = QRegularExpression("\\}\\}");

    QStringList keywordPatterns;
    keywordPatterns << "<\\ba\\b" << "<\\babbr\\b" << "<\\bacronym\\b" << "<\\baddress\\b" << "<\\bapplet\\b"
                    << "<\\barea\\b" << "<\\barticle\\b" << "<\\baside\\b" << "<\\baudio\\b" << "<\\bb\\b"
                    << "<\\bbase\\b" << "<\\bbasefont\\b" << "<\\bbdi\\b" << "<\\bbdo\\b" << "<\\bbgsound\\b"
                    << "<\\bblockquote\\b" << "<\\bbig\\b" << "<\\bbody\\b" << "<\\bblink\\b" << "<\\bbr\\b"
                    << "<\\bbutton\\b" << "<\\bcanvas\\b" << "<\\bcaption\\b" << "<\\bcenter\\b" << "<\\bcite\\b"
                    << "<\\bcode\\b" << "<\\bcol\\b" << "<\\bcolgroup\\b" << "<\\bcommand\\b" << "<\\bcomment\\b"
                    << "<\\bdata\\b" << "<\\bdatalist\\b" << "<\\bdd\\b" << "<\\bdel\\b" << "<\\bdetails\\b"
                    << "<\\bdfn\\b" << "<\\bdialog\\b" << "<\\bdir\\b" << "<\\bdiv\\b" << "<\\bdl\\b"
                    << "<\\bdt\\b" << "<\\bem\\b" << "<\\bembed\\b" << "<\\bfieldset\\b" << "<\\bfigcaption\\b"
                    << "<\\bfigure\\b" << "<\\bfont\\b" << "<\\bfooter\\b" << "<\\bform\\b" << "<\\bframe\\b"
                    << "<\\bframeset\\b" << "<\\bh1\\b" << "<\\bh2\\b" << "<\\bh3\\b" << "<\\bh4\\b"
                    << "<\\bh5\\b" << "<\\bh6\\b" << "<\\bhead\\b" << "<\\bheader\\b" << "<\\bhgroup\\b"
                    << "<\\bhr\\b" << "<\\bhtml\\b" << "<\\bi\\b" << "<\\biframe\\b" << "<\\bimg\\b"
                    << "<\\binput\\b" << "<\\bins\\b" << "<\\bisindex\\b" << "<\\bkbd\\b" << "<\\bkeygen\\b"
                    << "<\\blabel\\b" << "<\\blegend\\b" << "<\\bli\\b" << "<\\blink\\b" << "<\\blisting\\b"
                    << "<\\bmain\\b" << "<\\bmap\\b" << "<\\bmarquee\\b" << "<\\bmark\\b" << "<\\bmenu\\b"
                    << "<\\bamenuitem\\b" << "<\\bmeta\\b" << "<\\bmeter\\b" << "<\\bmulticol\\b" << "<\\bnav\\b"
                    << "<\\bnobr\\b" << "<\\bnoembed\\b" << "<\\bnoindex\\b" << "<\\bnoframes\\b" << "<\\bnoscript\\b"
                    << "<\\bobject\\b" << "<\\bol\\b" << "<\\boptgroup\\b" << "<\\boption\\b" << "<\\boutput\\b"
                    << "<\\bp\\b" << "<\\bparam\\b" << "<\\bpicture\\b" << "<\\bplaintext\\b" << "<\\bpre\\b"
                    << "<\\bprogress\\b" << "<\\bq\\b" << "<\\brp\\b" << "<\\brt\\b" << "<\\brtc\\b" << "<\\bruby\\b"
                    << "<\\bs\\b" << "<\\bsamp\\b" << "<\\bscript\\b" << "<\\bsection\\b" << "<\\bselect\\b"
                    << "<\\bsmall\\b" << "<\\bsource\\b" << "<\\bspacer\\b" << "<\\bspan\\b" << "<\\bstrike\\b"
                    << "<\\bstrong\\b" << "<\\bstyle\\b" << "<\\bsub\\b" << "<\\bsummary\\b" << "<\\bsup\\b"
                    << "<\\btable\\b" << "<\\btbody\\b" << "<\\btd\\b" << "<\\btemplate\\b" << "<\\btextarea\\b"
                    << "<\\btfoot\\b" << "<\\bth\\b" << "<\\bthead\\b" << "<\\btime\\b" << "<\\btitle\\b"
                    << "<\\btr\\b" << "<\\btrack\\b" << "<\\btt\\b" << "<\\bu\\b" << "<\\bul\\b" << "<\\bvar\\b"
                    << "<\\bvideo\\b" << "<\\bwbr\\b" << "<\\bxmp\\b";

    for (int i = 0; i < keywordPatterns.size(); ++i)
    {
        rule.pattern = QRegularExpression(keywordPatterns[i]);
        rule.format = tagsFormat;
        startTagRules.append(rule);
    }

    QStringList keywordPatterns_end;
    keywordPatterns_end << "<!\\bDOCTYPE\\b" << "</\\ba\\b" << "</\\babbr\\b" << "</\\bacronym\\b" << "</\\baddress\\b" << "</\\bapplet\\b"
                        << "</\\barea\\b" << "</\\barticle\\b" << "</\\baside\\b" << "</\\baudio\\b" << "</\\bb\\b"
                        << "</\\bbase\\b" << "</\\bbasefont\\b" << "</\\bbdi\\b" << "</\\bbdo\\b" << "</\\bbgsound\\b"
                        << "</\\bblockquote\\b" << "</\\bbig\\b" << "</\\bbody\\b" << "</\\bblink\\b" << "</\\bbr\\b"
                        << "</\\bbutton\\b" << "</\\bcanvas\\b" << "</\\bcaption\\b" << "</\\bcenter\\b" << "</\\bcite\\b"
                        << "</\\bcode\\b" << "</\\bcol\\b" << "</\\bcolgroup\\b" << "</\\bcommand\\b" << "</\\bcomment\\b"
                        << "</\\bdata\\b" << "</\\bdatalist\\b" << "</\\bdd\\b" << "</\\bdel\\b" << "</\\bdetails\\b"
                        << "</\\bdfn\\b" << "</\\bdialog\\b" << "</\\bdir\\b" << "</\\bdiv\\b" << "</\\bdl\\b"
                        << "</\\bdt\\b" << "</\\bem\\b" << "</\\bembed\\b" << "</\\bfieldset\\b" << "</\\bfigcaption\\b"
                        << "</\\bfigure\\b" << "</\\bfont\\b" << "</\\bfooter\\b" << "</\\bform\\b" << "</\\bframe\\b"
                        << "</\\bframeset\\b" << "</\\bh1\\b" << "</\\bh2\\b" << "</\\bh3\\b" << "</\\bh4\\b"
                        << "</\\bh5\\b" << "</\\bh6\\b" << "</\\bhead\\b" << "</\\bheader\\b" << "</\\bhgroup\\b"
                        << "</\\bhr\\b" << "</\\bhtml\\b" << "</\\bi\\b" << "</\\biframe\\b" << "</\\bimg\\b"
                        << "</\\binput\\b" << "</\\bins\\b" << "</\\bisindex\\b" << "</\\bkbd\\b" << "</\\bkeygen\\b"
                        << "</\\blabel\\b" << "</\\blegend\\b" << "</\\bli\\b" << "</\\blink\\b" << "</\\blisting\\b"
                        << "</\\bmain\\b" << "</\\bmap\\b" << "</\\bmarquee\\b" << "</\\bmark\\b" << "</\\bmenu\\b"
                        << "</\\bamenuitem\\b" << "</\\bmeta\\b" << "</\\bmeter\\b" << "</\\bmulticol\\b" << "</\\bnav\\b"
                        << "</\\bnobr\\b" << "</\\bnoembed\\b" << "</\\bnoindex\\b" << "</\\bnoframes\\b" << "</\\bnoscript\\b"
                        << "</\\bobject\\b" << "</\\bol\\b" << "</\\boptgroup\\b" << "</\\boption\\b" << "</\\boutput\\b"
                        << "</\\bp\\b" << "</\\bparam\\b" << "</\\bpicture\\b" << "</\\bplaintext\\b" << "</\\bpre\\b"
                        << "</\\bprogress\\b" << "</\\bq\\b" << "</\\brp\\b" << "</\\brt\\b" << "</\\brtc\\b" << "</\\bruby\\b"
                        << "</\\bs\\b" << "</\\bsamp\\b" << "</\\bscript\\b" << "</\\bsection\\b" << "</\\bselect\\b"
                        << "</\\bsmall\\b" << "</\\bsource\\b" << "</\\bspacer\\b" << "</\\bspan\\b" << "</\\bstrike\\b"
                        << "</\\bstrong\\b" << "</\\bstyle\\b" << "</\\bsub\\b" << "</\\bsummary\\b" << "</\\bsup\\b"
                        << "</\\btable\\b" << "</\\btbody\\b" << "</\\btd\\b" << "</\\btemplate\\b" << "</\\btextarea\\b"
                        << "</\\btfoot\\b" << "</\\bth\\b" << "</\\bthead\\b" << "</\\btime\\b" << "</\\btitle\\b"
                        << "</\\btr\\b" << "</\\btrack\\b" << "</\\btt\\b" << "</\\bu\\b" << "</\\bul\\b" << "</\\bvar\\b"
                        << "</\\bvideo\\b" << "</\\bwbr\\b" << "</\\bxmp\\b";

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
