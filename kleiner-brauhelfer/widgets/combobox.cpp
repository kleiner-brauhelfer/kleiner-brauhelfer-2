#include "combobox.h"
#include <QAbstractItemView>
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

void ComboBox::setToolTip(const QString &str)
{
    QComboBox::setToolTip(str);
    QAbstractItemView* v = view();
    if (v)
        v->setToolTip(str);
}
