#include "tabbraudaten.h"
#include "ui_tabbraudaten.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgvolumen.h"
#include "dialogs/dlgsudteilen.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBraudaten::TabBraudaten(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabBraudaten)
{
    ui->setupUi(this);
    ui->tbWuerzemengeKochbeginn->setColumn(ModelSud::ColWuerzemengeVorHopfenseihen);
    ui->tbWuerzemengeKochende->setColumn(ModelSud::ColWuerzemengeKochende);
    ui->tbSWKochende->setColumn(ModelSud::ColSWKochende);
    ui->tbSWAnstellen->setColumn(ModelSud::ColSWAnstellen);
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
    ui->tbKosten->setColumn(ModelSud::Colerg_Preis);
    ui->tbNebenkosten->setColumn(ModelSud::ColKostenWasserStrom);
    ui->lblCurrency->setText(QLocale().currencySymbol());
    ui->lblCurrency2->setText(QLocale().currencySymbol() + "/" + tr("l"));

    mTimerWebViewUpdate.setSingleShot(true);
    connect(&mTimerWebViewUpdate, SIGNAL(timeout()), this, SLOT(updateWebView()), Qt::QueuedConnection);
    ui->webview->setHtmlFile("braudaten.html");

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabBraudaten");

    ui->splitter->setSizes({500, 500});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
                    this, SLOT(sudDataChanged(const QModelIndex&)));
}

TabBraudaten::~TabBraudaten()
{
    delete ui;
}

void TabBraudaten::saveSettings()
{
    gSettings->beginGroup("TabBraudaten");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->endGroup();
}

void TabBraudaten::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
}

void TabBraudaten::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && now != ui->tbHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabBraudaten::sudLoaded()
{
    checkEnabled();
    updateValues();
    ui->tbSpeiseSRE->setValue(BierCalc::sreAusVergaerungsgrad(bh->sud()->getSW(), bh->sud()->getVergaerungsgrad()));
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
        ui->tbSpeiseSRE->setValue(BierCalc::sreAusVergaerungsgrad(bh->sud()->getSW(), bh->sud()->getVergaerungsgrad()));
        break;
    }
}

void TabBraudaten::onTabActivated()
{
    updateValues();
}

void TabBraudaten::checkEnabled()
{
    int status = bh->sud()->getStatus();
    bool gebraut = status != Sud_Status_Rezept && !gSettings->ForceEnabled;
    ui->tbBraudatum->setReadOnly(gebraut);
    ui->btnBraudatumHeute->setVisible(!gebraut);
    ui->tbWuerzemengeKochbeginn->setReadOnly(gebraut);
    ui->btnWuerzemengeKochbeginn->setVisible(!gebraut);
    ui->tbWuerzemengeKochende->setReadOnly(gebraut);
    ui->btnWuerzemengeKochende->setVisible(!gebraut);
    ui->tbSWKochende->setReadOnly(gebraut);
    ui->btnSWKochende->setVisible(!gebraut);
    ui->tbSWAnstellen->setReadOnly(gebraut);
    ui->btnSWAnstellen->setVisible(!gebraut);
    ui->btnWasserVerschneidung->setVisible(!gebraut);
    ui->btnWuerzemengeAnstellenTotal->setVisible(!gebraut);
    ui->tbWuerzemengeAnstellenTotal->setReadOnly(gebraut);
    ui->btnSpeisemengeNoetig->setVisible(!gebraut);
    ui->tbSpeisemenge->setReadOnly(gebraut);
    ui->tbWuerzemengeAnstellen->setReadOnly(gebraut);
    ui->tbNebenkosten->setReadOnly(gebraut);
    ui->btnSudGebraut->setEnabled(!gebraut);
    ui->btnSudTeilen->setEnabled(status != Sud_Status_Abgefuellt && status != Sud_Status_Verbraucht && !gSettings->ForceEnabled);
}

void TabBraudaten::updateValues()
{
    if (!isTabActive())
        return;

    for (DoubleSpinBoxSud *wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();

    double value;

    int status = bh->sud()->getStatus();

    QDateTime dt = bh->sud()->getBraudatum();
    ui->tbBraudatum->setDateTime(dt.isValid() ? dt : QDateTime::currentDateTime());

    ui->tbWuerzemengeKochende->setMaximum(ui->tbWuerzemengeKochbeginn->value());

    value = BierCalc::verschneidung(bh->sud()->getSWAnstellen(),
                                    bh->sud()->getSWSollAnstellen(),
                                    bh->sud()->getWuerzemengeKochende() * (1 + bh->sud()->gethighGravityFaktor()/100));
    ui->tbWasserVerschneidung->setValue(value);
    ui->wdgWasserVerschneidung->setVisible(status == Sud_Status_Rezept && value > 0);
    ui->btnWasserVerschneidung->setVisible(status == Sud_Status_Rezept && value > 0);
    ui->tbSWAnstellen->setMaximum(ui->tbSWKochende->value());

    value = BierCalc::speise(bh->sud()->getCO2(),
                             bh->sud()->getSWAnstellen(),
                             ui->tbSpeiseSRE->value(),
                             ui->tbSpeiseSRE->value(),
                             ui->tbSpeiseT->value());
    ui->tbSpeisemengeNoetig->setValue(value * bh->sud()->getWuerzemengeAnstellenTotal()/(1+value));
    ui->btnSpeisemengeNoetig->setVisible(status == Sud_Status_Rezept && ui->tbSpeisemenge->value() != ui->tbSpeisemengeNoetig->value());

    ui->cbDurchschnittIgnorieren->setChecked(bh->sud()->getAusbeuteIgnorieren());
    if (!ui->cbDurchschnittIgnorieren->isChecked())
        ui->lblWarnAusbeute->setVisible(bh->sud()->getSW_WZ_Maischen() > 0 || bh->sud()->getSW_WZ_Kochen() > 0);
    else
        ui->lblWarnAusbeute->setVisible(false);

    value = pow(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
    ui->tbMengeSollKochbeginn100->setValue(BierCalc::volumenWasser(20.0, ui->tbTempKochbeginn->value(), ui->tbMengeSollKochbeginn20->value()));
    ui->tbMengeSollcmVomBoden->setValue(ui->tbMengeSollKochbeginn100->value() / value);
    ui->tbMengeSollcmVonOben->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble() - ui->tbMengeSollcmVomBoden->value());
    ui->wdgSWSollKochbeginnMitWz->setVisible(bh->sud()->getSW_WZ_Kochen() > 0.0);
    ui->tbMengeSollKochende100->setValue(BierCalc::volumenWasser(20.0, ui->tbTempKochende->value(), ui->tbMengeSollKochende20->value()));
    ui->tbMengeSollEndecmVomBoden->setValue(ui->tbMengeSollKochende100->value() / value);
    ui->tbMengeSollEndecmVonOben->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble() - ui->tbMengeSollEndecmVomBoden->value());

    mTimerWebViewUpdate.start(200);
}

void TabBraudaten::updateWebView()
{
    TemplateTags::render(ui->webview, TemplateTags::TagAll, bh->sud()->row());
}

void TabBraudaten::on_tbBraudatum_dateTimeChanged(const QDateTime &dateTime)
{
    if (ui->tbBraudatum->hasFocus())
        bh->sud()->setBraudatum(dateTime);
}

void TabBraudaten::on_btnBraudatumHeute_clicked()
{
    bh->sud()->setBraudatum(QDateTime());
}

void TabBraudaten::on_btnWuerzemengeKochbeginn_clicked()
{
    double d = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
    double h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeKochbeginn->value());
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setWuerzemengeVorHopfenseihen(dlg.getLiter());
}

void TabBraudaten::on_tbTempKochbeginn_valueChanged(double)
{
    if (ui->tbTempKochbeginn->hasFocus())
        updateValues();
}

void TabBraudaten::on_btnWuerzemengeKochende_clicked()
{
    double d = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
    double h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeKochende->value());
    dlg.setVisibleVonOben(false);
    dlg.setVisibleVonUnten(false);
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setWuerzemengeKochende(dlg.getLiter());
}

void TabBraudaten::on_tbTempKochende_valueChanged(double)
{
    if (ui->tbTempKochende->hasFocus())
        updateValues();
}

void TabBraudaten::on_btnSWKochende_clicked()
{
    DlgRestextrakt dlg(ui->tbSWKochende->value(), 0.0, 20.0, this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setSWKochende(dlg.value());
}

void TabBraudaten::on_btnSWAnstellen_clicked()
{
    DlgRestextrakt dlg(ui->tbSWAnstellen->value(), 0.0, 20.0, this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setSWAnstellen(dlg.value());
}

void TabBraudaten::on_btnWasserVerschneidung_clicked()
{
    setFocus();
    double menge = bh->sud()->getWuerzemengeKochende() + ui->tbWasserVerschneidung->value();
    bh->sud()->setSWAnstellen(bh->sud()->getSWSollAnstellen());
    bh->sud()->setWuerzemengeAnstellenTotal(menge);
}

void TabBraudaten::on_btnWuerzemengeAnstellenTotal_clicked()
{
    double d = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble();
    double h = bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeAnstellenTotal->value());
    dlg.setVisibleVonOben(false);
    dlg.setVisibleVonUnten(false);
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setWuerzemengeAnstellenTotal(dlg.getLiter());
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
    bh->sud()->setBraudatum(ui->tbBraudatum->dateTime());
    bh->sud()->setStatus(Sud_Status_Gebraut);

    DlgRohstoffeAbziehen dlg(this);
    dlg.exec();

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

void TabBraudaten::on_btnSudTeilen_clicked()
{
    DlgSudTeilen dlg(bh->sud()->getSudname(), bh->sud()->getMengeIst(), this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sudTeilen(bh->sud()->id(), dlg.nameTeil1(), dlg.nameTeil2(), dlg.prozent());
}
