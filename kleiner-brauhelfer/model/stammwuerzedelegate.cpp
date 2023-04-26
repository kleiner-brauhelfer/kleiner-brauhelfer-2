#include "stammwuerzedelegate.h"
#include <QDoubleSpinBox>
#include "settings.h"
#include "units.h"

extern Settings* gSettings;

StammWuerzeDelegate::StammWuerzeDelegate(bool readonly, QObject *parent) :
    DoubleSpinBoxDelegate(1, parent)
{
    Units::Unit grvunit = Units::GravityUnit();
    mReadonly = readonly;
    mDecimals = Units::decimals(grvunit);
    mMin =  Units::minimum(grvunit);
    mMax =  Units::maximum(grvunit);
    mStep =  Units::singleStep(grvunit);
}

void StammWuerzeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
    w->setValue(Units::convert(Units::Plato, Units::GravityUnit(), index.data(Qt::EditRole).toDouble()));
}

void StammWuerzeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
    w->interpretText();
    model->setData(index, Units::convert(Units::GravityUnit(), Units::Plato, w->value()), Qt::EditRole);
}

QString StammWuerzeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return Units::convertStr(Units::Plato, Units::GravityUnit(), value.toDouble(), locale);
}
