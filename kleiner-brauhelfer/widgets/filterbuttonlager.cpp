#include "filterbuttonlager.h"
#include "radiobutton.h"
#include "proxymodelrohstoff.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QWidgetAction>
#include <QMenu>

FilterButtonLager::FilterButtonLager(QWidget *parent) :
    ToolButton(parent)
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

    mRadioButtonInGebrauch = new RadioButton(tr("Nicht gebraute Rezepte"), widget);
    widget->layout()->addWidget(mRadioButtonInGebrauch);
    connect(mRadioButtonInGebrauch, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    QWidgetAction* wdgAction = new QWidgetAction(this);
    wdgAction->setDefaultWidget(widget);
    addAction(wdgAction);
}

void FilterButtonLager::setModel(ProxyModelRohstoff* model)
{
    mModel = model;
    mRadioButtonAlle->setChecked(model->filter() == ProxyModelRohstoff::Alle);
    mRadioButtonVorhanden->setChecked(model->filter() == ProxyModelRohstoff::Vorhanden);
    mRadioButtonInGebrauch->setChecked(model->filter() == ProxyModelRohstoff::InGebrauch);
    updateChecked();
}

void FilterButtonLager::updateFilter()
{
    if (mRadioButtonAlle->isChecked())
        mModel->setFilter(ProxyModelRohstoff::Alle);
    else if (mRadioButtonVorhanden->isChecked())
        mModel->setFilter(ProxyModelRohstoff::Vorhanden);
    else if (mRadioButtonInGebrauch->isChecked())
        mModel->setFilter(ProxyModelRohstoff::InGebrauch);
    updateChecked();
}

void FilterButtonLager::updateChecked()
{
    setChecked(mModel->filter() != ProxyModelRohstoff::Alle);
}
