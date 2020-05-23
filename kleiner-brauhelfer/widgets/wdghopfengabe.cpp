#include "wdghopfengabe.h"
#include "ui_wdghopfengabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "tabrohstoffe.h"
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

bool WdgHopfenGabe::isValid() const
{
    return mValid;
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
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept;
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
    ui->lblEinheit->setVisible(mEnabled);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbMengeProLiter->setReadOnly(!mEnabled);
    ui->tbMengeProzent->setReadOnly(!mEnabled);
    ui->tbAnteilProzent->setReadOnly(!mEnabled);
    ui->tbKochdauer->setReadOnly(!mEnabled);
    ui->cbZeitpunkt->setEnabled(mEnabled);
    ui->btnAnteilKorrektur->setVisible(mEnabled);
    ui->btnMengeKorrektur->setVisible(mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);
}

void WdgHopfenGabe::updateValues(bool full)
{
    QString hopfenname = name();

    checkEnabled(full);

    int rowRohstoff = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, hopfenname);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(hopfenname);
    ui->btnZutat->setPalette(mValid ? palette() : gSettings->paletteErrorButton);
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbAnteilProzent->hasFocus())
        ui->tbAnteilProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data(ModelHopfengaben::Colerg_Menge).toDouble());
    if (!ui->tbMengeProLiter->hasFocus())
        ui->tbMengeProLiter->setValue(data(ModelHopfengaben::Colerg_Menge).toDouble() / bh->sud()->getMengeSoll());
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

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColMenge).toDouble());
        double benoetigt = 0;
        ProxyModel* model = bh->sud()->modelHopfengaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelHopfengaben::ColName).toString() == hopfenname)
                benoetigt += model->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
        }
        model = bh->sud()->modelWeitereZutatenGaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model->data(i, ModelWeitereZutatenGaben::ColTyp).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen && model->data(i, ModelWeitereZutatenGaben::ColName).toString() == hopfenname)
                benoetigt += model->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
        }
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);

        ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0);

        ui->tbKochdauer->setReadOnly(ui->cbZeitpunkt->currentIndex() == 0);

        Brauhelfer::BerechnungsartHopfen berechnungsArtHopfen = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->sud()->getberechnungsArtHopfen());
        switch (berechnungsArtHopfen)
        {
        case Brauhelfer::BerechnungsartHopfen::Keine:
            ui->btnMengeKorrektur->setVisible(false);
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(false);
            break;
        case Brauhelfer::BerechnungsartHopfen::Gewicht:
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->tbMengeProzent->setVisible(true);
            ui->lblAnteilProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(true);
            break;
        case Brauhelfer::BerechnungsartHopfen::IBU:
            ui->btnMengeKorrektur->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->tbAnteilProzent->setVisible(true);
            ui->lblMengeProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(true);
            break;
        }

        if (mIndex == 0 && bh->sud()->modelHopfengaben()->rowCount() == 1 && berechnungsArtHopfen != Brauhelfer::BerechnungsartHopfen::Keine)
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

    ui->btnNachOben->setEnabled(mIndex > 0);
    ui->btnNachUnten->setEnabled(mIndex < bh->sud()->modelHopfengaben()->rowCount() - 1);
}

void WdgHopfenGabe::on_btnZutat_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
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

int WdgHopfenGabe::row() const
{
    return mIndex;
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
        Brauhelfer::BerechnungsartHopfen berechnungsArtHopfen = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->sud()->getberechnungsArtHopfen());
        switch (berechnungsArtHopfen)
        {
        case Brauhelfer::BerechnungsartHopfen::Keine:
            break;
        case Brauhelfer::BerechnungsartHopfen::Gewicht:
            ui->btnMengeKorrektur->setText(text);
            ui->btnMengeKorrektur->setProperty("toadd", value);
            ui->btnMengeKorrektur->setVisible(value != 0.0);
            break;
        case Brauhelfer::BerechnungsartHopfen::IBU:
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
        setData(ModelHopfengaben::Colerg_Menge, value * bh->sud()->getMengeSoll());
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

void WdgHopfenGabe::on_btnNachOben_clicked()
{
    bh->sud()->modelHopfengaben()->swap(mIndex, mIndex - 1);
}

void WdgHopfenGabe::on_btnNachUnten_clicked()
{
    bh->sud()->modelHopfengaben()->swap(mIndex, mIndex + 1);
}
