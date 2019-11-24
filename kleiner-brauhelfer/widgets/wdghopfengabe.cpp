#include "wdghopfengabe.h"
#include "ui_wdghopfengabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "model/rohstoffauswahlproxymodel.h"

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
        RohstoffAuswahlProxyModel* model = new RohstoffAuswahlProxyModel(ui->cbZutat);
        model->setSourceModel(bh->modelHopfen());
        model->setColumnMenge(ModelHopfen::ColMenge);
        model->setIndexMengeBenoetigt(bh->sud()->modelHopfengaben()->index(mIndex, ModelHopfengaben::Colerg_Menge));
        model->sort(ModelHopfen::ColBeschreibung, Qt::AscendingOrder);
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(ModelHopfen::ColBeschreibung);
        ui->cbZutat->setEnabled(true);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(true);
        ui->tbVorhanden->setVisible(true);
        ui->btnAufbrauchen->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->lblEinheit->setVisible(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbMengeProzent->setReadOnly(false);
        ui->tbAnteilProzent->setReadOnly(false);
        ui->tbKochdauer->setReadOnly(false);
        ui->cbZeitpunkt->setEnabled(true);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbZutat);
        model->setItem(0, 0, new QStandardItem(name()));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(0);
        ui->cbZutat->setEnabled(false);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->btnAufbrauchen->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->lblEinheit->setVisible(false);
        ui->tbMenge->setReadOnly(true);
        ui->tbMengeProzent->setReadOnly(true);
        ui->tbAnteilProzent->setReadOnly(true);
        ui->tbKochdauer->setReadOnly(true);
        ui->cbZeitpunkt->setEnabled(false);
        ui->btnAnteilKorrektur->setVisible(false);
        ui->btnMengeKorrektur->setVisible(false);
    }
}

bool WdgHopfenGabe::prozentIbu() const
{
    return bh->sud()->getberechnungsArtHopfen() == Hopfen_Berechnung_IBU;
}

void WdgHopfenGabe::updateValues(bool full)
{
    QString hopfenname = name();

    checkEnabled(full);

    if (!ui->cbZutat->hasFocus())
    {
        ui->cbZutat->setCurrentIndex(-1);
        ui->cbZutat->setCurrentText(hopfenname);
    }
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbAnteilProzent->hasFocus())
        ui->tbAnteilProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data(ModelHopfengaben::Colerg_Menge).toInt());
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
        ui->tbVorhanden->setValue(bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, hopfenname, ModelHopfen::ColMenge).toInt());
        int benoetigt = 0;
        ProxyModel* model = bh->sud()->modelHopfengaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelHopfengaben::ColName).toString() == hopfenname)
                benoetigt += model->data(i, ModelHopfengaben::Colerg_Menge).toInt();
        }
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());

        ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0);

        ui->tbKochdauer->setReadOnly(ui->cbZeitpunkt->currentIndex() == 0);

        if (prozentIbu())
        {
            ui->btnMengeKorrektur->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->tbAnteilProzent->setVisible(true);
            ui->lblMengeProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(true);
        }
        else
        {
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->tbMengeProzent->setVisible(true);
            ui->lblAnteilProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(true);
        }
        ui->btnAufbrauchen->setVisible(ui->tbMenge->value() != ui->tbVorhanden->value());
    }
}

void WdgHopfenGabe::on_cbZutat_currentIndexChanged(const QString &text)
{
    if (ui->cbZutat->hasFocus())
        setData(ModelHopfengaben::ColName, text);
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
        QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 1) + " %";
        ui->btnMengeKorrektur->setText(text);
        ui->btnMengeKorrektur->setProperty("toadd", value);
        ui->btnAnteilKorrektur->setText(text);
        ui->btnAnteilKorrektur->setProperty("toadd", value);
        if (prozentIbu())
            ui->btnAnteilKorrektur->setVisible(value != 0.0);
        else
            ui->btnMengeKorrektur->setVisible(value != 0.0);
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

void WdgHopfenGabe::on_tbMenge_valueChanged(int value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelHopfengaben::Colerg_Menge, value);
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
