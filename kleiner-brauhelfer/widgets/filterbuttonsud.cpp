#include "filterbuttonsud.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QWidgetAction>
#include <QMenu>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

FilterButtonSud::FilterButtonSud(QWidget *parent) :
    QToolButton(parent),
    mModel(nullptr),
    mCheckBoxAlle(nullptr),
    mCheckBoxRezept(nullptr),
    mCheckBoxGebraut(nullptr),
    mCheckBoxAbgefuellt(nullptr),
    mCheckBoxAusgetrunken(nullptr),
    mCheckBoxMerkliste(nullptr),
    mComboBoxKategorie(nullptr),
    mCheckBoxDatum(nullptr),
    mDateEditMin(nullptr),
    mDateEditMax(nullptr)
{
    setCheckable(true);
    setPopupMode(ToolButtonPopupMode::InstantPopup);
}

ProxyModelSud* FilterButtonSud::model() const
{
    return mModel;
}

void FilterButtonSud::setModel(ProxyModelSud* model, Items items)
{
    mModel = model;
    connect(mModel, &ProxyModelSud::layoutChanged, this, &FilterButtonSud::updateWidgets);

    QFrame* line;
    QWidget* widget = new QWidget(this);
    QGridLayout* layout = new QGridLayout(widget);

    QWidgetAction* wdgAction = new QWidgetAction(this);
    wdgAction->setDefaultWidget(widget);
    addAction(wdgAction);

    widget->setLayout(layout);

    if (items.testFlag(Item::Status))
    {
        layout->addWidget(new QLabel(tr("Stadium"), widget), layout->rowCount(), 0);
        mCheckBoxAlle= new QCheckBox(tr("Alle"), widget);
        mCheckBoxAlle->setTristate(true);
        layout->addWidget(mCheckBoxAlle, layout->rowCount()-1, 1);
        connect(mCheckBoxAlle, &QAbstractButton::clicked, this, &FilterButtonSud::setStatusAlle);
        mCheckBoxRezept = new QCheckBox(tr("Rezept"), widget);
        layout->addWidget(mCheckBoxRezept, layout->rowCount(), 1);
        connect(mCheckBoxRezept, &QAbstractButton::clicked, this, &FilterButtonSud::setStatus);
        mCheckBoxGebraut = new QCheckBox(tr("Gebraut"), widget);
        layout->addWidget(mCheckBoxGebraut, layout->rowCount(), 1);
        connect(mCheckBoxGebraut, &QAbstractButton::clicked, this, &FilterButtonSud::setStatus);
        mCheckBoxAbgefuellt = new QCheckBox(tr("Abgefüllt"), widget);
        layout->addWidget(mCheckBoxAbgefuellt, layout->rowCount(), 1);
        connect(mCheckBoxAbgefuellt, &QAbstractButton::clicked, this, &FilterButtonSud::setStatus);
        mCheckBoxAusgetrunken = new QCheckBox(tr("Ausgetrunken"), widget);
        layout->addWidget(mCheckBoxAusgetrunken, layout->rowCount(), 1);
        connect(mCheckBoxAusgetrunken, &QAbstractButton::clicked, this, &FilterButtonSud::setStatus);
        line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(0);
        line->setMidLineWidth(1);
        layout->addWidget(line, layout->rowCount(), 0, 1, 2);
    }
    if (items.testFlag(Item::Merkliste))
    {
        layout->addWidget(new QLabel(tr("Merkliste"), widget), layout->rowCount(), 0);
        mCheckBoxMerkliste = new QCheckBox("", widget);
        layout->addWidget(mCheckBoxMerkliste, layout->rowCount()-1, 1);
        connect(mCheckBoxMerkliste, &QAbstractButton::clicked, this, &FilterButtonSud::setMerkliste);
        line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(0);
        line->setMidLineWidth(1);
        layout->addWidget(line, layout->rowCount(), 0, 1, 2);
    }
    if (items.testFlag(Item::Kategorie))
    {
        layout->addWidget(new QLabel(tr("Kategorie"), widget), layout->rowCount(), 0);
        mComboBoxKategorie = new QComboBox(this);
        ProxyModel* proxy = new ProxyModel(this);
        proxy->setSourceModel(bh->modelKategorien());
        mComboBoxKategorie->setModel(proxy);
        mComboBoxKategorie->setModelColumn(ModelKategorien::ColName);
        layout->addWidget(mComboBoxKategorie, layout->rowCount()-1, 1);
        connect(mComboBoxKategorie, &QComboBox::currentTextChanged, this, &FilterButtonSud::setKategorie);
        line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(0);
        line->setMidLineWidth(1);
        layout->addWidget(line, layout->rowCount(), 0, 1, 2);
    }
    if (items.testFlag(Item::Braudatum))
    {
        layout->addWidget(new QLabel(tr("Gebraut"), widget), layout->rowCount(), 0);
        mCheckBoxDatum = new QCheckBox(tr("zwischen"), widget);
        layout->addWidget(mCheckBoxDatum, layout->rowCount()-1, 1);
        connect(mCheckBoxDatum, &QAbstractButton::clicked, this, &FilterButtonSud::setDateAlle);
        mDateEditMin = new DateEdit(widget);
        layout->addWidget(mDateEditMin, layout->rowCount(), 1);
        connect(mDateEditMin, &QDateEdit::dateChanged, this, &FilterButtonSud::setMindate);
        layout->addWidget(new QLabel(tr("und"), widget), layout->rowCount(), 1);
        mDateEditMax = new DateEdit(widget);
        layout->addWidget(mDateEditMax, layout->rowCount(), 1);
        connect(mDateEditMax, &QDateEdit::dateChanged, this, &FilterButtonSud::setMaxdate);
    }
    updateWidgets();
}

void FilterButtonSud::setStatusAlle(bool value)
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

void FilterButtonSud::setStatus()
{
    ProxyModelSud::FilterStatus status = ProxyModelSud::Keine;
    if (mCheckBoxRezept && mCheckBoxRezept->isChecked())
        status |= ProxyModelSud::Rezept;
    if (mCheckBoxGebraut && mCheckBoxGebraut->isChecked())
        status |= ProxyModelSud::Gebraut;
    if (mCheckBoxAbgefuellt && mCheckBoxAbgefuellt->isChecked())
        status |= ProxyModelSud::Abgefuellt;
    if (mCheckBoxAusgetrunken && mCheckBoxAusgetrunken->isChecked())
        status |= ProxyModelSud::Verbraucht;
    mModel->setFilterStatus(status);
    if (mCheckBoxAlle)
    {
        if (status == ProxyModelSud::Alle)
            mCheckBoxAlle->setCheckState(Qt::Checked);
        else if (status == ProxyModelSud::Keine)
            mCheckBoxAlle->setCheckState(Qt::Unchecked);
        else
            mCheckBoxAlle->setCheckState(Qt::PartiallyChecked);
    }
    updateChecked();
}

void FilterButtonSud::setMerkliste(bool value)
{
    mModel->setFilterMerkliste(value);
    updateChecked();
}

void FilterButtonSud::setKategorie(const QString& value)
{
    mModel->setFilterKategorie(value);
    updateChecked();
}

void FilterButtonSud::setDateAlle(bool value)
{
    mModel->setFilterDate(value);
    mDateEditMin->setEnabled(value);
    mDateEditMax->setEnabled(value);
    updateChecked();
}

void FilterButtonSud::setMindate(const QDate& date)
{
    mModel->setFilterMinimumDate(QDateTime(date, QTime(1,0,0)));
    updateChecked();
}

void FilterButtonSud::setMaxdate(const QDate& date)
{
    mModel->setFilterMaximumDate(QDateTime(date, QTime(23,59,59)));
    updateChecked();
}

void FilterButtonSud::updateWidgets()
{
    if (mCheckBoxAlle)
    {
        if (mModel->filterStatus() == ProxyModelSud::Alle)
            mCheckBoxAlle->setCheckState(Qt::Checked);
        else if (mModel->filterStatus() == ProxyModelSud::Keine)
            mCheckBoxAlle->setCheckState(Qt::Unchecked);
        else
            mCheckBoxAlle->setCheckState(Qt::PartiallyChecked);
    }
    if (mCheckBoxRezept)
        mCheckBoxRezept->setChecked(mModel->filterStatus() & ProxyModelSud::Rezept);
    if (mCheckBoxGebraut)
        mCheckBoxGebraut->setChecked(mModel->filterStatus() & ProxyModelSud::Gebraut);
    if (mCheckBoxAbgefuellt)
        mCheckBoxAbgefuellt->setChecked(mModel->filterStatus() & ProxyModelSud::Abgefuellt);
    if (mCheckBoxAusgetrunken)
        mCheckBoxAusgetrunken->setChecked(mModel->filterStatus() & ProxyModelSud::Verbraucht);
    if (mCheckBoxMerkliste)
        mCheckBoxMerkliste->setChecked(mModel->filterMerkliste());
    if (mComboBoxKategorie)
        mComboBoxKategorie->setCurrentText(mModel->filterKategorie());
    if (mCheckBoxDatum)
        mCheckBoxDatum->setChecked(mModel->filterDate());
    if (mDateEditMin)
        mDateEditMin->setDateTime(mModel->filterMinimumDate());
    if (mDateEditMax)
        mDateEditMax->setDateTime(mModel->filterMaximumDate());
    if (mDateEditMax && mCheckBoxDatum)
        mDateEditMax->setEnabled(mCheckBoxDatum->isChecked());
    if (mDateEditMin && mCheckBoxDatum)
        mDateEditMin->setEnabled(mCheckBoxDatum->isChecked());
    updateChecked();
}

void FilterButtonSud::updateChecked()
{
    bool checked = false;
    if (mCheckBoxAlle && mModel->filterStatus() != ProxyModelSud::Alle)
        checked = true;
    if (!checked && mModel->filterMerkliste())
        checked = true;
    if (!checked && !mModel->filterKategorie().isEmpty())
        checked = true;
    if (!checked && mModel->filterDate())
        checked = true;
    setChecked(checked);
}
