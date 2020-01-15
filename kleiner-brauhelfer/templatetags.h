#ifndef TEMPLATETAGS_H
#define TEMPLATETAGS_H

#include <functional>
#include <QFlag>
#include "widgets/wdgwebvieweditable.h"

class TemplateTags
{
public:

    enum TagPart {
        TagNone = 0x0000,
        TagApp = 0x0001,
        TagRezept = 0x0002,
        TagSud = 0x0004,
        TagTags = 0x0008,
        TagAnhang = 0x0010,
        TagBraudaten = 0x0020,
        TagGeraete = 0x0040,
        TagAll = 0xffff
    };
    Q_DECLARE_FLAGS(TagParts, TagPart)

    static void render(WdgWebViewEditable* view, TagParts parts, int sudRow = -1);

    static void render(WdgWebViewEditable* view, TagParts parts, std::function<void(QVariantMap &)> fnc, int sudRow = -1);

    static void erstelleTagListe(QVariantMap& ctx, TagParts parts, int sudRow = -1);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TemplateTags::TagParts)

#endif // TEMPLATETAGS_H
