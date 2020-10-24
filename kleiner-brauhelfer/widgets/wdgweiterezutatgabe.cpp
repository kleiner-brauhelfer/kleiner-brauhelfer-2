#include "wdgweiterezutatgabe.h"
#include "ui_wdgweiterezutatgabe.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "tabrohstoffe.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgWeitereZutatGabe::WdgWeitereZutatGabe(int row, QLayout* parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelWeitereZutatenGaben(), row, parentLayout, parent),
    ui(new Ui::WdgWeitereZutatGabe),
    mEnabled(true)
{
    ui->setupUi(this);

    ui->tbMenge->setErrorOnLimit(true);
    ui->tbMengeTotal->setErrorOnLimit(true);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(mModel, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgWeitereZutatGabe::~WdgWeitereZutatGabe()
{
    delete ui;
}

bool WdgWeitereZutatGabe::isEnabled() const
{
    return mEnabled;
}

bool WdgWeitereZutatGabe::isValid() const
{
    return mValid;
}

QString WdgWeitereZutatGabe::name() const
{
    return data(ModelWeitereZutatenGaben::ColName).toString();
}

void WdgWeitereZutatGabe::checkEnabled(bool force)
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept;
    Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(data(ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
    if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
        enabled = status < Brauhelfer::SudStatus::Abgefuellt;
    if (gSettings->ForceEnabled)
        enabled = true;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    ui->btnZutat->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbVorhanden->setVisible(mEnabled);
    ui->btnAufbrauchen->setVisible(mEnabled);
    ui->lblVorhanden->setVisible(mEnabled);
    ui->lblEinheit2->setVisible(mEnabled);
    ui->cbZugabezeitpunkt->setEnabled(mEnabled);
    ui->cbEntnahme->setEnabled(mEnabled);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbMengeTotal->setReadOnly(!mEnabled);
    ui->tbExtrakt->setReadOnly(!mEnabled);
    ui->tbKochdauer->setReadOnly(!mEnabled);
    ui->tbZugabeNach->setReadOnly(!mEnabled);
    ui->tbDatumVon->setReadOnly(!mEnabled);
    ui->tbDauerTage->setReadOnly(!mEnabled);
    ui->tbDatumBis->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
    ui->cbEntnahme->setVisible(typ != Brauhelfer::ZusatzTyp::Hopfen);
}

void WdgWeitereZutatGabe::updateValues(bool full)
{
    QString zusatzname = name();
    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
    Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(data(ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
    Brauhelfer::ZusatzEntnahmeindex entnahmeindex = static_cast<Brauhelfer::ZusatzEntnahmeindex>(data(ModelWeitereZutatenGaben::ColEntnahmeindex).toInt());
    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
    Brauhelfer::ZusatzStatus zugabestatus = static_cast<Brauhelfer::ZusatzStatus>(data(ModelWeitereZutatenGaben::ColZugabestatus).toInt());
    int dauer = data(ModelWeitereZutatenGaben::ColZugabedauer).toInt();

    checkEnabled(full);

    int rowRohstoff;
    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
        rowRohstoff = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, zusatzname);
    else
        rowRohstoff = bh->modelWeitereZutaten()->getRowWithValue(ModelWeitereZutaten::ColName, zusatzname);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(zusatzname);
    ui->btnZutat->setPalette(mValid ? palette() : gSettings->paletteErrorButton);
    if (!ui->tbMenge->hasFocus())
    {
        if (einheit == Brauhelfer::Einheit::mg)
            ui->tbMenge->setValue(data(ModelWeitereZutatenGaben::ColMenge).toDouble() * 1000);
        else
            ui->tbMenge->setValue(data(ModelWeitereZutatenGaben::ColMenge).toDouble());
    }
    if (!ui->tbMengeTotal->hasFocus())
    {
        QString str = TabRohstoffe::Einheiten[static_cast<int>(einheit)];
        ui->lblEinheit->setText(str);
        ui->lblEinheit2->setText(str);
        switch (einheit)
        {
        case Brauhelfer::Einheit::Kg:
            ui->lblEinheitProLiter->setText(tr("g/l"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000);
            break;
        case Brauhelfer::Einheit::g:
            ui->lblEinheitProLiter->setText(tr("g/l"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
            break;
        case Brauhelfer::Einheit::mg:
            ui->lblEinheitProLiter->setText(tr("mg/l"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000);
            break;
        case Brauhelfer::Einheit::Stk:
            ui->lblEinheitProLiter->setText(tr("Stk./l"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(0);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
            break;
        case Brauhelfer::Einheit::l:
            ui->lblEinheitProLiter->setText(tr("ml/l"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000);
            break;
        case Brauhelfer::Einheit::ml:
            ui->lblEinheitProLiter->setText(tr("ml/l"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
            break;
        }
    }
    if (!ui->cbZugabezeitpunkt->hasFocus())
        ui->cbZugabezeitpunkt->setCurrentIndex(static_cast<int>(zeitpunkt));
    if (!ui->tbKochdauer->hasFocus())
    {
        ui->tbKochdauer->setMinimum(-bh->sud()->getNachisomerisierungszeit());
        ui->tbKochdauer->setMaximum(bh->sud()->getKochdauer());
        ui->tbKochdauer->setValue(dauer);
    }

    bool visible = data(ModelWeitereZutatenGaben::ColAusbeute).toDouble() > 0;
    if (!ui->tbExtrakt->hasFocus())
        ui->tbExtrakt->setValue(data(ModelWeitereZutatenGaben::ColExtrakt).toDouble());
    ui->tbExtrakt->setVisible(visible);
    ui->lblExtrakt->setVisible(visible);
    ui->lblExtraktEinheit->setVisible(visible);

    ui->cbEntnahme->setChecked(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
    if (!ui->tbZugabeNach->hasFocus())
        ui->tbZugabeNach->setValue(data(ModelWeitereZutatenGaben::ColZugabeNach).toInt());
    if (!ui->tbDauerTage->hasFocus())
        ui->tbDauerTage->setValue(dauer / 1440);

    QDateTime braudatum = bh->sud()->getBraudatum();
    if (braudatum.isValid())
    {
        ui->tbDatumVon->setMinimumDateTime(braudatum);
        if (!ui->tbDatumVon->hasFocus())
            ui->tbDatumVon->setDate(data(ModelWeitereZutatenGaben::ColZugabeDatum).toDate());
        ui->tbDatumBis->setMinimumDateTime(ui->tbDatumVon->dateTime());
        if (!ui->tbDatumBis->hasFocus())
            ui->tbDatumBis->setDate(data(ModelWeitereZutatenGaben::ColEntnahmeDatum).toDate());
    }
    ui->tbDatumVon->setVisible(braudatum.isValid());

    if (!ui->tbKomentar->hasFocus())
        ui->tbKomentar->setText(data(ModelWeitereZutatenGaben::ColBemerkung).toString());

    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
    {
        int idx = bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColTyp).toInt();
        if (idx >= 0 && idx < gSettings->HopfenTypBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Background, gSettings->HopfenTypBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
            ui->frameColor->setToolTip(TabRohstoffe::HopfenTypname[idx]);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
            ui->frameColor->setToolTip("");
        }
    }
    else
    {
        int idx = static_cast<int>(typ);
        if (idx >= 0 && idx < gSettings->WZTypBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Background, gSettings->WZTypBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
            ui->frameColor->setToolTip(TabRohstoffe::ZusatzTypname[idx]);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
            ui->frameColor->setToolTip("");
        }
    }

    if (mEnabled)
    {
        if (typ == Brauhelfer::ZusatzTyp::Hopfen)
            ui->tbVorhanden->setValue(bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColMenge).toInt());
        else
            ui->tbVorhanden->setValue(bh->modelWeitereZutaten()->data(rowRohstoff, ModelWeitereZutaten::ColMenge).toDouble());
        double benoetigt = 0.0;
        for (int i = 0; i < mModel->rowCount(); ++i)
        {
            if (mModel->data(i, ModelWeitereZutatenGaben::ColName).toString() == zusatzname)
                benoetigt += mModel->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
        }
        if (typ == Brauhelfer::ZusatzTyp::Hopfen)
        {
            ProxyModel* model = bh->sud()->modelHopfengaben();
            for (int i = 0; i < model->rowCount(); ++i)
            {
                if (model->data(i, ModelHopfengaben::ColName).toString() == zusatzname)
                    benoetigt += model->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
            }
        }
        if (einheit == Brauhelfer::Einheit::Kg || einheit == Brauhelfer::Einheit::l)
            benoetigt /= 1000;
        else if (einheit == Brauhelfer::Einheit::mg)
            benoetigt *= 1000;
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);

        ui->btnEntnehmen->setPalette(gSettings->palette);
        switch (zugabestatus)
        {
        case Brauhelfer::ZusatzStatus::NichtZugegeben:
            ui->tbVorhanden->setVisible(true);
            ui->lblVorhanden->setVisible(true);
            ui->lblEinheit2->setVisible(true);
            ui->btnLoeschen->setVisible(true);
            ui->tbZugabeNach->setReadOnly(false);
            ui->tbDatumVon->setReadOnly(false);
            ui->tbDatumBis->setReadOnly(false);
            ui->tbDauerTage->setReadOnly(false);
            ui->cbEntnahme->setEnabled(true);
            ui->tbMenge->setReadOnly(false);
            ui->tbMengeTotal->setReadOnly(false);
            ui->btnZutat->setEnabled(true);
            break;
        case Brauhelfer::ZusatzStatus::Zugegeben:
            ui->tbVorhanden->setVisible(false);
            ui->lblVorhanden->setVisible(false);
            ui->lblEinheit2->setVisible(false);
            ui->btnLoeschen->setVisible(false);
            ui->tbZugabeNach->setReadOnly(true);
            ui->tbDatumVon->setReadOnly(true);
            ui->tbDatumBis->setReadOnly(false);
            ui->tbDauerTage->setReadOnly(false);
            ui->cbEntnahme->setEnabled(true);
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeTotal->setReadOnly(true);
            ui->btnZutat->setEnabled(false);
            if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                QDate currentDate = QDate::currentDate();
                QDate dateBisSoll = data(ModelWeitereZutatenGaben::ColZugabeDatum).toDate().addDays(dauer / 1440);
                if (currentDate >= dateBisSoll)
                {
                    ui->btnEntnehmen->setPalette(gSettings->paletteErrorButton);
                }
            }
            break;
        case Brauhelfer::ZusatzStatus::Entnommen:
            ui->tbVorhanden->setVisible(false);
            ui->lblVorhanden->setVisible(false);
            ui->lblEinheit2->setVisible(false);
            ui->btnLoeschen->setVisible(false);
            ui->tbZugabeNach->setReadOnly(true);
            ui->tbDatumVon->setReadOnly(true);
            ui->tbDatumBis->setReadOnly(true);
            ui->tbDauerTage->setReadOnly(true);
            ui->cbEntnahme->setEnabled(false);
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeTotal->setReadOnly(true);
            ui->btnZutat->setEnabled(false);
            break;
        }
        ui->btnAufbrauchen->setVisible(qAbs(ui->tbVorhanden->value() - ui->tbMengeTotal->value()) > 0.001);
    }

    if (gSettings->ForceEnabled)
    {
        ui->tbVorhanden->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->lblEinheit2->setVisible(true);
        ui->btnLoeschen->setVisible(true);
        ui->tbZugabeNach->setReadOnly(false);
        ui->tbDatumVon->setReadOnly(false);
        ui->tbDatumBis->setReadOnly(false);
        ui->tbDauerTage->setReadOnly(false);
        ui->cbEntnahme->setEnabled(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbMengeTotal->setReadOnly(false);
        ui->btnZutat->setEnabled(true);
    }

    ui->tbKochdauer->setVisible(zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Kochen);
    ui->lblKochdauer->setVisible(zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Kochen);
    ui->lblKochdauerEinheit->setVisible(zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Kochen);
    ui->wdgZugabezeitpunkt->setVisible(typ != Brauhelfer::ZusatzTyp::Hopfen);
    ui->wdgZugabe->setVisible(zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung);
    ui->lblEntnahme->setVisible(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->tbDauerTage->setVisible(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->lblDauerTage->setVisible(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->tbDatumBis->setVisible(braudatum.isValid() && entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    ui->btnZugeben->setVisible(status == Brauhelfer::SudStatus::Gebraut && zugabestatus == Brauhelfer::ZusatzStatus::NichtZugegeben);
    ui->btnEntnehmen->setVisible(status == Brauhelfer::SudStatus::Gebraut && zugabestatus == Brauhelfer::ZusatzStatus::Zugegeben && entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->cbZugabezeitpunkt->setEnabled(status == Brauhelfer::SudStatus::Rezept || gSettings->ForceEnabled);

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
}

void WdgWeitereZutatGabe::on_btnZutat_clicked()
{
    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
    {
        DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
        dlg.select(name());
        if (dlg.exec() == QDialog::Accepted)
            setData(ModelHopfengaben::ColName, dlg.name());
    }
    else
    {
        DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
        dlg.select(name());
        if (dlg.exec() == QDialog::Accepted)
            setData(ModelWeitereZutatenGaben::ColName, dlg.name());
    }
}

void WdgWeitereZutatGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
    {
        Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
        if (einheit == Brauhelfer::Einheit::mg)
            setData(ModelWeitereZutatenGaben::ColMenge, value / 1000);
        else
            setData(ModelWeitereZutatenGaben::ColMenge, value);
    }
}

void WdgWeitereZutatGabe::on_tbMengeTotal_valueChanged(double value)
{
    if (ui->tbMengeTotal->hasFocus())
    {
        Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
        switch (einheit)
        {
        case Brauhelfer::Einheit::Kg:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
            break;
        case Brauhelfer::Einheit::g:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        case Brauhelfer::Einheit::mg:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value / 1000);
            break;
        case Brauhelfer::Einheit::Stk:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        case Brauhelfer::Einheit::l:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
            break;
        case Brauhelfer::Einheit::ml:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        }
    }
}

void WdgWeitereZutatGabe::on_cbZugabezeitpunkt_currentIndexChanged(int index)
{
    if (ui->cbZugabezeitpunkt->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZeitpunkt, index);
}

void WdgWeitereZutatGabe::on_tbKochdauer_valueChanged(int value)
{
    if (ui->tbKochdauer->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabedauer, value);
}

void WdgWeitereZutatGabe::on_tbExtrakt_valueChanged(double value)
{
    if (ui->tbExtrakt->hasFocus())
        setData(ModelWeitereZutatenGaben::ColExtrakt, value);
}

void WdgWeitereZutatGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatumVon->date();
    setData(ModelWeitereZutatenGaben::ColZugabeDatum, currentDate < date ? currentDate : date);
    setData(ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Zugegeben));

    Brauhelfer::ZusatzTyp zusatztyp = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
    Brauhelfer::RohstoffTyp typ = zusatztyp == Brauhelfer::ZusatzTyp::Hopfen ? Brauhelfer::RohstoffTyp::Hopfen : Brauhelfer::RohstoffTyp::Zusatz;
    DlgRohstoffeAbziehen dlg(typ,
                             data(ModelWeitereZutatenGaben::ColName).toString(),
                             data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble(),
                             this);
    dlg.exec();
}

void WdgWeitereZutatGabe::on_cbEntnahme_clicked(bool checked)
{
    setData(ModelWeitereZutatenGaben::ColEntnahmeindex, checked);
}

void WdgWeitereZutatGabe::on_tbZugabeNach_valueChanged(int value)
{
    if (ui->tbZugabeNach->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabeNach, value);
}

void WdgWeitereZutatGabe::on_tbDauerTage_valueChanged(int value)
{
    if (ui->tbDauerTage->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabedauer, value * 1440);
}

void WdgWeitereZutatGabe::on_tbDatumVon_dateChanged(const QDate &date)
{
    if (ui->tbDatumVon->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabeDatum, date);
}

void WdgWeitereZutatGabe::on_tbDatumBis_dateChanged(const QDate &date)
{
    if (ui->tbDatumBis->hasFocus())
        setData(ModelWeitereZutatenGaben::ColEntnahmeDatum, date);
}

void WdgWeitereZutatGabe::on_btnEntnehmen_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatumBis->date();
    setData(ModelWeitereZutatenGaben::ColEntnahmeDatum, currentDate < date ? currentDate : date);
    setData(ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Entnommen));
}

void WdgWeitereZutatGabe::on_tbKomentar_textChanged()
{
    if (ui->tbKomentar->hasFocus())
        setData(ModelWeitereZutatenGaben::ColBemerkung, ui->tbKomentar->toPlainText());
}

void WdgWeitereZutatGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgWeitereZutatGabe::on_btnAufbrauchen_clicked()
{
    double value = ui->tbVorhanden->value();
    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
    switch (einheit)
    {
    case Brauhelfer::Einheit::Kg:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
        break;
    case Brauhelfer::Einheit::g:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    case Brauhelfer::Einheit::mg:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value / 1000);
        break;
    case Brauhelfer::Einheit::Stk:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    case Brauhelfer::Einheit::l:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
        break;
    case Brauhelfer::Einheit::ml:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    }
}

void WdgWeitereZutatGabe::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgWeitereZutatGabe::on_btnNachUnten_clicked()
{
    moveDown();
}
