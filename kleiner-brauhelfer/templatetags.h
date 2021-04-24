#ifndef TEMPLATETAGS_H
#define TEMPLATETAGS_H

#include <functional>
#include "widgets/wdgwebvieweditable.h"

class TemplateTags
{
public:
    static void render(WdgWebViewEditable* view, int sudRow);
    static void render(WdgWebViewEditable* view, std::function<void(QVariantMap &)> fnc, int sudRow);
    static void erstelleTagListe(QVariantMap& ctx, int sudRow);
};

#endif // TEMPLATETAGS_H
