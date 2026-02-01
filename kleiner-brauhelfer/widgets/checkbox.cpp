#include "checkbox.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

CheckBox::CheckBox(QWidget *parent) :
    QCheckBox(parent),
    mDefaultPalette(gSettings->palette),
    mError(false)
{
    connect(this, &QCheckBox::checkStateChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

CheckBox::CheckBox(const QString &text, QWidget *parent) :
    QCheckBox(text, parent),
    mError(false)
{
    connect(this, &QCheckBox::checkStateChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool CheckBox::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QCheckBox::event(event);
}

void CheckBox::setDefaultPalette(const QPalette &p)
{
    mDefaultPalette = p;
    updatePalette();
}

void CheckBox::updatePalette()
{
    if (WidgetDecorator::contains(this))
    {
        if (palette() != gSettings->paletteChanged)
            setPalette(gSettings->paletteChanged);
    }
    else if (mError)
    {
        qDebug() << "updatePalette error";
        if (palette() != gSettings->paletteError)
            setPalette(gSettings->paletteError);
    }
    else
    {
        if (palette() != mDefaultPalette)
            setPalette(mDefaultPalette);
    }
}

void CheckBox::setError(bool e)
{
    qDebug() << "setError" << e;
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
