#include "wdghopfengabe.h"
#include "ui_wdghopfengabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "dialogs/dlgrohstoffauswahl.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgHopfenGabe::WdgHopfenGabe(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgHopfenGabe),
    mIndex(index),
    mEnabled(true)
{
    ui->setupUi(this);

    ui->frameColor->setToolTip(tr("<strong>Hopfentyp<br>"
                                  "<font color=\"%1\">Aroma</font><br>"
                                  "<font color=\"%2\">Bitter</font><br>"
                                  "<font color=\"%3\">Universal</font><strong>")
            .arg(gSettings->HopfenTypBackgrounds[1].name())
            .arg(gSettings->HopfenTypBackgrounds[2].name())
            .arg(gSettings->HopfenTypBackgrounds[3].name()));

    ui->btnMengeKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnAnteilKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnMengeKorrektur->setVisible(false);
    ui->btnAnteilKorrektur->setVisible(false);

    ui->tbMenge->setErrorOnLimit(true);
    ui->tbMengeProLiter->setErrorOnLimit(true);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgHopfenGabe::~WdgHopfenGabe()
{
    delete ui;
}

bool WdgHopfenGabe::isEnabled() const
{
    return mEnabled;
}

QVariant WdgHopfenGabe::data(int col) const
{
    return bh->sud()->modelHopfengaben()->data(mIndex, col);
}

bool WdgHopfenGabe::setData(int col, const QVariant &value)
{
    return bh->sud()->modelHopfengaben()->setData(mIndex, col, value);
}

void WdgHopfenGabe::checkEnabled(bool force)
{
    bool enabled = bh->sud()->getStatus() == Sud_Status_Rezept;
    if (gSettings->ForceEnabled)
        enabled = true;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        ui->btnZutat->setEnabled(true);
        ui->btnLoeschen->setVisible(true);
        ui->tbVorhanden->setVisible(true);
        ui->btnAufbrauchen->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->lblEinheit->setVisible(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbMengeProLiter->setReadOnly(false);
        ui->tbMengeProzent->setReadOnly(false);
        ui->tbAnteilProzent->setReadOnly(false);
        ui->tbKochdauer->setReadOnly(false);
        ui->cbZeitpunkt->setEnabled(true);
    }
    else
    {
        ui->btnZutat->setEnabled(false);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->btnAufbrauchen->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->lblEinheit->setVisible(false);
        ui->tbMenge->setReadOnly(true);
        ui->tbMengeProLiter->setReadOnly(true);
        ui->tbMengeProzent->setReadOnly(true);
        ui->tbAnteilProzent->setReadOnly(true);
        ui->tbKochdauer->setReadOnly(true);
        ui->cbZeitpunkt->setEnabled(false);
        ui->btnAnteilKorrektur->setVisible(false);
        ui->btnMengeKorrektur->setVisible(false);
    }
}

void WdgHopfenGabe::updateValues(bool full)
{
    QString hopfenname = name();

    checkEnabled(full);

    ui->btnZutat->setText(hopfenname);
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbAnteilProzent->hasFocus())
        ui->tbAnteilProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data(ModelHopfengaben::Colerg_Menge).toDouble());
    if (!ui->tbMengeProLiter->hasFocus())
        ui->tbMengeProLiter->setValue(data(ModelHopfengaben::Colerg_Menge).toDouble() / bh->sud()->getMenge());
    if (!ui->tbAnteil->hasFocus())
        ui->tbAnteil->setValue(data(ModelHopfengaben::ColIBUAnteil).toDouble());
    ui->tbAlpha->setValue(data(ModelHopfengaben::ColAlpha).toDouble());
    ui->tbAusbeute->setValue(data(ModelHopfengaben::ColAusbeute).toDouble());
    if (!ui->tbKochdauer->hasFocus())
    {
        ui->tbKochdauer->setMinimum(-bh->sud()->getNachisomerisierungszeit());
        ui->tbKochdauer->setMaximum(bh->sud()->getKochdauerNachBitterhopfung());
        ui->tbKochdauer->setValue(data(ModelHopfengaben::ColZeit).toInt());
    }
    if (data(ModelHopfengaben::ColVorderwuerze).toBool())
        ui->cbZeitpunkt->setCurrentIndex(0);
    else if (ui->tbKochdauer->value() == ui->tbKochdauer->maximum())
        ui->cbZeitpunkt->setCurrentIndex(1);
    else if (ui->tbKochdauer->value() == ui->tbKochdauer->minimum() && ui->tbKochdauer->minimum() != 0)
        ui->cbZeitpunkt->setCurrentIndex(4);
    else if (ui->tbKochdauer->value() <= 0)
        ui->cbZeitpunkt->setCurrentIndex(3);
    else
        ui->cbZeitpunkt->setCurrentIndex(2);
    int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, hopfenname, ModelHopfen::ColTyp).toInt();
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

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, hopfenname, ModelHopfen::ColMenge).toDouble());
        int benoetigt = 0;
        ProxyModel* model = bh->sud()->modelHopfengaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelHopfengaben::ColName).toString() == hopfenname)
                benoetigt += model->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
        }
        model = bh->sud()->modelWeitereZutatenGaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen &&
                model->data(i, ModelWeitereZutatenGaben::ColName).toString() == hopfenname)
                benoetigt += model->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
        }
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);

        ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0);

        ui->tbKochdauer->setReadOnly(ui->cbZeitpunkt->currentIndex() == 0);

        switch (bh->sud()->getberechnungsArtHopfen())
        {
        case Hopfen_Berechnung_Keine:
            ui->btnMengeKorrektur->setVisible(false);
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(false);
            break;
        case Hopfen_Berechnung_Gewicht:
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->tbMengeProzent->setVisible(true);
            ui->lblAnteilProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(true);
            break;
        case Hopfen_Berechnung_IBU:
            ui->btnMengeKorrektur->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->tbAnteilProzent->setVisible(true);
            ui->lblMengeProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(true);
            break;
        }

        if (mIndex == 0 && bh->sud()->modelHopfengaben()->rowCount() == 1)
        {
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeProLiter->setReadOnly(true);
            ui->tbMengeProzent->setReadOnly(true);
            ui->tbAnteilProzent->setReadOnly(true);
            ui->btnAufbrauchen->setVisible(false);
        }
        else
        {
            ui->tbMenge->setReadOnly(false);
            ui->tbMengeProLiter->setReadOnly(false);
            ui->tbMengeProzent->setReadOnly(false);
            ui->tbAnteilProzent->setReadOnly(false);
            ui->btnAufbrauchen->setVisible(qAbs(ui->tbVorhanden->value() - ui->tbMenge->value()) > 0.001);
        }
    }
}

void WdgHopfenGabe::on_btnZutat_clicked()
{
    DlgRohstoffAuswahl dlg(DlgRohstoffAuswahl::Hopfen, this);
    dlg.select(name());
    if (dlg.exec() == QDialog::Accepted)
        setData(ModelHopfengaben::ColName, dlg.name());
}

void WdgHopfenGabe::on_tbMengeProzent_valueChanged(double value)
{
    if (ui->tbMengeProzent->hasFocus())
        setData(ModelHopfengaben::ColProzent, value);
}

void WdgHopfenGabe::on_tbAnteilProzent_valueChanged(double value)
{
    if (ui->tbAnteilProzent->hasFocus())
        setData(ModelHopfengaben::ColProzent, value);
}

QString WdgHopfenGabe::name() const
{
    return data(ModelHopfengaben::ColName).toString();
}

double WdgHopfenGabe::prozent() const
{
    return data(ModelHopfengaben::ColProzent).toDouble();
}

void WdgHopfenGabe::setFehlProzent(double value)
{
    if (mEnabled)
    {
        if (value < 0.0)
        {
            double p = prozent();
            if (p == 0.0)
                value = 0.0;
            else if (value < -p)
                value = -p;
        }
        QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 2) + " %";
        switch (bh->sud()->getberechnungsArtHopfen())
        {
        case Hopfen_Berechnung_Keine:
            break;
        case Hopfen_Berechnung_Gewicht:
            ui->btnMengeKorrektur->setText(text);
            ui->btnMengeKorrektur->setProperty("toadd", value);
            ui->btnMengeKorrektur->setVisible(value != 0.0);
            break;
        case Hopfen_Berechnung_IBU:
            ui->btnAnteilKorrektur->setText(text);
            ui->btnAnteilKorrektur->setProperty("toadd", value);
            ui->btnAnteilKorrektur->setVisible(value != 0.0);
            break;
        }
    }
    else
    {
        ui->btnAnteilKorrektur->setVisible(false);
        ui->btnMengeKorrektur->setVisible(false);
    }
}

void WdgHopfenGabe::remove()
{
    bh->sud()->modelHopfengaben()->removeRow(mIndex);
}

void WdgHopfenGabe::on_btnMengeKorrektur_clicked()
{
    setFocus();
    double toadd = ui->btnMengeKorrektur->property("toadd").toDouble();
    setData(ModelHopfengaben::ColProzent, prozent() + toadd);
}

void WdgHopfenGabe::on_btnAnteilKorrektur_clicked()
{
    setFocus();
    double toadd = ui->btnAnteilKorrektur->property("toadd").toDouble();
    setData(ModelHopfengaben::ColProzent, prozent() + toadd);
}

void WdgHopfenGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelHopfengaben::Colerg_Menge, value);
}

void WdgHopfenGabe::on_tbMengeProLiter_valueChanged(double value)
{
    if (ui->tbMengeProLiter->hasFocus())
        setData(ModelHopfengaben::Colerg_Menge, value * bh->sud()->getMenge());
}

void WdgHopfenGabe::on_tbKochdauer_valueChanged(int dauer)
{
    if (ui->tbKochdauer->hasFocus())
        setData(ModelHopfengaben::ColZeit, dauer);
}

void WdgHopfenGabe::on_cbZeitpunkt_currentIndexChanged(int index)
{
    if (ui->cbZeitpunkt->hasFocus())
    {
        if (index == 0)
        {
            setData(ModelHopfengaben::ColVorderwuerze, true);
        }
        else
        {
            setData(ModelHopfengaben::ColVorderwuerze, false);
            if (index == 1)
                setData(ModelHopfengaben::ColZeit, ui->tbKochdauer->maximum());
            else if (index == 3)
                setData(ModelHopfengaben::ColZeit, 0);
            else if (index == 4)
                setData(ModelHopfengaben::ColZeit, ui->tbKochdauer->minimum());
        }
    }
}

void WdgHopfenGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgHopfenGabe::on_btnAufbrauchen_clicked()
{
    setData(ModelHopfengaben::Colerg_Menge, ui->tbVorhanden->value());
}
