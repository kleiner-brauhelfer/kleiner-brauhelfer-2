#include "tababfuellen.h"
#include "ui_tababfuellen.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgsudteilen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabAbfuellen::TabAbfuellen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabAbfuellen)
{
    ui->setupUi(this);
    ui->lblCurrency->setText(QLocale().currencySymbol());
    ui->lblCurrency2->setText(QLocale().currencySymbol() + "/" + tr("l"));

    ui->webview->setHtmlFile("abfuelldaten.html");

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabAbfuellen");

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());

    ui->tbZuckerFaktor->setValue(gSettings->value("ZuckerFaktor", 1.0).toDouble());
    ui->tbFlasche->setValue(gSettings->value("FlaschenGroesse", 0.5).toDouble());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
                    this, SLOT(sudDataChanged(const QModelIndex&)));
}

TabAbfuellen::~TabAbfuellen()
{
    delete ui;
}

void TabAbfuellen::saveSettings()
{
    gSettings->beginGroup("TabAbfuellen");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->setValue("ZuckerFaktor", ui->tbZuckerFaktor->value());
    gSettings->setValue("FlaschenGroesse", ui->tbFlasche->value());
    gSettings->endGroup();
}

void TabAbfuellen::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
}

void TabAbfuellen::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && now != ui->tbHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabAbfuellen::sudLoaded()
{
    checkEnabled();
    updateValues();
}

void TabAbfuellen::sudDataChanged(const QModelIndex& index)
{
    const SqlTableModel* model = static_cast<const SqlTableModel*>(index.model());
    QString fieldname = model->fieldName(index.column());
    if (fieldname == "Status")
    {
        checkEnabled();
    }
}

void TabAbfuellen::checkEnabled()
{
    int status = bh->sud()->getStatus();
    bool abgefuellt = status >= Sud_Status_Abgefuellt && !gSettings->ForceEnabled;
    ui->tbAbfuelldatum->setReadOnly(abgefuellt);
    ui->btnAbfuelldatumHeute->setVisible(!abgefuellt);
    ui->cbSchnellgaerprobeAktiv->setEnabled(!abgefuellt);
    ui->tbSWSchnellgaerprobe->setReadOnly(abgefuellt);
    ui->btnSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked() && !abgefuellt);
    ui->tbSWJungbier->setReadOnly(abgefuellt);
    ui->btnSWJungbier->setVisible(!abgefuellt);
    ui->tbTemperaturJungbier->setReadOnly(abgefuellt);
    ui->cbSpunden->setEnabled(!abgefuellt);
    ui->tbJungbiermengeAbfuellen->setReadOnly(abgefuellt);
    ui->tbBiermengeAbfuellen->setReadOnly(abgefuellt);
    ui->tbSpeisemengeAbgefuellt->setReadOnly(abgefuellt);
    ui->tbNebenkosten->setReadOnly(abgefuellt);
    ui->btnSudAbgefuellt->setEnabled(status == Sud_Status_Gebraut && !gSettings->ForceEnabled);
    ui->btnSudVerbraucht->setEnabled(status == Sud_Status_Abgefuellt && !gSettings->ForceEnabled);
    ui->btnSudTeilen->setEnabled(status == Sud_Status_Abgefuellt && !gSettings->ForceEnabled);
}

void TabAbfuellen::updateValues()
{
    double value;

    QDateTime dt = bh->sud()->getAbfuelldatum();
    ui->tbAbfuelldatum->setMinimumDateTime(bh->sud()->getBraudatum());
    ui->tbAbfuelldatum->setDateTime(dt.isValid() ? dt : QDateTime::currentDateTime());

    ui->tbDauerHauptgaerung->setValue((int)bh->sud()->getBraudatum().daysTo(ui->tbAbfuelldatum->dateTime()));

    ui->cbSchnellgaerprobeAktiv->setChecked(bh->sud()->getSchnellgaerprobeAktiv());
    ui->tbSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblSWSchnellgaerprobeEinheit->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->btnSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    if (!ui->tbSWSchnellgaerprobe->hasFocus())
        ui->tbSWSchnellgaerprobe->setValue(bh->sud()->getSWSchnellgaerprobe());
    if (!ui->tbSWJungbier->hasFocus())
        ui->tbSWJungbier->setValue(bh->sud()->getSWJungbier());
    if (!ui->tbTemperaturJungbier->hasFocus())
        ui->tbTemperaturJungbier->setValue(bh->sud()->getTemperaturJungbier());
    ui->cbSpunden->setChecked(bh->sud()->getSpunden());
    ui->groupKarbonisierung->setVisible(!ui->cbSpunden->isChecked());
    if (!ui->tbJungbiermengeAbfuellen->hasFocus())
        ui->tbJungbiermengeAbfuellen->setValue(bh->sud()->getJungbiermengeAbfuellen());
    if (!ui->tbBiermengeAbfuellen->hasFocus())
        ui->tbBiermengeAbfuellen->setValue(bh->sud()->geterg_AbgefuellteBiermenge());
    ui->tbJungbierVerlust->setValue(bh->sud()->getWuerzemengeAnstellen() - bh->sud()->getJungbiermengeAbfuellen());
    ui->tbSpeisemengeGesamt2->setValue(bh->sud()->getSpeiseAnteil() / 1000);

    if (!ui->tbSpeisemengeAbgefuellt->hasFocus())
        ui->tbSpeisemengeAbgefuellt->setValue(bh->sud()->getSpeisemenge());
    ui->tbSpeisemengeGesamt->setValue((int)bh->sud()->getSpeiseAnteil());

    ui->tbZuckerGesamt->setValue((int)(bh->sud()->getZuckerAnteil() / ui->tbZuckerFaktor->value()));
    ui->tbZuckerGesamt->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->lblZuckerGesamt->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->lblZuckerGesamtEinheit->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->tbZuckerFaktor->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->lblZuckerFaktor->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->tbZuckerFlasche->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->lblZuckerFlasche->setVisible(ui->tbZuckerGesamt->value() > 0.0);
    ui->lblZuckerFlascheEinheit->setVisible(ui->tbZuckerGesamt->value() > 0.0);

    value = ui->tbFlasche->value() / bh->sud()->getJungbiermengeAbfuellen();
    ui->tbSpeisemengeFlasche->setValue((int)(ui->tbSpeisemengeGesamt->value() * value));
    ui->tbZuckerFlasche->setValue((int)(ui->tbZuckerGesamt->value() * value));
    ui->tbGruenschlauchzeitpunkt->setValue(bh->sud()->getGruenschlauchzeitpunkt());
    ui->tbGruenschlauchzeitpunkt->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblGruenschlauchzeitpunkt->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblGruenschlauchzeitpunktEinheit->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());

    ui->lblBiermengeAbfuellen->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
    ui->tbBiermengeAbfuellen->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
    ui->lblBiermengeAbfuellenEinheit->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);

    ui->lblSpeisemengeGesamt2->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
    ui->tbSpeisemengeGesamt2->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
    ui->lblSpeisemengeGesamtEinheit2->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);

    if (!ui->tbNebenkosten->hasFocus())
        ui->tbNebenkosten->setValue(bh->sud()->getKostenWasserStrom());
    ui->tbKosten->setValue(bh->sud()->geterg_Preis());

    ui->tbTEVG->setValue(bh->sud()->gettEVG());
    ui->tbSEVG->setValue(bh->sud()->getsEVG());
    ui->tbAlkohol->setValue(bh->sud()->geterg_Alkohol());
    ui->tbSpundungsdruck->setValue(bh->sud()->getSpundungsdruck());

    TemplateTags::render(ui->webview, TemplateTags::TagAll, bh->sud()->row());
}

void TabAbfuellen::on_tbAbfuelldatum_dateTimeChanged(const QDateTime &dateTime)
{
    if (ui->tbAbfuelldatum->hasFocus())
        bh->sud()->setAbfuelldatum(dateTime);
}

void TabAbfuellen::on_btnAbfuelldatumHeute_clicked()
{
    bh->sud()->setAbfuelldatum(QDateTime());
}

void TabAbfuellen::on_cbSchnellgaerprobeAktiv_clicked(bool checked)
{
    bh->sud()->setSchnellgaerprobeAktiv(checked);
}

void TabAbfuellen::on_tbSWSchnellgaerprobe_valueChanged(double value)
{
    if (ui->tbSWSchnellgaerprobe->hasFocus())
        bh->sud()->setSWSchnellgaerprobe(value);
}

void TabAbfuellen::on_btnSWSchnellgaerprobe_clicked()
{
    DlgRestextrakt dlg(ui->tbSWSchnellgaerprobe->value(),
                       bh->sud()->getSWIst(),
                       ui->tbTemperaturJungbier->value(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbSWSchnellgaerprobe->setValue(dlg.value());
}

void TabAbfuellen::on_tbSWJungbier_valueChanged(double value)
{
    if (ui->tbSWJungbier->hasFocus())
        bh->sud()->setSWJungbier(value);
}

void TabAbfuellen::on_btnSWJungbier_clicked()
{
    DlgRestextrakt dlg(ui->tbSWJungbier->value(),
                       bh->sud()->getSWIst(),
                       ui->tbTemperaturJungbier->value(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbSWJungbier->setValue(dlg.value());
}

void TabAbfuellen::on_tbTemperaturJungbier_valueChanged(double value)
{
    if (ui->tbTemperaturJungbier->hasFocus())
        bh->sud()->setTemperaturJungbier(value);
}

void TabAbfuellen::on_cbSpunden_clicked(bool checked)
{
    bh->sud()->setSpunden(checked);
}

void TabAbfuellen::on_tbJungbiermengeAbfuellen_valueChanged(double value)
{
    if (ui->tbJungbiermengeAbfuellen->hasFocus())
        bh->sud()->setJungbiermengeAbfuellen(value);
}

void TabAbfuellen::on_tbBiermengeAbfuellen_valueChanged(double value)
{
    if (ui->tbBiermengeAbfuellen->hasFocus())
        bh->sud()->seterg_AbgefuellteBiermenge(value);
}


void TabAbfuellen::on_tbSpeisemengeAbgefuellt_valueChanged(double value)
{
    if (ui->tbSpeisemengeAbgefuellt->hasFocus())
        bh->sud()->setSpeisemenge(value);
}

void TabAbfuellen::on_tbZuckerFaktor_valueChanged(double)
{
    if (ui->tbZuckerFaktor->hasFocus())
        updateValues();
}

void TabAbfuellen::on_tbFlasche_valueChanged(double)
{
    if (ui->tbFlasche->hasFocus())
        updateValues();
}

void TabAbfuellen::on_tbNebenkosten_valueChanged(double value)
{
    if (ui->tbNebenkosten->hasFocus())
        bh->sud()->setKostenWasserStrom(value);
}

void TabAbfuellen::on_btnSudAbgefuellt_clicked()
{
    if (!bh->sud()->getAbfuellenBereitZutaten())
    {
        QMessageBox::warning(this, tr("Zutaten Gärung"),
                             tr("Es wurden noch nicht alle Zutaten für die Gärung zugegeben oder entnommen."));
        return;
    }

    if (bh->sud()->getSchnellgaerprobeAktiv())
    {
        if (bh->sud()->getSWJungbier() > bh->sud()->getGruenschlauchzeitpunkt())
        {
            QMessageBox::warning(this, tr("Grünschlauchzeitpunkt nicht erreicht"),
                                 tr("Der Grünschlauchzeitpunkt wurde noch nicht erreicht."));
            return;
        }
        else if (bh->sud()->getSWJungbier() < bh->sud()->getSWSchnellgaerprobe())
        {
            QMessageBox::warning(this, tr("Schnellgärprobe"),
                                 tr("Die Stammwürze des Jungbiers liegt tiefer als die der Schnellgärprobe."));
            return;
        }
    }

    bh->sud()->setAbfuelldatum(ui->tbAbfuelldatum->dateTime());
    bh->sud()->setStatus(Sud_Status_Abgefuellt);

    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Zeitstempel", bh->sud()->getAbfuelldatum()},
                        {"Druck", 0.0}, {"Temp", bh->sud()->getTemperaturJungbier()}});
    if (bh->sud()->modelNachgaerverlauf()->rowCount() == 0)
        bh->sud()->modelNachgaerverlauf()->append(values);
}

void TabAbfuellen::on_btnSudTeilen_clicked()
{
    DlgSudTeilen dlg(bh->sud()->getSudname(), bh->sud()->getMengeIst(), this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sudTeilen(bh->sud()->id(), dlg.nameTeil1(), dlg.nameTeil2(), dlg.prozent());
}

void TabAbfuellen::on_btnSudVerbraucht_clicked()
{
    bh->sud()->setStatus(Sud_Status_Verbraucht);
}
