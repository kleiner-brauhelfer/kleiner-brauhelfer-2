#include "tabbewertung.h"
#include "ui_tabbewertung.h"
#include <QScrollBar>
#include "brauhelfer.h"
#include "settings.h"
#include "widgets/wdgbewertung.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBewertung::TabBewertung(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabBewertung),
    mIndex(-1)
{
    ui->setupUi(this);
    connect(bh, &Brauhelfer::discarded, this, &TabBewertung::updateValues);
    connect(bh->sud(), &SudObject::loadedChanged, this, &TabBewertung::sudLoaded);
    connect(bh->sud(), &SudObject::dataChanged, this, &TabBewertung::sudDataChanged);
    connect(bh->sud()->modelBewertungen(), &ProxyModel::layoutChanged, this, &TabBewertung::modelModified);
    connect(bh->sud()->modelBewertungen(), &ProxyModel::rowsInserted, this, &TabBewertung::modelModified);
    connect(bh->sud()->modelBewertungen(), &ProxyModel::rowsRemoved, this, &TabBewertung::modelModified);
    connect(bh->sud()->modelBewertungen(), &ProxyModel::modified, this, &TabBewertung::updateValues);
    connect(ui->wdgRating, &WdgRating::clicked, this, &TabBewertung::sterneChanged);
    connect(ui->wdgBemerkung, &WdgBemerkung::changed, this, [this](const QString& html){setData(ModelBewertungen::ColBemerkung, html);});
}

TabBewertung::~TabBewertung()
{
    delete ui;
}

void TabBewertung::sudLoaded()
{
    mIndex = -1;
    if (bh->sud()->modelBewertungen()->rowCount() > 0)
        clicked(0);
    else
       updateValues();
}

void TabBewertung::sudDataChanged(const QModelIndex& index)
{
    if (index.column() == ModelSud::ColStatus)
    {
        updateValues();
    }
}

void TabBewertung::modelModified()
{
    const int nModel = bh->sud()->modelBewertungen()->rowCount();
    int nLayout = ui->layoutBewertungen->count();
    while (nLayout < nModel)
    {
        WdgBewertung *wdg = new WdgBewertung(nLayout++);
        connect(wdg, &WdgBewertung::clicked, this, &TabBewertung::clicked);
        ui->layoutBewertungen->addWidget(wdg);
    }
    while (ui->layoutBewertungen->count() != nModel)
        delete ui->layoutBewertungen->itemAt(ui->layoutBewertungen->count() - 1)->widget();
    for (int i = 0; i < ui->layoutBewertungen->count(); ++i)
        static_cast<WdgBewertung*>(ui->layoutBewertungen->itemAt(i)->widget())->updateValues();
    updateValues();
}

void TabBewertung::clicked(int index)
{
    mIndex = index;
    for (int i = 0; i < ui->layoutBewertungen->count(); ++i)
    {
        WdgBewertung *wdg = static_cast<WdgBewertung*>(ui->layoutBewertungen->itemAt(i)->widget());
        wdg->setActive(i == mIndex);
        wdg->updateValues();
    }
    updateValues();
}

void TabBewertung::on_btnNeueBewertung_clicked()
{
    QMap<int, QVariant> values({{ModelBewertungen::ColSudID, bh->sud()->id()}});
    int index = bh->sud()->modelBewertungen()->append(values);
    ui->scrollAreaBewertungen->verticalScrollBar()->setValue(ui->scrollAreaBewertungen->verticalScrollBar()->maximum());
    clicked(index);
}

QVariant TabBewertung::data(int col) const
{
    return bh->sud()->modelBewertungen()->data(mIndex, col);
}

bool TabBewertung::setData(int col, const QVariant &value)
{
    return bh->sud()->modelBewertungen()->setData(mIndex, col, value);
}

void TabBewertung::updateValues()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    ui->btnNeueBewertung->setVisible(status == Brauhelfer::SudStatus::Abgefuellt);
    ui->lineNeueBewertung->setVisible(status == Brauhelfer::SudStatus::Abgefuellt);

    ui->scrollAreaWidgetContents->setEnabled(mIndex >= 0 && mIndex < bh->sud()->modelBewertungen()->rowCount());

    ui->wdgRating->setSterne(data(ModelBewertungen::ColSterne).toInt());
    ui->tbDatum->setMinimumDate(bh->sud()->getAbfuelldatum().date());
    ui->tbDatum->setMaximumDate(QDate::currentDate());
    if (!ui->tbDatum->hasFocus())
        ui->tbDatum->setDate(data(ModelBewertungen::ColDatum).toDate());
    ui->wdgBemerkung->setHtml(data(ModelBewertungen::ColBemerkung).toString());

    int value = data(ModelBewertungen::ColFarbe).toInt();
    ui->rbFarbe_ka->setChecked(true);
    ui->rbFarbe_ka2->setChecked(true);
    ui->rbFarbe_0->setChecked(value & (1 << 0));
    ui->rbFarbe_1->setChecked(value & (1 << 1));
    ui->rbFarbe_2->setChecked(value & (1 << 2));
    ui->rbFarbe_3->setChecked(value & (1 << 3));
    ui->rbFarbe_4->setChecked(value & (1 << 4));
    ui->rbFarbe_5->setChecked(value & (1 << 5));
    ui->rbFarbe_6->setChecked(value & (1 << 6));
    ui->rbFarbe_7->setChecked(value & (1 << 7));
    ui->rbFarbe_8->setChecked(value & (1 << 8));
    ui->rbFarbe_9->setChecked(value & (1 << 9));
    ui->rbFarbe_10->setChecked(value & (1 << 10));
    if (!ui->tbFarbe->hasFocus())
        ui->tbFarbe->setPlainText(data(ModelBewertungen::ColFarbeBemerkung).toString());

    value = data(ModelBewertungen::ColSchaum).toInt();
    ui->rbSchaum_ka->setChecked(true);
    ui->rbSchaum_ka2->setChecked(true);
    ui->rbSchaum_ka3->setChecked(true);
    ui->rbSchaum_ka4->setChecked(true);
    ui->rbSchaum_0->setChecked(value & (1 << 0));
    ui->rbSchaum_1->setChecked(value & (1 << 1));
    ui->rbSchaum_2->setChecked(value & (1 << 2));
    ui->rbSchaum_3->setChecked(value & (1 << 3));
    ui->rbSchaum_4->setChecked(value & (1 << 4));
    ui->rbSchaum_5->setChecked(value & (1 << 5));
    ui->rbSchaum_6->setChecked(value & (1 << 6));
    ui->rbSchaum_7->setChecked(value & (1 << 7));
    ui->rbSchaum_8->setChecked(value & (1 << 8));
    ui->rbSchaum_9->setChecked(value & (1 << 9));
    ui->rbSchaum_10->setChecked(value & (1 << 10));
    if (!ui->tbSchaum->hasFocus())
        ui->tbSchaum->setPlainText(data(ModelBewertungen::ColSchaumBemerkung).toString());

    value = data(ModelBewertungen::ColGeruch).toInt();
    ui->rbGeruch_0->setChecked(value & (1 << 0));
    ui->rbGeruch_1->setChecked(value & (1 << 1));
    ui->rbGeruch_2->setChecked(value & (1 << 2));
    ui->rbGeruch_3->setChecked(value & (1 << 3));
    ui->rbGeruch_4->setChecked(value & (1 << 4));
    ui->rbGeruch_5->setChecked(value & (1 << 5));
    ui->rbGeruch_6->setChecked(value & (1 << 6));
    ui->rbGeruch_7->setChecked(value & (1 << 7));
    ui->rbGeruch_8->setChecked(value & (1 << 8));
    ui->rbGeruch_9->setChecked(value & (1 << 9));
    ui->rbGeruch_10->setChecked(value & (1 << 10));
    ui->rbGeruch_11->setChecked(value & (1 << 11));
    if (!ui->tbGeruch->hasFocus())
        ui->tbGeruch->setPlainText(data(ModelBewertungen::ColGeruchBemerkung).toString());

    value = data(ModelBewertungen::ColGeschmack).toInt();
    ui->rbGeschmack_0->setChecked(value & (1 << 0));
    ui->rbGeschmack_1->setChecked(value & (1 << 1));
    ui->rbGeschmack_2->setChecked(value & (1 << 2));
    ui->rbGeschmack_3->setChecked(value & (1 << 3));
    ui->rbGeschmack_4->setChecked(value & (1 << 4));
    ui->rbGeschmack_5->setChecked(value & (1 << 5));
    ui->rbGeschmack_6->setChecked(value & (1 << 6));
    ui->rbGeschmack_7->setChecked(value & (1 << 7));
    ui->rbGeschmack_8->setChecked(value & (1 << 8));
    ui->rbGeschmack_9->setChecked(value & (1 << 9));
    ui->rbGeschmack_10->setChecked(value & (1 << 10));
    ui->rbGeschmack_11->setChecked(value & (1 << 11));
    ui->rbGeschmack_12->setChecked(value & (1 << 12));
    if (!ui->tbGeschmack->hasFocus())
        ui->tbGeschmack->setPlainText(data(ModelBewertungen::ColGeschmackBemerkung).toString());

    value = data(ModelBewertungen::ColAntrunk).toInt();
    ui->rbAntrunk_ka->setChecked(true);
    ui->rbAntrunk_0->setChecked(value & (1 << 0));
    ui->rbAntrunk_1->setChecked(value & (1 << 1));
    ui->rbAntrunk_2->setChecked(value & (1 << 2));
    ui->rbAntrunk_3->setChecked(value & (1 << 3));
    ui->rbAntrunk_4->setChecked(value & (1 << 4));
    ui->rbAntrunk_5->setChecked(value & (1 << 5));
    ui->rbAntrunk_6->setChecked(value & (1 << 6));
    ui->rbAntrunk_7->setChecked(value & (1 << 7));
    if (!ui->tbAntrunk->hasFocus())
        ui->tbAntrunk->setPlainText(data(ModelBewertungen::ColAntrunkBemerkung).toString());

    value = data(ModelBewertungen::ColHaupttrunk).toInt();
    ui->rbHaupttrunk_ka->setChecked(true);
    ui->rbHaupttrunk_0->setChecked(value & (1 << 0));
    ui->rbHaupttrunk_1->setChecked(value & (1 << 1));
    ui->rbHaupttrunk_2->setChecked(value & (1 << 2));
    ui->rbHaupttrunk_3->setChecked(value & (1 << 3));
    ui->rbHaupttrunk_4->setChecked(value & (1 << 4));
    if (!ui->tbHaupttrunk->hasFocus())
        ui->tbHaupttrunk->setPlainText(data(ModelBewertungen::ColHaupttrunkBemerkung).toString());

    value = data(ModelBewertungen::ColNachtrunk).toInt();
    ui->rbNachtrunk_ka->setChecked(true);
    ui->rbNachtrunk_0->setChecked(value & (1 << 0));
    ui->rbNachtrunk_1->setChecked(value & (1 << 1));
    ui->rbNachtrunk_2->setChecked(value & (1 << 2));
    ui->rbNachtrunk_3->setChecked(value & (1 << 3));
    ui->rbNachtrunk_4->setChecked(value & (1 << 4));
    ui->rbNachtrunk_5->setChecked(value & (1 << 5));
    ui->rbNachtrunk_6->setChecked(value & (1 << 6));
    ui->rbNachtrunk_7->setChecked(value & (1 << 7));
    ui->rbNachtrunk_8->setChecked(value & (1 << 8));
    if (!ui->tbNachtrunk->hasFocus())
        ui->tbNachtrunk->setPlainText(data(ModelBewertungen::ColNachtrunkBemerkung).toString());

    value = data(ModelBewertungen::ColGesamteindruck).toInt();
    ui->rbGesamteindruck_ka->setChecked(true);
    ui->rbGesamteindruck_0->setChecked(value & (1 << 0));
    ui->rbGesamteindruck_1->setChecked(value & (1 << 1));
    ui->rbGesamteindruck_2->setChecked(value & (1 << 2));
    ui->rbGesamteindruck_3->setChecked(value & (1 << 3));
    ui->rbGesamteindruck_4->setChecked(value & (1 << 4));
    ui->rbGesamteindruck_5->setChecked(value & (1 << 5));
    ui->rbGesamteindruck_6->setChecked(value & (1 << 6));
    ui->rbGesamteindruck_7->setChecked(value & (1 << 7));
    if (!ui->tbGesamteindruck->hasFocus())
        ui->tbGesamteindruck->setPlainText(data(ModelBewertungen::ColGesamteindruckBemerkung).toString());
}

void TabBewertung::sterneChanged(int sterne)
{
    setData(ModelBewertungen::ColSterne, sterne);
}

void TabBewertung::on_tbDatum_dateChanged(const QDate &date)
{
    if (ui->tbDatum->hasFocus())
        setData(ModelBewertungen::ColDatum, date);
}

void TabBewertung::on_btnDatumHeute_clicked()
{
    setData(ModelBewertungen::ColDatum, QDate::currentDate());
}

void TabBewertung::setFarbe()
{
    int value = 0;
    if (ui->rbFarbe_0->isChecked())
        value += (1 << 0);
    if (ui->rbFarbe_1->isChecked())
        value += (1 << 1);
    if (ui->rbFarbe_2->isChecked())
        value += (1 << 2);
    if (ui->rbFarbe_3->isChecked())
        value += (1 << 3);
    if (ui->rbFarbe_4->isChecked())
        value += (1 << 4);
    if (ui->rbFarbe_5->isChecked())
        value += (1 << 5);
    if (ui->rbFarbe_6->isChecked())
        value += (1 << 6);
    if (ui->rbFarbe_7->isChecked())
        value += (1 << 7);
    if (ui->rbFarbe_8->isChecked())
        value += (1 << 8);
    if (ui->rbFarbe_9->isChecked())
        value += (1 << 9);
    if (ui->rbFarbe_10->isChecked())
        value += (1 << 10);
    setData(ModelBewertungen::ColFarbe, value);
}

void TabBewertung::on_rbFarbe_0_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_1_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_2_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_3_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_4_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_5_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_6_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_7_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_8_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_9_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_10_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_ka_clicked()
{
    setFarbe();
}

void TabBewertung::on_rbFarbe_ka2_clicked()
{
    setFarbe();
}

void TabBewertung::on_tbFarbe_textChanged()
{
    if (ui->tbFarbe->hasFocus())
        setData(ModelBewertungen::ColFarbeBemerkung, ui->tbFarbe->toPlainText());
}

void TabBewertung::setSchaum()
{
    int value = 0;
    if (ui->rbSchaum_0->isChecked())
        value += (1 << 0);
    if (ui->rbSchaum_1->isChecked())
        value += (1 << 1);
    if (ui->rbSchaum_2->isChecked())
        value += (1 << 2);
    if (ui->rbSchaum_3->isChecked())
        value += (1 << 3);
    if (ui->rbSchaum_4->isChecked())
        value += (1 << 4);
    if (ui->rbSchaum_5->isChecked())
        value += (1 << 5);
    if (ui->rbSchaum_6->isChecked())
        value += (1 << 6);
    if (ui->rbSchaum_7->isChecked())
        value += (1 << 7);
    if (ui->rbSchaum_8->isChecked())
        value += (1 << 8);
    if (ui->rbSchaum_9->isChecked())
        value += (1 << 9);
    if (ui->rbSchaum_10->isChecked())
        value += (1 << 10);
    setData(ModelBewertungen::ColSchaum, value);
}

void TabBewertung::on_rbSchaum_0_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_1_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_2_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_3_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_4_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_5_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_6_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_7_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_8_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_9_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_10_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_ka_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_ka2_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_ka3_clicked()
{
    setSchaum();
}

void TabBewertung::on_rbSchaum_ka4_clicked()
{
    setSchaum();
}

void TabBewertung::on_tbSchaum_textChanged()
{
    if (ui->tbSchaum->hasFocus())
        setData(ModelBewertungen::ColSchaumBemerkung, ui->tbSchaum->toPlainText());
}

void TabBewertung::setGeruch()
{
    int value = 0;
    if (ui->rbGeruch_0->isChecked())
        value += (1 << 0);
    if (ui->rbGeruch_1->isChecked())
        value += (1 << 1);
    if (ui->rbGeruch_2->isChecked())
        value += (1 << 2);
    if (ui->rbGeruch_3->isChecked())
        value += (1 << 3);
    if (ui->rbGeruch_4->isChecked())
        value += (1 << 4);
    if (ui->rbGeruch_5->isChecked())
        value += (1 << 5);
    if (ui->rbGeruch_6->isChecked())
        value += (1 << 6);
    if (ui->rbGeruch_7->isChecked())
        value += (1 << 7);
    if (ui->rbGeruch_8->isChecked())
        value += (1 << 8);
    if (ui->rbGeruch_9->isChecked())
        value += (1 << 9);
    if (ui->rbGeruch_10->isChecked())
        value += (1 << 10);
    if (ui->rbGeruch_11->isChecked())
        value += (1 << 11);
    setData(ModelBewertungen::ColGeruch, value);
}

void TabBewertung::on_rbGeruch_0_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_1_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_2_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_3_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_4_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_5_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_6_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_7_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_8_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_9_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_10_clicked()
{
    setGeruch();
}

void TabBewertung::on_rbGeruch_11_clicked()
{
    setGeruch();
}

void TabBewertung::on_tbGeruch_textChanged()
{
    if (ui->tbGeruch->hasFocus())
        setData(ModelBewertungen::ColGeruchBemerkung, ui->tbGeruch->toPlainText());
}

void TabBewertung::setGeschmack()
{
    int value = 0;
    if (ui->rbGeschmack_0->isChecked())
        value += (1 << 0);
    if (ui->rbGeschmack_1->isChecked())
        value += (1 << 1);
    if (ui->rbGeschmack_2->isChecked())
        value += (1 << 2);
    if (ui->rbGeschmack_3->isChecked())
        value += (1 << 3);
    if (ui->rbGeschmack_4->isChecked())
        value += (1 << 4);
    if (ui->rbGeschmack_5->isChecked())
        value += (1 << 5);
    if (ui->rbGeschmack_6->isChecked())
        value += (1 << 6);
    if (ui->rbGeschmack_7->isChecked())
        value += (1 << 7);
    if (ui->rbGeschmack_8->isChecked())
        value += (1 << 8);
    if (ui->rbGeschmack_9->isChecked())
        value += (1 << 9);
    if (ui->rbGeschmack_10->isChecked())
        value += (1 << 10);
    if (ui->rbGeschmack_11->isChecked())
        value += (1 << 11);
    if (ui->rbGeschmack_12->isChecked())
        value += (1 << 12);
    setData(ModelBewertungen::ColGeschmack, value);
}

void TabBewertung::on_rbGeschmack_0_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_1_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_2_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_3_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_4_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_5_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_6_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_7_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_8_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_9_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_10_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_11_clicked()
{
    setGeschmack();
}

void TabBewertung::on_rbGeschmack_12_clicked()
{
    setGeschmack();
}

void TabBewertung::on_tbGeschmack_textChanged()
{
    if (ui->tbGeschmack->hasFocus())
        setData(ModelBewertungen::ColGeschmackBemerkung, ui->tbGeschmack->toPlainText());
}

void TabBewertung::setAntrunk()
{
    int value = 0;
    if (ui->rbAntrunk_0->isChecked())
        value += (1 << 0);
    if (ui->rbAntrunk_1->isChecked())
        value += (1 << 1);
    if (ui->rbAntrunk_2->isChecked())
        value += (1 << 2);
    if (ui->rbAntrunk_3->isChecked())
        value += (1 << 3);
    if (ui->rbAntrunk_4->isChecked())
        value += (1 << 4);
    if (ui->rbAntrunk_5->isChecked())
        value += (1 << 5);
    if (ui->rbAntrunk_6->isChecked())
        value += (1 << 6);
    if (ui->rbAntrunk_7->isChecked())
        value += (1 << 7);
    setData(ModelBewertungen::ColAntrunk, value);
}

void TabBewertung::on_rbAntrunk_0_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_1_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_2_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_3_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_4_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_5_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_6_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_7_clicked()
{
    setAntrunk();
}

void TabBewertung::on_rbAntrunk_ka_clicked()
{
    setAntrunk();
}

void TabBewertung::on_tbAntrunk_textChanged()
{
    if (ui->tbAntrunk->hasFocus())
        setData(ModelBewertungen::ColAntrunkBemerkung, ui->tbAntrunk->toPlainText());
}

void TabBewertung::setHaupttrunk()
{
    int value = 0;
    if (ui->rbHaupttrunk_0->isChecked())
        value += (1 << 0);
    if (ui->rbHaupttrunk_1->isChecked())
        value += (1 << 1);
    if (ui->rbHaupttrunk_2->isChecked())
        value += (1 << 2);
    if (ui->rbHaupttrunk_3->isChecked())
        value += (1 << 3);
    if (ui->rbHaupttrunk_4->isChecked())
        value += (1 << 4);
    setData(ModelBewertungen::ColHaupttrunk, value);
}

void TabBewertung::on_rbHaupttrunk_0_clicked()
{
    setHaupttrunk();
}

void TabBewertung::on_rbHaupttrunk_1_clicked()
{
    setHaupttrunk();
}

void TabBewertung::on_rbHaupttrunk_2_clicked()
{
    setHaupttrunk();
}

void TabBewertung::on_rbHaupttrunk_3_clicked()
{
    setHaupttrunk();
}

void TabBewertung::on_rbHaupttrunk_4_clicked()
{
    setHaupttrunk();
}

void TabBewertung::on_rbHaupttrunk_ka_clicked()
{
    setHaupttrunk();
}

void TabBewertung::on_tbHaupttrunk_textChanged()
{
    if (ui->tbHaupttrunk->hasFocus())
        setData(ModelBewertungen::ColHaupttrunkBemerkung, ui->tbHaupttrunk->toPlainText());
}

void TabBewertung::setNachtrunk()
{
    int value = 0;
    if (ui->rbNachtrunk_0->isChecked())
        value += (1 << 0);
    if (ui->rbNachtrunk_1->isChecked())
        value += (1 << 1);
    if (ui->rbNachtrunk_2->isChecked())
        value += (1 << 2);
    if (ui->rbNachtrunk_3->isChecked())
        value += (1 << 3);
    if (ui->rbNachtrunk_4->isChecked())
        value += (1 << 4);
    if (ui->rbNachtrunk_5->isChecked())
        value += (1 << 5);
    if (ui->rbNachtrunk_6->isChecked())
        value += (1 << 6);
    if (ui->rbNachtrunk_7->isChecked())
        value += (1 << 7);
    if (ui->rbNachtrunk_8->isChecked())
        value += (1 << 8);
    setData(ModelBewertungen::ColNachtrunk, value);
}

void TabBewertung::on_rbNachtrunk_0_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_1_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_2_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_3_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_4_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_5_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_6_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_7_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_8_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_rbNachtrunk_ka_clicked()
{
    setNachtrunk();
}

void TabBewertung::on_tbNachtrunk_textChanged()
{
    if (ui->tbNachtrunk->hasFocus())
        setData(ModelBewertungen::ColNachtrunkBemerkung, ui->tbNachtrunk->toPlainText());
}

void TabBewertung::setGesamteindruck()
{
    int value = 0;
    if (ui->rbGesamteindruck_0->isChecked())
        value += (1 << 0);
    if (ui->rbGesamteindruck_1->isChecked())
        value += (1 << 1);
    if (ui->rbGesamteindruck_2->isChecked())
        value += (1 << 2);
    if (ui->rbGesamteindruck_3->isChecked())
        value += (1 << 3);
    if (ui->rbGesamteindruck_4->isChecked())
        value += (1 << 4);
    if (ui->rbGesamteindruck_5->isChecked())
        value += (1 << 5);
    if (ui->rbGesamteindruck_6->isChecked())
        value += (1 << 6);
    if (ui->rbGesamteindruck_7->isChecked())
        value += (1 << 7);
    setData(ModelBewertungen::ColGesamteindruck, value);
}

void TabBewertung::on_rbGesamteindruck_0_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_1_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_2_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_3_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_4_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_5_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_6_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_7_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_rbGesamteindruck_ka_clicked()
{
    setGesamteindruck();
}

void TabBewertung::on_tbGesamteindruck_textChanged()
{
    if (ui->tbGesamteindruck->hasFocus())
        setData(ModelBewertungen::ColGesamteindruckBemerkung, ui->tbGesamteindruck->toPlainText());
}
