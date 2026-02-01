#include "radiobutton.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

RadioButton::RadioButton(QWidget *parent) :
    QRadioButton(parent),
    mError(false)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

RadioButton::RadioButton(const QString &text, QWidget *parent) :
    QRadioButton(text, parent),
    mError(false)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool RadioButton::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QRadioButton::event(event);
}

void RadioButton::updatePalette()
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

void RadioButton::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
