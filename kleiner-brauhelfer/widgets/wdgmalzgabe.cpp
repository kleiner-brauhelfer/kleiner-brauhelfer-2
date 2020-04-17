#include "wdgmalzgabe.h"
#include "ui_wdgmalzgabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "dialogs/dlgrohstoffauswahl.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgMalzGabe::WdgMalzGabe(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgMalzGabe),
    mIndex(index),
    mEnabled(true)
{
    ui->setupUi(this);
    ui->btnKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnKorrektur->setVisible(false);
    ui->lblWarnung->setPalette(gSettings->paletteErrorLabel);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgMalzGabe::~WdgMalzGabe()
{
    delete ui;
}

bool WdgMalzGabe::isEnabled() const
{
    return mEnabled;
}

QVariant WdgMalzGabe::data(int col) const
{
    return bh->sud()->modelMalzschuettung()->data(mIndex, col);
}

bool WdgMalzGabe::setData(int col, const QVariant &value)
{
    return bh->sud()->modelMalzschuettung()->setData(mIndex, col, value);
}

void WdgMalzGabe::checkEnabled(bool force)
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept;
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
        ui->tbMengeProzent->setReadOnly(false);
        ui->tbMenge->setReadOnly(false);
    }
    else
    {
        ui->btnZutat->setEnabled(false);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->btnAufbrauchen->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->lblEinheit->setVisible(false);
        ui->tbMengeProzent->setReadOnly(true);
        ui->tbMenge->setReadOnly(true);
        ui->btnKorrektur->setVisible(false);
        ui->lblWarnung->setVisible(false);
    }
}

void WdgMalzGabe::updateValues(bool full)
{
    QString malzname = name();

    checkEnabled(full);

    ui->btnZutat->setText(malzname);
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data(ModelMalzschuettung::ColProzent).toDouble());
    if (!ui->tbMenge->hasFocus())
    {
        ui->tbMenge->setMaximum(bh->sud()->geterg_S_Gesamt());
        ui->tbMenge->setValue(data(ModelMalzschuettung::Colerg_Menge).toDouble());
    }

    double ebc = data(ModelMalzschuettung::ColFarbe).toDouble();
    QPalette pal = ui->frameColor->palette();
    pal.setColor(QPalette::Background, BierCalc::ebcToColor(ebc));
    ui->frameColor->setPalette(pal);
    ui->frameColor->setToolTip(QString::number(ebc) + " EBC");

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelMalz()->getValueFromSameRow(ModelMalz::ColName, malzname, ModelMalz::ColMenge).toDouble());
        double benoetigt = 0.0;
        ProxyModel* model = bh->sud()->modelMalzschuettung();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelMalzschuettung::ColName).toString() == malzname)
                benoetigt += model->data(i, ModelMalzschuettung::Colerg_Menge).toDouble();
        }
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);
        ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0);

        int max = bh->modelMalz()->getValueFromSameRow(ModelMalz::ColName, malzname, ModelMalz::ColMaxProzent).toInt();
        if (max > 0 && ui->tbMengeProzent->value() > max)
        {
            ui->lblWarnung->setVisible(true);
            ui->lblWarnung->setText(tr("Der maximal empfohlener Schüttungsanteil (%1%) wurde überschritten.").arg(max));
        }
        else
        {
            ui->lblWarnung->setVisible(false);
        }

        if (mIndex == 0 && bh->sud()->modelMalzschuettung()->rowCount() == 1)
        {
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeProzent->setReadOnly(true);
            ui->btnAufbrauchen->setVisible(false);
        }
        else
        {
            ui->tbMenge->setReadOnly(false);
            ui->tbMengeProzent->setReadOnly(false);
            ui->btnAufbrauchen->setVisible(qAbs(ui->tbVorhanden->value() - ui->tbMenge->value()) > 0.001);
        }
    }
}

void WdgMalzGabe::on_btnZutat_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Malz, this);
    dlg.select(name());
    if (dlg.exec() == QDialog::Accepted)
        setData(ModelMalzschuettung::ColName, dlg.name());
}

void WdgMalzGabe::on_tbMengeProzent_valueChanged(double value)
{
    if (ui->tbMengeProzent->hasFocus())
        setData(ModelMalzschuettung::ColProzent, value);
}

void WdgMalzGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelMalzschuettung::Colerg_Menge, value);
}

QString WdgMalzGabe::name() const
{
    return data(ModelMalzschuettung::ColName).toString();
}

double WdgMalzGabe::prozent() const
{
    return data(ModelMalzschuettung::ColProzent).toDouble();
}

void WdgMalzGabe::setFehlProzent(double value)
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
        ui->btnKorrektur->setText(text);
        ui->btnKorrektur->setProperty("toadd", value);
        ui->btnKorrektur->setVisible(value != 0.0);
    }
    else
    {
        ui->btnKorrektur->setVisible(false);
    }
}

void WdgMalzGabe::remove()
{
    bh->sud()->modelMalzschuettung()->removeRow(mIndex);
}

void WdgMalzGabe::on_btnKorrektur_clicked()
{
    setFocus();
    double toadd = ui->btnKorrektur->property("toadd").toDouble();
    setData(ModelMalzschuettung::ColProzent, prozent() + toadd);
}

void WdgMalzGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgMalzGabe::on_btnAufbrauchen_clicked()
{
    setData(ModelMalzschuettung::Colerg_Menge, ui->tbVorhanden->value());
}
