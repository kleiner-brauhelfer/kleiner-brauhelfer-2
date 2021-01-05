#include "wdghefegabe.h"
#include "ui_wdghefegabe.h"
#include <QtMath>
#include <QStandardItemModel>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "tabrohstoffe.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgHefeGabe::WdgHefeGabe(int row, QLayout* parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelHefegaben(), row, parentLayout, parent),
    ui(new Ui::WdgHefeGabe),
    mEnabled(true)
{
    ui->setupUi(this);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorHefe);
    setPalette(pal);

    ui->tbMenge->setErrorOnLimit(true);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(mModel, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgHefeGabe::~WdgHefeGabe()
{
    delete ui;
}

bool WdgHefeGabe::isEnabled() const
{
    return mEnabled;
}

bool WdgHefeGabe::isValid() const
{
    return mValid;
}

QString WdgHefeGabe::name() const
{
    return data(ModelHefegaben::ColName).toString();
}

int WdgHefeGabe::menge() const
{
    return data(ModelHefegaben::ColMenge).toInt();
}

void WdgHefeGabe::checkEnabled(bool force)
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status < Brauhelfer::SudStatus::Abgefuellt;
    if (data(ModelHefegaben::ColZugegeben).toBool())
        enabled = false;
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
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbMengeEmpfohlen->setVisible(mEnabled);
    ui->lblEmpfohlen->setVisible(mEnabled);
    ui->tbTage->setReadOnly(!mEnabled);
    ui->tbDatum->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);
}

void WdgHefeGabe::updateValues(bool full)
{
    QString hefename = name();

    checkEnabled(full);

    int rowRohstoff = bh->modelHefe()->getRowWithValue(ModelHefe::ColName, hefename);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(hefename);
    ui->btnZutat->setPalette(mValid ? palette() : gSettings->paletteErrorButton);
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(menge());
    if (!ui->tbTage->hasFocus())
        ui->tbTage->setValue(data(ModelHefegaben::ColZugabeNach).toInt());
    QDateTime braudatum = bh->sud()->getBraudatum();
    if (braudatum.isValid())
    {
        ui->tbDatum->setMinimumDateTime(braudatum);
        if (!ui->tbDatum->hasFocus())
            ui->tbDatum->setDate(data(ModelHefegaben::ColZugabeDatum).toDate());
    }
    ui->tbDatum->setVisible(braudatum.isValid());

    if (rowRohstoff >= 0)
    {
        int idx = bh->modelHefe()->data(rowRohstoff, ModelHefe::ColTypOGUG).toInt();
        if (idx >= 0 && idx < gSettings->HefeTypOgUgBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Background, gSettings->HefeTypOgUgBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
        }
        ui->frameColor->setToolTip(TabRohstoffe::HefeTypname[idx]);

        double mengeHefe = bh->modelHefe()->data(rowRohstoff, ModelHefe::ColWuerzemenge).toDouble();
        if (mengeHefe > 0)
            ui->tbMengeEmpfohlen->setValue(qCeil(bh->sud()->getMengeSoll() / mengeHefe));
        else
            ui->tbMengeEmpfohlen->setValue(0);
    }
    else
    {
        ui->frameColor->setPalette(gSettings->palette);
        ui->frameColor->setToolTip("");
        ui->tbMengeEmpfohlen->setValue(0);
    }

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    ui->btnZugeben->setVisible(mEnabled && status == Brauhelfer::SudStatus::Gebraut);

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelHefe()->data(rowRohstoff, ModelHefe::ColMenge).toInt());
        int benoetigt = 0;
        for (int i = 0; i < mModel->rowCount(); ++i)
        {
            if (mModel->data(i, ModelHefegaben::ColName).toString() == hefename)
                benoetigt += mModel->data(i, ModelHefegaben::ColMenge).toInt();
        }
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());
        ui->btnAufbrauchen->setVisible(ui->tbMenge->value() != ui->tbVorhanden->value());
    }

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
}

void WdgHefeGabe::on_btnZutat_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hefe, this);
    dlg.select(name());
    if (dlg.exec() == QDialog::Accepted)
        setData(ModelHefegaben::ColName, dlg.name());
}

void WdgHefeGabe::on_tbMenge_valueChanged(int value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelHefegaben::ColMenge, value);
}

void WdgHefeGabe::on_tbTage_valueChanged(int value)
{
    if (ui->tbTage->hasFocus())
        setData(ModelHefegaben::ColZugabeNach, value);
}

void WdgHefeGabe::on_tbDatum_dateChanged(const QDate &date)
{
    if (ui->tbDatum->hasFocus())
        setData(ModelHefegaben::ColZugabeDatum, date);
}

void WdgHefeGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatum->date();
    setData(ModelHefegaben::ColZugabeDatum, currentDate < date ? currentDate : date);
    setData(ModelHefegaben::ColZugegeben, true);

    DlgRohstoffeAbziehen dlg(true, Brauhelfer::RohstoffTyp::Hefe, name(), menge(), this);
    dlg.exec();
}

void WdgHefeGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgHefeGabe::on_btnAufbrauchen_clicked()
{
    setData(ModelHefegaben::ColMenge, ui->tbVorhanden->value());
}

void WdgHefeGabe::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgHefeGabe::on_btnNachUnten_clicked()
{
    moveDown();
}
