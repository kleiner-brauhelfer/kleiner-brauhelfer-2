#include "lineedit.h"
#include <QGuiApplication>
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent),
    mError(false)
{
    connect(this, &QLineEdit::textChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool LineEdit::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QLineEdit::event(event);
}

void LineEdit::updatePalette()
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
        if (palette() != QGuiApplication::palette())
            setPalette(QGuiApplication::palette());
    }
}

void LineEdit::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}

