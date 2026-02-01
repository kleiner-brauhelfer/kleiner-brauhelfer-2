#include "toolbutton.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

ToolButton::ToolButton(QWidget *parent) :
    QToolButton(parent),
    mError(false)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool ToolButton::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QToolButton::event(event);
}

void ToolButton::updatePalette()
{
    if (WidgetDecorator::contains(this))
    {
        if (palette() != gSettings->paletteChanged)
            setPalette(gSettings->paletteChanged);
    }
    else if (mError)
    {
        if (palette() != gSettings->paletteError)
            setPalette(gSettings->paletteError);
    }
    else
    {
        if (palette() != gSettings->palette)
            setPalette(gSettings->palette);
    }
}

void ToolButton::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
