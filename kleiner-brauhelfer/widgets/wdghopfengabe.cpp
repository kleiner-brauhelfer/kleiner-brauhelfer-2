#include "wdghopfengabe.h"
#include "ui_wdghopfengabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "tabrohstoffe.h"
#include "dialogs/dlgrohstoffauswahl.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgHopfenGabe::WdgHopfenGabe(int row, QLayout* parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelHopfengaben(), row, parentLayout, parent),
    ui(new Ui::WdgHopfenGabe),
    mEnabled(true),
    mFehlProzent(0)
{
    ui->setupUi(this);
    if (gSettings->theme() == Settings::Theme::Dark)
    {
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QString(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorHopfen);
    setPalette(pal);

    ui->btnMengeKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnAnteilKorrektur->setPalette(gSettings->paletteErrorButton);

    ui->tbMenge->setErrorOnLimit(true);
    ui->tbMengeProLiter->setErrorOnLimit(true);

    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(mModel, SIGNAL(modified()), this, SLOT(updateValues()));
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

void WdgHopfenGabe::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    mEnabled = status == Brauhelfer::SudStatus::Rezept;
    if (gSettings->ForceEnabled)
        mEnabled = true;
}

void WdgHopfenGabe::updateValues()
{
    QString hopfenname = name();

    checkEnabled();

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
        ui->tbKochdauer->setMaximum(bh->sud()->getKochdauer());
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
        for (int i = 0; i < mModel->rowCount(); ++i)
        {
            if (mModel->data(i, ModelHopfengaben::ColName).toString() == hopfenname)
                benoetigt += mModel->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
        }
        ProxyModel* model = bh->sud()->modelWeitereZutatenGaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model->data(i, ModelWeitereZutatenGaben::ColTyp).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen && model->data(i, ModelWeitereZutatenGaben::ColName).toString() == hopfenname)
                benoetigt += model->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
        }
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);
        ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0);
        ui->btnMengeKorrektur->setVisible(mFehlProzent != 0.0);
        ui->btnAnteilKorrektur->setVisible(mFehlProzent != 0.0);

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

        if (mRow == 0 && mModel->rowCount() == 1 && berechnungsArtHopfen != Brauhelfer::BerechnungsartHopfen::Keine)
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

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
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
    if (value < 0.0)
    {
        double p = prozent();
        if (p == 0.0)
            value = 0.0;
        else if (value < -p)
            value = -p;
    }
    mFehlProzent = value;
    QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 2) + " %";
    ui->btnMengeKorrektur->setText(text);
    ui->btnAnteilKorrektur->setText(text);
}

void WdgHopfenGabe::on_btnMengeKorrektur_clicked()
{
    setFocus();
    setData(ModelHopfengaben::ColProzent, prozent() + mFehlProzent);
}

void WdgHopfenGabe::on_btnAnteilKorrektur_clicked()
{
    setFocus();
    setData(ModelHopfengaben::ColProzent, prozent() + mFehlProzent);
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
    moveUp();
}

void WdgHopfenGabe::on_btnNachUnten_clicked()
{
    moveDown();
}
