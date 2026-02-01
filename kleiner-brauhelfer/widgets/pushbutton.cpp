#include "pushbutton.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent),
    mDefaultPalette(gSettings->palette),
    mAction(nullptr),
    mError(false)
{
    setAutoDefault(false);
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void PushButton::setAction(QAction *action)
{
    if (mAction)
    {
        disconnect(mAction, &QAction::changed, this, &PushButton::onActionChanged);
        disconnect(this, &PushButton::clicked, mAction, &QAction::trigger);
    }
    mAction = action;
    if (mAction)
    {
        connect(action, &QAction::changed, this, &PushButton::onActionChanged);
        connect(this, &PushButton::clicked, mAction, &QAction::trigger);
        onActionChanged();
    }
    else
    {
        setText(QString());
        setStatusTip(QString());
        setToolTip(QString());
        setIcon(QIcon());
        setEnabled(false);
        setCheckable(false);
        setChecked(false);
    }
}

void PushButton::onActionChanged()
{
    setText(mAction->text());
    setStatusTip(mAction->statusTip());
    setToolTip(mAction->toolTip());
    setIcon(mAction->icon());
    setEnabled(mAction->isEnabled());
    setCheckable(mAction->isCheckable());
    setChecked(mAction->isChecked());
}

bool PushButton::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QPushButton::event(event);
}

void PushButton::setDefaultPalette(const QPalette &p)
{
    mDefaultPalette = p;
    updatePalette();
}

void PushButton::updatePalette()
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
        if (palette() != mDefaultPalette)
            setPalette(mDefaultPalette);
    }
}

void PushButton::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
