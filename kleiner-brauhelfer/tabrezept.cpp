#include "tabrezept.h"
#include "ui_tabrezept.h"
#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QScrollBar>
#include "biercalc.h"
#include "settings.h"
#include "mainwindow.h"
#include "model/textdelegate.h"
#include "model/tagglobaldelegate.h"
#include "widgets/wdgrast.h"
#include "widgets/wdgmalzgabe.h"
#include "widgets/wdghopfengabe.h"
#include "widgets/wdghefegabe.h"
#include "widgets/wdgweiterezutatgabe.h"
#include "widgets/wdgwasseraufbereitung.h"
#include "widgets/wdganhang.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlguebernahmerezept.h"
#include "dialogs/dlgtableview.h"
#include "dialogs/dlgwasseraufbereitung.h"
#include "dialogs/dlgwasserprofile.h"
#include "widgets/widgetdecorator.h"
#include "commands/undostack.h"

extern Settings* gSettings;

TabRezept::TabRezept(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabRezept),
    mSud(nullptr)
{
    ui->setupUi(this);
}

TabRezept::~TabRezept()
{
    delete ui;
    delete mGlasSvg;
}

void TabRezept::setup(SudObject *sud)
{
    mSud = sud;

    QPalette pal;

    ui->tbCO2->setColumn(mSud, ModelSud::ColCO2);
    ui->tbSW->setColumn(mSud, ModelSud::ColSW);
    ui->tbMenge->setColumn(mSud, ModelSud::ColMenge);
    ui->tbFaktorHauptguss->setColumn(mSud, ModelSud::ColFaktorHauptguss);
    ui->tbGesamtschuettung->setColumn(mSud, ModelSud::Colerg_S_Gesamt);
    ui->tbSWMalz->setColumn(mSud, ModelSud::ColSWAnteilMalz);
    ui->tbSWWZMaischen->setColumn(mSud, ModelSud::ColSWAnteilZusatzMaischen);
    ui->tbSWWZKochen->setColumn(mSud, ModelSud::ColSWAnteilZusatzKochen);
    ui->tbSWWZGaerung->setColumn(mSud, ModelSud::ColSWAnteilZusatzGaerung);
    ui->tbSWAnteilHefestarter->setColumn(mSud, ModelSud::ColSWAnteilHefestarter);
    ui->tbSudnummer->setColumn(mSud, ModelSud::ColSudnummer);
    ui->tbBittere->setColumn(mSud, ModelSud::ColIBU);
    ui->tbFarbe->setColumn(mSud, ModelSud::Colerg_Farbe);
    ui->tbSudhausausbeute->setColumn(mSud, ModelSud::ColSudhausausbeute);
    ui->tbVerdampfungsrate->setColumn(mSud, ModelSud::ColVerdampfungsrate);
    ui->tbVergaerungsgrad->setColumn(mSud, ModelSud::ColVergaerungsgrad);
    ui->tbReifezeit->setColumn(mSud, ModelSud::ColReifezeit);
    ui->tbHGF->setColumn(mSud, ModelSud::ColhighGravityFaktor);
    ui->tbKochzeit->setColumn(mSud, ModelSud::ColKochdauer);
    ui->tbNachisomerisierungszeit->setColumn(mSud, ModelSud::ColNachisomerisierungszeit);
    ui->tbKosten->setColumn(mSud, ModelSud::Colerg_Preis);
    ui->tbWasserGesamt->setColumn(mSud, ModelSud::Colerg_W_Gesamt);
    ui->tbHauptguss->setColumn(mSud, ModelSud::Colerg_WHauptguss);
    ui->tbNachguss->setColumn(mSud, ModelSud::Colerg_WNachguss);
    ui->tbWasserHGF->setColumn(mSud, ModelSud::ColMengeSollHgf);
    ui->tbRestextrakt->setColumn(mSud, ModelSud::ColSRESoll);
    ui->tbAlkohol->setColumn(mSud, ModelSud::ColAlkoholSoll);
    ui->tbRestalkalitaetSoll->setColumn(mSud, ModelSud::ColRestalkalitaetSoll);
    ui->tbRestalkalitaetWasser->setColumn(mSud, ModelSud::ColRestalkalitaetWasser);
    ui->tbRestalkalitaetIst->setColumn(mSud, ModelSud::ColRestalkalitaetIst);
    ui->tbPhMalz->setColumn(mSud, ModelSud::ColPhMalz);
    ui->tbPhMaische->setColumn(mSud, ModelSud::ColPhMaische);
    ui->tbPhMaischeSoll->setColumn(mSud, ModelSud::ColPhMaischeSoll);
    ui->tbMengeHefestarter->setColumn(mSud, ModelSud::ColMengeHefestarter);
    ui->tbSWHefestarter->setColumn(mSud, ModelSud::ColSWHefestarter);

    ui->lblWarnungMalz->setPalette(gSettings->paletteErrorLabel);
    ui->btnMalzAusgleichen->setError(true);
    ui->btnShaAnpassen->setError(true);
    ui->lblWarnungHopfen->setPalette(gSettings->paletteErrorLabel);
    ui->btnHopfenAusgleichen->setError(true);
    ui->lblWarnungMaischplanWasser->setPalette(gSettings->paletteErrorLabel);
    ui->lblWarnungMaischplanMalz->setPalette(gSettings->paletteErrorLabel);
    ui->btnMaischplanAusgleichen->setError(true);
    ui->btnMaischplanFaktorAnpassen->setError(true);
    ui->btnMaischplanAusgleichenMalz->setError(true);
    ui->lblWarnungPh->setPalette(gSettings->paletteErrorLabel);

    pal = ui->btnNeueMalzGabe->palette();
    pal.setColor(QPalette::Button, gSettings->colorMalz);
    ui->btnNeueMalzGabe->setDefaultPalette(pal);
    ui->btnMalzGabenUebernehmen->setDefaultPalette(pal);

    pal = ui->btnNeueHopfenGabe->palette();
    pal.setColor(QPalette::Button, gSettings->colorHopfen);
    ui->btnNeueHopfenGabe->setDefaultPalette(pal);
    ui->btnHopfenGabenUebernehmen->setDefaultPalette(pal);
    ui->btnNeueHopfenGabeGaerung->setDefaultPalette(pal);
    ui->btnHopfenGabenUebernehmenGaerung->setDefaultPalette(pal);

    pal = ui->btnNeueHefeGabe->palette();
    pal.setColor(QPalette::Button, gSettings->colorHefe);
    ui->btnNeueHefeGabe->setDefaultPalette(pal);
    ui->btnHefeGabenUebernehmen->setDefaultPalette(pal);

    pal = ui->btnNeueZusatzGabeKochen->palette();
    pal.setColor(QPalette::Button, gSettings->colorZusatz);
    ui->btnNeueZusatzGabeMaischen->setDefaultPalette(pal);
    ui->btnZusazGabenUebernehmenMaischen->setDefaultPalette(pal);
    ui->btnNeueZusatzGabeKochen->setDefaultPalette(pal);
    ui->btnZusazGabenUebernehmenKochen->setDefaultPalette(pal);
    ui->btnNeueZusatzGabeGaerung->setDefaultPalette(pal);
    ui->btnZusazGabenUebernehmenGaerung->setDefaultPalette(pal);

    pal = ui->btnNeueRast->palette();
    pal.setColor(QPalette::Button, gSettings->colorRast);
    ui->btnNeueRast->setDefaultPalette(pal);
    ui->btnRastenUebernehmen->setDefaultPalette(pal);

    pal = ui->btnNeueWasseraufbereitung->palette();
    pal.setColor(QPalette::Button, gSettings->colorWasser);
    ui->btnNeueWasseraufbereitung->setDefaultPalette(pal);
    ui->btnWasseraufbereitungUebernehmen->setDefaultPalette(pal);

    pal = ui->btnNeuerAnhang->palette();
    pal.setColor(QPalette::Button, gSettings->colorAnhang);
    ui->btnNeuerAnhang->setDefaultPalette(pal);

    mGlasSvg = new QGraphicsSvgItem(gSettings->theme() == Qt::ColorScheme::Dark ? QStringLiteral(":/images/icons/dark/svg/bier.svg") : QStringLiteral(":/images/icons/light/svg/bier.svg"));
    ui->lblKostenEinheit->setText(QLocale().currencySymbol() + "/L");

    ProxyModel* proxy = new ProxyModel(this);
    proxy->setSourceModel(mSud->bh()->modelKategorien());
    ui->cbKategorie->setModel(proxy);
    ui->cbKategorie->setModelColumn(ModelKategorien::ColName);

    ui->wdgBemerkung->setSudObject(mSud);
    ui->wdgBemerkung->setPlaceholderText(tr("Bemerkung Rezept"));
    ui->wdgBemerkungMaischen->setSudObject(mSud);
    ui->wdgBemerkungMaischen->setPlaceholderText(tr("Bemerkung Maischen"));
    ui->wdgBemerkungKochen->setSudObject(mSud);
    ui->wdgBemerkungKochen->setPlaceholderText(tr("Bemerkung Kochen"));
    ui->wdgBemerkungGaerung->setSudObject(mSud);
    ui->wdgBemerkungGaerung->setPlaceholderText(tr("Bemerkung Gärung"));
    ui->wdgBemerkungMaischplan->setSudObject(mSud);
    ui->wdgBemerkungMaischplan->setPlaceholderText(tr("Bemerkung Maischplan"));
    ui->wdgBemerkungWasseraufbereitung->setSudObject(mSud);
    ui->wdgBemerkungWasseraufbereitung->setPlaceholderText(tr("Bemerkung Wasseraufbereitung"));

    gSettings->beginGroup("TabRezept");

    ui->splitter->setSizes({200, 100, 200});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterMaischen->setSizes({1, 150, 50});
    mDefaultSplitterMaischenState = ui->splitterMaischen->saveState();
    ui->splitterMaischen->restoreState(gSettings->value("splitterMaischenState").toByteArray());
    ui->splitterKochen->setSizes({1, 150, 50});
    mDefaultSplitterKochenState = ui->splitterKochen->saveState();
    ui->splitterKochen->restoreState(gSettings->value("splitterKochenState").toByteArray());
    ui->splitterGaerung->setSizes({1, 50});
    mDefaultSplitterGaerungState = ui->splitterGaerung->saveState();
    ui->splitterGaerung->restoreState(gSettings->value("splitterGaerungState").toByteArray());
    ui->splitterMaischplan->setSizes({1, 200, 50});
    mDefaultSplitterMaischplanState = ui->splitterMaischplan->saveState();
    ui->splitterMaischplan->restoreState(gSettings->value("splitterMaischplanState").toByteArray());
    ui->splitterWasseraufbereitung->setSizes({1, 50});
    mDefaultSplitterWasseraufbereitungState = ui->splitterWasseraufbereitung->saveState();
    ui->splitterWasseraufbereitung->restoreState(gSettings->value("splitterWasseraufbereitungState").toByteArray());

    gSettings->endGroup();

    connect(mSud->bh(), &Brauhelfer::modified, this, &TabRezept::updateValues);
    connect(mSud->bh(), &Brauhelfer::discarded, this, &TabRezept::sudLoaded);
    connect(mSud, &SudObject::loadedChanged, this, &TabRezept::sudLoaded);
    connect(mSud, &SudObject::dataChanged, this, &TabRezept::sudDataChanged);

    connect(mSud->modelMaischplan(), &ProxyModel::layoutChanged,this, &TabRezept::rasten_modified);
    connect(mSud->modelMaischplan(), &ProxyModel::rowsInserted, this, &TabRezept::rasten_modified);
    connect(mSud->modelMaischplan(), &ProxyModel::rowsRemoved, this, &TabRezept::rasten_modified);
    connect(mSud->modelMaischplan(), &ProxyModel::dataChanged, this, &TabRezept::updateMaischplan);

    connect(mSud->modelMalzschuettung(), &ProxyModel::layoutChanged, this, &TabRezept::malzGaben_modified);
    connect(mSud->modelMalzschuettung(), &ProxyModel::rowsInserted, this, &TabRezept::malzGaben_modified);
    connect(mSud->modelMalzschuettung(), &ProxyModel::rowsRemoved, this, &TabRezept::malzGaben_modified);
    connect(mSud->modelMalzschuettung(), &ProxyModel::dataChanged, this, &TabRezept::updateMalzGaben);

    connect(mSud->modelHopfengaben(), &ProxyModel::layoutChanged, this, &TabRezept::hopfenGaben_modified);
    connect(mSud->modelHopfengaben(), &ProxyModel::rowsInserted, this, &TabRezept::hopfenGaben_modified);
    connect(mSud->modelHopfengaben(), &ProxyModel::rowsRemoved, this, &TabRezept::hopfenGaben_modified);
    connect(mSud->modelHopfengaben(), &ProxyModel::dataChanged, this, &TabRezept::updateHopfenGaben);

    connect(mSud->modelHefegaben(), &ProxyModel::layoutChanged, this, &TabRezept::hefeGaben_modified);
    connect(mSud->modelHefegaben(), &ProxyModel::rowsInserted, this, &TabRezept::hefeGaben_modified);
    connect(mSud->modelHefegaben(), &ProxyModel::rowsRemoved, this, &TabRezept::hefeGaben_modified);

    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::layoutChanged, this, &TabRezept::weitereZutatenGaben_modified);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::rowsInserted, this, &TabRezept::weitereZutatenGaben_modified);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::rowsRemoved, this, &TabRezept::weitereZutatenGaben_modified);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::dataChanged, this, &TabRezept::updateExtrakt);

    connect(mSud->modelWasseraufbereitung(), &ProxyModel::layoutChanged, this, &TabRezept::wasseraufbereitung_modified);
    connect(mSud->modelWasseraufbereitung(), &ProxyModel::rowsInserted, this, &TabRezept::wasseraufbereitung_modified);
    connect(mSud->modelWasseraufbereitung(), &ProxyModel::rowsRemoved, this, &TabRezept::wasseraufbereitung_modified);

    connect(mSud->modelAnhang(), &ProxyModel::layoutChanged, this, &TabRezept::anhaenge_modified);
    connect(mSud->modelAnhang(), &ProxyModel::rowsInserted, this, &TabRezept::anhaenge_modified);
    connect(mSud->modelAnhang(), &ProxyModel::rowsRemoved, this, &TabRezept::anhaenge_modified);

    connect(ui->wdgBemerkung, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColKommentar, html));});
    connect(ui->wdgBemerkungMaischen, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungZutatenMaischen, html));});
    connect(ui->wdgBemerkungKochen, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungZutatenKochen, html));});
    connect(ui->wdgBemerkungGaerung, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungZutatenGaerung, html));});
    connect(ui->wdgBemerkungMaischplan, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungMaischplan, html));});
    connect(ui->wdgBemerkungWasseraufbereitung, &WdgBemerkung::changed, this, [this](const QString& html){gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColBemerkungWasseraufbereitung, html));});

    connect(ui->btnAnlage, &QAbstractButton::clicked, MainWindow::getInstance()->getAction("actionAusruestung"), &QAction::triggered);

    TableView *table = ui->tableTags;
    table->setModel(mSud->modelTags());
    table->appendCol({ModelTags::ColKey, true, false, 0, new TextDelegate(table)});
    table->appendCol({ModelTags::ColValue, true, false, -1, new TextDelegate(table)});
    table->appendCol({ModelTags::ColGlobal, true, false, 0, new TagGlobalDelegate(mSud, table)});
    table->build();
}

void TabRezept::saveSettings()
{
    gSettings->beginGroup("TabRezept");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterMaischenState", ui->splitterMaischen->saveState());
    gSettings->setValue("splitterKochenState", ui->splitterKochen->saveState());
    gSettings->setValue("splitterGaerungState", ui->splitterGaerung->saveState());
    gSettings->setValue("splitterMaischplanState", ui->splitterMaischplan->saveState());
    gSettings->setValue("splitterWasseraufbereitungState", ui->splitterWasseraufbereitung->saveState());
    gSettings->endGroup();
}

void TabRezept::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterMaischen->restoreState(mDefaultSplitterMaischenState);
    ui->splitterKochen->restoreState(mDefaultSplitterKochenState);
    ui->splitterGaerung->restoreState(mDefaultSplitterGaerungState);
    ui->splitterMaischplan->restoreState(mDefaultSplitterMaischplanState);
    ui->splitterWasseraufbereitung->restoreState(mDefaultSplitterWasseraufbereitungState);
}

void TabRezept::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleAusruestung))
    {
        gSettings->setVisibleModule(Settings::ModuleAusruestung,
                         {ui->cbAnlage,
                          ui->lblAnlage,
                          ui->btnAnlage,
                          ui->btnSudhausausbeute,
                          ui->btnVerdampfungsrate,
                          ui->groupAnlage});
    }
    if (modules.testFlag(Settings::ModuleWasseraufbereitung))
    {
        if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
            ui->tabMitte->addTab(ui->tabWasseraufbereitung, tr("Wasseraufbereitung"));
        else
            ui->tabMitte->removeTab(ui->tabMitte->indexOf(ui->tabWasseraufbereitung));
    }
    if (modules.testFlag(Settings::ModulePreiskalkulation))
    {
        gSettings->setVisibleModule(Settings::ModulePreiskalkulation,
                         {ui->tbKosten,
                          ui->lblKosten,
                          ui->lblKostenEinheit,
                          ui->lineKosten});
    }
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        if (mSud->isLoaded())
        {
            malzGaben_modified();
            hopfenGaben_modified();
            hefeGaben_modified();
            weitereZutatenGaben_modified();
        }
    }
    if (mSud->isLoaded())
    {
        checkEnabled();
        updateValues();
    }
}

void TabRezept::sudLoaded()
{
    checkEnabled();
    ui->cbAnlage->setCurrentIndex(-1);
    ui->cbWasserProfil->setCurrentIndex(-1);
    ui->wdgBemerkung->setHtml(mSud->getKommentar());
    ui->wdgBemerkungMaischen->setHtml(mSud->getBemerkungZutatenMaischen());
    ui->wdgBemerkungKochen->setHtml(mSud->getBemerkungZutatenKochen());
    ui->wdgBemerkungGaerung->setHtml(mSud->getBemerkungZutatenGaerung());
    ui->wdgBemerkungMaischplan->setHtml(mSud->getBemerkungMaischplan());
    ui->wdgBemerkungWasseraufbereitung->setHtml(mSud->getBemerkungWasseraufbereitung());
    if (mSud->isLoaded())
    {
        updateAnlageModel();
        updateWasserModel();
        updateValues();
        rasten_modified();
        malzGaben_modified();
        hopfenGaben_modified();
        hefeGaben_modified();
        weitereZutatenGaben_modified();
        wasseraufbereitung_modified();
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
        wasseraufbereitung_modified();
        checkRohstoffe();
        break;
    case ModelSud::ColKommentar:
        ui->wdgBemerkung->setHtml(mSud->getKommentar());
        break;
    case ModelSud::ColBemerkungZutatenMaischen:
        ui->wdgBemerkungMaischen->setHtml(mSud->getBemerkungZutatenMaischen());
        break;
    case ModelSud::ColBemerkungZutatenKochen:
        ui->wdgBemerkungKochen->setHtml(mSud->getBemerkungZutatenKochen());
        break;
    case ModelSud::ColBemerkungZutatenGaerung:
        ui->wdgBemerkungGaerung->setHtml(mSud->getBemerkungZutatenGaerung());
        break;
    case ModelSud::ColBemerkungMaischplan:
        ui->wdgBemerkungMaischplan->setHtml(mSud->getBemerkungMaischplan());
        break;
    case ModelSud::ColBemerkungWasseraufbereitung:
        ui->wdgBemerkungWasseraufbereitung->setHtml(mSud->getBemerkungWasseraufbereitung());
        break;
    }
}

void TabRezept::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateValues();
}

void TabRezept::changeEvent(QEvent* event)
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

void TabRezept::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    bool gebraut = status != Brauhelfer::SudStatus::Rezept && !gSettings->ForceEnabled;
    bool abgefuellt = status > Brauhelfer::SudStatus::Gebraut  && !gSettings->ForceEnabled;
    ui->cbAnlage->setEnabled(!gebraut);
    ui->tbMenge->setDisabled(gebraut);
    ui->tbSW->setDisabled(gebraut);
    ui->tbSudhausausbeute->setDisabled(gebraut);
    ui->tbFaktorHauptguss->setDisabled(gebraut);
    ui->tbHGF->setDisabled(gebraut);
    ui->tbBittere->setDisabled(gebraut);
    ui->tbKochzeit->setDisabled(gebraut);
    ui->tbVerdampfungsrate->setDisabled(gebraut);
    ui->tbNachisomerisierungszeit->setDisabled(gebraut);
    ui->tbVergaerungsgrad->setDisabled(gebraut);
    ui->tbCO2->setDisabled(gebraut);
    ui->cbWasserProfil->setEnabled(!gebraut);
    ui->tbRestalkalitaetSoll->setDisabled(gebraut);
    ui->tbPhMaischeSoll->setDisabled(gebraut);
    ui->btnWasseraufbereitungUebernehmen->setVisible(!gebraut);
    ui->tbReifezeit->setDisabled(gebraut);
    ui->btnNeueRast->setVisible(!gebraut);
    ui->btnRastenUebernehmen->setVisible(!gebraut);
    ui->lineNeueRast->setVisible(!gebraut);
    ui->wdgAddMaischen->setVisible(!gebraut);
    ui->wdgAddKochen->setVisible(!gebraut);
    ui->cbBerechnungsartHopfen->setEnabled(!gebraut);
    ui->tbMengeHefestarter->setDisabled(gebraut);
    ui->tbSWHefestarter->setDisabled(gebraut);
    ui->wdgAddGaerung->setVisible(!abgefuellt);
    ui->btnNeueWasseraufbereitung->setVisible(!gebraut);
    ui->lineNeueWasseraufbereitung->setVisible(!gebraut);
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutZusaetzeMaischen->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeMaischen->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutZusaetzeKochen->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeKochen->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
        static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutZusaetzeGaerung->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeGaerung->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
        static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget())->updateValues();
    for (int i = 0; i < ui->layoutWasseraufbereitung->count(); ++i)
        static_cast<WdgWasseraufbereitung*>(ui->layoutWasseraufbereitung->itemAt(i)->widget())->updateValues();
}

int TabRezept::checkRohstoffeDialog(Brauhelfer::RohstoffTyp typ, const QString& name)
{
    QString msg;
    switch(typ)
    {
    case Brauhelfer::RohstoffTyp::Malz:
        msg = tr("Das Malz \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll es jetzt hinzugefügt werden?").arg(name);
        break;
    case Brauhelfer::RohstoffTyp::Hopfen:
        msg = tr("Der Hopfen \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll es jetzt hinzugefügt werden?").arg(name);
        break;
    case Brauhelfer::RohstoffTyp::Hefe:
        msg = tr("Die Hefe \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll es jetzt hinzugefügt werden?").arg(name);
        break;
    case Brauhelfer::RohstoffTyp::Zusatz:
        msg = tr("Die Zutat \"%1\" ist nicht in der Rohstoffliste vorhanden. Soll jetzt es hinzugefügt werden?").arg(name);
        break;
    }
    return QMessageBox::question(this, tr("Rohstoff importieren?"), msg,
                                 QMessageBox::Yes | QMessageBox::YesAll | QMessageBox::No | QMessageBox::NoAll | QMessageBox::Cancel);
}

void TabRezept::checkRohstoffe()
{
    bool yesAll = false;
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = yesAll ? QMessageBox::Yes : checkRohstoffeDialog(Brauhelfer::RohstoffTyp::Malz, wdg->name());
            if (ret == QMessageBox::Yes || ret == QMessageBox::YesAll)
            {
                if (ret == QMessageBox::YesAll)
                    yesAll = true;
                mSud->bh()->modelMalzschuettung()->import(mSud->modelMalzschuettung()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                mSud->unload();
                return;
            }
        }
    }
    for (int i = 0; i < ui->layoutZusaetzeMaischen->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeMaischen->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = yesAll ? QMessageBox::Yes : checkRohstoffeDialog(Brauhelfer::RohstoffTyp::Zusatz, wdg->name());
            if (ret == QMessageBox::Yes || ret == QMessageBox::YesAll)
            {
                if (ret == QMessageBox::YesAll)
                    yesAll = true;
                mSud->bh()->modelWeitereZutatenGaben()->import(mSud->modelWeitereZutatenGaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                mSud->unload();
                return;
            }
        }
    }
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = yesAll ? QMessageBox::Yes : checkRohstoffeDialog(Brauhelfer::RohstoffTyp::Hopfen, wdg->name());
            if (ret == QMessageBox::Yes || ret == QMessageBox::YesAll)
            {
                if (ret == QMessageBox::YesAll)
                    yesAll = true;
                mSud->bh()->modelHopfengaben()->import(mSud->modelHopfengaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                mSud->unload();
                return;
            }
        }
    }
    for (int i = 0; i < ui->layoutZusaetzeKochen->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeKochen->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = yesAll ? QMessageBox::Yes : checkRohstoffeDialog(Brauhelfer::RohstoffTyp::Zusatz, wdg->name());
            if (ret == QMessageBox::Yes || ret == QMessageBox::YesAll)
            {
                if (ret == QMessageBox::YesAll)
                    yesAll = true;
                mSud->bh()->modelWeitereZutatenGaben()->import(mSud->modelWeitereZutatenGaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                mSud->unload();
                return;
            }
        }
    }
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
    {
        WdgHefeGabe* wdg = static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = yesAll ? QMessageBox::Yes : checkRohstoffeDialog(Brauhelfer::RohstoffTyp::Hefe, wdg->name());
            if (ret == QMessageBox::Yes || ret == QMessageBox::YesAll)
            {
                if (ret == QMessageBox::YesAll)
                    yesAll = true;
                mSud->bh()->modelHefegaben()->import(mSud->modelHefegaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                mSud->unload();
                return;
            }
        }
    }
    for (int i = 0; i < ui->layoutZusaetzeGaerung->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeGaerung->itemAt(i)->widget());
        if (!wdg->isValid())
        {
            int ret = yesAll ? QMessageBox::Yes : checkRohstoffeDialog(Brauhelfer::RohstoffTyp::Zusatz, wdg->name());
            if (ret == QMessageBox::Yes || ret == QMessageBox::YesAll)
            {
                if (ret == QMessageBox::YesAll)
                    yesAll = true;
                mSud->bh()->modelWeitereZutatenGaben()->import(mSud->modelWeitereZutatenGaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                mSud->unload();
                return;
            }
        }
    }
}

void TabRezept::updateValues()
{
    double fVal, diff;
    if (!isVisible())
        return;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept || gSettings->ForceEnabled;

    for (auto& wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();
    for (auto& wdg : findChildren<SpinBoxSud*>())
        wdg->updateValue();

    if (!ui->tbSudname->hasFocus())
    {
        ui->tbSudname->setText(mSud->getSudname());
        ui->tbSudname->setCursorPosition(0);
    }
    if (!ui->cbKategorie->hasFocus())
        ui->cbKategorie->setCurrentIndex(ui->cbKategorie->findText(mSud->getKategorie()));

    // ModuleAusruestung
    if (!ui->cbAnlage->hasFocus())
        ui->cbAnlage->setCurrentIndex(ui->cbAnlage->findText(mSud->getAnlage()));
    if (ui->cbAnlage->currentIndex() == -1 || !gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        ui->cbAnlage->setError(ui->cbAnlage->currentIndex() == -1);
        ui->groupAnlage->setVisible(false);
        ui->btnSudhausausbeute->setVisible(false);
        ui->btnVerdampfungsrate->setVisible(false);
    }
    else
    {
        ui->cbAnlage->setError(false);
        ui->groupAnlage->setVisible(true);
        diff = mSud->getSudhausausbeute() - mSud->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble();
        ui->btnSudhausausbeute->setVisible(enabled && qAbs(diff) > 0.05);
        diff = mSud->getVerdampfungsrate() - mSud->getAnlageData(ModelAusruestung::ColVerdampfungsrate).toDouble();
        ui->btnVerdampfungsrate->setVisible(enabled && qAbs(diff) > 0.05);
        ui->lblAnlageName->setText(mSud->getAnlage());
        ui->tbAnlageKorrekturSollmenge->setValue(mSud->getAnlageData(ModelAusruestung::ColKorrekturMenge).toDouble());
        ui->wdgAnlageKorrekturSollmenge->setVisible(ui->tbAnlageKorrekturSollmenge->value() > 0);
        ui->tbAnlageSudhausausbeute->setValue(mSud->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
        ui->tbAnlageVerdampfung->setValue(mSud->getAnlageData(ModelAusruestung::ColVerdampfungsrate).toDouble());
        ui->tbAnlageVolumenMaische->setValue(mSud->getAnlageData(ModelAusruestung::ColMaischebottich_MaxFuellvolumen).toDouble());
        ui->tbVolumenMaische->setValue(mSud->geterg_WHauptguss() + BierCalc::MalzVerdraengung * mSud->geterg_S_Gesamt());
        ui->tbVolumenMaische->setError(ui->tbVolumenMaische->value() > ui->tbAnlageVolumenMaische->value());
        ui->tbAnlageVolumenKochen->setValue(mSud->getAnlageData(ModelAusruestung::ColSudpfanne_MaxFuellvolumen).toDouble());
        ui->tbVolumenKochen->setValue(BierCalc::volumenWasser(20.0, 100.0, mSud->getMengeSollKochbeginn()));
        ui->tbVolumenKochen->setError(ui->tbVolumenKochen->value() > ui->tbAnlageVolumenKochen->value());
    }

    // ModuleWasseraufbereitung
    if (!ui->cbWasserProfil->hasFocus())
        ui->cbWasserProfil->setCurrentIndex(ui->cbWasserProfil->findText(mSud->getWasserprofil()));
    ui->cbWasserProfil->setError(ui->cbWasserProfil->currentIndex() == -1);
    diff = ui->tbRestalkalitaetSoll->value() - ui->tbRestalkalitaetIst->value();
    ui->tbRestalkalitaetIst->setError(enabled && qAbs(diff) > 0.05);
    bool phOk = ui->tbPhMalz->value() > 0;
    ui->lblWarnungPh->setVisible(!phOk);
    ui->tbPhMalz->setVisible(phOk);
    ui->tbPhMaischeSoll->setVisible(phOk);
    ui->tbPhMaische->setVisible(phOk);
    ui->lblPhMalz->setVisible(phOk);
    ui->lblPhMaischeSoll->setVisible(phOk);
    ui->lblPhMaische->setVisible(phOk);
    ui->tbPhMaischeSoll->setError(enabled && (ui->tbPhMaischeSoll->value() < 5.2 || ui->tbPhMaischeSoll->value() > 5.8));
    diff = ui->tbPhMaischeSoll->value() - ui->tbPhMaische->value();
    ui->tbPhMaische->setError(enabled && qAbs(diff) > 0.005);

    ui->wdgSWMalz->setVisible(ui->tbSWMalz->value() > 0.0);
    ui->wdgSWWZMaischen->setVisible(ui->tbSWWZMaischen->value() > 0.0);
    ui->wdgSWWZKochen->setVisible(ui->tbSWWZKochen->value() > 0.0);
    ui->wdgSWWZGaerung->setVisible(ui->tbSWWZGaerung->value() > 0.0);
    ui->wdgSWAnteilHefestarter->setVisible(ui->tbSWAnteilHefestarter->value() > 0.0);
    fVal = mSud->getIBU() / mSud->getSW();
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

    Brauhelfer::AnlageTyp anlageTyp = static_cast<Brauhelfer::AnlageTyp>(mSud->getAnlageData(ModelAusruestung::ColTyp).toInt());
    ui->tbFaktorHauptgussEmpfehlung->setValue(BierCalc::hauptgussFaktor(mSud->geterg_Farbe()));
    ui->wdgFaktorHauptguss->setVisible(anlageTyp != Brauhelfer::AnlageTyp::GrainfatherG30 && anlageTyp != Brauhelfer::AnlageTyp::BrauheldPro30);
    ui->wdgWasserHGF->setVisible(mSud->gethighGravityFaktor() != 0.0);

    ui->tbKochzeit->setError(ui->tbKochzeit->value() == 0.0);
    Brauhelfer::BerechnungsartHopfen berechnungsArtHopfen = static_cast<Brauhelfer::BerechnungsartHopfen>(mSud->getberechnungsArtHopfen());
    if (!ui->cbBerechnungsartHopfen->hasFocus())
        ui->cbBerechnungsartHopfen->setCurrentIndex(static_cast<int>(berechnungsArtHopfen) + 1);
    switch (berechnungsArtHopfen)
    {
    case Brauhelfer::BerechnungsartHopfen::Keine:
        ui->lblWarnungHopfen->setText(tr("Die Berechnung der Hopfengaben ist deaktiviert."));
        ui->btnHopfenAusgleichen->setVisible(false);
        break;
    case Brauhelfer::BerechnungsartHopfen::Gewicht:
        ui->lblWarnungHopfen->setText(tr("Die Summe der angegebenen Hopfenmengen entspricht nicht der Gesamtmenge."));
        ui->btnHopfenAusgleichen->setVisible(true);
        break;
    case Brauhelfer::BerechnungsartHopfen::IBU:
        ui->lblWarnungHopfen->setText(tr("Die Summe der angegebenen Anteile an Bittere entspricht nicht der Gesamtbittere."));
        ui->btnHopfenAusgleichen->setVisible(true);
        break;
    }

    updateGlas();
}

void TabRezept::updateGlas()
{
    QGraphicsScene* scene = new QGraphicsScene();
    QPen pen(Qt::white);
    QBrush brush(BierCalc::ebcToColor(mSud->geterg_Farbe()));
    scene->addRect(10, 10, 980, 980, pen, brush);
    scene->addItem(mGlasSvg);
    ui->gvGlas->setScene(scene);
    ui->gvGlas->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void TabRezept::updateAnlageModel()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        ProxyModel *model = new ProxyModel(ui->cbAnlage);
        model->setSourceModel(mSud->bh()->modelAusruestung());
        ui->cbAnlage->setModel(model);
        ui->cbAnlage->setModelColumn(ModelAusruestung::ColName);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbAnlage);
        model->setItem(0, 0, new QStandardItem(mSud->getAnlage()));
        ui->cbAnlage->setModel(model);
        ui->cbAnlage->setModelColumn(0);
    }
    ui->cbAnlage->setCurrentIndex(-1);
}

void TabRezept::updateWasserModel()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        ProxyModel *model = new ProxyModel(ui->cbWasserProfil);
        model->setSourceModel(mSud->bh()->modelWasser());
        ui->cbWasserProfil->setModel(model);
        ui->cbWasserProfil->setModelColumn(ModelWasser::ColName);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbWasserProfil);
        model->setItem(0, 0, new QStandardItem(mSud->getWasserprofil()));
        ui->cbWasserProfil->setModel(model);
        ui->cbWasserProfil->setModelColumn(0);
    }
    ui->cbWasserProfil->setCurrentIndex(-1);
}

void TabRezept::rasten_modified()
{    
    const int nModel = mSud->modelMaischplan()->rowCount();
    int nLayout = ui->layoutRasten->count();
    while (nLayout < nModel)
        ui->layoutRasten->addWidget(new WdgRast(mSud, nLayout++, ui->layoutRasten, this));
    while (ui->layoutRasten->count() != nModel)
        delete ui->layoutRasten->itemAt(ui->layoutRasten->count() - 1)->widget();
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
        static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget())->updateValues();
    updateMaischplan();
}

void TabRezept::updateMaischplan()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        double pWasser = 100.0, pMalz = 100.0;
        int count = ui->layoutRasten->count();
        for (int i = 0; i < count; ++i)
        {
            WdgRast* wdg = static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget());
            pWasser -= wdg->prozentWasser();
            pMalz -= wdg->prozentMalz();
        }
        if (std::fabs(pWasser) < 0.1)
            pWasser = 0.0;
        if (std::fabs(pMalz) < 0.1)
            pMalz = 0.0;
        for (int i = 0; i < count; ++i)
        {
            WdgRast* wdg = static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget());
            wdg->setFehlProzentWasser(pWasser);
            wdg->setFehlProzentMalz(pMalz);
        }
        ui->wdgWarnungMaischplanWasser->setVisible(pWasser != 0.0 && ui->layoutRasten->count() > 0);
        ui->wdgWarnungMaischplanMalz->setVisible(pMalz != 0.0 && ui->layoutRasten->count() > 0);
    }
    else
    {
        ui->wdgWarnungMaischplanWasser->setVisible(false);
        ui->wdgWarnungMaischplanMalz->setVisible(false);
    }
    updateRastenDiagram();
}

void TabRezept::updateRastenDiagram()
{
    ui->diagramRasten->update(mSud);
    ui->diagramRasten->setVisible(mSud->modelMaischplan()->rowCount() > 0);
}

void TabRezept::on_btnNeueRast_clicked()
{
    QMap<int, QVariant> values;
    if (mSud->modelMaischplan()->rowCount() == 0)
    {
        values = {{ModelMaischplan::ColSudID, mSud->id()},
                  {ModelMaischplan::ColTyp, static_cast<int>(Brauhelfer::RastTyp::Einmaischen)},
                  {ModelMaischplan::ColName, tr("Einmaischen")}};
    }
    else
    {
        values = {{ModelMaischplan::ColSudID, mSud->id()},
                  {ModelMaischplan::ColTyp, static_cast<int>(Brauhelfer::RastTyp::Aufheizen)},
                  {ModelMaischplan::ColName, tr("Aufheizen")}};
    }
    mSud->modelMaischplan()->append(values);
    ui->scrollAreaRasten->verticalScrollBar()->setValue(ui->scrollAreaRasten->verticalScrollBar()->maximum());
}

void TabRezept::on_btnRastenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Maischplan);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSud->bh()->sudKopierenModel(mSud->bh()->modelMaischplan(),
                             ModelMaischplan::ColSudID, dlg.sudId(),
                             {{ModelMaischplan::ColSudID, mSud->id()}});
        mSud->modelMaischplan()->invalidate();
    }
}

void TabRezept::on_btnMaischplanAusgleichen_clicked()
{
    ProxyModel *model = mSud->modelMaischplan();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelMaischplan::ColAnteilWasser).toDouble();
    if (totalProzent > 0)
    {
        WidgetDecorator::suspendValueChangedClear = true;
        double factor = 100.0 / totalProzent;
        for (int i = 0; i < model->rowCount(); ++i)
            model->setData(i, ModelMaischplan::ColAnteilWasser, model->data(i, ModelMaischplan::ColAnteilWasser).toDouble() * factor);
        WidgetDecorator::suspendValueChangedClear = false;
    }
}

void TabRezept::on_btnMaischplanFaktorAnpassen_clicked()
{
    ProxyModel *model = mSud->modelMaischplan();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelMaischplan::ColAnteilWasser).toDouble();
    if (totalProzent > 0)
    {
        WidgetDecorator::suspendValueChangedClear = true;
        mSud->setFaktorHauptguss(totalProzent / 100.0 * mSud->geterg_WHauptguss() / mSud->geterg_S_Gesamt());
        double factor = 100.0 / totalProzent;
        for (int i = 0; i < model->rowCount(); ++i)
            model->setData(i, ModelMaischplan::ColAnteilWasser, model->data(i, ModelMaischplan::ColAnteilWasser).toDouble() * factor);
        WidgetDecorator::suspendValueChangedClear = false;
    }
}

void TabRezept::on_btnMaischplanAusgleichenMalz_clicked()
{
    ProxyModel *model = mSud->modelMaischplan();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelMaischplan::ColAnteilMalz).toDouble();
    if (totalProzent > 0)
    {
        WidgetDecorator::suspendValueChangedClear = true;
        double factor = 100.0 / totalProzent;
        for (int i = 0; i < model->rowCount(); ++i)
            model->setData(i, ModelMaischplan::ColAnteilMalz, model->data(i, ModelMaischplan::ColAnteilMalz).toDouble() * factor);
        WidgetDecorator::suspendValueChangedClear = false;
    }
}

void TabRezept::updateExtrakt()
{
    double p = 100.0;
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        p -= wdg->prozentExtrakt();
    }
    for (int i = 0; i < ui->layoutZusaetzeMaischen->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeMaischen->itemAt(i)->widget());
        p -= wdg->prozentExtrakt();
    }
    for (int i = 0; i < ui->layoutZusaetzeKochen->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeKochen->itemAt(i)->widget());
        p -= wdg->prozentExtrakt();
    }
    for (int i = 0; i < ui->layoutZusaetzeGaerung->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeGaerung->itemAt(i)->widget());
        p -= wdg->prozentExtrakt();
    }
    if (std::fabs(p) < 0.01)
        p = 0.0;
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        wdg->setFehlProzentExtrakt(p);
    }
    for (int i = 0; i < ui->layoutZusaetzeMaischen->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeMaischen->itemAt(i)->widget());
        wdg->setFehlProzentExtrakt(p);
    }
    for (int i = 0; i < ui->layoutZusaetzeKochen->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeKochen->itemAt(i)->widget());
        wdg->setFehlProzentExtrakt(p);
    }
    for (int i = 0; i < ui->layoutZusaetzeGaerung->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeGaerung->itemAt(i)->widget());
        wdg->setFehlProzentExtrakt(p);
    }
}

void TabRezept::malzGaben_modified()
{
    const int nModel = mSud->modelMalzschuettung()->rowCount();
    int nLayout = ui->layoutMalzGaben->count();
    while (nLayout < nModel)
        ui->layoutMalzGaben->addWidget(new WdgMalzGabe(mSud, nLayout++, ui->layoutMalzGaben, this));
    while (ui->layoutMalzGaben->count() != nModel)
        delete ui->layoutMalzGaben->itemAt(ui->layoutMalzGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget())->updateValues();
    updateMalzGaben();
}

void TabRezept::updateMalzGaben()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        double p = 100.0;
        int count = ui->layoutMalzGaben->count();
        for (int i = 0; i < count; ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (std::fabs(p) < 0.01)
            p = 0.0;
        for (int i = 0; i < count; ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            wdg->setFehlProzent(p);
        }
        ui->wdgWarnungMalz->setVisible(p != 0.0 && count > 1);
        updateExtrakt();
    }
    else
    {
        ui->wdgWarnungMalz->setVisible(false);
    }
    updateMalzDiagram();
}

void TabRezept::updateMalzDiagram()
{
    ui->diagramMalz->update(mSud);
    ui->diagramMalz->setVisible(mSud->modelMalzschuettung()->rowCount() > 0);
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
        if (std::fabs(p) < 0.01)
            p = 0.0;
        QMap<int, QVariant> values({{ModelMalzschuettung::ColSudID, mSud->id()},
                                    {ModelMalzschuettung::ColName, dlg.name()},
                                    {ModelMalzschuettung::ColProzent, p}});
        mSud->modelMalzschuettung()->append(values);
        ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnMalzGabenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Malz);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSud->bh()->sudKopierenModel(mSud->bh()->modelMalzschuettung(),
                             ModelMalzschuettung::ColSudID, dlg.sudId(),
                             {{ModelMalzschuettung::ColSudID, mSud->id()}});
        mSud->modelMalzschuettung()->invalidate();
    }
}

void TabRezept::on_btnNeueZusatzGabeMaischen_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Maischen)}});
        mSud->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnZusazGabenUebernehmenMaischen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::WZutatenMaischen);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = mSud->bh()->modelWeitereZutatenGaben();
        int N = model->rowCount();
        for (int r = 0; r < N; ++r)
        {
            if (model->data(r, ModelWeitereZutatenGaben::ColSudID) == sudId &&
                model->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() == static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Maischen))
            {
                QMap<int, QVariant> values = model->copyValues(r);
                QMap<int, QVariant>::const_iterator it = overrideValues.constBegin();
                while (it != overrideValues.constEnd())
                {
                    values.insert(it.key(), it.value());
                    ++it;
                }
                model->appendDirect(values);
            }
        }
        mSud->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::on_btnMalzAusgleichen_clicked()
{
    ProxyModel *model = mSud->modelMalzschuettung();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelMalzschuettung::ColProzent).toDouble();
    WidgetDecorator::suspendValueChangedClear = true;
    if (totalProzent > 0)
    {
        double factor = 100 / totalProzent;
        for (int i = 0; i < model->rowCount(); ++i)
        {
            double prozent = model->data(i, ModelMalzschuettung::ColProzent).toDouble();
            model->setData(i, ModelMalzschuettung::ColProzent, prozent * factor);
        }
    }
    else
    {
        double prozent = 100.0 / model->rowCount();
        for (int i = 0; i < model->rowCount(); ++i)
            model->setData(i, ModelMalzschuettung::ColProzent, prozent);
    }
    WidgetDecorator::suspendValueChangedClear = false;
}

void TabRezept::on_btnShaAnpassen_clicked()
{
    ProxyModel *model = mSud->modelMalzschuettung();
    double schuettung = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        schuettung += model->data(i, ModelMalzschuettung::Colerg_Menge).toDouble();
    if (schuettung > 0)
    {
        WidgetDecorator::suspendValueChangedClear = true;
        for (int i = 0; i < model->rowCount(); ++i)
        {
            double menge = model->data(i, ModelMalzschuettung::Colerg_Menge).toDouble();
            model->setData(i, ModelMalzschuettung::ColProzent, menge / schuettung * 100);
        }
        double sw = mSud->getSWAnteilMalz();
        double sw_dichte = sw + mSud->getSWAnteilZusatzMaischen() + mSud->getSWAnteilZusatzKochen();
        double menge = mSud->getMengeSollAnstellen();
        mSud->setSudhausausbeute(BierCalc::sudhausausbeute(sw, sw_dichte, menge, schuettung));
        WidgetDecorator::suspendValueChangedClear = false;
    }
}

void TabRezept::hopfenGaben_modified()
{
    const int nModel = mSud->modelHopfengaben()->rowCount();
    int nLayout = ui->layoutHopfenGaben->count();
    while (nLayout < nModel)
        ui->layoutHopfenGaben->addWidget(new WdgHopfenGabe(mSud, nLayout++, ui->layoutHopfenGaben, this));
    while (ui->layoutHopfenGaben->count() != nModel)
        delete ui->layoutHopfenGaben->itemAt(ui->layoutHopfenGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget())->updateValues();
    updateHopfenGaben();
}

void TabRezept::updateHopfenDiagram()
{
    ui->diagramHopfen->update(mSud);
    ui->diagramHopfen->setVisible(mSud->modelHopfengaben()->rowCount() > 0);
}

void TabRezept::updateHopfenGaben()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        double p = 100.0;
        int count = ui->layoutHopfenGaben->count();
        for (int i = 0; i < count; ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (std::fabs(p) < 0.01)
            p = 0.0;
        for (int i = 0; i < count; ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            wdg->setFehlProzent(p);
        }
        ui->wdgWarnungHopfen->setVisible(p != 0.0 && count > 1);
    }
    else
    {
        ui->wdgWarnungHopfen->setVisible(false);
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
        if (std::fabs(p) < 0.01)
            p = 0.0;
        QMap<int, QVariant> values({{ModelHopfengaben::ColSudID, mSud->id()},
                                    {ModelHopfengaben::ColName, dlg.name()},
                                    {ModelHopfengaben::ColZeit, mSud->getKochdauer()},
                                    {ModelHopfengaben::ColProzent, p}});
        mSud->modelHopfengaben()->append(values);
        ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnHopfenGabenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Hopfen);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSud->bh()->sudKopierenModel(mSud->bh()->modelHopfengaben(),
                             ModelHopfengaben::ColSudID, dlg.sudId(),
                             {{ModelHopfengaben::ColSudID, mSud->id()}});
        mSud->modelHopfengaben()->invalidate();
    }
}

void TabRezept::on_btnNeueZusatzGabeKochen_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen)}});
        mSud->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnZusazGabenUebernehmenKochen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::WZutatenKochen);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = mSud->bh()->modelWeitereZutatenGaben();
        int N = model->rowCount();
        for (int r = 0; r < N; ++r)
        {
            if (model->data(r, ModelWeitereZutatenGaben::ColSudID) == sudId &&
                model->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() == static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen))
            {
                QMap<int, QVariant> values = model->copyValues(r);
                QMap<int, QVariant>::const_iterator it = overrideValues.constBegin();
                while (it != overrideValues.constEnd())
                {
                    values.insert(it.key(), it.value());
                    ++it;
                }
                model->appendDirect(values);
            }
        }
        mSud->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::on_cbBerechnungsartHopfen_currentIndexChanged(int index)
{
    if (ui->cbBerechnungsartHopfen->hasFocus())
    {
        mSud->setberechnungsArtHopfen(index - 1);
        updateHopfenGaben();
    }
}

void TabRezept::on_btnHopfenAusgleichen_clicked()
{
    ProxyModel *model = mSud->modelHopfengaben();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelHopfengaben::ColProzent).toDouble();
    WidgetDecorator::suspendValueChangedClear = true;
    if (totalProzent > 0)
    {
        double factor = 100.0 / totalProzent;
        for (int i = 0; i < model->rowCount(); ++i)
        {
            double prozent = model->data(i, ModelHopfengaben::ColProzent).toDouble();
            model->setData(i, ModelHopfengaben::ColProzent, prozent * factor);
        }
    }
    else
    {
        double prozent = 100.0 / model->rowCount();
        for (int i = 0; i < model->rowCount(); ++i)
            model->setData(i, ModelHopfengaben::ColProzent, prozent);
    }
    WidgetDecorator::suspendValueChangedClear = false;
}

void TabRezept::hefeGaben_modified()
{
    const int nModel = mSud->modelHefegaben()->rowCount();
    int nLayout = ui->layoutHefeGaben->count();
    while (nLayout < nModel)
        ui->layoutHefeGaben->addWidget(new WdgHefeGabe(mSud, nLayout++, ui->layoutHefeGaben, this));
    while (ui->layoutHefeGaben->count() != nModel)
        delete ui->layoutHefeGaben->itemAt(ui->layoutHefeGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
        static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget())->updateValues();
}

void TabRezept::on_btnNeueHefeGabe_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hefe, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelHefegaben::ColSudID, mSud->id()},
                                    {ModelHefegaben::ColName, dlg.name()}});
        mSud->modelHefegaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
        vergaerungsgradUebernehmen(dlg.name());
    }
}

void TabRezept::vergaerungsgradUebernehmen(const QString& hefe)
{
    QString str = mSud->bh()->modelHefe()->getValueFromSameRow(ModelHefe::ColName, hefe, ModelHefe::ColEVG).toString();
    static QRegularExpression regxExp(QStringLiteral("[-+]?[0-9]*[.,]?[0-9]+"));
    QRegularExpressionMatchIterator i = regxExp.globalMatch(str);
    double mean = 0;
    int N = 0;
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        double val = match.captured().replace(',', '.').toDouble();
        mean += val;
        N++;
    }
    if (N > 0 && mean > 0)
    {
        mean /= N;
        int evg = qRound(mean);
        if (QMessageBox::question(this, tr("Vergärungsgrad übernehmen?"),
                                  tr("Soll der Vergärungsgrad der Hefe (%1%) übernommen werden?").arg(evg),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColVergaerungsgrad, evg));
    }
}

void TabRezept::on_btnHefeGabenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Hefe);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSud->bh()->sudKopierenModel(mSud->bh()->modelHefegaben(),
                             ModelHefegaben::ColSudID, dlg.sudId(),
                             {{ModelHefegaben::ColSudID, mSud->id()},
                              {ModelHefegaben::ColZugegeben, false}});
        mSud->modelHefegaben()->invalidate();
    }
}

void TabRezept::on_btnNeueHopfenGabeGaerung_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen)},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung)}});
        mSud->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnHopfenGabenUebernehmenGaerung_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::HopfenGaerung);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = mSud->bh()->modelWeitereZutatenGaben();
        int N = model->rowCount();
        for (int r = 0; r < N; ++r)
        {
            if (model->data(r, ModelWeitereZutatenGaben::ColSudID) == sudId &&
                model->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() == static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung) &&
                model->data(r, ModelWeitereZutatenGaben::ColTyp).toInt() == static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen))
            {
                QMap<int, QVariant> values = model->copyValues(r);
                QMap<int, QVariant>::const_iterator it = overrideValues.constBegin();
                while (it != overrideValues.constEnd())
                {
                    values.insert(it.key(), it.value());
                    ++it;
                }
                model->appendDirect(values);
            }
        }
        mSud->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::on_btnNeueZusatzGabeGaerung_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung)}});
        mSud->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnZusazGabenUebernehmenGaerung_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::WZutatenGaerung);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, mSud->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = mSud->bh()->modelWeitereZutatenGaben();
        int N = model->rowCount();
        for (int r = 0; r < N; ++r)
        {
            if (model->data(r, ModelWeitereZutatenGaben::ColSudID) == sudId &&
                model->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() == static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung) &&
                model->data(r, ModelWeitereZutatenGaben::ColTyp).toInt() != static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen))
            {
                QMap<int, QVariant> values = model->copyValues(r);
                QMap<int, QVariant>::const_iterator it = overrideValues.constBegin();
                while (it != overrideValues.constEnd())
                {
                    values.insert(it.key(), it.value());
                    ++it;
                }
                model->appendDirect(values);
            }
        }
        mSud->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::weitereZutatenGaben_modified()
{
    int nLayout;
    int nMaischen = 0;
    int nKochen = 0;
    int nGaerung = 0;
    ProxyModel* model = mSud->modelWeitereZutatenGaben();
    for (int row = 0; row < model->rowCount(); row++)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model->data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
        switch (zeitpunkt)
        {
        case Brauhelfer::ZusatzZeitpunkt::Maischen:
            nMaischen++;
            break;
        case Brauhelfer::ZusatzZeitpunkt::Kochen:
            nKochen++;
            break;
        case Brauhelfer::ZusatzZeitpunkt::Gaerung:
            nGaerung++;
            break;
        }
    }

    nLayout = ui->layoutZusaetzeMaischen->count();
    while (nLayout < nMaischen)
        ui->layoutZusaetzeMaischen->addWidget(new WdgWeitereZutatGabe(mSud, Brauhelfer::ZusatzZeitpunkt::Maischen, nLayout++, ui->layoutZusaetzeMaischen, this));
    while (ui->layoutZusaetzeMaischen->count() != nMaischen)
        delete ui->layoutZusaetzeMaischen->itemAt(ui->layoutZusaetzeMaischen->count() - 1)->widget();
    for (int i = 0; i < ui->layoutZusaetzeMaischen->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeMaischen->itemAt(i)->widget())->updateValues();

    nLayout = ui->layoutZusaetzeKochen->count();
    while (nLayout < nKochen)
        ui->layoutZusaetzeKochen->addWidget(new WdgWeitereZutatGabe(mSud, Brauhelfer::ZusatzZeitpunkt::Kochen, nLayout++, ui->layoutZusaetzeKochen, this));
    while (ui->layoutZusaetzeKochen->count() != nKochen)
        delete ui->layoutZusaetzeKochen->itemAt(ui->layoutZusaetzeKochen->count() - 1)->widget();
    for (int i = 0; i < ui->layoutZusaetzeKochen->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeKochen->itemAt(i)->widget())->updateValues();

    nLayout = ui->layoutZusaetzeGaerung->count();
    while (nLayout < nGaerung)
        ui->layoutZusaetzeGaerung->addWidget(new WdgWeitereZutatGabe(mSud, Brauhelfer::ZusatzZeitpunkt::Gaerung, nLayout++, ui->layoutZusaetzeGaerung, this));
    while (ui->layoutZusaetzeGaerung->count() != nGaerung)
        delete ui->layoutZusaetzeGaerung->itemAt(ui->layoutZusaetzeGaerung->count() - 1)->widget();
    for (int i = 0; i < ui->layoutZusaetzeGaerung->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeGaerung->itemAt(i)->widget())->updateValues();

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        updateExtrakt();
    }
}

void TabRezept::wasseraufbereitung_modified()
{
    const int nModel = mSud->modelWasseraufbereitung()->rowCount();
    int nLayout = ui->layoutWasseraufbereitung->count();
    while (nLayout < nModel)
        ui->layoutWasseraufbereitung->addWidget(new WdgWasseraufbereitung(mSud, nLayout++, ui->layoutWasseraufbereitung, this));
    while (ui->layoutWasseraufbereitung->count() != nModel)
        delete ui->layoutWasseraufbereitung->itemAt(ui->layoutWasseraufbereitung->count() - 1)->widget();
    for (int i = 0; i < ui->layoutWasseraufbereitung->count(); ++i)
        static_cast<WdgWasseraufbereitung*>(ui->layoutWasseraufbereitung->itemAt(i)->widget())->updateValues();
}

void TabRezept::on_btnNeueWasseraufbereitung_clicked()
{
    DlgWasseraufbereitung dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        double ra = mSud->getRestalkalitaetSoll() - mSud->getRestalkalitaetIst();
        QMap<int, QVariant> values({{ModelWasseraufbereitung::ColSudID, mSud->id()},
                                    {ModelWasseraufbereitung::ColName, dlg.name()},
                                    {ModelWasseraufbereitung::ColEinheit, dlg.einheit()},
                                    {ModelWasseraufbereitung::ColFaktor, dlg.faktor()},
                                    {ModelWasseraufbereitung::ColRestalkalitaet, ra}});
        mSud->modelWasseraufbereitung()->append(values);
        ui->scrollAreaWasseraufbereitung->verticalScrollBar()->setValue(ui->scrollAreaWasseraufbereitung->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnWasseraufbereitungUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Wasseraufbereitung);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSud->bh()->sudKopierenModel(mSud->bh()->modelWasseraufbereitung(),
                             ModelWasseraufbereitung::ColSudID, dlg.sudId(),
                             {{ModelWasseraufbereitung::ColSudID, mSud->id()}});
        mSud->modelWasseraufbereitung()->invalidate();
    }
}

void TabRezept::anhaenge_modified()
{
    const int nModel = mSud->modelAnhang()->rowCount();
    int nLayout = ui->layoutAnhang->count();
    while (nLayout < nModel)
        ui->layoutAnhang->addWidget(new WdgAnhang(mSud, nLayout++));
    while (ui->layoutAnhang->count() != nModel)
        delete ui->layoutAnhang->itemAt(ui->layoutAnhang->count() - 1)->widget();
    for (int i = 0; i < ui->layoutAnhang->count(); ++i)
        static_cast<WdgAnhang*>(ui->layoutAnhang->itemAt(i)->widget())->updateValues();
}

void TabRezept::on_btnNeuerAnhang_clicked()
{
    QMap<int, QVariant> values({{ModelAnhang::ColSudID, mSud->id()}});
    int index = mSud->modelAnhang()->append(values);
    static_cast<WdgAnhang*>(ui->layoutAnhang->itemAt(index)->widget())->openDialog();
    ui->scrollAreaAnhang->verticalScrollBar()->setValue(ui->scrollAreaAnhang->verticalScrollBar()->maximum());
}

void TabRezept::on_tbSudname_textChanged(const QString &value)
{
    if (ui->tbSudname->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSudname, value));
}

void TabRezept::on_cbKategorie_currentTextChanged(const QString &value)
{
    if (ui->cbKategorie->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColKategorie, value));
}

void TabRezept::on_btnKategorienVerwalten_clicked()
{
    QList<TableView::ColumnDefinition> colums = {{ModelKategorien::ColName, true, false, 100, new TextDelegate(false, Qt::AlignCenter, this)},
                                                 {ModelKategorien::ColBemerkung, true, false, -1, nullptr}};
    DlgTableView dlg(mSud->bh()->modelKategorien(), colums, {{ModelKategorien::ColName, tr("Kategorie")}}, ModelKategorien::ColName, this);
    dlg.setWindowTitle(tr("Sudkategorien verwalten"));
    dlg.exec();
}

void TabRezept::on_cbAnlage_currentTextChanged(const QString &value)
{
    if (ui->cbAnlage->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColAnlage, value));
}

void TabRezept::on_btnSudhausausbeute_clicked()
{
    WidgetDecorator::suspendValueChangedClear = true;
    QVariant value = mSud->getAnlageData(ModelAusruestung::ColSudhausausbeute);
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColSudhausausbeute, value));
    WidgetDecorator::suspendValueChangedClear = false;
}

void TabRezept::on_btnVerdampfungsrate_clicked()
{
    WidgetDecorator::suspendValueChangedClear = true;
    QVariant value = mSud->getAnlageData(ModelAusruestung::ColVerdampfungsrate);
    gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColVerdampfungsrate, value));
    WidgetDecorator::suspendValueChangedClear = false;
}

void TabRezept::on_cbWasserProfil_currentTextChanged(const QString &value)
{
    if (ui->cbWasserProfil->hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), ModelSud::ColWasserprofil, value));
}

void TabRezept::on_btnWasserProfil_clicked()
{
    DlgWasserprofile dlg(this);
    dlg.select(mSud->getWasserprofil());
    dlg.exec();
}

void TabRezept::on_btnTagNeu_clicked()
{
    QMap<int, QVariant> values({{ModelTags::ColSudID, mSud->id()},
                                {ModelTags::ColKey, tr("Neuer Tag")}});
    ProxyModel *model = mSud->modelTags();
    int row = model->append(values);
    if (row >= 0)
    {
        QModelIndex index = model->index(row, ModelTags::ColKey);
        ui->tableTags->setCurrentIndex(index);
        ui->tableTags->scrollTo(index);
        ui->tableTags->edit(index);
    }
}

void TabRezept::on_btnTagUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Tags);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSud->bh()->sudKopierenModel(mSud->bh()->modelTags(),
                             ModelTags::ColSudID, dlg.sudId(),
                             {{ModelTags::ColSudID, mSud->id()}});
        mSud->modelTags()->invalidate();
    }
}

void TabRezept::on_btnTagLoeschen_clicked()
{
    ProxyModel *model = mSud->modelTags();
    QModelIndexList indices = ui->tableTags->selectionModel()->selectedIndexes();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : std::as_const(indices))
        model->removeRow(index.row());
}
