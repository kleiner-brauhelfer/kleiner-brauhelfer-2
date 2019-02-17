#include "combobox.h"
#include "settings.h"

extern Settings *gSettings;

ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
}

void ComboBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QComboBox::wheelEvent(event);
}

void ComboBox::updatePalette()
{
    if (!isEnabled())
        setPalette(gSettings->palette);
    else if (mError)
        setPalette(gSettings->paletteErrorButton);
    else
        setPalette(gSettings->paletteInput);
}

void ComboBox::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QComboBox::paintEvent(event);
}

void ComboBox::setError(bool e)
{
    mError = e;
}
