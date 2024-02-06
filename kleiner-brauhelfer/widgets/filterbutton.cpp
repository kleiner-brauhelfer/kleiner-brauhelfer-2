#include "filterbutton.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QWidgetAction>
#include <QMenu>

FilterButton::FilterButton(QWidget *parent) :
    ToolButton(parent)
{
    setCheckable(true);
    setPopupMode(ToolButtonPopupMode::InstantPopup);
}

void FilterButton::init(ProxyModelSud* model)
{
    mModel = model;

    QWidget* widget = new QWidget(this);
    widget->setLayout(new QVBoxLayout(widget));

    mCheckBoxAlle= new CheckBox(tr("Alle"), widget);
    mCheckBoxAlle->setTristate(true);
    if (model->filterStatus() == ProxyModelSud::Alle)
        mCheckBoxAlle->setCheckState(Qt::Checked);
    else if (model->filterStatus() == ProxyModelSud::Keine)
        mCheckBoxAlle->setCheckState(Qt::Unchecked);
    else
        mCheckBoxAlle->setCheckState(Qt::PartiallyChecked);
    widget->layout()->addWidget(mCheckBoxAlle);
    connect(mCheckBoxAlle, &QAbstractButton::clicked, this, &FilterButton::setStatusAlle);

    mCheckBoxRezept = new CheckBox(tr("Rezept"), widget);
    mCheckBoxRezept->setChecked(model->filterStatus() & ProxyModelSud::Rezept);
    widget->layout()->addWidget(mCheckBoxRezept);
    connect(mCheckBoxRezept, &QAbstractButton::clicked, this, &FilterButton::setStatus);

    mCheckBoxGebraut = new CheckBox(tr("Gebraut"), widget);
    mCheckBoxGebraut->setChecked(model->filterStatus() & ProxyModelSud::Gebraut);
    widget->layout()->addWidget(mCheckBoxGebraut);
    connect(mCheckBoxGebraut, &QAbstractButton::clicked, this, &FilterButton::setStatus);

    mCheckBoxAbgefuellt = new CheckBox(tr("Abgefüllt"), widget);
    mCheckBoxAbgefuellt->setChecked(model->filterStatus() & ProxyModelSud::Abgefuellt);
    widget->layout()->addWidget(mCheckBoxAbgefuellt);
    connect(mCheckBoxAbgefuellt, &QAbstractButton::clicked, this, &FilterButton::setStatus);

    mCheckBoxAusgetrunken = new CheckBox(tr("Ausgetrunken"), widget);
    mCheckBoxAusgetrunken->setChecked(model->filterStatus() & ProxyModelSud::Verbraucht);
    widget->layout()->addWidget(mCheckBoxAusgetrunken);
    connect(mCheckBoxAusgetrunken, &QAbstractButton::clicked, this, &FilterButton::setStatus);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);

    widget->layout()->addWidget(line);
    mCheckBoxMerkliste = new CheckBox(tr("Merkliste"), widget);
    mCheckBoxMerkliste->setChecked(model->filterMerkliste());
    widget->layout()->addWidget(mCheckBoxMerkliste);
    connect(mCheckBoxMerkliste, &QAbstractButton::clicked, this, &FilterButton::setMerkliste);

    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    widget->layout()->addWidget(line);

    mCheckBoxDatum = new CheckBox(tr("Gebraut zwischen"), widget);
    mCheckBoxDatum->setChecked(model->filterDate());
    widget->layout()->addWidget(mCheckBoxDatum);
    connect(mCheckBoxDatum, &QAbstractButton::clicked, this, &FilterButton::setDateAlle);

    mDateEditMin = new DateEdit(widget);
    mDateEditMin->setDateTime(model->filterMinimumDate());
    mDateEditMin->setEnabled(mCheckBoxDatum->isChecked());
    widget->layout()->addWidget(mDateEditMin);
    connect(mDateEditMin, &QDateEdit::dateChanged, this, &FilterButton::setMindate);

    widget->layout()->addWidget(new QLabel(tr("und"), widget));

    mDateEditMax = new DateEdit(widget);
    mDateEditMax->setDateTime(model->filterMaximumDate());
    mDateEditMax->setEnabled(mCheckBoxDatum->isChecked());
    widget->layout()->addWidget(mDateEditMax);
    connect(mDateEditMax, &QDateEdit::dateChanged, this, &FilterButton::setMaxdate);

    QWidgetAction* wdgAction = new QWidgetAction(this);
    wdgAction->setDefaultWidget(widget);
    addAction(wdgAction);

    updateChecked();
}

void FilterButton::setStatusAlle(bool value)
{
    if (value)
    {
        mModel->setFilterStatus(ProxyModelSud::Alle);
        mCheckBoxRezept->setChecked(true);
        mCheckBoxGebraut->setChecked(true);
        mCheckBoxAbgefuellt->setChecked(true);
        mCheckBoxAusgetrunken->setChecked(true);
        mCheckBoxAlle->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        mModel->setFilterStatus(ProxyModelSud::Keine);
        mCheckBoxRezept->setChecked(false);
        mCheckBoxGebraut->setChecked(false);
        mCheckBoxAbgefuellt->setChecked(false);
        mCheckBoxAusgetrunken->setChecked(false);
    }
    updateChecked();
}

void FilterButton::setStatus()
{
    ProxyModelSud::FilterStatus status = ProxyModelSud::Keine;
    if (mCheckBoxRezept->isChecked())
        status |= ProxyModelSud::Rezept;
    if (mCheckBoxGebraut->isChecked())
        status |= ProxyModelSud::Gebraut;
    if (mCheckBoxAbgefuellt->isChecked())
        status |= ProxyModelSud::Abgefuellt;
    if (mCheckBoxAusgetrunken->isChecked())
        status |= ProxyModelSud::Verbraucht;
    mModel->setFilterStatus(status);
    if (status == ProxyModelSud::Alle)
        mCheckBoxAlle->setCheckState(Qt::Checked);
    else if (status == ProxyModelSud::Keine)
        mCheckBoxAlle->setCheckState(Qt::Unchecked);
    else
        mCheckBoxAlle->setCheckState(Qt::PartiallyChecked);
    updateChecked();
}

void FilterButton::setMerkliste(bool value)
{
    mModel->setFilterMerkliste(value);
    updateChecked();
}

void FilterButton::setDateAlle(bool value)
{
    mModel->setFilterDate(value);
    mDateEditMin->setEnabled(value);
    mDateEditMax->setEnabled(value);
    updateChecked();
}

void FilterButton::setMindate(const QDate& date)
{
    mModel->setFilterMinimumDate(QDateTime(date, QTime(1,0,0)));
    updateChecked();
}

void FilterButton::setMaxdate(const QDate& date)
{
    mModel->setFilterMaximumDate(QDateTime(date, QTime(23,59,59)));
    updateChecked();
}

void FilterButton::updateChecked()
{
    bool checked = false;
    if (mModel->filterStatus() != ProxyModelSud::Alle)
        checked = true;
    if (mModel->filterMerkliste())
        checked = true;
    if (mModel->filterDate())
        checked = true;
    setChecked(checked);
}
