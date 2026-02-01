#include "combobox.h"
#include <QAbstractItemView>
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(this, &QComboBox::currentIndexChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
    connect(this, &QComboBox::currentTextChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool ComboBox::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QComboBox::event(event);
}

void ComboBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QComboBox::wheelEvent(event);
}

void ComboBox::updatePalette()
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

void ComboBox::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}

void ComboBox::setToolTip(const QString &str)
{
    QComboBox::setToolTip(str);
    QAbstractItemView* v = view();
    if (v)
        v->setToolTip(str);
}
