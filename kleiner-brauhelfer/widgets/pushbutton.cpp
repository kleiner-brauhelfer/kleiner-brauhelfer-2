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

void PushButton::setDefaultPalette(const QPalette &p)
{
    mDefaultPalette = p;
    update();
}

void PushButton::paintEvent(QPaintEvent *event)
{
    if (WidgetDecorator::contains(this))
        setPalette(gSettings->paletteChanged);
    else if (mError)
        setPalette(gSettings->paletteErrorButton);
    else
        setPalette(mDefaultPalette);
    QPushButton::paintEvent(event);
}

void PushButton::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        update();
    }
}
