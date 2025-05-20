#include "wdghefegabe.h"
#include "ui_wdghefegabe.h"
#include <QtMath>
#include <QStandardItemModel>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "mainwindow.h"
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
    if (gSettings->theme() == Qt::ColorScheme::Dark)
    {
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QStringLiteral(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorHefe);
    setPalette(pal);

    ui->tbMenge->setErrorRange(0, ui->tbMenge->maximum());

    updateValues();
    connect(bh, &Brauhelfer::modified, this, &WdgHefeGabe::updateValues);
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

void WdgHefeGabe::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    mEnabled = status < Brauhelfer::SudStatus::Abgefuellt;
    if (data(ModelHefegaben::ColZugegeben).toBool())
        mEnabled = false;
    if (gSettings->ForceEnabled)
        mEnabled = true;
}

void WdgHefeGabe::updateValues()
{
    QString hefename = name();

    checkEnabled();

    ui->btnZutat->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbVorhanden->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->lblVorhanden->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->btnAufbrauchen->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbMengeEmpfohlen->setVisible(mEnabled);
    ui->lblEmpfohlen->setVisible(mEnabled);
    ui->tbTage->setReadOnly(!mEnabled);
    ui->tbDatum->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    int rowRohstoff = bh->modelHefe()->getRowWithValue(ModelHefe::ColName, hefename);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(hefename);
    ui->btnZutat->setError(!mValid);
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
            pal.setColor(QPalette::Window, gSettings->HefeTypOgUgBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
        }
        ui->frameColor->setToolTip(MainWindow::HefeTypname[idx]);

        double mengeHefe = bh->modelHefe()->data(rowRohstoff, ModelHefe::ColWuerzemenge).toDouble();
        if (mengeHefe > 0)
            ui->tbMengeEmpfohlen->setValue(qCeil(bh->sud()->getMengeSollAnstellen() / mengeHefe));
        else
            ui->tbMengeEmpfohlen->setValue(0);
    }
    else
    {
        ui->frameColor->setPalette(gSettings->palette);
        ui->frameColor->setToolTip(QStringLiteral(""));
        ui->tbMengeEmpfohlen->setValue(0);
    }

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    ui->btnZugeben->setVisible(mEnabled && status == Brauhelfer::SudStatus::Gebraut);

    if (mEnabled)
    {
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
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
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        DlgRohstoffeAbziehen dlg(true, Brauhelfer::RohstoffTyp::Hefe, name(), menge(), this);
        dlg.exec();
    }
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
