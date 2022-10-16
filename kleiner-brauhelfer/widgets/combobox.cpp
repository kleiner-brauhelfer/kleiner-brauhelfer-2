#include "combobox.h"
#include <QAbstractItemView>
#include "settings.h"

extern Settings *gSettings;

ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent),
    WidgetDecorator(),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(this, SIGNAL(currentIndexChanged(int)), SLOT(on_valueChanged()));
    connect(this, SIGNAL(currentTextChanged(QString)), SLOT(on_valueChanged()));
}

void ComboBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QComboBox::wheelEvent(event);
}

void ComboBox::updatePalette()
{
    if (mValueChanged)
        setPalette(gSettings->paletteChanged);
    else if (!isEnabled())
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

void ComboBox::on_valueChanged()
{
    waValueChanged(this);
}

void ComboBox::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QComboBox::focusOutEvent(event);
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
