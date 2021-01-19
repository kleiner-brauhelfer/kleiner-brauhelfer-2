#include "wdgmalzgabe.h"
#include "ui_wdgmalzgabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "dialogs/dlgrohstoffauswahl.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgMalzGabe::WdgMalzGabe(int row, QLayout *parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelMalzschuettung(), row, parentLayout, parent),
    ui(new Ui::WdgMalzGabe),
    mEnabled(true)
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
                icon.addFile(QString(":/images/dark/%1.svg").arg(name), QSize(), QIcon::Normal, QIcon::Off);
                button->setIcon(icon);
            }
        }
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorMalz);
    setPalette(pal);

    ui->btnKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnKorrektur->setVisible(false);
    ui->lblWarnung->setPalette(gSettings->paletteErrorLabel);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(mModel, SIGNAL(modified()), this, SLOT(updateValues()));
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

bool WdgMalzGabe::isValid() const
{
    return mValid;
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
    ui->btnZutat->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbVorhanden->setVisible(mEnabled);
    ui->btnAufbrauchen->setVisible(mEnabled);
    ui->lblVorhanden->setVisible(mEnabled);
    ui->lblEinheit->setVisible(mEnabled);
    ui->tbMengeProzent->setReadOnly(!mEnabled);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbExtraktProzent->setReadOnly(!mEnabled);
    ui->tbExtrakt->setReadOnly(!mEnabled);
    ui->btnKorrektur->setVisible(mEnabled);
    ui->lblWarnung->setVisible(mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);
}

void WdgMalzGabe::updateValues(bool full)
{
    QString malzname = name();

    checkEnabled(full);

    int rowRohstoff = bh->modelMalz()->getRowWithValue(ModelMalz::ColName, malzname);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(malzname);
    ui->btnZutat->setPalette(mValid ? palette() : gSettings->paletteErrorButton);
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data(ModelMalzschuettung::ColProzent).toDouble());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data(ModelMalzschuettung::Colerg_Menge).toDouble());

    if (!ui->tbExtrakt->hasFocus())
        ui->tbExtrakt->setValue(data(ModelMalzschuettung::ColExtrakt).toDouble());
    if (!ui->tbExtraktProzent->hasFocus())
        ui->tbExtraktProzent->setValue(data(ModelMalzschuettung::ColExtraktProzent).toDouble());
    bool visible = bh->sud()->getSW() != bh->sud()->getSW_Malz();
    ui->tbExtraktProzent->setVisible(visible);
    ui->lblExtraktProzent->setVisible(visible);

    double ebc = data(ModelMalzschuettung::ColFarbe).toDouble();
    QPalette pal = ui->frameColor->palette();
    pal.setColor(QPalette::Background, BierCalc::ebcToColor(ebc));
    ui->frameColor->setPalette(pal);
    ui->frameColor->setToolTip(QString::number(ebc) + " EBC");

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelMalz()->data(rowRohstoff, ModelMalz::ColMenge).toDouble());
        double benoetigt = 0.0;
        for (int i = 0; i < mModel->rowCount(); ++i)
        {
            if (mModel->data(i, ModelMalzschuettung::ColName).toString() == malzname)
                benoetigt += mModel->data(i, ModelMalzschuettung::Colerg_Menge).toDouble();
        }
        ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);
        ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0);

        int max = bh->modelMalz()->data(rowRohstoff, ModelMalz::ColMaxProzent).toInt();
        if (max > 0 && ui->tbMengeProzent->value() > max)
        {
            ui->lblWarnung->setVisible(true);
            ui->lblWarnung->setText(tr("Der maximal empfohlener Schüttungsanteil (%1%) wurde überschritten.").arg(max));
        }
        else
        {
            ui->lblWarnung->setVisible(false);
        }

        if (mRow == 0 && mModel->rowCount() == 1)
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

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
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

void WdgMalzGabe::on_tbExtrakt_valueChanged(double value)
{
    if (ui->tbExtrakt->hasFocus())
        setData(ModelMalzschuettung::ColExtrakt, value);
}

void WdgMalzGabe::on_tbExtraktProzent_valueChanged(double value)
{
    if (ui->tbExtraktProzent->hasFocus())
        setData(ModelMalzschuettung::ColExtraktProzent, value);
}

QString WdgMalzGabe::name() const
{
    return data(ModelMalzschuettung::ColName).toString();
}

double WdgMalzGabe::prozent() const
{
    return data(ModelMalzschuettung::ColProzent).toDouble();
}

double WdgMalzGabe::fehlProzent() const
{
    return ui->btnKorrektur->property("toadd").toDouble();
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

void WdgMalzGabe::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgMalzGabe::on_btnNachUnten_clicked()
{
    moveDown();
}
