#include "filterbuttonlager.h"
#include "proxymodelrohstoff.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QWidgetAction>
#include <QMenu>
#include <QRadioButton>
#include "settings.h"

extern Settings* gSettings;

FilterButtonLager::FilterButtonLager(QWidget *parent) :
    QToolButton(parent),
    mModel(nullptr),
    mRadioButtonAlle(nullptr),
    mRadioButtonVorhanden(nullptr),
    mRadioButtonNichtVorhanden(nullptr),
    mRadioButtonInGebrauch(nullptr)
{
    setCheckable(true);
    setPopupMode(ToolButtonPopupMode::InstantPopup);

    QWidget* widget = new QWidget(this);
    widget->setLayout(new QVBoxLayout(widget));

    mRadioButtonAlle = new QRadioButton(tr("Alle"), widget);
    widget->layout()->addWidget(mRadioButtonAlle);
    connect(mRadioButtonAlle, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    mRadioButtonVorhanden = new QRadioButton(tr("Vorhanden"), widget);
    widget->layout()->addWidget(mRadioButtonVorhanden);
    connect(mRadioButtonVorhanden, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    mRadioButtonNichtVorhanden = new QRadioButton(tr("Nicht vorhanden"), widget);
    widget->layout()->addWidget(mRadioButtonNichtVorhanden);
    connect(mRadioButtonNichtVorhanden, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    mRadioButtonInGebrauch = new QRadioButton(tr("Nicht gebraute Rezepte"), widget);
    widget->layout()->addWidget(mRadioButtonInGebrauch);
    connect(mRadioButtonInGebrauch, &QAbstractButton::clicked, this, &FilterButtonLager::updateFilter);

    QWidgetAction* wdgAction = new QWidgetAction(this);
    wdgAction->setDefaultWidget(widget);
    addAction(wdgAction);

    connect(gSettings, &Settings::modulesChanged, this, &FilterButtonLager::modulesChanged);
    modulesChanged(Settings::ModuleLagerverwaltung);
}

void FilterButtonLager::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        mRadioButtonVorhanden->setVisible(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
        mRadioButtonNichtVorhanden->setVisible(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    }
}

ProxyModelRohstoff* FilterButtonLager::model() const
{
    return mModel;
}

void FilterButtonLager::setModel(ProxyModelRohstoff* model)
{
    mModel = model;
    connect(mModel, &ProxyModelRohstoff::filterChanged, this, &FilterButtonLager::filterChanged);
    filterChanged();
}

void FilterButtonLager::clear()
{
    mModel->clearFilter();
}

void FilterButtonLager::filterChanged()
{
    ProxyModelRohstoff::Filter filter = mModel ? mModel->filter() : ProxyModelRohstoff::Alle;
    mRadioButtonAlle->setChecked(filter == ProxyModelRohstoff::Alle);
    mRadioButtonVorhanden->setChecked(filter == ProxyModelRohstoff::Vorhanden);
    mRadioButtonNichtVorhanden->setChecked(filter == ProxyModelRohstoff::NichtVorhanden);
    mRadioButtonInGebrauch->setChecked(filter == ProxyModelRohstoff::InGebrauch);
    updateChecked();
}

void FilterButtonLager::updateFilter()
{
    if (!mModel)
        return;
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
    if (!mModel)
        return;
    setChecked(mModel->filter() != ProxyModelRohstoff::Alle);
}
