#include "tabbraudaten.h"
#include "ui_tabbraudaten.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgvolumen.h"
#include "dialogs/dlgrohstoffeabziehen.h"
#include "widgets/widgetdecorator.h"
#include "commands/undostack.h"

extern Settings* gSettings;

TabBraudaten::TabBraudaten(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBraudaten),
    mSud(nullptr)
{
    ui->setupUi(this);
}

TabBraudaten::~TabBraudaten()
{
    delete ui;
}

void TabBraudaten::setup(SudObject *sud)
{
    mSud = sud;

    ui->tbSWKochbeginn->setColumn(mSud, ModelSud::ColSWKochbeginn);
    ui->tbWuerzemengeKochbeginn->setColumn(mSud, ModelSud::ColWuerzemengeKochbeginn);
    ui->tbWuerzemengeVorHopfenseihen->setColumn(mSud, ModelSud::ColWuerzemengeVorHopfenseihen);
    ui->tbWuerzemengeKochende->setColumn(mSud, ModelSud::ColWuerzemengeKochende);
    ui->tbSWKochende->setColumn(mSud, ModelSud::ColSWKochende);
    ui->tbSWAnstellen->setColumn(mSud, ModelSud::ColSWAnstellen);
    ui->tbVerduennung->setColumn(mSud, ModelSud::ColVerduennungAnstellen);
    ui->tbWuerzemengeAnstellen->setColumn(mSud, ModelSud::ColWuerzemengeAnstellen);
    ui->tbSpeisemenge->setColumn(mSud, ModelSud::ColSpeisemenge);
    ui->tbWuerzemengeAnstellenTotal->setColumn(mSud, ModelSud::ColWuerzemengeAnstellenTotal);
    ui->tbMengeSollKochbeginn20->setColumn(mSud, ModelSud::ColMengeSollKochbeginn);
    ui->tbSWSollKochbeginn->setColumn(mSud, ModelSud::ColSWSollKochbeginn);
    ui->tbSWSollKochbeginnMitWz->setColumn(mSud, ModelSud::ColSWSollKochbeginnMitWz);
    ui->tbMengeSollKochende20->setColumn(mSud, ModelSud::ColMengeSollKochende);
    ui->tbSWSollKochende->setColumn(mSud, ModelSud::ColSWSollKochende);
    ui->tbVerdampfung->setColumn(mSud, ModelSud::ColVerdampfungsrateIst);
    ui->tbAusbeute->setColumn(mSud, ModelSud::Colerg_Sudhausausbeute);
    ui->tbAusbeuteEffektiv->setColumn(mSud, ModelSud::Colerg_EffektiveAusbeute);
    ui->tbVerdampfungRezept->setColumn(mSud, ModelSud::ColVerdampfungsrate);
    ui->tbAusbeuteRezept->setColumn(mSud, ModelSud::ColSudhausausbeute);
    ui->tbSWAnstellenSoll->setColumn(mSud, ModelSud::ColSWSollAnstellen);
    ui->tbMengeHefestarter->setColumn(mSud, ModelSud::ColMengeHefestarter);
    ui->tbSWHefestarter->setColumn(mSud, ModelSud::ColSWHefestarter);
    ui->tbKosten->setColumn(mSud, ModelSud::Colerg_Preis);
    ui->tbNebenkosten->setColumn(mSud, ModelSud::ColKostenWasserStrom);
    ui->lblNebenkostenEinheit->setText(QLocale().currencySymbol());
    ui->lblKostenEinheit->setText(QLocale().currencySymbol() + "/L");
    ui->lblDurchschnittWarnung->setPalette(gSettings->paletteErrorLabel);

    ui->wdgBemerkung->setSudObject(mSud);
    ui->wdgBemerkung->setPlaceholderText(tr("Bemerkung Braudaten"));

    gSettings->beginGroup("TabBraudaten");

    ui->splitter->setSizes({100, 400});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterCharts->setSizes({300, 300, 100});
    mDefaultSplitterChartsState = ui->splitterCharts->saveState();
    ui->splitterCharts->restoreState(gSettings->value("splitterChartsState").toByteArray());

    gSettings->endGroup();

    connect(mSud->bh(), &Brauhelfer::modified, this, &TabBraudaten::updateValues);
    connect(mSud->bh(), &Brauhelfer::discarded, this, &TabBraudaten::sudLoaded);
    connect(mSud, &SudObject::loadedChanged, this, &TabBraudaten::sudLoaded);
    connect(mSud, &SudObject::dataChanged, this, &TabBraudaten::sudDataChanged);

    connect(ui->wdgBemerkung, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungBrauen, html));});
}

void TabBraudaten::saveSettings()
{
    gSettings->beginGroup("TabBraudaten");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterChartsState", ui->splitterCharts->saveState());
    gSettings->endGroup();
}

void TabBraudaten::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterCharts->restoreState(mDefaultSplitterChartsState);
}

void TabBraudaten::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleAusruestung))
    {
        gSettings->setVisibleModule(Settings::ModuleAusruestung,
                         {ui->cbDurchschnittIgnorieren,
                         ui->lblDurchschnittIgnorieren,
                         ui->lblDurchschnittWarnung,
                         ui->lineDurchschnittIgnorieren,
                         ui->wdgMengeSollcmVonOben,
                         ui->wdgMengeSollcmVomBoden,
                         ui->wdgMengeSollEndecmVonOben,
                         ui->wdgMengeSollEndecmVomBoden});
    }
    if (modules.testFlag(Settings::ModuleSpeise))
    {
        gSettings->setVisibleModule(Settings::ModuleSpeise,
                         {ui->tbSpeisemenge,
                          ui->lblSpeisemenge,
                          ui->lblSpeisemengeEinheit,
                          ui->wdgSpeisemengeNoetig,
                          ui->btnSpeisemengeNoetig,
                          ui->tbWuerzemengeAnstellenTotal,
                          ui->lblWuerzemengeAnstellenTotal,
                          ui->lblWuerzemengeAnstellenTotalEinheit,
                          ui->btnWuerzemengeAnstellenTotal,
                          ui->lineWuerzemengeAnstellenTotal,
                          ui->lineWuerzemengeAnstellenTotal2});
    }
    if (modules.testFlag(Settings::ModulePreiskalkulation))
    {
        gSettings->setVisibleModule(Settings::ModulePreiskalkulation,
                         {ui->tbKosten,
                          ui->lblKosten,
                          ui->lblKostenEinheit,
                          ui->tbNebenkosten,
                          ui->lblNebenkosten,
                          ui->lblNebenkostenEinheit,
                          ui->lblNebenkostenSpacer,
                          ui->lineKosten,
                          ui->lineKosten2});
    }
    if (mSud->isLoaded())
    {
        checkEnabled();
        updateValues();
    }
}

void TabBraudaten::sudLoaded()
{
    checkEnabled();
    updateValues();
    ui->tbSpeiseSRE->setValue(mSud->getSRESoll());
    ui->wdgBemerkung->setHtml(mSud->getBemerkungBrauen());
}

void TabBraudaten::sudDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        checkEnabled();
        break;
    case ModelSud::ColSW:
    case ModelSud::ColVergaerungsgrad:
        ui->tbSpeiseSRE->setValue(mSud->getSRESoll());
        break;
    case ModelSud::ColBemerkungBrauen:
        ui->wdgBemerkung->setHtml(mSud->getBemerkungBrauen());
        break;
    }
}

void TabBraudaten::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateValues();
}

void TabBraudaten::changeEvent(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
    QWidget::changeEvent(event);
}

void TabBraudaten::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    bool gebraut = status != Brauhelfer::SudStatus::Rezept && !gSettings->ForceEnabled;
    ui->tbBraudatum->setReadOnly(gebraut);
    ui->tbBraudatumZeit->setReadOnly(gebraut);
    ui->btnBraudatumHeute->setVisible(!gebraut);
    ui->tbSWKochbeginn->setReadOnly(gebraut);
    ui->btnSWKochbeginn->setVisible(!gebraut);
    ui->tbWuerzemengeKochbeginn->setReadOnly(gebraut);
    ui->btnWuerzemengeKochbeginn->setVisible(!gebraut);
    ui->tbWuerzemengeVorHopfenseihen->setReadOnly(gebraut);
    ui->btnWuerzemengeVorHopfenseihen->setVisible(!gebraut);
    ui->tbWuerzemengeKochende->setReadOnly(gebraut);
    ui->btnWuerzemengeKochende->setVisible(!gebraut);
    ui->tbSWKochende->setReadOnly(gebraut);
    ui->btnSWKochende->setVisible(!gebraut);
    ui->tbSWAnstellen->setReadOnly(gebraut);
    ui->btnSWAnstellen->setVisible(!gebraut);
    ui->btnWuerzemengeAnstellen->setVisible(!gebraut);
    ui->btnWuerzemengeAnstellenTotal->setVisible(!gebraut && gSettings->isModuleEnabled(Settings::ModuleSpeise));
    ui->tbWuerzemengeAnstellenTotal->setReadOnly(gebraut);
    ui->btnSpeisemengeNoetig->setVisible(!gebraut && gSettings->isModuleEnabled(Settings::ModuleSpeise));
    if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        ui->wdgMengeSollcmVonOben->setVisible(!gebraut);
        ui->wdgMengeSollcmVomBoden->setVisible(!gebraut);
        ui->wdgMengeSollEndecmVonOben->setVisible(!gebraut);
        ui->wdgMengeSollEndecmVomBoden->setVisible(!gebraut);
    }
    ui->tbSpeisemenge->setReadOnly(gebraut);
    ui->tbVerduennung->setReadOnly(gebraut);
    ui->tbWuerzemengeAnstellen->setReadOnly(gebraut);
    ui->tbNebenkosten->setReadOnly(gebraut);
    ui->btnSudGebraut->setEnabled(!gebraut);
    ui->btnSudGebrautReset->setEnabled(status == Brauhelfer::SudStatus::Gebraut);
    ui->tbTempKochbeginn->setReadOnly(gebraut);
    ui->tbTempKochende->setReadOnly(gebraut);
    ui->tbSpeiseSRE->setReadOnly(gebraut);
    ui->tbSpeiseT->setReadOnly(gebraut);
    ui->tbMengeHefestarter->setReadOnly(gebraut);
    ui->tbSWHefestarter->setReadOnly(gebraut);
    ui->tbSWHefestarterBrix->setReadOnly(gebraut);
    ui->tbWasserVerschneidung->setReadOnly(gebraut);
    ui->tbWasserVerschneidungBrix->setReadOnly(gebraut);
}

void TabBraudaten::updateValues()
{
    if (!isVisible())
        return;

    for (auto& wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();

    double value;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());

    QDateTime dt = mSud->getBraudatum();
    ui->tbBraudatum->setDate(dt.isValid() ? dt.date() : QDateTime::currentDateTime().date());
    ui->tbBraudatumZeit->setTime(dt.isValid() ? dt.time() : QDateTime::currentDateTime().time());

    ui->tbHopfenseihenVerlust->setValue(mSud->getWuerzemengeVorHopfenseihen() - mSud->getWuerzemengeKochende());

    if (!ui->tbSWHefestarterBrix->hasFocus())
        ui->tbSWHefestarterBrix->setValue(BierCalc::platoToBrix(mSud->getSWHefestarter()));
    if (!ui->tbWasserVerschneidungBrix->hasFocus())
        ui->tbWasserVerschneidungBrix->setValue(BierCalc::platoToBrix(ui->tbWasserVerschneidungSW->value()));
    value = BierCalc::verschneidung(mSud->getSWAnstellen(),
                                    mSud->getSWSollAnstellen(),
                                    ui->tbWasserVerschneidungSW->value(),
                                    mSud->getWuerzemengeAnstellenTotal());
    ui->tbWasserVerschneidung->setValue(value);
    ui->btnVerduennung->setVisible(status == Brauhelfer::SudStatus::Rezept && value > 0);

    // ModuleSpeise
    if (gSettings->isModuleEnabled(Settings::ModuleSpeise))
    {
        value = BierCalc::speise(mSud->getCO2(),
                                 mSud->getSWAnstellen(),
                                 ui->tbSpeiseSRE->value(),
                                 ui->tbSpeiseSRE->value(),
                                 ui->tbSpeiseT->value());
        ui->tbSpeisemengeNoetig->setValue(value * mSud->getWuerzemengeAnstellenTotal()/(1+value));
        ui->btnSpeisemengeNoetig->setVisible(status == Brauhelfer::SudStatus::Rezept && qAbs(ui->tbSpeisemenge->value() - ui->tbSpeisemengeNoetig->value()) > 0.25);
    }

    double mengeSollKochbeginn100 = BierCalc::volumenWasser(20.0, ui->tbTempKochbeginn->value(), mSud->getMengeSollKochbeginn());
    ui->tbMengeSollKochbeginn100->setValue(mengeSollKochbeginn100);
    double mengeSollKochende100 = BierCalc::volumenWasser(20.0, ui->tbTempKochende->value(), mSud->getMengeSollKochende());
    ui->tbMengeSollKochende100->setValue(mengeSollKochende100);

    // ModuleAusruestung
    ui->cbDurchschnittIgnorieren->setChecked(mSud->getAusbeuteIgnorieren());
    if (ui->cbDurchschnittIgnorieren->isChecked() || !gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        ui->lblDurchschnittWarnung->setVisible(false);
    }
    else
    {
        double h;
        double d = pow(mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
        ui->tbMengeSollcmVomBoden->setValue(mengeSollKochbeginn100 / d);
        h = mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
        ui->tbMengeSollcmVonOben->setValue(h - mengeSollKochbeginn100 / d);
        ui->tbMengeSollEndecmVomBoden->setValue(mengeSollKochende100 / d);
        h = mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
        ui->tbMengeSollEndecmVonOben->setValue(h - mengeSollKochende100 / d);
        ui->lblDurchschnittWarnung->setVisible(mSud->getSWAnteilZusatzMaischen() > 0 || mSud->getSWAnteilZusatzKochen() > 0);
    }

    ui->wdgSWSollKochbeginnMitWz->setVisible(mSud->getSWAnteilZusatzKochen() > 0.0);
    ui->tbSWSollKochbeginnBrix->setValue(BierCalc::platoToBrix(mSud->getSWSollKochbeginn()));
    ui->tbSWSollKochbeginnMitWzBrix->setValue(BierCalc::platoToBrix(mSud->getSWSollKochbeginnMitWz()));
    ui->tbSWSollKochendeBrix->setValue(BierCalc::platoToBrix(mSud->getSWSollKochende()));
    ui->tbSWAnstellenSollBrix->setValue(BierCalc::platoToBrix(mSud->getSWSollAnstellen()));

    ui->chartBraudaten->update(mSud);
    ui->chartAusbeute->update(mSud);
}

void TabBraudaten::on_tbBraudatum_dateChanged(const QDate &date)
{
    if (ui->tbBraudatum->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBraudatum, QDateTime(date, ui->tbBraudatumZeit->time())));
}

void TabBraudaten::on_tbBraudatumZeit_timeChanged(const QTime &time)
{
    if (ui->tbBraudatumZeit->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBraudatum, QDateTime(ui->tbBraudatum->date(), time)));
}

void TabBraudaten::on_btnBraudatumHeute_clicked()
{
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBraudatum, QDateTime()));
}

void TabBraudaten::on_btnSWKochbeginn_clicked()
{
    DlgRestextrakt dlg(mSud->getSWKochbeginn());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWKochbeginn, dlg.value()));
}

void TabBraudaten::on_btnWuerzemengeKochbeginn_clicked()
{
    double d = 0, h = 0;
    if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        d = mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
        h = mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    }
    DlgVolumen dlg(d, h, this);
    dlg.setValue(ui->tbWuerzemengeKochbeginn->value());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColWuerzemengeKochbeginn, dlg.value()));
}

void TabBraudaten::on_tbTempKochbeginn_valueChanged(double)
{
    if (ui->tbTempKochbeginn->hasFocus())
        updateValues();
}

void TabBraudaten::on_btnWuerzemengeVorHopfenseihen_clicked()
{
    double d = 0, h = 0;
    if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        d = mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
        h = mSud->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    }
    DlgVolumen dlg(d, h, this);
    dlg.setValue(ui->tbWuerzemengeVorHopfenseihen->value());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColWuerzemengeVorHopfenseihen, dlg.value()));
}

void TabBraudaten::on_btnWuerzemengeKochende_clicked()
{
    DlgVolumen dlg(this);
    dlg.setValue(ui->tbWuerzemengeKochende->value());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColWuerzemengeKochende, dlg.value()));
}

void TabBraudaten::on_tbTempKochende_valueChanged(double)
{
    if (ui->tbTempKochende->hasFocus())
        updateValues();
}

void TabBraudaten::on_btnSWKochende_clicked()
{
    DlgRestextrakt dlg(mSud->getSWKochende());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWKochende, dlg.value()));
}

void TabBraudaten::on_btnSWAnstellen_clicked()
{
    DlgRestextrakt dlg(mSud->getSWAnstellen());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWAnstellen, dlg.value()));
}

void TabBraudaten::on_btnWuerzemengeAnstellenTotal_clicked()
{
    DlgVolumen dlg(this);
    dlg.setValue(mSud->getWuerzemengeAnstellenTotal());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColWuerzemengeAnstellenTotal, dlg.value()));
}

void TabBraudaten::on_btnWuerzemengeAnstellen_clicked()
{
    DlgVolumen dlg(this);
    dlg.setValue(mSud->getWuerzemengeAnstellen());
    if (dlg.exec() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColWuerzemengeAnstellen, dlg.value()));
}

void TabBraudaten::on_tbSpeiseSRE_valueChanged(double)
{
    if (ui->tbSpeiseSRE->hasFocus())
        updateValues();
}

void TabBraudaten::on_tbSpeiseT_valueChanged(double)
{
    if (ui->tbSpeiseT->hasFocus())
        updateValues();
}

void TabBraudaten::on_tbWasserVerschneidungSW_valueChanged(double)
{
    if (ui->tbWasserVerschneidungSW->hasFocus())
        updateValues();
}

void TabBraudaten::on_tbSWHefestarterBrix_valueChanged(double value)
{
    if (ui->tbSWHefestarterBrix->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWHefestarter, BierCalc::brixToPlato(value)));
}

void TabBraudaten::on_tbWasserVerschneidungBrix_valueChanged(double value)
{
    if (ui->tbWasserVerschneidungBrix->hasFocus())
    {
        ui->tbWasserVerschneidungSW->setValue(BierCalc::brixToPlato(value));
        updateValues();
    }
}

void TabBraudaten::on_btnVerduennung_clicked()
{
    WidgetDecorator::suspendValueChangedClear = true;
    gUndoStack->beginMacro(QStringLiteral("macro"));
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColVerduennungAnstellen, ui->tbWasserVerschneidung->value()));
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWAnstellen, ui->tbSWAnstellenSoll->value()));
    gUndoStack->endMacro();
    WidgetDecorator::suspendValueChangedClear = false;
}

void TabBraudaten::on_btnSpeisemengeNoetig_clicked()
{
    WidgetDecorator::suspendValueChangedClear = true;
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSpeisemenge, ui->tbSpeisemengeNoetig->value()));
    WidgetDecorator::suspendValueChangedClear = false;
}

void TabBraudaten::on_cbDurchschnittIgnorieren_clicked(bool checked)
{
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColAusbeuteIgnorieren, checked));
}

void TabBraudaten::on_btnSudGebraut_clicked()
{
    QDateTime dt = mSud->getBraudatum();
    if (!dt.isValid())
        dt = QDateTime::currentDateTime();
    QString dtStr = QLocale().toString(dt, QLocale::ShortFormat);
    if (QMessageBox::question(this, tr("Stadium auf \"gebraut\" setzen?"),
                              tr("Soll das Stadium auf \"gebraut\" gesetzt werden?\nBraudatum: %1").arg(dtStr),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
    {
        gUndoStack->beginMacro(QStringLiteral("macro"));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBraudatum, dt));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColStatus, static_cast<int>(Brauhelfer::SudStatus::Gebraut)));
        gUndoStack->endMacro();

        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            DlgRohstoffeAbziehen dlg(mSud, true, this);
            dlg.exec();
        }

        if (mSud->modelSchnellgaerverlauf()->rowCount() == 0)
        {
            QMap<int, QVariant> values({{ModelSchnellgaerverlauf::ColSudID, mSud->id()},
                                        {ModelSchnellgaerverlauf::ColZeitstempel, mSud->getBraudatum()},
                                        {ModelSchnellgaerverlauf::ColRestextrakt, mSud->getSWIst()}});
            mSud->modelSchnellgaerverlauf()->append(values);
        }

        if (mSud->modelHauptgaerverlauf()->rowCount() == 0)
        {
            QMap<int, QVariant> values({{ModelHauptgaerverlauf::ColSudID, mSud->id()},
                                        {ModelHauptgaerverlauf::ColZeitstempel, mSud->getBraudatum()},
                                        {ModelHauptgaerverlauf::ColRestextrakt, mSud->getSWIst()}});
            mSud->modelHauptgaerverlauf()->append(values);
        }
    }
}

void TabBraudaten::on_btnSudGebrautReset_clicked()
{
    if (QMessageBox::question(this, tr("Stadium \"gebraut\" zurücksetzen?"),
                              tr("Soll das Stadium \"gebraut\" zurückgesetzt werden?"),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
    {
        mSud->setStatus(static_cast<int>(Brauhelfer::SudStatus::Rezept));
        if (mSud->modelSchnellgaerverlauf()->rowCount() == 1)
            mSud->modelSchnellgaerverlauf()->removeRow(0);
        if (mSud->modelHauptgaerverlauf()->rowCount() == 1)
            mSud->modelHauptgaerverlauf()->removeRow(0);
        if (mSud->modelNachgaerverlauf()->rowCount() == 1)
            mSud->modelNachgaerverlauf()->removeRow(0);
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            DlgRohstoffeAbziehen dlg(mSud, false, this);
            dlg.exec();
        }
    }
}
