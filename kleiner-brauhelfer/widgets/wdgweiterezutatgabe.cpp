#include "wdgweiterezutatgabe.h"
#include "ui_wdgweiterezutatgabe.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgWeitereZutatGabe::WdgWeitereZutatGabe(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgWeitereZutatGabe),
    mIndex(index),
    mEnabled(true)
{
    ui->setupUi(this);

    ui->tbMenge->setErrorOnLimit(true);
    ui->tbMengeTotal->setErrorOnLimit(true);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(modified()), this, SLOT(updateValues()));
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

QVariant WdgWeitereZutatGabe::data(int col) const
{
    return bh->sud()->modelWeitereZutatenGaben()->data(mIndex, col);
}

bool WdgWeitereZutatGabe::setData(int col, const QVariant &value)
{
    return bh->sud()->modelWeitereZutatenGaben()->setData(mIndex, col, value);
}

QString WdgWeitereZutatGabe::name() const
{
    return data(ModelWeitereZutatenGaben::ColName).toString();
}

void WdgWeitereZutatGabe::checkEnabled(bool force)
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept;
    if (data(ModelWeitereZutatenGaben::ColZeitpunkt).toInt() == EWZ_Zeitpunkt_Gaerung)
        enabled = status < Brauhelfer::SudStatus::Abgefuellt;
    if (gSettings->ForceEnabled)
        enabled = true;
    if (enabled == mEnabled && !force)
        return;

    int typ = data(ModelWeitereZutatenGaben::ColTyp).toInt();

    if (typ == EWZ_Typ_Hopfen)
    {
        ui->frameColor->setToolTip(tr("<strong>Hopfentyp<br>"
                                      "<font color=\"%1\">Aroma</font><br>"
                                      "<font color=\"%2\">Bitter</font><br>"
                                      "<font color=\"%3\">Universal</font></strong>")
                .arg(gSettings->HopfenTypBackgrounds[1].name())
                .arg(gSettings->HopfenTypBackgrounds[2].name())
                .arg(gSettings->HopfenTypBackgrounds[3].name()));
    }
    else
    {
        ui->frameColor->setToolTip(tr("<strong>Zutattyp<br>"
                                  "<font color=\"%1\">Honig</font><br>"
                                  "<font color=\"%2\">Zucker</font><br>"
                                  "<font color=\"%3\">Gewürz</font><br>"
                                  "<font color=\"%4\">Frucht</font><br>"
                                  "<font color=\"%5\">Sonstiges</font></strong>")
            .arg(gSettings->WZTypBackgrounds[0].name())
            .arg(gSettings->WZTypBackgrounds[1].name())
            .arg(gSettings->WZTypBackgrounds[2].name())
            .arg(gSettings->WZTypBackgrounds[3].name())
            .arg(gSettings->WZTypBackgrounds[4].name()));
    }

    mEnabled = enabled;
    if (mEnabled)
    {
        ui->btnZutat->setEnabled(true);
        ui->btnLoeschen->setVisible(true);
        ui->tbVorhanden->setVisible(true);
        ui->btnAufbrauchen->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->lblEinheit2->setVisible(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbMengeTotal->setReadOnly(false);
        ui->tbDauerMin->setReadOnly(false);
        ui->tbZugabeNach->setReadOnly(false);
        ui->tbDatumVon->setReadOnly(false);
        ui->tbDauerTage->setReadOnly(false);
        ui->tbDatumBis->setReadOnly(false);
    }
    else
    {
        ui->btnZutat->setEnabled(false);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->btnAufbrauchen->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->lblEinheit2->setVisible(false);
        ui->cbZugabezeitpunkt->setEnabled(false);
        ui->cbEntnahme->setEnabled(false);
        ui->wdgKochdauer->setVisible(false);
        ui->tbMenge->setReadOnly(true);
        ui->tbMengeTotal->setReadOnly(true);
        ui->tbDauerMin->setReadOnly(true);
        ui->tbZugabeNach->setReadOnly(true);
        ui->tbDatumVon->setReadOnly(true);
        ui->tbDauerTage->setReadOnly(true);
        ui->tbDatumBis->setReadOnly(true);
    }
    ui->cbEntnahme->setVisible(typ != EWZ_Typ_Hopfen);
}

void WdgWeitereZutatGabe::updateValues(bool full)
{
    QString zusatzname = name();
    int typ = data(ModelWeitereZutatenGaben::ColTyp).toInt();
    int zeitpunkt = data(ModelWeitereZutatenGaben::ColZeitpunkt).toInt();
    int entnahme = data(ModelWeitereZutatenGaben::ColEntnahmeindex).toInt();
    int einheit = data(ModelWeitereZutatenGaben::ColEinheit).toInt();
    int zugabestatus = data(ModelWeitereZutatenGaben::ColZugabestatus).toInt();
    int dauer = data(ModelWeitereZutatenGaben::ColZugabedauer).toInt();

    checkEnabled(full);

    ui->btnZutat->setText(zusatzname);
    if (!ui->tbMenge->hasFocus())
    {
        if (einheit == EWZ_Einheit_mg)
            ui->tbMenge->setValue(data(ModelWeitereZutatenGaben::ColMenge).toDouble() * 1000);
        else
            ui->tbMenge->setValue(data(ModelWeitereZutatenGaben::ColMenge).toDouble());
    }
    if (!ui->tbMengeTotal->hasFocus())
    {
        switch (einheit)
        {
        case EWZ_Einheit_Kg:
            ui->lblEinheit->setText(tr("kg"));
            ui->lblEinheit2->setText(tr("kg"));
            ui->lblEinheitProLiter->setText(tr("g/l"));
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000);
            break;
        case EWZ_Einheit_g:
            ui->lblEinheit->setText(tr("g"));
            ui->lblEinheit2->setText(tr("g"));
            ui->lblEinheitProLiter->setText(tr("g/l"));
            ui->tbMengeTotal->setDecimals(1);
            ui->tbVorhanden->setDecimals(1);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
            break;
        case EWZ_Einheit_mg:
            ui->lblEinheit->setText(tr("mg"));
            ui->lblEinheit2->setText(tr("mg"));
            ui->lblEinheitProLiter->setText(tr("mg/l"));
            ui->tbMengeTotal->setDecimals(0);
            ui->tbVorhanden->setDecimals(0);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000);
            break;
        case EWZ_Einheit_Stk:
            ui->lblEinheit->setText(tr("Stk."));
            ui->lblEinheit2->setText(tr("Stk."));
            ui->lblEinheitProLiter->setText(tr("Stk./l"));
            ui->tbMengeTotal->setDecimals(1);
            ui->tbVorhanden->setDecimals(0);
            ui->tbMengeTotal->setValue(data(ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
            break;
        }
    }
    if (!ui->cbZugabezeitpunkt->hasFocus())
        ui->cbZugabezeitpunkt->setCurrentIndex(zeitpunkt);
    if (!ui->tbDauerMin->hasFocus())
    {
        ui->tbDauerMin->setMinimum(-bh->sud()->getNachisomerisierungszeit());
        ui->tbDauerMin->setMaximum(bh->sud()->getKochdauerNachBitterhopfung());
        ui->tbDauerMin->setValue(dauer);
    }
    ui->cbEntnahme->setChecked(entnahme == EWZ_Entnahmeindex_KeineEntnahme);
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

    if (typ == EWZ_Typ_Hopfen)
    {
        int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, zusatzname, ModelHopfen::ColTyp).toInt();
        if (idx >= 0 && idx < gSettings->HopfenTypBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Background, gSettings->HopfenTypBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
        }
    }
    else
    {
        if (typ >= 0 && typ < gSettings->WZTypBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Background, gSettings->WZTypBackgrounds[typ]);
            ui->frameColor->setPalette(pal);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
        }
    }

    if (mEnabled)
    {
        if (typ == EWZ_Typ_Hopfen)
            ui->tbVorhanden->setValue(bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, zusatzname, ModelHopfen::ColMenge).toInt());
        else
            ui->tbVorhanden->setValue(bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColBeschreibung, zusatzname, ModelWeitereZutaten::ColMenge).toDouble());
        double benoetigt = 0.0;
        ProxyModel* model = bh->sud()->modelWeitereZutatenGaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelWeitereZutatenGaben::ColName).toString() == zusatzname)
                benoetigt += model->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
        }
        if (typ == EWZ_Typ_Hopfen)
        {
            model = bh->sud()->modelHopfengaben();
            for (int i = 0; i < model->rowCount(); ++i)
            {
                if (model->data(i, ModelHopfengaben::ColName).toString() == zusatzname)
                    benoetigt += model->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
            }
        }
        if (einheit == EWZ_Einheit_Kg)
            benoetigt /= 1000;
        else if (einheit == EWZ_Einheit_mg)
            benoetigt *= 1000;
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);

        ui->btnEntnehmen->setPalette(gSettings->palette);
        switch (zugabestatus)
        {
        case EWZ_Zugabestatus_nichtZugegeben:
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
            ui->btnZutat->setEnabled(true);
            break;
        case EWZ_Zugabestatus_Zugegeben:
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
            ui->btnZutat->setEnabled(false);
            if (zeitpunkt == EWZ_Zeitpunkt_Gaerung)
            {
                QDate currentDate = QDate::currentDate();
                QDate dateBisSoll = data(ModelWeitereZutatenGaben::ColZugabeDatum).toDate().addDays(dauer / 1440);
                if (currentDate >= dateBisSoll)
                {
                    ui->btnEntnehmen->setPalette(gSettings->paletteErrorButton);
                }
            }
            break;
        case EWZ_Zugabestatus_Entnommen:
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
        ui->btnZutat->setEnabled(true);
    }

    switch (zeitpunkt)
    {
    case EWZ_Zeitpunkt_Gaerung:
        ui->wdgKochdauer->setVisible(false);
        break;
    case EWZ_Zeitpunkt_Kochen:
        ui->wdgKochdauer->setVisible(true);
        break;
    case EWZ_Zeitpunkt_Maischen:
        ui->wdgKochdauer->setVisible(false);
        break;
    }
    ui->wdgZugabezeitpunkt->setVisible(typ != EWZ_Typ_Hopfen);
    ui->wdgZugabe->setVisible(zeitpunkt == EWZ_Zeitpunkt_Gaerung);
    ui->lblEntnahme->setVisible(entnahme == EWZ_Entnahmeindex_MitEntnahme);
    ui->tbDauerTage->setVisible(entnahme == EWZ_Entnahmeindex_MitEntnahme);
    ui->lblDauerTage->setVisible(entnahme == EWZ_Entnahmeindex_MitEntnahme);
    ui->tbDatumBis->setVisible(braudatum.isValid() && entnahme == EWZ_Entnahmeindex_MitEntnahme);
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    ui->btnZugeben->setVisible(status == Brauhelfer::SudStatus::Gebraut && zugabestatus == EWZ_Zugabestatus_nichtZugegeben);
    ui->btnEntnehmen->setVisible(status == Brauhelfer::SudStatus::Gebraut && zugabestatus == EWZ_Zugabestatus_Zugegeben && entnahme == EWZ_Entnahmeindex_MitEntnahme);
    ui->cbZugabezeitpunkt->setEnabled(status == Brauhelfer::SudStatus::Rezept || gSettings->ForceEnabled);
}

void WdgWeitereZutatGabe::on_btnZutat_clicked()
{
    if (data(ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen)
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

void WdgWeitereZutatGabe::remove()
{
    bh->sud()->modelWeitereZutatenGaben()->removeRow(mIndex);
}

void WdgWeitereZutatGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
    {
        if (data(ModelWeitereZutatenGaben::ColEinheit).toInt() == EWZ_Einheit_mg)
            setData(ModelWeitereZutatenGaben::ColMenge, value / 1000);
        else
            setData(ModelWeitereZutatenGaben::ColMenge, value);
    }
}

void WdgWeitereZutatGabe::on_tbMengeTotal_valueChanged(double value)
{
    if (ui->tbMengeTotal->hasFocus())
    {
        switch (data(ModelWeitereZutatenGaben::ColEinheit).toInt())
        {
        case EWZ_Einheit_Kg:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
            break;
        case EWZ_Einheit_g:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        case EWZ_Einheit_mg:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value / 1000);
            break;
        case EWZ_Einheit_Stk:
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

void WdgWeitereZutatGabe::on_tbDauerMin_valueChanged(int value)
{
    if (ui->tbDauerMin->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabedauer, value);
}

void WdgWeitereZutatGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatumVon->date();
    setData(ModelWeitereZutatenGaben::ColZugabeDatum, currentDate < date ? currentDate : date);
    setData(ModelWeitereZutatenGaben::ColZugabestatus, EWZ_Zugabestatus_Zugegeben);

    DlgRohstoffeAbziehen dlg(data(ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen ? 1 : 3,
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
    setData(ModelWeitereZutatenGaben::ColZugabestatus, EWZ_Zugabestatus_Entnommen);
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
    switch (data(ModelWeitereZutatenGaben::ColEinheit).toInt())
    {
    case EWZ_Einheit_Kg:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
        break;
    case EWZ_Einheit_g:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    case EWZ_Einheit_mg:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value / 1000);
        break;
    case EWZ_Einheit_Stk:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    }
}
