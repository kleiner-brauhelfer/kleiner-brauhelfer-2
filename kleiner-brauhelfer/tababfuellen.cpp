#include "tababfuellen.h"
#include "ui_tababfuellen.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"
#include "dialogs/dlgrestextrakt.h"
#include "commands/undostack.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabAbfuellen::TabAbfuellen(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabAbfuellen)
{
    ui->setupUi(this);
    ui->tbReifezeit->setColumn(ModelSud::ColWoche);
    ui->tbSWSchnellgaerprobe->setColumn(ModelSud::ColSWSchnellgaerprobe);
    ui->tbSWJungbier->setColumn(ModelSud::ColSWJungbier);
    ui->tbSWJungbierSoll->setColumn(ModelSud::ColSREErwartet);
    ui->tbBiermengeAbfuellen->setColumn(ModelSud::Colerg_AbgefuellteBiermenge);
    ui->tbJungbiermengeAbfuellen->setColumn(ModelSud::ColJungbiermengeAbfuellen);
    ui->tbSpeisemengeAbgefuellt->setColumn(ModelSud::ColSpeisemenge);
    ui->tbTemperaturJungbier->setColumn(ModelSud::ColTemperaturJungbier);
    ui->tbNebenkosten->setColumn(ModelSud::ColKostenWasserStrom);
    ui->tbSw->setColumn(ModelSud::ColSWIst);
    ui->tbEVG->setColumn(ModelSud::ColsEVG);
    ui->tbEVGRezept->setColumn(ModelSud::ColVergaerungsgrad);
    ui->tbGruenschlauchzeitpunkt->setColumn(ModelSud::ColGruenschlauchzeitpunkt);
    ui->tbAlkohol->setColumn(ModelSud::Colerg_Alkohol);
    ui->tbAlkoholRezept->setColumn(ModelSud::ColAlkoholSoll);
    ui->tbSpundungsdruck->setColumn(ModelSud::ColSpundungsdruck);
    ui->tbTemperaturKarbonisierung->setColumn(ModelSud::ColTemperaturKarbonisierung);
    ui->tbWassserZuckerloesung->setColumn(ModelSud::ColVerschneidungAbfuellen);
    ui->tbKosten->setColumn(ModelSud::Colerg_Preis);
    ui->lblNebenkostenEinheit->setText(QLocale().currencySymbol());
    ui->lblKostenEinheit->setText(QLocale().currencySymbol() + "/L");

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    ui->wdgBemerkungAbfuellen->setPlaceholderText(tr("Bemerkung Abfüllen"));
    ui->wdgBemerkungGaerung->setPlaceholderText(tr("Bemerkung Gärung & Reifung"));

    gSettings->beginGroup("TabAbfuellen");

    ui->splitter->setSizes({100, 400});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterCharts->setSizes({300, 300, 100, 50});
    mDefaultSplitterChartsState = ui->splitterCharts->saveState();
    ui->splitterCharts->restoreState(gSettings->value("splitterChartsState").toByteArray());

    ui->tbZuckerFaktor->setValue(gSettings->value("ZuckerFaktor", 1.0).toDouble());
    ui->tbFlaschengroesse->setValue(gSettings->value("FlaschenGroesse", 0.5).toDouble());

    gSettings->endGroup();

    connect(qApp, &QApplication::focusChanged, this, &TabAbfuellen::focusChanged);
    connect(bh, &Brauhelfer::modified, this, &TabAbfuellen::updateValues);
    connect(bh, &Brauhelfer::discarded, this, &TabAbfuellen::sudLoaded);
    connect(bh->sud(), &SudObject::loadedChanged, this, &TabAbfuellen::sudLoaded);
    connect(bh->sud(), &SudObject::dataChanged, this, &TabAbfuellen::sudDataChanged);

    connect(ui->wdgBemerkungAbfuellen, &WdgBemerkung::changed, this, [](const QString& html){gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColBemerkungAbfuellen, html));});
    connect(ui->wdgBemerkungGaerung, &WdgBemerkung::changed, this, [](const QString& html){gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColBemerkungGaerung, html));});
}

TabAbfuellen::~TabAbfuellen()
{
    delete ui;
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
        setVisibleModule(Settings::ModulePreiskalkulation,
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
        setVisibleModule(Settings::ModuleSpeise,
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
        setVisibleModule(Settings::ModuleSchnellgaerprobe,
                         {ui->cbSchnellgaerprobeAktiv});
    }
    if (bh->sud()->isLoaded())
    {
        checkEnabled();
        updateValues();
    }
}

void TabAbfuellen::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && isAncestorOf(now) && now != ui->tbHelp && !qobject_cast<QSplitter*>(now))
        ui->tbHelp->setHtml(now->toolTip());
}

void TabAbfuellen::sudLoaded()
{
    checkEnabled();
    updateValues();
    ui->wdgBemerkungAbfuellen->setHtml(bh->sud()->getBemerkungAbfuellen());
    ui->wdgBemerkungGaerung->setHtml(bh->sud()->getBemerkungGaerung());
}

void TabAbfuellen::sudDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        checkEnabled();
        break;
    case ModelSud::ColBemerkungAbfuellen:
        ui->wdgBemerkungAbfuellen->setHtml(bh->sud()->getBemerkungAbfuellen());
        break;
    case ModelSud::ColBemerkungGaerung:
        ui->wdgBemerkungGaerung->setHtml(bh->sud()->getBemerkungGaerung());
        break;
    }
}

void TabAbfuellen::onTabActivated()
{
    updateValues();
}

void TabAbfuellen::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
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
    ui->btnSudVerbraucht->setEnabled(status == Brauhelfer::SudStatus::Abgefuellt && !gSettings->ForceEnabled);
}

void TabAbfuellen::updateValues()
{
    if (!isTabActive())
        return;

    for (auto& wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();
    for (auto& wdg : findChildren<SpinBoxSud*>())
        wdg->updateValue();

    QDateTime dt = bh->sud()->getAbfuelldatum();
    ui->tbAbfuelldatum->setMinimumDate(bh->sud()->getBraudatum().date());
    ui->tbAbfuelldatum->setDate(dt.isValid() ? dt.date() : QDateTime::currentDateTime().date());
    ui->tbAbfuelldatumZeit->setTime(dt.isValid() ? dt.time() : QDateTime::currentDateTime().time());
    ui->tbDauerHauptgaerung->setValue((int)bh->sud()->getBraudatum().daysTo(ui->tbAbfuelldatum->dateTime()));
    dt = bh->sud()->getReifungStart();
    ui->tbReifung->setMinimumDate(ui->tbAbfuelldatum->date());
    ui->tbReifung->setDate(dt.isValid() ? dt.date() : QDateTime::currentDateTime().date());

    ui->cbSchnellgaerprobeAktiv->setChecked(bh->sud()->getSchnellgaerprobeAktiv() && gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->tbSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblSWSchnellgaerprobeEinheit->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->btnSWSchnellgaerprobe->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->tbGruenschlauchzeitpunkt->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblGruenschlauchzeitpunkt->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->lblGruenschlauchzeitpunktEinheit->setVisible(ui->cbSchnellgaerprobeAktiv->isChecked());
    ui->tbAlkoholGaerung->setValue(BierCalc::alkohol(bh->sud()->getSWIst(), bh->sud()->getSREIst()));

    ui->cbSpunden->setChecked(bh->sud()->getSpunden());
    ui->tbJungbierVerlust->setValue(bh->sud()->getWuerzemengeAnstellen() - bh->sud()->getJungbiermengeAbfuellen());

    ui->groupKarbonisierung->setVisible(!ui->cbSpunden->isChecked());
    double flascheFaktor = ui->tbFlaschengroesse->value() / bh->sud()->getJungbiermengeAbfuellen();

    // ModuleSpeise
    if (gSettings->isModuleEnabled(Settings::ModuleSpeise))
    {
        ui->tbSpeisemengeGesamt->setValue((int)bh->sud()->getSpeiseAnteil());
        ui->tbSpeisemengeGesamt->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
        ui->lblSpeisemengeGesamt->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
        ui->lblSpeisemengeGesamtEinheit->setVisible(ui->tbSpeisemengeGesamt->value() > 0.0);
        ui->tbSpeisemengeFlasche->setValue(ui->tbSpeisemengeGesamt->value() * flascheFaktor);
        ui->tbSpeisemengeFlasche->setVisible(ui->tbSpeisemengeFlasche->value() > 0.0);
        ui->lblSpeisemengeFlasche->setVisible(ui->tbSpeisemengeFlasche->value() > 0.0);
        ui->lblSpeisemengeFlascheEinheit->setVisible(ui->tbSpeisemengeFlasche->value() > 0.0);
    }

    ui->tbZuckerGesamt->setValue((int)(bh->sud()->getZuckerAnteil() / ui->tbZuckerFaktor->value()));
    ui->tbZuckerFlasche->setValue(ui->tbZuckerGesamt->value() * flascheFaktor);
    ui->tbFlaschen->setValue(bh->sud()->geterg_AbgefuellteBiermenge() / ui->tbFlaschengroesse->value());
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

    ui->chartAbfuelldaten->update();
    ui->chartRestextrakt->update();
}

void TabAbfuellen::on_tbAbfuelldatum_dateChanged(const QDate &date)
{
    if (ui->tbAbfuelldatum->hasFocus())
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColAbfuelldatum, QDateTime(date, ui->tbAbfuelldatumZeit->time())));
}

void TabAbfuellen::on_tbAbfuelldatumZeit_timeChanged(const QTime &time)
{
    if (ui->tbAbfuelldatumZeit->hasFocus())
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColAbfuelldatum, QDateTime(ui->tbAbfuelldatum->date(), time)));
}

void TabAbfuellen::on_btnAbfuelldatumHeute_clicked()
{
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColAbfuelldatum, QDateTime()));
}

void TabAbfuellen::on_tbReifung_dateChanged(const QDate &date)
{
    if (ui->tbReifung->hasFocus())
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColReifungStart, QDateTime(date, QTime())));
}

void TabAbfuellen::on_btnReifungHeute_clicked()
{
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColReifungStart, QDateTime()));
}

void TabAbfuellen::on_cbSchnellgaerprobeAktiv_clicked(bool checked)
{
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColSchnellgaerprobeAktiv, checked));
}

void TabAbfuellen::on_btnSWSchnellgaerprobe_clicked()
{
    DlgRestextrakt dlg(ui->tbSWSchnellgaerprobe->value(),
                       bh->sud()->getSWIst(),
                       ui->tbTemperaturJungbier->value(),
                       QDateTime(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
    {
        gUndoStack->beginMacro(QStringLiteral("macro"));
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColTemperaturJungbier, dlg.temperatur()));
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColSWSchnellgaerprobe, dlg.value()));
        gUndoStack->endMacro();
    }
}

void TabAbfuellen::on_btnSWJungbier_clicked()
{
    DlgRestextrakt dlg(ui->tbSWJungbier->value(),
                       bh->sud()->getSWIst(),
                       ui->tbTemperaturJungbier->value(),
                       QDateTime(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
    {
        gUndoStack->beginMacro(QStringLiteral("macro"));
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColTemperaturJungbier, dlg.temperatur()));
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColSWJungbier, dlg.value()));
        gUndoStack->endMacro();
    }
}

void TabAbfuellen::on_cbSpunden_clicked(bool checked)
{
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColSpunden, checked));
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
    if (!bh->sud()->getAbfuellenBereitZutaten())
    {
        if (QMessageBox::warning(this, tr("Zutaten Gärung"),
                             tr("Es wurden noch nicht alle Zutaten für die Gärung zugegeben oder entnommen.")
                             + "\n" + tr("Soll der Sud trotzdem als abgefüllt markiert werden?"),
                             QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    }

    if (bh->sud()->getSchnellgaerprobeAktiv())
    {
        if (bh->sud()->getSWJungbier() > bh->sud()->getGruenschlauchzeitpunkt())
        {
            if (QMessageBox::warning(this, tr("Grünschlauchzeitpunkt nicht erreicht"),
                                 tr("Der Grünschlauchzeitpunkt wurde noch nicht erreicht.")
                                 + "\n" + tr("Soll der Sud trotzdem als abgefüllt markiert werden?"),
                                 QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
        }
        else if (bh->sud()->getSWJungbier() < bh->sud()->getSWSchnellgaerprobe())
        {
            if (QMessageBox::warning(this, tr("Schnellgärprobe"),
                                 tr("Die Stammwürze des Jungbiers liegt tiefer als die der Schnellgärprobe.")
                                 + "\n" + tr("Soll der Sud trotzdem als abgefüllt markiert werden?"),
                                 QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
        }
    }

    QDateTime dt(ui->tbAbfuelldatum->date(), ui->tbAbfuelldatumZeit->time());
    QString dtStr = QLocale().toString(dt, QLocale::ShortFormat);
    if (QMessageBox::question(this, tr("Sud als abgefüllt markieren?"),
                                    tr("Soll der Sud als abgefüllt markiert werden?\n\nAbfülldatum: %1").arg(dtStr),
                                    QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
        return;

    gUndoStack->beginMacro(QStringLiteral("macro"));
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColAbfuelldatum, dt));
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColReifungStart, ui->tbReifung->dateTime()));
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColStatus, static_cast<int>(Brauhelfer::SudStatus::Abgefuellt)));
    gUndoStack->endMacro();

    QMap<int, QVariant> values({{ModelNachgaerverlauf::ColSudID, bh->sud()->id()},
                                {ModelNachgaerverlauf::ColZeitstempel, bh->sud()->getAbfuelldatum()},
                                {ModelNachgaerverlauf::ColDruck, 0.0},
                                {ModelNachgaerverlauf::ColTemp, bh->sud()->getTemperaturJungbier()}});
    if (bh->sud()->modelNachgaerverlauf()->rowCount() == 0)
        bh->sud()->modelNachgaerverlauf()->append(values);
}

void TabAbfuellen::on_btnSudVerbraucht_clicked()
{
    if (QMessageBox::question(this, tr("Sud als verbraucht markieren?"),
                                    tr("Soll der Sud als verbraucht markiert werden?"),
                                    QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
        return;
    gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), ModelSud::ColStatus, static_cast<int>(Brauhelfer::SudStatus::Verbraucht)));
}
