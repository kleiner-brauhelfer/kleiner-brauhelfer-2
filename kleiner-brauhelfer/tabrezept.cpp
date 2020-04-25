#include "tabrezept.h"
#include "ui_tabrezept.h"
#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QStandardItemModel>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "model/checkboxdelegate.h"
#include "widgets/wdgrast.h"
#include "widgets/wdgmalzgabe.h"
#include "widgets/wdghopfengabe.h"
#include "widgets/wdghefegabe.h"
#include "widgets/wdgweiterezutatgabe.h"
#include "widgets/wdganhang.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgeinmaischtemp.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabRezept::TabRezept(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabRezept)
{
    ui->setupUi(this);
    ui->tbRestalkalitaet->setColumn(ModelSud::ColRestalkalitaetSoll);
    ui->tbCO2->setColumn(ModelSud::ColCO2);
    ui->tbSW->setColumn(ModelSud::ColSW);
    ui->tbMenge->setColumn(ModelSud::ColMenge);
    ui->tbFaktorHauptguss->setColumn(ModelSud::ColFaktorHauptguss);
    ui->tbGesamtschuettung->setColumn(ModelSud::Colerg_S_Gesamt);
    ui->tbSWMalz->setColumn(ModelSud::ColSW_Malz);
    ui->tbSWWZMaischen->setColumn(ModelSud::ColSW_WZ_Maischen);
    ui->tbSWWZKochen->setColumn(ModelSud::ColSW_WZ_Kochen);
    ui->tbSWWZGaerung->setColumn(ModelSud::ColSW_WZ_Gaerung);
    ui->tbSudnummer->setColumn(ModelSud::ColSudnummer);
    ui->tbBittere->setColumn(ModelSud::ColIBU);
    ui->tbFarbe->setColumn(ModelSud::Colerg_Farbe);
    ui->tbSudhausausbeute->setColumn(ModelSud::ColSudhausausbeute);
    ui->tbVerdampfungsziffer->setColumn(ModelSud::ColVerdampfungsrate);
    ui->tbVergaerungsgrad->setColumn(ModelSud::ColVergaerungsgrad);
    ui->tbReifezeit->setColumn(ModelSud::ColReifezeit);
    ui->tbHGF->setColumn(ModelSud::ColhighGravityFaktor);
    ui->tbEinmaischtemperatur->setColumn(ModelSud::ColEinmaischenTemp);
    ui->tbKochzeit->setColumn(ModelSud::ColKochdauerNachBitterhopfung);
    ui->tbNachisomerisierungszeit->setColumn(ModelSud::ColNachisomerisierungszeit);
    ui->tbKosten->setColumn(ModelSud::Colerg_Preis);
    ui->tbFaktorHauptgussEmpfehlung->setColumn(ModelSud::ColFaktorHauptgussEmpfehlung);
    ui->tbHauptguss->setColumn(ModelSud::Colerg_WHauptguss);
    ui->tbNachguss->setColumn(ModelSud::Colerg_WNachguss);
    ui->tbAlkohol->setColumn(ModelSud::ColAlkohol);

    ui->lblBerechnungsartHopfenWarnung->setPalette(gSettings->paletteErrorLabel);

    mGlasSvg = new QGraphicsSvgItem(":/images/bier.svg");
    ui->lblCurrency->setText(QLocale().currencySymbol() + "/" + tr("l"));

  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    ui->diagramRasten->chart()->legend()->hide();
  #endif

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabRezept");

    ui->splitter->setSizes({500, 500, 500});
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setStretchFactor(2, 1);
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());
    ui->splitterMalzDiagram->restoreState(gSettings->value("splitterMalzDiagramState").toByteArray());
    ui->splitterHopfenDiagram->restoreState(gSettings->value("splitterHopfenDiagramState").toByteArray());
    ui->splitterHefeDiagram->restoreState(gSettings->value("splitterHefeDiagramState").toByteArray());
    ui->splitterRastenDiagram->restoreState(gSettings->value("splitterRastenDiagramState").toByteArray());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));

    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(sudDataChanged(const QModelIndex&)));

    connect(bh->sud()->modelRasten(), SIGNAL(layoutChanged()), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(modified()), this, SLOT(updateRastenDiagram()));

    connect(bh->sud()->modelMalzschuettung(), SIGNAL(layoutChanged()), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(malzGaben_dataChanged()));

    connect(bh->sud()->modelHopfengaben(), SIGNAL(layoutChanged()), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(hopfenGaben_dataChanged()));

    connect(bh->sud()->modelHefegaben(), SIGNAL(layoutChanged()), this, SLOT(hefeGaben_modified()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(hefeGaben_modified()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(hefeGaben_modified()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateHefeDiagram()));

    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(layoutChanged()), this, SLOT(weitereZutatenGaben_modified()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(weitereZutatenGaben_modified()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(weitereZutatenGaben_modified()));

    connect(bh->sud()->modelAnhang(), SIGNAL(layoutChanged()), this, SLOT(anhaenge_modified()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(anhaenge_modified()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(anhaenge_modified()));

    ProxyModel *model = bh->sud()->modelTags();
    model->setHeaderData(ModelTags::ColKey, Qt::Horizontal, tr("Tag"));
    model->setHeaderData(ModelTags::ColValue, Qt::Horizontal, tr("Wert"));
    model->setHeaderData(ModelTags::ColGlobal, Qt::Horizontal, tr("Global"));
    TableView *table = ui->tableTags;
    table->setModel(model);
    table->cols.append({ModelTags::ColKey, true, false, 0, nullptr});
    table->cols.append({ModelTags::ColValue, true, false, -1, nullptr});
    table->cols.append({ModelTags::ColGlobal, true, false, 0, new CheckBoxDelegate(table)});
    table->build();
}

TabRezept::~TabRezept()
{
    delete ui;
    delete mGlasSvg;
}

void TabRezept::saveSettings()
{
    gSettings->beginGroup("TabRezept");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->setValue("splitterMalzDiagramState", ui->splitterMalzDiagram->saveState());
    gSettings->setValue("splitterHopfenDiagramState", ui->splitterHopfenDiagram->saveState());
    gSettings->setValue("splitterHefeDiagramState", ui->splitterHefeDiagram->saveState());
    gSettings->setValue("splitterRastenDiagramState", ui->splitterRastenDiagram->saveState());
    gSettings->endGroup();
}

void TabRezept::restoreView(bool full)
{
    if (full)
    {
        ui->splitter->restoreState(mDefaultSplitterState);
        ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
    }
}

void TabRezept::focusChanged(QWidget *old, QWidget *now)
{
    if (old == ui->tbKommentar)
        ui->tbKommentar->setHtml(bh->sud()->getKommentar().replace("\n", "<br>"));
    if (now == ui->tbKommentar)
        ui->tbKommentar->setPlainText(bh->sud()->getKommentar());
    if (now && now != ui->tbHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabRezept::sudLoaded()
{
    checkEnabled();
    ui->cbAnlage->setCurrentIndex(-1);
    ui->cbWasserProfil->setCurrentIndex(-1);
    if (bh->sud()->isLoaded())
    {
        updateAnlageModel();
        updateWasserModel();
        updateValues();
        checkRohstoffe();
    }
}

void TabRezept::sudDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        checkEnabled();
        updateAnlageModel();
        updateWasserModel();
        rasten_modified();
        malzGaben_modified();
        hopfenGaben_modified();
        hefeGaben_modified();
        weitereZutatenGaben_modified();
        checkRohstoffe();
        break;

    case ModelSud::ColEinmaischenTemp:
        updateRastenDiagram();
        break;
    }
}

void TabRezept::onTabActivated()
{
    updateValues();
}

void TabRezept::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool gebraut = status != Brauhelfer::SudStatus::Rezept && !gSettings->ForceEnabled;
    bool abgefuellt = status > Brauhelfer::SudStatus::Gebraut  && !gSettings->ForceEnabled;
    ui->cbAnlage->setEnabled(!gebraut);
    ui->tbMenge->setReadOnly(gebraut);
    ui->tbSW->setReadOnly(gebraut);
    ui->tbSudhausausbeute->setReadOnly(gebraut);
    ui->tbFaktorHauptguss->setReadOnly(gebraut);
    ui->tbHGF->setReadOnly(gebraut);
    ui->tbBittere->setReadOnly(gebraut);
    ui->tbKochzeit->setReadOnly(gebraut);
    ui->tbVerdampfungsziffer->setReadOnly(gebraut);
    ui->tbNachisomerisierungszeit->setReadOnly(gebraut);
    ui->tbVergaerungsgrad->setReadOnly(gebraut);
    ui->tbCO2->setReadOnly(gebraut);
    ui->cbWasserProfil->setEnabled(!gebraut);
    ui->tbRestalkalitaet->setReadOnly(gebraut);
    ui->tbReifezeit->setReadOnly(gebraut);
    ui->tbEinmaischtemperatur->setReadOnly(gebraut);
    ui->btnEinmaischtemperatur->setVisible(!gebraut);
    ui->btnNeueRast->setVisible(!gebraut);
    ui->lineNeueRast->setVisible(!gebraut);
    ui->btnNeueMalzGabe->setVisible(!gebraut);
    ui->lineNeueMalzGabe->setVisible(!gebraut);
    ui->cbBerechnungsartHopfen->setEnabled(!gebraut);
    ui->btnNeueHopfenGabe->setVisible(!gebraut);
    ui->lineNeueHopfenGabe->setVisible(!gebraut);
    ui->btnNeueHefeGabe->setVisible(!abgefuellt);
    ui->lineNeueHefeGabe->setVisible(!gebraut);
    ui->btnNeueHopfenstopfenGabe->setVisible(!abgefuellt);
    ui->btnNeueWeitereZutat->setVisible(!abgefuellt);
    ui->lineNeueWeitereZutat->setVisible(!gebraut);
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
        static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutWeitereZutatenGaben->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutWeitereZutatenGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
        static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget())->updateValues();
}

void TabRezept::checkRohstoffe()
{
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = QMessageBox::question(this, tr("Rohstoff importieren?"),
                                            tr("Das Malz \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll es jetzt hinzugefügt werden?").arg(wdg->name()));
            if (ret == QMessageBox::Yes)
            {
                QMap<int, QVariant> values({{ModelMalz::ColBeschreibung, wdg->name()},
                                            {ModelMalz::ColFarbe, wdg->data(ModelMalzschuettung::ColFarbe)}});
                bh->modelMalz()->append(values);
                wdg->updateValues();
            }
        }
    }

    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = QMessageBox::question(this, tr("Rohstoff importieren?"),
                                            tr("Der Hopfen \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll es jetzt hinzugefügt werden?").arg(wdg->name()));
            if (ret == QMessageBox::Yes)
            {
                QMap<int, QVariant> values({{ModelHopfen::ColBeschreibung, wdg->name()},
                                            {ModelHopfen::ColAlpha, wdg->data(ModelHopfengaben::ColAlpha)},
                                            {ModelHopfen::ColPellets, wdg->data(ModelHopfengaben::ColPellets)}});
                bh->modelHopfen()->append(values);
                wdg->updateValues();
            }
        }
    }

    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
    {
        WdgHefeGabe* wdg = static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = QMessageBox::question(this, tr("Rohstoff importieren?"),
                                            tr("Die Hefe \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll es jetzt hinzugefügt werden?").arg(wdg->name()));
            if (ret == QMessageBox::Yes)
            {
                QMap<int, QVariant> values({{ModelHefe::ColBeschreibung, wdg->name()}});
                bh->modelHefe()->append(values);
                wdg->updateValues();
            }
        }
    }

    for (int i = 0; i < ui->layoutWeitereZutatenGaben->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutWeitereZutatenGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = QMessageBox::question(this, tr("Rohstoff importieren?"),
                                            tr("Die Zutat \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll jetzt es hinzugefügt werden?").arg(wdg->name()));
            if (ret == QMessageBox::Yes)
            {
                Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(wdg->data(ModelWeitereZutatenGaben::ColTyp).toInt());
                if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                {
                    QMap<int, QVariant> values({{ModelHopfen::ColBeschreibung, wdg->name()}});
                    bh->modelHopfen()->append(values);
                }
                else
                {
                    QMap<int, QVariant> values({{ModelWeitereZutaten::ColBeschreibung, wdg->name()},
                                                {ModelWeitereZutaten::ColEinheiten, wdg->data(ModelWeitereZutatenGaben::ColEinheit)},
                                                {ModelWeitereZutaten::ColTyp, wdg->data(ModelWeitereZutatenGaben::ColTyp)},
                                                {ModelWeitereZutaten::ColAusbeute, wdg->data(ModelWeitereZutatenGaben::ColAusbeute)},
                                                {ModelWeitereZutaten::ColEBC, wdg->data(ModelWeitereZutatenGaben::ColFarbe)}});
                    bh->modelWeitereZutaten()->append(values);
                }
                wdg->updateValues();
            }
        }
    }
}

void TabRezept::updateValues()
{
    double fVal;
    if (!isTabActive())
        return;

    for (DoubleSpinBoxSud *wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();
    for (SpinBoxSud *wdg : findChildren<SpinBoxSud*>())
        wdg->updateValue();

    if (!ui->tbSudname->hasFocus())
    {
        ui->tbSudname->setText(bh->sud()->getSudname());
        ui->tbSudname->setCursorPosition(0);
    }

    ui->btnSudhausausbeute->setVisible(bh->sud()->getSudhausausbeute() != bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
    ui->btnVerdampfungsziffer->setVisible(bh->sud()->getVerdampfungsrate() != bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsziffer).toDouble());

    ui->wdgSWMalz->setVisible(ui->tbSWMalz->value() > 0.0);
    ui->wdgSWWZMaischen->setVisible(ui->tbSWWZMaischen->value() > 0.0);
    ui->wdgSWWZKochen->setVisible(ui->tbSWWZKochen->value() > 0.0);
    ui->wdgSWWZGaerung->setVisible(ui->tbSWWZGaerung->value() > 0.0);
    fVal = bh->sud()->getIBU() / bh->sud()->getSW();
    if (fVal <= 1)
        ui->lblBittere->setText(tr("sehr mild"));
    else if (fVal <= 1.5)
        ui->lblBittere->setText(tr("mild"));
    else if (fVal <= 2.5)
        ui->lblBittere->setText(tr("ausgewogen"));
    else if (fVal <= 3)
        ui->lblBittere->setText(tr("moderat herb"));
    else
        ui->lblBittere->setText(tr("sehr herb"));
    ui->tbRestextrakt->setValue(BierCalc::sreAusVergaerungsgrad(bh->sud()->getSW(), bh->sud()->getVergaerungsgrad()));
    if (!ui->cbAnlage->hasFocus())
        ui->cbAnlage->setCurrentText(bh->sud()->getAnlage());
    ui->cbAnlage->setError(ui->cbAnlage->currentIndex() == -1);
    if (!ui->cbWasserProfil->hasFocus())
        ui->cbWasserProfil->setCurrentText(bh->sud()->getWasserprofil());
    ui->cbWasserProfil->setError(ui->cbWasserProfil->currentIndex() == -1);
    ui->tbRestalkalitaetWasser->setValue(bh->sud()->getWasserData(ModelWasser::ColRestalkalitaet).toDouble());
    ui->tbRestalkalitaet->setMaximum(ui->tbRestalkalitaetWasser->value());
    ui->lblWasserprofil->setText(bh->sud()->getWasserprofil());
    double restalkalitaetFaktor = bh->sud()->getRestalkalitaetFaktor();
    ui->tbMilchsaeureHG->setValue(ui->tbHauptguss->value() * restalkalitaetFaktor);
    ui->tbMilchsaeureNG->setValue(ui->tbNachguss->value() * restalkalitaetFaktor);
    if (ui->tbHGF->value() != 0.0)
    {
        ui->tbWasserHGF->setValue(bh->sud()->getMenge() - bh->sud()->getMengeSollKochende());
        ui->tbMilchsaeureHGF->setValue(ui->tbWasserHGF->value() * restalkalitaetFaktor);
        ui->tbWasserHGF->setVisible(true);
        ui->lblWasserHGF->setVisible(true);
        ui->lblWasserHGFEinheit->setVisible(true);
        ui->tbMilchsaeureHGF->setVisible(restalkalitaetFaktor > 0.0);
        ui->lblMilchsaeureHGF->setVisible(restalkalitaetFaktor > 0.0);
        ui->lblMilchsaeureHGFEinheit->setVisible(restalkalitaetFaktor > 0.0);
    }
    else
    {
        ui->tbWasserHGF->setValue(0.0);
        ui->tbMilchsaeureHGF->setValue(0.0);
        ui->tbWasserHGF->setVisible(false);
        ui->lblWasserHGF->setVisible(false);
        ui->lblWasserHGFEinheit->setVisible(false);
        ui->tbMilchsaeureHGF->setVisible(false);
        ui->lblMilchsaeureHGF->setVisible(false);
        ui->lblMilchsaeureHGFEinheit->setVisible(false);
    }
    Brauhelfer::AnlageTyp anlageTyp = static_cast<Brauhelfer::AnlageTyp>(bh->sud()->getAnlageData(ModelAusruestung::ColTyp).toInt());
    ui->wdgFaktorHauptguss->setVisible(anlageTyp != Brauhelfer::AnlageTyp::GrainfatherG30);
    ui->tbWasserGesamt->setValue(ui->tbHauptguss->value() + ui->tbNachguss->value() + ui->tbWasserHGF->value());
    ui->tbMilchsaeureGesamt->setValue(ui->tbWasserGesamt->value() * restalkalitaetFaktor);
    ui->tbMilchsaeureGesamt->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblMilchsaeureGesamt->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblMilchsaeureGesamtEinheit->setVisible(restalkalitaetFaktor > 0.0);
    ui->tbMilchsaeureHG->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblMilchsaeureHG->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblMilchsaeureHGEinheit->setVisible(restalkalitaetFaktor > 0.0);
    ui->tbMilchsaeureNG->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblMilchsaeureNG->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblMilchsaeureNGEinheit->setVisible(restalkalitaetFaktor > 0.0);
    ui->lblAnlageName->setText(bh->sud()->getAnlage());
    ui->tbAnlageKorrekturSollmenge->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColKorrekturMenge).toDouble());
    ui->wdgAnlageKorrekturSollmenge->setVisible(ui->tbAnlageKorrekturSollmenge->value() > 0);
    ui->tbAnlageSudhausausbeute->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
    ui->tbAnlageVerdampfung->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsziffer).toDouble());
    ui->tbAnlageVolumenMaische->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColMaischebottich_MaxFuellvolumen).toDouble());
    ui->tbVolumenMaische->setValue(bh->sud()->geterg_WHauptguss() + BierCalc::MalzVerdraengung * bh->sud()->geterg_S_Gesamt());
    ui->tbVolumenMaische->setError(ui->tbVolumenMaische->value() > ui->tbAnlageVolumenMaische->value());
    ui->tbAnlageVolumenKochen->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_MaxFuellvolumen).toDouble());
    ui->tbVolumenKochen->setValue(BierCalc::volumenWasser(20.0, 100.0, bh->sud()->getMengeSollKochbeginn()));
    ui->tbVolumenKochen->setError(ui->tbVolumenKochen->value() > ui->tbAnlageVolumenKochen->value());

    ui->tbKochzeit->setError(ui->tbKochzeit->value() == 0.0);
    Brauhelfer::BerechnungsartHopfen berechnungsArtHopfen = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->sud()->getberechnungsArtHopfen());
    if (!ui->cbBerechnungsartHopfen->hasFocus())
        ui->cbBerechnungsartHopfen->setCurrentIndex(static_cast<int>(berechnungsArtHopfen) + 1);
    ui->lblBerechnungsartHopfenWarnung->setVisible(berechnungsArtHopfen == Brauhelfer::BerechnungsartHopfen::Keine);
    if (!ui->tbKommentar->hasFocus())
        ui->tbKommentar->setHtml(bh->sud()->getKommentar().replace("\n", "<br>"));
    updateGlas();
}

void TabRezept::updateGlas()
{
    QGraphicsScene* scene = new QGraphicsScene();
    QPen pen(Qt::white);
    QBrush brush(BierCalc::ebcToColor(bh->sud()->geterg_Farbe()));
    scene->addRect(10, 10, 980, 980, pen, brush);
    scene->addItem(mGlasSvg);
    ui->gvGlas->setScene(scene);
    ui->gvGlas->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void TabRezept::updateAnlageModel()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        ProxyModel *model = new ProxyModel(ui->cbAnlage);
        model->setSourceModel(bh->modelAusruestung());
        ui->cbAnlage->setModel(model);
        ui->cbAnlage->setModelColumn(ModelAusruestung::ColName);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbAnlage);
        model->setItem(0, 0, new QStandardItem(bh->sud()->getAnlage()));
        ui->cbAnlage->setModel(model);
        ui->cbAnlage->setModelColumn(0);
    }
    ui->cbAnlage->setCurrentIndex(-1);
}

void TabRezept::updateWasserModel()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        ProxyModel *model = new ProxyModel(ui->cbWasserProfil);
        model->setSourceModel(bh->modelWasser());
        ui->cbWasserProfil->setModel(model);
        ui->cbWasserProfil->setModelColumn(ModelWasser::ColName);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbWasserProfil);
        model->setItem(0, 0, new QStandardItem(bh->sud()->getWasserprofil()));
        ui->cbWasserProfil->setModel(model);
        ui->cbWasserProfil->setModelColumn(0);
    }
    ui->cbWasserProfil->setCurrentIndex(-1);
}

void TabRezept::rasten_modified()
{    
    const int nModel = bh->sud()->modelRasten()->rowCount();
    int nLayout = ui->layoutRasten->count();
    while (nLayout < nModel)
        ui->layoutRasten->addWidget(new WdgRast(nLayout++));
    while (ui->layoutRasten->count() != nModel)
        delete ui->layoutRasten->itemAt(ui->layoutRasten->count() - 1)->widget();
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
        static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget())->updateValues();
    updateRastenDiagram();
}

void TabRezept::updateRastenDiagram()
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QLineSeries *series = new QLineSeries();
    int t = 0;
    int tempMin = 30;
    int tempMax = 80;
    series->append(t, bh->sud()->getEinmaischenTemp());
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
    {
        const WdgRast* wdg = static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget());
        int temp = wdg->temperatur();
        series->append(t, temp);
        t += wdg->dauer();
        series->append(t, temp);
        if (temp < tempMin)
            tempMin = temp;
        if (temp > tempMax)
            tempMax = temp;
    }
    QChart *chart = ui->diagramRasten->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
    chart->createDefaultAxes();
    QValueAxis *axis =  static_cast<QValueAxis*>(chart->axes(Qt::Horizontal).back());
    axis->setRange(0, t);
    axis->setLabelFormat("%d min");
    axis =  static_cast<QValueAxis*>(chart->axes(Qt::Vertical).back());
    axis->setRange(tempMin, tempMax);
    axis->setLabelFormat("%d C");
  #endif
}

void TabRezept::on_btnEinmaischtemperatur_clicked()
{
    int rastTemp = bh->sud()->modelRasten()->rowCount() > 0 ? bh->sud()->modelRasten()->data(0, ModelRasten::ColTemp).toInt() : 57;
    DlgEinmaischTemp dlg(bh->sud()->geterg_S_Gesamt(), 18, bh->sud()->geterg_WHauptguss(), rastTemp, this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setEinmaischenTemp(dlg.value());
}

void TabRezept::on_btnNeueRast_clicked()
{
    QMap<int, QVariant> values({{ModelRasten::ColSudID, bh->sud()->id()}, {ModelRasten::ColTemp, 78}});
    bh->sud()->modelRasten()->append(values);
    ui->scrollAreaRasten->verticalScrollBar()->setValue(ui->scrollAreaRasten->verticalScrollBar()->maximum());
}

void TabRezept::malzGaben_modified()
{
    const int nModel = bh->sud()->modelMalzschuettung()->rowCount();
    int nLayout = ui->layoutMalzGaben->count();
    while (nLayout < nModel)
        ui->layoutMalzGaben->addWidget(new WdgMalzGabe(nLayout++));
    while (ui->layoutMalzGaben->count() != nModel)
        delete ui->layoutMalzGaben->itemAt(ui->layoutMalzGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget())->updateValues();
    updateMalzGaben();
}

void TabRezept::malzGaben_dataChanged()
{
    updateMalzGaben();
    updateMalzDiagram();
}

void TabRezept::updateMalzGaben()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        double p = 100.0;
        for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (fabs(p) < 0.01)
            p = 0.0;
        for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            wdg->setFehlProzent(p);
        }
    }
    updateMalzDiagram();
}

void TabRezept::updateMalzDiagram()
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QPieSeries *series = new QPieSeries();
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        const WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        series->append(wdg->name(), wdg->prozent());
    }
    QChart *chart = ui->diagramMalz->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
  #endif
}

void TabRezept::on_btnNeueMalzGabe_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Malz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        double p = 100.0;
        for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (fabs(p) < 0.01)
            p = 0.0;
        QMap<int, QVariant> values({{ModelMalzschuettung::ColSudID, bh->sud()->id()},
                                    {ModelMalzschuettung::ColName, dlg.name()},
                                    {ModelMalzschuettung::ColProzent, p}});
        bh->sud()->modelMalzschuettung()->append(values);
        ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::hopfenGaben_modified()
{
    const int nModel = bh->sud()->modelHopfengaben()->rowCount();
    int nLayout = ui->layoutHopfenGaben->count();
    while (nLayout < nModel)
        ui->layoutHopfenGaben->addWidget(new WdgHopfenGabe(nLayout++));
    while (ui->layoutHopfenGaben->count() != nModel)
        delete ui->layoutHopfenGaben->itemAt(ui->layoutHopfenGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget())->updateValues();
    updateHopfenGaben();
}

void TabRezept::hopfenGaben_dataChanged()
{
    updateHopfenGaben();
    updateHopfenDiagram();
}

void TabRezept::updateHopfenDiagram()
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QPieSeries *series = new QPieSeries();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        const WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        series->append(wdg->name(), wdg->prozent());
    }
    QChart *chart = ui->diagramHopfen->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
  #endif
}

void TabRezept::updateHopfenGaben()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        double p = 100.0;
        for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (fabs(p) < 0.01)
            p = 0.0;
        for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            wdg->setFehlProzent(p);
        }
    }
    updateHopfenDiagram();
}

void TabRezept::on_btnNeueHopfenGabe_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        double p = 100.0;
        for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (fabs(p) < 0.01)
            p = 0.0;
        QMap<int, QVariant> values({{ModelHopfengaben::ColSudID, bh->sud()->id()},
                                    {ModelHopfengaben::ColName, dlg.name()},
                                    {ModelHopfengaben::ColZeit, bh->sud()->getKochdauerNachBitterhopfung()},
                                    {ModelHopfengaben::ColProzent, p}});
        bh->sud()->modelHopfengaben()->append(values);
        ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_cbBerechnungsartHopfen_currentIndexChanged(int index)
{
    if (ui->cbBerechnungsartHopfen->hasFocus())
    {
        bh->sud()->setberechnungsArtHopfen(index - 1);
        updateHopfenGaben();
    }
}

void TabRezept::hefeGaben_modified()
{
    const int nModel = bh->sud()->modelHefegaben()->rowCount();
    int nLayout = ui->layoutHefeGaben->count();
    while (nLayout < nModel)
        ui->layoutHefeGaben->addWidget(new WdgHefeGabe(nLayout++));
    while (ui->layoutHefeGaben->count() != nModel)
        delete ui->layoutHefeGaben->itemAt(ui->layoutHefeGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
        static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget())->updateValues();
    updateHefeDiagram();
}

void TabRezept::updateHefeDiagram()
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QPieSeries *series = new QPieSeries();
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
    {
        const WdgHefeGabe* wdg = static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget());
        series->append(wdg->name(), wdg->menge());
    }
    QChart *chart = ui->diagramHefe->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
  #endif
}

void TabRezept::on_btnNeueHefeGabe_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hefe, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelHefegaben::ColSudID, bh->sud()->id()},
                                    {ModelHefegaben::ColName, dlg.name()}});
        bh->sud()->modelHefegaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::weitereZutatenGaben_modified()
{
    const int nModel = bh->sud()->modelWeitereZutatenGaben()->rowCount();
    int nLayout = ui->layoutWeitereZutatenGaben->count();
    while (nLayout < nModel)
        ui->layoutWeitereZutatenGaben->addWidget(new WdgWeitereZutatGabe(nLayout++));
    while (ui->layoutWeitereZutatenGaben->count() != nModel)
        delete ui->layoutWeitereZutatenGaben->itemAt(ui->layoutWeitereZutatenGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutWeitereZutatenGaben->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutWeitereZutatenGaben->itemAt(i)->widget())->updateValues(true);
}

void TabRezept::on_btnNeueHopfenstopfenGabe_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen)}});
        bh->sud()->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->setValue(ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnNeueWeitereZutat_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()}});
        bh->sud()->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->setValue(ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::anhaenge_modified()
{
    const int nModel = bh->sud()->modelAnhang()->rowCount();
    int nLayout = ui->layoutAnhang->count();
    while (nLayout < nModel)
        ui->layoutAnhang->addWidget(new WdgAnhang(nLayout++));
    while (ui->layoutAnhang->count() != nModel)
        delete ui->layoutAnhang->itemAt(ui->layoutAnhang->count() - 1)->widget();
    for (int i = 0; i < ui->layoutAnhang->count(); ++i)
        static_cast<WdgAnhang*>(ui->layoutAnhang->itemAt(i)->widget())->updateValues();
}

void TabRezept::on_btnNeuerAnhang_clicked()
{
    QMap<int, QVariant> values({{ModelAnhang::ColSudID, bh->sud()->id()}});
    int index = bh->sud()->modelAnhang()->append(values);
    static_cast<WdgAnhang*>(ui->layoutAnhang->itemAt(index)->widget())->openDialog();
    ui->scrollAreaAnhang->verticalScrollBar()->setValue(ui->scrollAreaAnhang->verticalScrollBar()->maximum());
}

void TabRezept::on_tbSudname_textChanged(const QString &value)
{
    if (ui->tbSudname->hasFocus())
        bh->sud()->setSudname(value);
}

void TabRezept::on_cbAnlage_currentIndexChanged(const QString &value)
{
    if (ui->cbAnlage->hasFocus())
        bh->sud()->setAnlage(value);
}

void TabRezept::on_tbKommentar_textChanged()
{
    if (ui->tbKommentar->hasFocus())
    {
        QString kommentar = ui->tbKommentar->toPlainText().replace("<br>", "\n");
        if (kommentar != bh->sud()->getKommentar())
            bh->sud()->setKommentar(kommentar);
    }
}

void TabRezept::on_btnSudhausausbeute_clicked()
{
    bh->sud()->setSudhausausbeute(bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
}

void TabRezept::on_btnVerdampfungsziffer_clicked()
{
    bh->sud()->setVerdampfungsrate(bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsziffer).toDouble());
}

void TabRezept::on_cbWasserProfil_currentIndexChanged(const QString &value)
{
    if (ui->cbWasserProfil->hasFocus())
        bh->sud()->setWasserprofil(value);
}

void TabRezept::on_btnTagNeu_clicked()
{
    QMap<int, QVariant> values({{ModelTags::ColSudID, bh->sud()->id()},
                                {ModelTags::ColKey, tr("Neuer Tag")}});
    ProxyModel *model = bh->sud()->modelTags();
    int row = model->append(values);
    if (row >= 0)
    {
        QModelIndex index = model->index(row, ModelTags::ColKey);
        ui->tableTags->setCurrentIndex(index);
        ui->tableTags->scrollTo(index);
        ui->tableTags->edit(index);
    }
}

void TabRezept::on_btnTagLoeschen_clicked()
{
    ProxyModel *model = bh->sud()->modelTags();
    QModelIndexList indices = ui->tableTags->selectionModel()->selectedIndexes();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        model->removeRow(index.row());
}
