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

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBraudaten::TabBraudaten(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBraudaten)
{
    ui->setupUi(this);
    ui->lblCurrency->setText(QLocale().currencySymbol());
    ui->lblCurrency2->setText(QLocale().currencySymbol() + "/" + tr("l"));

    ui->webview->setHtmlFile("braudaten.html");

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabBraudaten");

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
}

void TabBraudaten::sudDataChanged(const QModelIndex& index)
{
    const SqlTableModel* model = static_cast<const SqlTableModel*>(index.model());
    QString fieldname = model->fieldName(index.column());
    if (fieldname == "Status")
    {
        checkEnabled();
    }
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
    double value;

    int status = bh->sud()->getStatus();
    QDateTime dt = bh->sud()->getBraudatum();
    ui->tbBraudatum->setDateTime(dt.isValid() ? dt : QDateTime::currentDateTime());

    if (!ui->tbWuerzemengeKochbeginn->hasFocus())
           ui->tbWuerzemengeKochbeginn->setValue(bh->sud()->getWuerzemengeVorHopfenseihen());
    ui->tbWuerzemengeKochende->setMaximum(ui->tbWuerzemengeKochbeginn->value());
    if (!ui->tbWuerzemengeKochende->hasFocus())
        ui->tbWuerzemengeKochende->setValue(bh->sud()->getWuerzemengeKochende());
    if (!ui->tbSWKochende->hasFocus())
        ui->tbSWKochende->setValue(bh->sud()->getSWKochende());
    ui->tbSWAnstellenSoll->setValue(bh->sud()->getSWSollAnstellen());
    value = BierCalc::verschneidung(bh->sud()->getSWAnstellen(),
                                    bh->sud()->getSWSollAnstellen(),
                                    bh->sud()->getWuerzemengeKochende() * (1 + bh->sud()->gethighGravityFaktor()/100));
    ui->tbWasserVerschneidung->setValue(value);
    ui->wdgWasserVerschneidung->setVisible(status == Sud_Status_Rezept && value > 0);
    ui->btnWasserVerschneidung->setVisible(status == Sud_Status_Rezept && value > 0);
    ui->tbSWAnstellen->setMaximum(ui->tbSWKochende->value());
    if (!ui->tbSWAnstellen->hasFocus())
        ui->tbSWAnstellen->setValue(bh->sud()->getSWAnstellen());
    if (!ui->tbWuerzemengeAnstellenTotal->hasFocus())
        ui->tbWuerzemengeAnstellenTotal->setValue(bh->sud()->getWuerzemengeAnstellenTotal());
    if (!ui->tbSpeisemenge->hasFocus())
        ui->tbSpeisemenge->setValue(bh->sud()->getSpeisemenge());
    if (!ui->tbWuerzemengeAnstellen->hasFocus())
        ui->tbWuerzemengeAnstellen->setValue(bh->sud()->getWuerzemengeAnstellen());
    value = BierCalc::speise(bh->sud()->getCO2(),
                             bh->sud()->getSWAnstellen(),
                             ui->tbSpeiseSRE->value(),
                             ui->tbSpeiseSRE->value(),
                             20.0);
    ui->tbSpeisemengeNoetig->setValue(value * bh->sud()->getWuerzemengeAnstellenTotal()/(1+value));
    ui->btnSpeisemengeNoetig->setVisible(status == Sud_Status_Rezept && ui->tbSpeisemenge->value() != ui->tbSpeisemengeNoetig->value());

    ui->cbDurchschnittIgnorieren->setChecked(bh->sud()->getAusbeuteIgnorieren());
    if (!ui->cbDurchschnittIgnorieren->isChecked())
        ui->lblWarnAusbeute->setVisible(bh->sud()->getSW_WZ_Maischen() > 0 || bh->sud()->getSW_WZ_Kochen() > 0);
    else
        ui->lblWarnAusbeute->setVisible(false);

    value = pow(bh->sud()->getAnlageData("Sudpfanne_Durchmesser").toDouble() / 2, 2) * M_PI / 1000;
    ui->tbMengeSollKochbeginn20->setValue(bh->sud()->getMengeSollKochbeginn());
    ui->tbMengeSollKochbeginn100->setValue(BierCalc::volumenWasser(20.0, ui->tbTempKochbeginn->value(), ui->tbMengeSollKochbeginn20->value()));
    ui->tbMengeSollcmVomBoden->setValue(ui->tbMengeSollKochbeginn100->value() / value);
    ui->tbMengeSollcmVonOben->setValue(bh->sud()->getAnlageData("Sudpfanne_Hoehe").toDouble() - ui->tbMengeSollcmVomBoden->value());
    ui->tbSWSollKochbeginn->setValue(bh->sud()->getSWSollKochbeginn());

    ui->tbMengeSollKochende20->setValue(bh->sud()->getMengeSollKochende());
    ui->tbMengeSollKochende100->setValue(BierCalc::volumenWasser(20.0, ui->tbTempKochende->value(), ui->tbMengeSollKochende20->value()));
    ui->tbMengeSollEndecmVomBoden->setValue(ui->tbMengeSollKochende100->value() / value);
    ui->tbMengeSollEndecmVonOben->setValue(bh->sud()->getAnlageData("Sudpfanne_Hoehe").toDouble() - ui->tbMengeSollEndecmVomBoden->value());
    ui->tbSWSollKochende->setValue(bh->sud()->getSWSollKochende());

    ui->tbVerdampfung->setValue(bh->sud()->getVerdampfungsziffer());
    ui->tbAusbeute->setValue(bh->sud()->geterg_Sudhausausbeute());
    ui->tbAusbeuteEffektiv->setValue(bh->sud()->geterg_EffektiveAusbeute());

    if (!ui->tbNebenkosten->hasFocus())
        ui->tbNebenkosten->setValue(bh->sud()->getKostenWasserStrom());
    ui->tbKosten->setValue(bh->sud()->geterg_Preis());

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

void TabBraudaten::on_tbWuerzemengeKochbeginn_valueChanged(double value)
{
    if (ui->tbWuerzemengeKochbeginn->hasFocus())
        bh->sud()->setWuerzemengeVorHopfenseihen(value);
}

void TabBraudaten::on_btnWuerzemengeKochbeginn_clicked()
{
    double d = bh->sud()->getAnlageData("Sudpfanne_Durchmesser").toDouble();
    double h = bh->sud()->getAnlageData("Sudpfanne_Hoehe").toDouble();
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

void TabBraudaten::on_tbWuerzemengeKochende_valueChanged(double value)
{
    if (ui->tbWuerzemengeKochende->hasFocus())
        bh->sud()->setWuerzemengeKochende(value);
}

void TabBraudaten::on_btnWuerzemengeKochende_clicked()
{
    double d = bh->sud()->getAnlageData("Sudpfanne_Durchmesser").toDouble();
    double h = bh->sud()->getAnlageData("Sudpfanne_Hoehe").toDouble();
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

void TabBraudaten::on_tbSWKochende_valueChanged(double value)
{
    if (ui->tbSWKochende->hasFocus())
        bh->sud()->setSWKochende(value);
}

void TabBraudaten::on_btnSWKochende_clicked()
{
    DlgRestextrakt dlg(ui->tbSWKochende->value(), 0.0, 20.0, this);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbSWKochende->setValue(dlg.value());
}

void TabBraudaten::on_tbSWAnstellen_valueChanged(double value)
{
    if (ui->tbSWAnstellen->hasFocus())
        bh->sud()->setSWAnstellen(value);
}

void TabBraudaten::on_btnSWAnstellen_clicked()
{
    DlgRestextrakt dlg(ui->tbSWAnstellen->value(), 0.0, 20.0, this);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbSWAnstellen->setValue(dlg.value());
}

void TabBraudaten::on_btnWasserVerschneidung_clicked()
{
    setFocus();
    double menge = bh->sud()->getWuerzemengeKochende() + ui->tbWasserVerschneidung->value();
    bh->sud()->setSWAnstellen(bh->sud()->getSWSollAnstellen());
    bh->sud()->setWuerzemengeAnstellenTotal(menge);
}

void TabBraudaten::on_tbWuerzemengeAnstellenTotal_valueChanged(double value)
{
    if (ui->tbWuerzemengeAnstellenTotal->hasFocus())
        bh->sud()->setWuerzemengeAnstellenTotal(value);
}

void TabBraudaten::on_btnWuerzemengeAnstellenTotal_clicked()
{
    double d = bh->sud()->getAnlageData("Sudpfanne_Durchmesser").toDouble();
    double h = bh->sud()->getAnlageData("Sudpfanne_Hoehe").toDouble();
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

void TabBraudaten::on_btnSpeisemengeNoetig_clicked()
{
    bh->sud()->setSpeisemenge(ui->tbSpeisemengeNoetig->value());
}

void TabBraudaten::on_tbSpeisemenge_valueChanged(double value)
{
    if (ui->tbSpeisemenge->hasFocus())
        bh->sud()->setSpeisemenge(value);
}

void TabBraudaten::on_tbWuerzemengeAnstellen_valueChanged(double value)
{
    if (ui->tbWuerzemengeAnstellen->hasFocus())
        bh->sud()->setWuerzemengeAnstellen(value);
}

void TabBraudaten::on_tbNebenkosten_valueChanged(double value)
{
    if (ui->tbNebenkosten->hasFocus())
        bh->sud()->setKostenWasserStrom(value);
}

void TabBraudaten::on_cbDurchschnittIgnorieren_clicked(bool checked)
{
    bh->sud()->setAusbeuteIgnorieren(checked);
}

void TabBraudaten::on_btnSudGebraut_clicked()
{
    bh->sud()->setBraudatum(ui->tbBraudatum->dateTime());
    bh->sud()->setStatus(Sud_Status_Gebraut);

    if (QMessageBox::question(this, tr("Zutaten vom Bestand abziehen"),
                              tr("Sollen die bisher verwendeten Zutaten vom Bestand abgezogen werden?")
       ) == QMessageBox::Yes)
        bh->sud()->brauzutatenAbziehen();

    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Zeitstempel", bh->sud()->getBraudatum()},
                        {"SW", bh->sud()->getSWIst()}, {"Temp", 20.0}});
    if (bh->sud()->modelSchnellgaerverlauf()->rowCount() == 0)
        bh->sud()->modelSchnellgaerverlauf()->append(values);
    if (bh->sud()->modelHauptgaerverlauf()->rowCount() == 0)
        bh->sud()->modelHauptgaerverlauf()->append(values);
}

void TabBraudaten::on_btnSudTeilen_clicked()
{
    DlgSudTeilen dlg(bh->sud()->getSudname(), bh->sud()->getMengeIst(), this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sudTeilen(bh->sud()->id(), dlg.nameTeil1(), dlg.nameTeil2(), dlg.prozent());
}
