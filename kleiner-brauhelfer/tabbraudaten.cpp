#include "tabbraudaten.h"
#include "ui_tabbraudaten.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "settings.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgvolumen.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBraudaten::TabBraudaten(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabBraudaten)
{
    ui->setupUi(this);
    ui->tbSWKochbeginn->setColumn(ModelSud::ColSWKochbeginn);
    ui->tbWuerzemengeKochbeginn->setColumn(ModelSud::ColWuerzemengeKochbeginn);
    ui->tbWuerzemengeVorHopfenseihen->setColumn(ModelSud::ColWuerzemengeVorHopfenseihen);
    ui->tbWuerzemengeKochende->setColumn(ModelSud::ColWuerzemengeKochende);
    ui->tbSWKochende->setColumn(ModelSud::ColSWKochende);
    ui->tbSWAnstellen->setColumn(ModelSud::ColSWAnstellen);
    ui->tbVerduennung->setColumn(ModelSud::ColVerduennungAnstellen);
    ui->tbWuerzemengeAnstellen->setColumn(ModelSud::ColWuerzemengeAnstellen);
    ui->tbSpeisemenge->setColumn(ModelSud::ColSpeisemenge);
    ui->tbWuerzemengeAnstellenTotal->setColumn(ModelSud::ColWuerzemengeAnstellenTotal);
    ui->tbMengeSollKochbeginn20->setColumn(ModelSud::ColMengeSollKochbeginn);
    ui->tbSWSollKochbeginn->setColumn(ModelSud::ColSWSollKochbeginn);
    ui->tbSWSollKochbeginnMitWz->setColumn(ModelSud::ColSWSollKochbeginnMitWz);
    ui->tbMengeSollKochende20->setColumn(ModelSud::ColMengeSollKochende);
    ui->tbSWSollKochende->setColumn(ModelSud::ColSWSollKochende);
    ui->tbVerdampfung->setColumn(ModelSud::ColVerdampfungsrateIst);
    ui->tbAusbeute->setColumn(ModelSud::Colerg_Sudhausausbeute);
    ui->tbAusbeuteEffektiv->setColumn(ModelSud::Colerg_EffektiveAusbeute);
    ui->tbVerdampfungRezept->setColumn(ModelSud::ColVerdampfungsrate);
    ui->tbAusbeuteRezept->setColumn(ModelSud::ColSudhausausbeute);
    ui->tbSWAnstellenSoll->setColumn(ModelSud::ColSWSollAnstellen);
    ui->tbMengeHefestarter->setColumn(ModelSud::ColMengeHefestarter);
    ui->tbSWHefestarter->setColumn(ModelSud::ColSWHefestarter);
    ui->tbKosten->setColumn(ModelSud::Colerg_Preis);
    ui->tbNebenkosten->setColumn(ModelSud::ColKostenWasserStrom);
    ui->lblNebenkostenEinheit->setText(QLocale().currencySymbol());
    ui->lblKostenEinheit->setText(QLocale().currencySymbol() + "/" + tr("l"));
    ui->lblDurchschnittWarnung->setPalette(gSettings->paletteErrorLabel);

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabBraudaten");

    ui->splitter->setSizes({100, 400});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterCharts->setSizes({300, 300, 100, 50});
    mDefaultSplitterChartsState = ui->splitterCharts->saveState();
    ui->splitterCharts->restoreState(gSettings->value("splitterChartsState").toByteArray());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                    this, SLOT(sudDataChanged(QModelIndex)));

    connect(ui->wdgBemerkung, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setBemerkungBrauen(html);});
}

TabBraudaten::~TabBraudaten()
{
    delete ui;
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
        setVisibleModule(Settings::ModuleAusruestung,
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
        setVisibleModule(Settings::ModuleSpeise,
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
    checkEnabled();
    updateValues();
}

void TabBraudaten::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && isAncestorOf(now) && now != ui->tbHelp && !qobject_cast<QSplitter*>(now))
        ui->tbHelp->setHtml(now->toolTip());
}

void TabBraudaten::sudLoaded()
{
    checkEnabled();
    updateValues();
    ui->tbSpeiseSRE->setValue(bh->sud()->getSRESoll());
    ui->wdgBemerkung->setHtml(bh->sud()->getBemerkungBrauen());
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
        ui->tbSpeiseSRE->setValue(bh->sud()->getSRESoll());
        break;
    case ModelSud::ColBemerkungBrauen:
        ui->wdgBemerkung->setHtml(bh->sud()->getBemerkungBrauen());
        break;
    }
}

void TabBraudaten::onTabActivated()
{
    updateValues();
}

void TabBraudaten::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
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
    ui->tbTempKochbeginn->setReadOnly(gebraut);
    ui->tbTempKochende->setReadOnly(gebraut);
    ui->tbSpeiseSRE->setReadOnly(gebraut);
    ui->tbSpeiseT->setReadOnly(gebraut);
}

void TabBraudaten::updateValues()
{
    if (!isTabActive())
        return;

    for (auto& wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();

    double value;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());

    QDateTime dt = bh->sud()->getBraudatum();
    ui->tbBraudatum->setDate(dt.isValid() ? dt.date() : QDateTime::currentDateTime().date());
    ui->tbBraudatumZeit->setTime(dt.isValid() ? dt.time() : QDateTime::currentDateTime().time());

    ui->tbHopfenseihenVerlust->setValue(bh->sud()->getWuerzemengeVorHopfenseihen() - bh->sud()->getWuerzemengeKochende());

    value = BierCalc::verschneidung(bh->sud()->getSWAnstellen(),
                                    bh->sud()->getSWSollAnstellen(),
                                    bh->sud()->getWuerzemengeAnstellenTotal());
    ui->tbWasserVerschneidung->setValue(value);
    ui->wdgWasserVerschneidung->setVisible(status == Brauhelfer::SudStatus::Rezept && value > 0);
    ui->btnVerduennung->setVisible(status == Brauhelfer::SudStatus::Rezept && value > 0);

    // ModuleSpeise
    if (gSettings->isModuleEnabled(Settings::ModuleSpeise))
    {
        value = BierCalc::speise(bh->sud()->getCO2(),
                                 bh->sud()->getSWAnstellen(),
                                 ui->tbSpeiseSRE->value(),
                                 ui->tbSpeiseSRE->value(),
                                 ui->tbSpeiseT->value());
        ui->tbSpeisemengeNoetig->setValue(value * bh->sud()->getWuerzemengeAnstellenTotal()/(1+value));
        ui->btnSpeisemengeNoetig->setVisible(status == Brauhelfer::SudStatus::Rezept && qAbs(ui->tbSpeisemenge->value() - ui->tbSpeisemengeNoetig->value()) > 0.25);
    }

    double mengeSollKochbeginn100 = BierCalc::volumenWasser(20.0, ui->tbTempKochbeginn->value(), bh->sud()->getMengeSollKochbeginn());
    ui->tbMengeSollKochbeginn100->setValue(mengeSollKochbeginn100);
    double mengeSollKochende100 = BierCalc::volumenWasser(20.0, ui->tbTempKochende->value(), bh->sud()->getMengeSollKochende());
    ui->tbMengeSollKochende100->setValue(mengeSollKochende100);

    // ModuleAusruestung
    ui->cbDurchschnittIgnorieren->setChecked(bh->sud()->getAusbeuteIgnorieren());
    if (ui->cbDurchschnittIgnorieren->isChecked() || !gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        ui->lblDurchschnittWarnung->setVisible(false);
    }
    else
    {
        double h;
        double d = pow(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
        ui->tbMengeSollcmVomBoden->setValue(mengeSollKochbeginn100 / d);
        h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
        ui->tbMengeSollcmVonOben->setValue(h - mengeSollKochbeginn100 / d);
        ui->tbMengeSollEndecmVomBoden->setValue(mengeSollKochende100 / d);
        h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
        ui->tbMengeSollEndecmVonOben->setValue(h - mengeSollKochende100 / d);
        ui->lblDurchschnittWarnung->setVisible(bh->sud()->getSWAnteilZusatzMaischen() > 0 || bh->sud()->getSWAnteilZusatzKochen() > 0);
    }

    ui->wdgSWSollKochbeginnMitWz->setVisible(bh->sud()->getSWAnteilZusatzKochen() > 0.0);
    ui->tbSWSollKochbeginnBrix->setValue(BierCalc::platoToBrix(bh->sud()->getSWSollKochbeginn()));
    ui->tbSWSollKochbeginnMitWzBrix->setValue(BierCalc::platoToBrix(bh->sud()->getSWSollKochbeginnMitWz()));
    ui->tbSWSollKochendeBrix->setValue(BierCalc::platoToBrix(bh->sud()->getSWSollKochende()));
    ui->tbSWAnstellenSollBrix->setValue(BierCalc::platoToBrix(bh->sud()->getSWSollAnstellen()));

    ui->tbSWHefestarterBrix->setValue(BierCalc::platoToBrix(bh->sud()->getSWHefestarter()));
    ui->wdgHefestarter->setVisible(bh->sud()->getMengeHefestarter() > 0.0);

    ui->chartBraudaten->update();
    ui->chartAusbeute->update();
}

void TabBraudaten::on_tbBraudatum_dateChanged(const QDate &date)
{
    if (ui->tbBraudatum->hasFocus())
        bh->sud()->setBraudatum(QDateTime(date, ui->tbBraudatumZeit->time()));
}

void TabBraudaten::on_tbBraudatumZeit_timeChanged(const QTime &time)
{
    if (ui->tbBraudatumZeit->hasFocus())
        bh->sud()->setBraudatum(QDateTime(ui->tbBraudatum->date(), time));
}

void TabBraudaten::on_btnBraudatumHeute_clicked()
{
    bh->sud()->setBraudatum(QDateTime());
}

void TabBraudaten::on_btnSWKochbeginn_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    DlgRestextrakt dlg(bh->sud()->getSWKochbeginn(), 0.0, -1.0, QDateTime(), this);
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setSWKochbeginn(dlg.value());
}

void TabBraudaten::on_btnWuerzemengeKochbeginn_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    double d = 0, h = 0;
    if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        d = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
        h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    }
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeKochbeginn->value());
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setWuerzemengeKochbeginn(dlg.getLiter());
}

void TabBraudaten::on_tbTempKochbeginn_valueChanged(double)
{
    if (ui->tbTempKochbeginn->hasFocus())
        updateValues();
}

void TabBraudaten::on_btnWuerzemengeVorHopfenseihen_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    double d = 0, h = 0;
    if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        d = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
        h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    }
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeVorHopfenseihen->value());
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setWuerzemengeVorHopfenseihen(dlg.getLiter());
}

void TabBraudaten::on_btnWuerzemengeKochende_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    DlgVolumen dlg(this);
    dlg.setLiter(ui->tbWuerzemengeKochende->value());
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setWuerzemengeKochende(dlg.getLiter());
}

void TabBraudaten::on_tbTempKochende_valueChanged(double)
{
    if (ui->tbTempKochende->hasFocus())
        updateValues();
}

void TabBraudaten::on_btnSWKochende_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    DlgRestextrakt dlg(bh->sud()->getSWKochende(), 0.0, -1.0, QDateTime(),this);
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setSWKochende(dlg.value());
}

void TabBraudaten::on_btnSWAnstellen_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    DlgRestextrakt dlg(bh->sud()->getSWAnstellen(), 0.0, -1.0, QDateTime(),this);
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setSWAnstellen(dlg.value());
}

void TabBraudaten::on_btnWuerzemengeAnstellenTotal_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    DlgVolumen dlg(this);
    dlg.setLiter(bh->sud()->getWuerzemengeAnstellenTotal());
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setWuerzemengeAnstellenTotal(dlg.getLiter());
}

void TabBraudaten::on_btnWuerzemengeAnstellen_clicked()
{
    WidgetDecorator::suspendValueChanged(true);
    DlgVolumen dlg(this);
    dlg.setLiter(bh->sud()->getWuerzemengeAnstellen());
    int dlgRet = dlg.exec();
    WidgetDecorator::suspendValueChanged(false);
    if (dlgRet == QDialog::Accepted)
        bh->sud()->setWuerzemengeAnstellen(dlg.getLiter());
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

void TabBraudaten::on_btnVerduennung_clicked()
{
    bh->sud()->setVerduennungAnstellen(ui->tbWasserVerschneidung->value());
    bh->sud()->setSWAnstellen(ui->tbSWAnstellenSoll->value());
}

void TabBraudaten::on_btnSpeisemengeNoetig_clicked()
{
    bh->sud()->setSpeisemenge(ui->tbSpeisemengeNoetig->value());
}

void TabBraudaten::on_cbDurchschnittIgnorieren_clicked(bool checked)
{
    bh->sud()->setAusbeuteIgnorieren(checked);
}

void TabBraudaten::on_btnSudGebraut_clicked()
{
    QDateTime dt(ui->tbBraudatum->date(), ui->tbBraudatumZeit->time());
    QString dtStr = QLocale().toString(dt, QLocale::ShortFormat);
    if (QMessageBox::question(this, tr("Sud als gebraut markieren?"),
                                    tr("Soll der Sud als gebraut markiert werden?\n\nBraudatum: %1").arg(dtStr),
                                    QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
        return;

    bh->sud()->setBraudatum(dt);
    bh->sud()->setStatus(static_cast<int>(Brauhelfer::SudStatus::Gebraut));

    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        DlgRohstoffeAbziehen dlg(true, this);
        dlg.exec();
    }

    if (bh->sud()->modelSchnellgaerverlauf()->rowCount() == 0)
    {
        QMap<int, QVariant> values({{ModelSchnellgaerverlauf::ColSudID, bh->sud()->id()},
                                    {ModelSchnellgaerverlauf::ColZeitstempel, bh->sud()->getBraudatum()},
                                    {ModelSchnellgaerverlauf::ColRestextrakt, bh->sud()->getSWIst()}});
        bh->sud()->modelSchnellgaerverlauf()->append(values);
    }

    if (bh->sud()->modelHauptgaerverlauf()->rowCount() == 0)
    {
        QMap<int, QVariant> values({{ModelHauptgaerverlauf::ColSudID, bh->sud()->id()},
                                    {ModelHauptgaerverlauf::ColZeitstempel, bh->sud()->getBraudatum()},
                                    {ModelHauptgaerverlauf::ColRestextrakt, bh->sud()->getSWIst()}});
        bh->sud()->modelHauptgaerverlauf()->append(values);
    }
}
