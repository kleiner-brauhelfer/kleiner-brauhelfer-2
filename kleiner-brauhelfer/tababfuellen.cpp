#include "tababfuellen.h"
#include "ui_tababfuellen.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"
#include "dialogs/dlgrestextrakt.h"
#include "commands/undostack.h"

extern Settings* gSettings;

TabAbfuellen::TabAbfuellen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabAbfuellen),
    mSud(nullptr)
{
    ui->setupUi(this);
}

TabAbfuellen::~TabAbfuellen()
{
    delete ui;
}

void TabAbfuellen::setup(SudObject *sud)
{
    mSud = sud;

    ui->tbReifezeit->setColumn(mSud, ModelSud::ColWoche);
    ui->tbSWSchnellgaerprobe->setColumn(mSud, ModelSud::ColSWSchnellgaerprobe);
    ui->tbSWJungbier->setColumn(mSud, ModelSud::ColSWJungbier);
    ui->tbSWJungbierSoll->setColumn(mSud, ModelSud::ColSREErwartet);
    ui->tbBiermengeAbfuellen->setColumn(mSud, ModelSud::Colerg_AbgefuellteBiermenge);
    ui->tbJungbiermengeAbfuellen->setColumn(mSud, ModelSud::ColJungbiermengeAbfuellen);
    ui->tbSpeisemengeAbgefuellt->setColumn(mSud, ModelSud::ColSpeisemenge);
    ui->tbTemperaturJungbier->setColumn(mSud, ModelSud::ColTemperaturJungbier);
    ui->tbNebenkosten->setColumn(mSud, ModelSud::ColKostenWasserStrom);
    ui->tbSw->setColumn(mSud, ModelSud::ColSWIst);
    ui->tbEVG->setColumn(mSud, ModelSud::ColsEVG);
    ui->tbEVGRezept->setColumn(mSud, ModelSud::ColVergaerungsgrad);
    ui->tbGruenschlauchzeitpunkt->setColumn(mSud, ModelSud::ColGruenschlauchzeitpunkt);
    ui->tbAlkohol->setColumn(mSud, ModelSud::Colerg_Alkohol);
    ui->tbAlkoholRezept->setColumn(mSud, ModelSud::ColAlkoholSoll);
    ui->tbSpundungsdruck->setColumn(mSud, ModelSud::ColSpundungsdruck);
    ui->tbTemperaturKarbonisierung->setColumn(mSud, ModelSud::ColTemperaturKarbonisierung);
    ui->tbWassserZuckerloesung->setColumn(mSud, ModelSud::ColVerschneidungAbfuellen);
    ui->tbKosten->setColumn(mSud, ModelSud::Colerg_Preis);
    ui->lblNebenkostenEinheit->setText(QLocale().currencySymbol());
    ui->lblKostenEinheit->setText(QLocale().currencySymbol() + "/L");

    ui->wdgBemerkungAbfuellen->setSudObject(mSud);
    ui->wdgBemerkungAbfuellen->setPlaceholderText(tr("Bemerkung Abfüllen"));
    ui->wdgBemerkungGaerung->setSudObject(mSud);
    ui->wdgBemerkungGaerung->setPlaceholderText(tr("Bemerkung Gärung & Reifung"));

    gSettings->beginGroup("TabAbfuellen");

    ui->splitter->setSizes({100, 400});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterCharts->setSizes({300, 300, 100});
    mDefaultSplitterChartsState = ui->splitterCharts->saveState();
    ui->splitterCharts->restoreState(gSettings->value("splitterChartsState").toByteArray());

    ui->tbZuckerFaktor->setValue(gSettings->value("ZuckerFaktor", 1.0).toDouble());
    ui->tbFlaschengroesse->setValue(gSettings->value("FlaschenGroesse", 0.5).toDouble());

    gSettings->endGroup();

    connect(mSud->bh(), &Brauhelfer::modified, this, &TabAbfuellen::updateValues);
    connect(mSud->bh(), &Brauhelfer::discarded, this, &TabAbfuellen::sudLoaded);
    connect(mSud, &SudObject::loadedChanged, this, &TabAbfuellen::sudLoaded);
    connect(mSud, &SudObject::dataChanged, this, &TabAbfuellen::sudDataChanged);

    connect(ui->wdgBemerkungAbfuellen, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungAbfuellen, html));});
    connect(ui->wdgBemerkungGaerung, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungGaerung, html));});
}

void TabAbfuellen::saveSettings()
{
    gSettings->beginGroup("TabAbfuellen");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterChartsState", ui->splitterCharts->saveState());
    gSettings->setValue("ZuckerFaktor", ui->tbZuckerFaktor->value());
    gSettings->setValue("FlaschenGroesse", ui->tbFlaschengroesse->value());
    gSettings->endGroup();
}

void TabAbfuellen::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterCharts->restoreState(mDefaultSplitterChartsState);
}

void TabAbfuellen::modulesChanged(Settings::Modules modules)
{
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
    if (modules.testFlag(Settings::ModuleSpeise))
    {
        gSettings->setVisibleModule(Settings::ModuleSpeise,
                         {ui->tbSpeisemengeAbgefuellt,
                          ui->lblSpeisemengeAbgefuellt,
                          ui->lblSpeisemengeAbgefuelltEinheit,
                          ui->tbSpeisemengeGesamt,
                          ui->lblSpeisemengeGesamt,
                          ui->lblSpeisemengeGesamtEinheit,
                          ui->tbSpeisemengeFlasche,
                          ui->lblSpeisemengeFlasche,
                          ui->lblSpeisemengeFlascheEinheit});
    }
    if (modules.testFlag(Settings::ModuleSchnellgaerprobe))
    {
        gSettings->setVisibleModule(Settings::ModuleSchnellgaerprobe,
                         {ui->cbSchnellgaerprobeAktiv});
    }
    if (mSud->isLoaded())
    {
        checkEnabled();
        updateValues();
    }
}

void TabAbfuellen::sudLoaded()
{
    checkEnabled();
    updateValues();
    ui->wdgBemerkungAbfuellen->setHtml(mSud->getBemerkungAbfuellen());
    ui->wdgBemerkungGaerung->setHtml(mSud->getBemerkungGaerung());
}

void TabAbfuellen::sudDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        checkEnabled();
        break;
    case ModelSud::ColBemerkungAbfuellen:
        ui->wdgBemerkungAbfuellen->setHtml(mSud->getBemerkungAbfuellen());
        break;
    case ModelSud::ColBemerkungGaerung:
        ui->wdgBemerkungGaerung->setHtml(mSud->getBemerkungGaerung());
        break;
    }
}

void TabAbfuellen::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateValues();
}

void TabAbfuellen::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    bool abgefuellt = status >= Brauhelfer::SudStatus::Abgefuellt && !gSettings->ForceEnabled;
    bool verbraucht = status >= Brauhelfer::SudStatus::Verbraucht && !gSettings->ForceEnabled;
    ui->tbAbfuelldatum->setReadOnly(abgefuellt);
    ui->tbAbfuelldatumZeit->setReadOnly(abgefuellt);
    ui->btnAbfuelldatumHeute->setVisible(!abgefuellt);
    ui->tbReifung->setReadOnly(verbraucht);
    ui->btnReifungHeute->setVisible(!abgefuellt);
    ui->tbReifezeit->setVisible(!verbraucht);
    ui->lblReifezeit->setVisible(!verbraucht);
    ui->lblReifezeitEinheit->setVisible(!verbraucht);
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
    ui->tbZuckerFaktor->setReadOnly(abgefuellt);
    ui->tbWassserZuckerloesung->setReadOnly(abgefuellt);
    ui->tbTemperaturKarbonisierung->setReadOnly(abgefuellt);
    ui->tbFlaschengroesse->setReadOnly(abgefuellt);
    ui->tbNebenkosten->setReadOnly(abgefuellt);
    ui->btnSudAbgefuellt->setEnabled(status == Brauhelfer::SudStatus::Gebraut && !gSettings->ForceEnabled);
    ui->btnSudAbgefuelltReset->setEnabled(status == Brauhelfer::SudStatus::Abgefuellt);
    ui->btnSudVerbraucht->setEnabled(status == Brauhelfer::SudStatus::Abgefuellt && !gSettings->ForceEnabled);
    ui->btnSudVerbrauchtReset->setEnabled(status == Brauhelfer::SudStatus::Verbraucht);
}

void TabAbfuellen::updateValues()
{
    if (!isVisible())
        return;

    for (auto& wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();
    for (auto& wdg : findChildren<SpinBoxSud*>())
        wdg->updateValue();

    QDateTime dt = mSud->getAbfuelldatum();
    ui->tbAbfuelldatum->setMinimumDate(mSud->getBraudatum().date());
    ui->tbAbfuelldatum->setDate(dt.isValid() ? dt.date() : QDateTime::currentDateTime().date());
    ui->tbAbfuelldatumZeit->setTime(dt.isValid() ? dt.time() : QDateTime::currentDateTime().time());
    ui->tbDauerHauptgaerung->setValue((int)mSud->getBraudatum().daysTo(ui->tbAbfuelldatum->dateTime()));
    dt = mSud->getReifungStart();
    ui->tbReifung->setMinimumDate(ui->tbAbfuelldatum->date());
    ui->tbReifung->setDate(dt.isValid() ? dt.date() : QDateTime::currentDateTime().date());

    ui->cbSchnellgaerprobeAktiv->setChecked(mSud->getSchnellgaerprobeAktiv() && gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->tbSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblSWSchnellgaerprobeEinheit->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->btnSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->tbGruenschlauchzeitpunkt->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblGruenschlauchzeitpunkt->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblGruenschlauchzeitpunktEinheit->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->tbAlkoholGaerung->setValue(BierCalc::alkohol(mSud->getSWIst(), mSud->getSREIst()));

    ui->cbSpunden->setChecked(mSud->getSpunden());
    ui->tbJungbierVerlust->setValue(mSud->getWuerzemengeAnstellen() - mSud->getJungbiermengeAbfuellen());

    ui->groupKarbonisierung->setVisible(!ui->cbSpunden->isChecked());
    double flascheFaktor = ui->tbFlaschengroesse->value() / mSud->getJungbiermengeAbfuellen();

    // ModuleSpeise
    if (gSettings->isModuleEnabled(Settings::ModuleSpeise))
    {
        ui->tbSpeisemengeGesamt->setValue((int)mSud->getSpeiseAnteil());
        ui->tbSpeisemengeGesamt->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
        ui->lblSpeisemengeGesamt->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
        ui->lblSpeisemengeGesamtEinheit->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
        ui->tbSpeisemengeFlasche->setValue(ui->tbSpeisemengeGesamt->value() * flascheFaktor);
        ui->tbSpeisemengeFlasche->setVisible(ui->tbSpeisemengeFlasche->value() > 0.0);
        ui->lblSpeisemengeFlasche->setVisible(ui->tbSpeisemengeFlasche->value() > 0.0);
        ui->lblSpeisemengeFlascheEinheit->setVisible(ui->tbSpeisemengeFlasche->value() > 0.0);
    }

    ui->tbZuckerGesamt->setValue((int)(mSud->getZuckerAnteil() / ui->tbZuckerFaktor->value()));
    ui->tbZuckerFlasche->setValue(ui->tbZuckerGesamt->value() * flascheFaktor);
    ui->tbFlaschen->setValue(mSud->geterg_AbgefuellteBiermenge() / ui->tbFlaschengroesse->value());
    ui->tbKonzentrationZuckerloesung->setValue(ui->tbZuckerGesamt->value() / ui->tbWassserZuckerloesung->value());
    bool hasZucker = ui->tbZuckerGesamt->value() > 0.0;
    ui->tbZuckerGesamt->setVisible(hasZucker);
    ui->lblZuckerGesamt->setVisible(hasZucker);
    ui->lblZuckerGesamtEinheit->setVisible(hasZucker);
    ui->tbZuckerFaktor->setVisible(hasZucker);
    ui->lblZuckerFaktor->setVisible(hasZucker);
    ui->tbZuckerFlasche->setVisible(hasZucker);
    ui->lblZuckerFlasche->setVisible(hasZucker);
    ui->lblZuckerFlascheEinheit->setVisible(hasZucker);
    ui->lblWassserZuckerloesung->setVisible(hasZucker);
    ui->tbWassserZuckerloesung->setVisible(hasZucker);
    ui->tbWassserZuckerloesungEinheit->setVisible(hasZucker);
    bool hasZuckerLoesung = ui->tbWassserZuckerloesung->value() > 0.0 && hasZucker;
    ui->lblKonzentrationZuckerloesung->setVisible(hasZuckerLoesung);
    ui->tbKonzentrationZuckerloesung->setVisible(hasZuckerLoesung);
    ui->tbKonzentrationZuckerloesungEinheit->setVisible(hasZuckerLoesung);

    ui->chartAbfuelldaten->update(mSud);
    ui->chartRestextrakt->update(mSud);
}

void TabAbfuellen::on_tbAbfuelldatum_dateChanged(const QDate &date)
{
    if (ui->tbAbfuelldatum->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColAbfuelldatum, QDateTime(date, ui->tbAbfuelldatumZeit->time())));
}

void TabAbfuellen::on_tbAbfuelldatumZeit_timeChanged(const QTime &time)
{
    if (ui->tbAbfuelldatumZeit->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColAbfuelldatum, QDateTime(ui->tbAbfuelldatum->date(), time)));
}

void TabAbfuellen::on_btnAbfuelldatumHeute_clicked()
{
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColAbfuelldatum, QDateTime()));
}

void TabAbfuellen::on_tbReifung_dateChanged(const QDate &date)
{
    if (ui->tbReifung->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColReifungStart, QDateTime(date, QTime())));
}

void TabAbfuellen::on_btnReifungHeute_clicked()
{
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColReifungStart, QDateTime()));
}

void TabAbfuellen::on_cbSchnellgaerprobeAktiv_clicked(bool checked)
{
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSchnellgaerprobeAktiv, checked));
}

void TabAbfuellen::on_btnSWSchnellgaerprobe_clicked()
{
    DlgRestextrakt dlg(ui->tbSWSchnellgaerprobe->value(),
                       mSud->getSWIst(),
                       ui->tbTemperaturJungbier->value(),
                       QDateTime(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
    {
        gUndoStack->beginMacro(QStringLiteral("macro"));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColTemperaturJungbier, dlg.temperatur()));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWSchnellgaerprobe, dlg.value()));
        gUndoStack->endMacro();
    }
}

void TabAbfuellen::on_btnSWJungbier_clicked()
{
    DlgRestextrakt dlg(ui->tbSWJungbier->value(),
                       mSud->getSWIst(),
                       ui->tbTemperaturJungbier->value(),
                       QDateTime(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
    {
        gUndoStack->beginMacro(QStringLiteral("macro"));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColTemperaturJungbier, dlg.temperatur()));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSWJungbier, dlg.value()));
        gUndoStack->endMacro();
    }
}

void TabAbfuellen::on_cbSpunden_clicked(bool checked)
{
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSpunden, checked));
}

void TabAbfuellen::on_tbZuckerFaktor_valueChanged(double)
{
    if (ui->tbZuckerFaktor->hasFocus())
        updateValues();
}

void TabAbfuellen::on_tbFlaschengroesse_valueChanged(double)
{
    if (ui->tbFlaschengroesse->hasFocus())
        updateValues();
}

void TabAbfuellen::on_btnSudAbgefuellt_clicked()
{
    if (!mSud->getAbfuellenBereitZutaten())
    {
        if (QMessageBox::warning(this, tr("Zutaten Gärung"),
                                 tr("Es wurden noch nicht alle Zutaten für die Gärung zugegeben oder entnommen.")
                                     + "\n" + tr("Soll der Sud trotzdem als abgefüllt markiert werden?"),
                                 QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }

    if (mSud->getSchnellgaerprobeAktiv())
    {
        if (mSud->getSWJungbier() > mSud->getGruenschlauchzeitpunkt())
        {
            if (QMessageBox::warning(this, tr("Grünschlauchzeitpunkt nicht erreicht"),
                                     tr("Der Grünschlauchzeitpunkt wurde noch nicht erreicht.")
                                         + "\n" + tr("Soll der Sud trotzdem als abgefüllt markiert werden?"),
                                     QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
                return;
        }
        else if (mSud->getSWJungbier() < mSud->getSWSchnellgaerprobe())
        {
            if (QMessageBox::warning(this, tr("Schnellgärprobe"),
                                     tr("Die Stammwürze des Jungbiers liegt tiefer als die der Schnellgärprobe.")
                                         + "\n" + tr("Soll der Sud trotzdem als abgefüllt markiert werden?"),
                                     QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
                return;
        }
    }

    QDateTime dt = mSud->getAbfuelldatum();
    if (!dt.isValid())
        dt = QDateTime::currentDateTime();
    QString dtStr = QLocale().toString(dt, QLocale::ShortFormat);
    if (QMessageBox::question(this, tr("Stadium auf \"abgefüllt\" setzen?"),
                              tr("Soll das Stadium auf \"abgefüllt\" gesetzt werden?\nAbfülldatum: %1").arg(dtStr),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
    {
        gUndoStack->beginMacro(QStringLiteral("macro"));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColAbfuelldatum, dt));
        QDateTime dtReifung = mSud->getReifungStart();
        if (!dtReifung.isValid())
            dtReifung = QDateTime::currentDateTime();
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColReifungStart, dtReifung));
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColStatus, static_cast<int>(Brauhelfer::SudStatus::Abgefuellt)));
        gUndoStack->endMacro();

        QMap<int, QVariant> values({{ModelNachgaerverlauf::ColSudID, mSud->id()},
                                    {ModelNachgaerverlauf::ColZeitstempel, mSud->getAbfuelldatum()},
                                    {ModelNachgaerverlauf::ColDruck, 0.0},
                                    {ModelNachgaerverlauf::ColTemp, mSud->getTemperaturJungbier()}});
        if (mSud->modelNachgaerverlauf()->rowCount() == 0)
            mSud->modelNachgaerverlauf()->append(values);
    }
}

void TabAbfuellen::on_btnSudAbgefuelltReset_clicked()
{
    if (QMessageBox::question(this, tr("Stadium \"abgefüllt\" zurücksetzen?"),
                              tr("Soll das Stadium \"abgefüllt\" zurückgesetzt werden?"),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
    {
        mSud->setStatus(static_cast<int>(Brauhelfer::SudStatus::Gebraut));
        if (mSud->modelNachgaerverlauf()->rowCount() == 1)
            mSud->modelNachgaerverlauf()->removeRow(0);
    }
}

void TabAbfuellen::on_btnSudVerbraucht_clicked()
{
    if (QMessageBox::question(this, tr("Stadium auf \"ausgetrunken\" setzen?"),
                              tr("Soll das Stadium auf \"ausgetrunken\" gesetzt werden?"),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
    {
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColStatus, static_cast<int>(Brauhelfer::SudStatus::Verbraucht)));
    }
}

void TabAbfuellen::on_btnSudVerbrauchtReset_clicked()
{
    if (QMessageBox::question(this, tr("Stadium \"ausgetrunken\" zurücksetzen?"),
                              tr("Soll das Stadium \"ausgetrunken\" zurückgesetzt werden?"),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes)
    {
        mSud->setStatus(static_cast<int>(Brauhelfer::SudStatus::Abgefuellt));
    }
}
