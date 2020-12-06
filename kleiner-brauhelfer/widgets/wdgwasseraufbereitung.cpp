#include "wdgwasseraufbereitung.h"
#include "ui_wdgwasseraufbereitung.h"
#include "tabrohstoffe.h"
#include "dialogs/dlgwasseraufbereitung.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgWasseraufbereitung::WdgWasseraufbereitung(int row, QLayout *parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelWasseraufbereitung(), row, parentLayout, parent),
    ui(new Ui::WdgWasseraufbereitung)
{
    ui->setupUi(this);

    checkEnabled(true);
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(mModel, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgWasseraufbereitung::~WdgWasseraufbereitung()
{
    delete ui;
}

void WdgWasseraufbereitung::checkEnabled(bool force)
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept;
    if (gSettings->ForceEnabled)
        enabled = true;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    ui->tbName->setEnabled(mEnabled);
    ui->btnAuswahl->setVisible(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbRestalkalitaet->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);
}

void WdgWasseraufbereitung::updateValues(bool full)
{
    checkEnabled(full);

    double menge = data(ModelWasseraufbereitung::ColMenge).toDouble();
    if (!ui->tbName->hasFocus())
        ui->tbName->setText(data(ModelWasseraufbereitung::ColName).toString());
    ui->cbEinheit->setCurrentIndex(data(ModelWasseraufbereitung::ColEinheit).toInt());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(menge);
    ui->tbMengeGesamt->setValue(menge * bh->sud()->geterg_W_Gesamt());
    ui->tbMengeHg->setValue(menge * bh->sud()->geterg_WHauptguss());
    ui->tbMengeNg->setValue(menge * bh->sud()->geterg_WNachguss());
    double w = bh->sud()->getWasserHgf();
    ui->tbMengeHgf->setValue(menge * w);
    ui->tbMengeHgf->setVisible(w > 0);
    ui->lblHgf->setVisible(w > 0);
    ui->lblEinheitHgf->setVisible(ui->tbMengeHgf->value() > 0);
    if (!ui->tbRestalkalitaet->hasFocus())
        ui->tbRestalkalitaet->setValue(data(ModelWasseraufbereitung::ColRestalkalitaet).toDouble());
    if (!ui->tbFaktor->hasFocus())
        ui->tbFaktor->setValue(data(ModelWasseraufbereitung::ColFaktor).toDouble());
    if (ui->tbFaktor->value() > 0)
    {
        ui->tbRestalkalitaet->setMinimum(0);
        ui->tbRestalkalitaet->setMaximum(99);
    }
    else
    {
        ui->tbRestalkalitaet->setMinimum(-99);
        ui->tbRestalkalitaet->setMaximum(0);
    }
    double diff = bh->sud()->getRestalkalitaetSoll() - bh->sud()->getRestalkalitaetIst();
    ui->btnAusgleichen->setVisible(mEnabled && qAbs(diff) > 0.005);
    QString einheit = TabRohstoffe::Einheiten[data(ModelWasseraufbereitung::ColEinheit).toInt()];
    ui->lblEinheit->setText(einheit + tr("/l"));
    ui->lblEinheitGesamt->setText(einheit);
    ui->lblEinheitHg->setText(einheit);
    ui->lblEinheitNg->setText(einheit);
    ui->lblEinheitHgf->setText(einheit);
    ui->lblEinheitFaktor->setText(einheit + tr("/l"));
    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
}

void WdgWasseraufbereitung::on_tbName_textChanged(const QString &text)
{
    if (ui->tbName->hasFocus())
        setData(ModelWasseraufbereitung::ColName, text);
}

void WdgWasseraufbereitung::on_cbEinheit_currentIndexChanged(int index)
{
    if (ui->cbEinheit->hasFocus())
        setData(ModelWasseraufbereitung::ColEinheit, index);
}

void WdgWasseraufbereitung::on_btnAuswahl_clicked()
{
    DlgWasseraufbereitung dlg(ui->tbName->text(), ui->cbEinheit->currentIndex(), ui->tbFaktor->value(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        setData(ModelWasseraufbereitung::ColName, dlg.name());
        setData(ModelWasseraufbereitung::ColEinheit, dlg.einheit());
        setData(ModelWasseraufbereitung::ColFaktor, dlg.faktor());
    }
}

void WdgWasseraufbereitung::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelWasseraufbereitung::ColMenge, value);
}

void WdgWasseraufbereitung::on_tbMengeGesamt_valueChanged(double value)
{
    if (ui->tbMengeGesamt->hasFocus())
        setData(ModelWasseraufbereitung::ColMenge, value / bh->sud()->geterg_W_Gesamt());
}

void WdgWasseraufbereitung::on_tbRestalkalitaet_valueChanged(double value)
{
    if (ui->tbRestalkalitaet->hasFocus())
        setData(ModelWasseraufbereitung::ColRestalkalitaet, value);
}

void WdgWasseraufbereitung::on_tbFaktor_valueChanged(double value)
{
    if (ui->tbFaktor->hasFocus())
        setData(ModelWasseraufbereitung::ColFaktor, value);
}

void WdgWasseraufbereitung::on_btnAusgleichen_clicked()
{
    double ra = bh->sud()->getRestalkalitaetSoll() - bh->sud()->getRestalkalitaetIst() + data(ModelWasseraufbereitung::ColRestalkalitaet).toDouble();
    setData(ModelWasseraufbereitung::ColRestalkalitaet, ra);
}

void WdgWasseraufbereitung::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgWasseraufbereitung::on_btnNachUnten_clicked()
{
    moveDown();
}

void WdgWasseraufbereitung::on_btnLoeschen_clicked()
{
    remove();
}
