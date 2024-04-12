#include "filterbuttonlager.h"
#include "radiobutton.h"
#include "proxymodelrohstoff.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QWidgetAction>
#include <QMenu>

FilterButtonLager::FilterButtonLager(QWidget *parent) :
    ToolButton(parent),
    mModel(nullptr)
{
    setCheckable(true);
    setPopupMode(ToolButtonPopupMode::InstantPopup);

    QWidget* widget = new QWidget(this);
    widget->setLayout(new QVBoxLayout(widget));

    mRadioButtonAlle = new RadioButton(tr("Alle"), widget);
    widget->layout()->addWidget(mRadioButtonAlle);
    connect(mRadioButtonAlle, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    mRadioButtonVorhanden = new RadioButton(tr("Vorhanden"), widget);
    widget->layout()->addWidget(mRadioButtonVorhanden);
    connect(mRadioButtonVorhanden, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    mRadioButtonNichtVorhanden = new RadioButton(tr("Nicht horhanden"), widget);
    widget->layout()->addWidget(mRadioButtonNichtVorhanden);
    connect(mRadioButtonNichtVorhanden, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    mRadioButtonInGebrauch = new RadioButton(tr("Nicht gebraute Rezepte"), widget);
    widget->layout()->addWidget(mRadioButtonInGebrauch);
    connect(mRadioButtonInGebrauch, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    QWidgetAction* wdgAction = new QWidgetAction(this);
    wdgAction->setDefaultWidget(widget);
    addAction(wdgAction);
}

ProxyModelRohstoff* FilterButtonLager::model() const
{
    return mModel;
}

void FilterButtonLager::setModel(ProxyModelRohstoff* model)
{
    mModel = model;
    mRadioButtonAlle->setChecked(model->filter() == ProxyModelRohstoff::Alle);
    mRadioButtonVorhanden->setChecked(model->filter() == ProxyModelRohstoff::Vorhanden);
    mRadioButtonNichtVorhanden->setChecked(model->filter() == ProxyModelRohstoff::NichtVorhanden);
    mRadioButtonInGebrauch->setChecked(model->filter() == ProxyModelRohstoff::InGebrauch);
    updateChecked();
}

void FilterButtonLager::updateFilter()
{
    if (mRadioButtonAlle->isChecked())
        mModel->setFilter(ProxyModelRohstoff::Alle);
    else if (mRadioButtonVorhanden->isChecked())
        mModel->setFilter(ProxyModelRohstoff::Vorhanden);
    else if (mRadioButtonNichtVorhanden->isChecked())
        mModel->setFilter(ProxyModelRohstoff::NichtVorhanden);
    else if (mRadioButtonInGebrauch->isChecked())
        mModel->setFilter(ProxyModelRohstoff::InGebrauch);
    updateChecked();
}

void FilterButtonLager::updateChecked()
{
    setChecked(mModel->filter() != ProxyModelRohstoff::Alle);
}
