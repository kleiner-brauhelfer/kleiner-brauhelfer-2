#include "tabrezept.h"
#include "ui_tabrezept.h"
#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QScrollBar>
#include "settings.h"
#include "mainwindow.h"
#include "model/textdelegate.h"
#include "model/checkboxdelegate.h"
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

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

TabRezept::TabRezept(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabRezept)
{
    QPalette pal;

  ui->setupUi(this);
    ui->tbCO2->setColumn(ModelSud::ColCO2);
    ui->tbSW->setColumn(ModelSud::ColSW);
    ui->tbMenge->setColumn(ModelSud::ColMenge);
    ui->tbFaktorHauptguss->setColumn(ModelSud::ColFaktorHauptguss);
    ui->tbGesamtschuettung->setColumn(ModelSud::Colerg_S_Gesamt);
    ui->tbSWMalz->setColumn(ModelSud::ColSWAnteilMalz);
    ui->tbSWWZMaischen->setColumn(ModelSud::ColSWAnteilZusatzMaischen);
    ui->tbSWWZKochen->setColumn(ModelSud::ColSWAnteilZusatzKochen);
    ui->tbSWWZGaerung->setColumn(ModelSud::ColSWAnteilZusatzGaerung);
    ui->tbSWAnteilHefestarter->setColumn(ModelSud::ColSWAnteilHefestarter);
    ui->tbSudnummer->setColumn(ModelSud::ColSudnummer);
    ui->tbBittere->setColumn(ModelSud::ColIBU);
    ui->tbFarbe->setColumn(ModelSud::Colerg_Farbe);
    ui->tbSudhausausbeute->setColumn(ModelSud::ColSudhausausbeute);
    ui->tbVerdampfungsrate->setColumn(ModelSud::ColVerdampfungsrate);
    ui->tbVergaerungsgrad->setColumn(ModelSud::ColVergaerungsgrad);
    ui->tbReifezeit->setColumn(ModelSud::ColReifezeit);
    ui->tbHGF->setColumn(ModelSud::ColhighGravityFaktor);
    ui->tbKochzeit->setColumn(ModelSud::ColKochdauer);
    ui->tbNachisomerisierungszeit->setColumn(ModelSud::ColNachisomerisierungszeit);
    ui->tbKosten->setColumn(ModelSud::Colerg_Preis);
    ui->tbWasserGesamt->setColumn(ModelSud::Colerg_W_Gesamt);
    ui->tbHauptguss->setColumn(ModelSud::Colerg_WHauptguss);
    ui->tbHauptgussEmpfohlen->setColumn(ModelSud::ColWHauptgussEmpfehlung);
    ui->tbNachguss->setColumn(ModelSud::Colerg_WNachguss);
    ui->tbWasserHGF->setColumn(ModelSud::ColMengeSollHgf);
    ui->tbRestextrakt->setColumn(ModelSud::ColSRESoll);
    ui->tbAlkohol->setColumn(ModelSud::ColAlkoholSoll);
    ui->tbRestalkalitaetSoll->setColumn(ModelSud::ColRestalkalitaetSoll);
    ui->tbRestalkalitaetWasser->setColumn(ModelSud::ColRestalkalitaetWasser);
    ui->tbRestalkalitaetIst->setColumn(ModelSud::ColRestalkalitaetIst);
    ui->tbPhMalz->setColumn(ModelSud::ColPhMalz);
    ui->tbPhMaische->setColumn(ModelSud::ColPhMaische);
    ui->tbPhMaischeSoll->setColumn(ModelSud::ColPhMaischeSoll);
    ui->tbMengeHefestarter->setColumn(ModelSud::ColMengeHefestarter);
    ui->tbSWHefestarter->setColumn(ModelSud::ColSWHefestarter);

    ui->lblWarnungMalz->setPalette(gSettings->paletteErrorLabel);
    ui->btnMalzAusgleichen->setError(true);
    ui->lblWarnungHopfen->setPalette(gSettings->paletteErrorLabel);
    ui->btnHopfenAusgleichen->setError(true);
    ui->lblWarnungPh->setPalette(gSettings->paletteErrorLabel);

    pal = ui->btnNeueMalzGabe->palette();
    pal.setColor(QPalette::Button, gSettings->colorMalz);
    ui->btnNeueMalzGabe->setPalette(pal);
    ui->btnMalzGabenUebernehmen->setPalette(pal);

    pal = ui->btnNeueHopfenGabe->palette();
    pal.setColor(QPalette::Button, gSettings->colorHopfen);
    ui->btnNeueHopfenGabe->setPalette(pal);
    ui->btnHopfenGabenUebernehmen->setPalette(pal);
    ui->btnNeueHopfenGabeGaerung->setPalette(pal);
    ui->btnHopfenGabenUebernehmenGaerung->setPalette(pal);

    pal = ui->btnNeueHefeGabe->palette();
    pal.setColor(QPalette::Button, gSettings->colorHefe);
    ui->btnNeueHefeGabe->setPalette(pal);
    ui->btnHefeGabenUebernehmen->setPalette(pal);

    pal = ui->btnNeueZusatzGabeKochen->palette();
    pal.setColor(QPalette::Button, gSettings->colorZusatz);
    ui->btnNeueZusatzGabeMaischen->setPalette(pal);
    ui->btnZusazGabenUebernehmenMaischen->setPalette(pal);
    ui->btnNeueZusatzGabeKochen->setPalette(pal);
    ui->btnZusazGabenUebernehmenKochen->setPalette(pal);
    ui->btnNeueZusatzGabeGaerung->setPalette(pal);
    ui->btnZusazGabenUebernehmenGaerung->setPalette(pal);

    pal = ui->btnNeueRast->palette();
    pal.setColor(QPalette::Button, gSettings->colorRast);
    ui->btnNeueRast->setPalette(pal);
    ui->btnRastenUebernehmen->setPalette(pal);

    pal = ui->btnNeueWasseraufbereitung->palette();
    pal.setColor(QPalette::Button, gSettings->colorWasser);
    ui->btnNeueWasseraufbereitung->setPalette(pal);
    ui->btnWasseraufbereitungUebernehmen->setPalette(pal);

    pal = ui->btnNeuerAnhang->palette();
    pal.setColor(QPalette::Button, gSettings->colorAnhang);
    ui->btnNeuerAnhang->setPalette(pal);

    mGlasSvg = new QGraphicsSvgItem(gSettings->theme() == Settings::Theme::Dark ? ":/images/dark/bier.svg" : ":/images/light/bier.svg");
    ui->lblKostenEinheit->setText(QLocale().currencySymbol() + "/" + tr("L"));

    ProxyModel* proxy = new ProxyModel(this);
    proxy->setSourceModel(bh->modelKategorien());
    ui->cbKategorie->setModel(proxy);
    ui->cbKategorie->setModelColumn(ModelKategorien::ColName);
	
    pal = ui->tbHelp->palette();
    pal.setBrush(QPalette::Base, pal.brush(QPalette::ToolTipBase));
    pal.setBrush(QPalette::Text, pal.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(pal);

    ui->wdgBemerkung->setPlaceholderText(tr("Bemerkung allgemein"));
    ui->wdgBemerkungMaischen->setPlaceholderText(tr("Bemerkung Maischen"));
    ui->wdgBemerkungKochen->setPlaceholderText(tr("Bemerkung Kochen"));
    ui->wdgBemerkungGaerung->setPlaceholderText(tr("Bemerkung Gärung"));
    ui->wdgBemerkungMaischplan->setPlaceholderText(tr("Bemerkung Maischplan"));
    ui->wdgBemerkungWasseraufbereitung->setPlaceholderText(tr("Bemerkung Wasseraufbereitung"));

    gSettings->beginGroup("TabRezept");

    ui->splitter->setSizes({200, 100, 200});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterHelp->setSizes({100, 50});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());

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

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));

    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(sudDataChanged(QModelIndex)));

    connect(bh->sud()->modelRasten(), SIGNAL(layoutChanged()), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(updateRastenDiagram()));

    connect(bh->sud()->modelMalzschuettung(), SIGNAL(layoutChanged()), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(updateMalzGaben()));

    connect(bh->sud()->modelHopfengaben(), SIGNAL(layoutChanged()), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(updateHopfenGaben()));

    connect(bh->sud()->modelHefegaben(), SIGNAL(layoutChanged()), this, SLOT(hefeGaben_modified()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(hefeGaben_modified()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(hefeGaben_modified()));

    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(layoutChanged()), this, SLOT(weitereZutatenGaben_modified()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(weitereZutatenGaben_modified()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(weitereZutatenGaben_modified()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(updateExtrakt()));

    connect(bh->sud()->modelWasseraufbereitung(), SIGNAL(layoutChanged()), this, SLOT(wasseraufbereitung_modified()));
    connect(bh->sud()->modelWasseraufbereitung(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(wasseraufbereitung_modified()));
    connect(bh->sud()->modelWasseraufbereitung(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(wasseraufbereitung_modified()));

    connect(bh->sud()->modelAnhang(), SIGNAL(layoutChanged()), this, SLOT(anhaenge_modified()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(anhaenge_modified()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(anhaenge_modified()));

    connect(ui->wdgBemerkung, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setKommentar(html);});
    connect(ui->wdgBemerkungMaischen, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setBemerkungZutatenMaischen(html);});
    connect(ui->wdgBemerkungKochen, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setBemerkungZutatenKochen(html);});
    connect(ui->wdgBemerkungGaerung, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setBemerkungZutatenGaerung(html);});
    connect(ui->wdgBemerkungMaischplan, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setBemerkungMaischplan(html);});
    connect(ui->wdgBemerkungWasseraufbereitung, &WdgBemerkung::changed, this, [](const QString& html){bh->sud()->setBemerkungWasseraufbereitung(html);});

    connect(ui->btnAnlage, SIGNAL(clicked()), MainWindow::getInstance(), SLOT(showDialogAusruestung()));

    TableView *table = ui->tableTags;
    table->setModel(bh->sud()->modelTags());
    table->appendCol({ModelTags::ColKey, true, false, 0, nullptr});
    table->appendCol({ModelTags::ColValue, true, false, -1, nullptr});
    table->appendCol({ModelTags::ColGlobal, true, false, 0, new CheckBoxDelegate(table)});
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
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
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
        setVisibleModule(Settings::ModuleAusruestung,
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
        setVisibleModule(Settings::ModulePreiskalkulation,
                         {ui->tbKosten,
                          ui->lblKosten,
                          ui->lblKostenEinheit,
                          ui->lineKosten});
    }
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        malzGaben_modified();
        hopfenGaben_modified();
        hefeGaben_modified();
        weitereZutatenGaben_modified();
    }
    checkEnabled();
    updateValues();
}

void TabRezept::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && isAncestorOf(now) && now != ui->tbHelp && !qobject_cast<QSplitter*>(now))
        ui->tbHelp->setHtml(now->toolTip());
}

void TabRezept::sudLoaded()
{
    checkEnabled();
    ui->cbAnlage->setCurrentIndex(-1);
    ui->cbWasserProfil->setCurrentIndex(-1);
    ui->wdgBemerkung->setHtml(bh->sud()->getKommentar());
    ui->wdgBemerkungMaischen->setHtml(bh->sud()->getBemerkungZutatenMaischen());
    ui->wdgBemerkungKochen->setHtml(bh->sud()->getBemerkungZutatenKochen());
    ui->wdgBemerkungGaerung->setHtml(bh->sud()->getBemerkungZutatenGaerung());
    ui->wdgBemerkungMaischplan->setHtml(bh->sud()->getBemerkungMaischplan());
    ui->wdgBemerkungWasseraufbereitung->setHtml(bh->sud()->getBemerkungWasseraufbereitung());
    if (bh->sud()->isLoaded())
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
        ui->wdgBemerkung->setHtml(bh->sud()->getKommentar());
        break;
    case ModelSud::ColBemerkungZutatenMaischen:
        ui->wdgBemerkungMaischen->setHtml(bh->sud()->getBemerkungZutatenMaischen());
        break;
    case ModelSud::ColBemerkungZutatenKochen:
        ui->wdgBemerkungKochen->setHtml(bh->sud()->getBemerkungZutatenKochen());
        break;
    case ModelSud::ColBemerkungZutatenGaerung:
        ui->wdgBemerkungGaerung->setHtml(bh->sud()->getBemerkungZutatenGaerung());
        break;
    case ModelSud::ColBemerkungMaischplan:
        ui->wdgBemerkungMaischplan->setHtml(bh->sud()->getBemerkungMaischplan());
        break;
    case ModelSud::ColBemerkungWasseraufbereitung:
        ui->wdgBemerkungWasseraufbereitung->setHtml(bh->sud()->getBemerkungWasseraufbereitung());
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
    ui->tbVerdampfungsrate->setReadOnly(gebraut);
    ui->tbNachisomerisierungszeit->setReadOnly(gebraut);
    ui->tbVergaerungsgrad->setReadOnly(gebraut);
    ui->tbCO2->setReadOnly(gebraut);
    ui->cbWasserProfil->setEnabled(!gebraut);
    ui->tbRestalkalitaetSoll->setReadOnly(gebraut);
    ui->tbPhMaischeSoll->setReadOnly(gebraut);
    ui->btnWasseraufbereitungUebernehmen->setVisible(!gebraut);
    ui->tbReifezeit->setReadOnly(gebraut);
    ui->btnNeueRast->setVisible(!gebraut);
    ui->btnRastenUebernehmen->setVisible(!gebraut);
    ui->lineNeueRast->setVisible(!gebraut);
    ui->wdgAddMaischen->setVisible(!gebraut);
    ui->wdgAddKochen->setVisible(!gebraut);
    ui->cbBerechnungsartHopfen->setEnabled(!gebraut);
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
                bh->modelMalzschuettung()->import(bh->sud()->modelMalzschuettung()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                bh->sud()->unload();
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
                bh->modelWeitereZutatenGaben()->import(bh->sud()->modelWeitereZutatenGaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                bh->sud()->unload();
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
                bh->modelHopfengaben()->import(bh->sud()->modelHopfengaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                bh->sud()->unload();
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
                bh->modelWeitereZutatenGaben()->import(bh->sud()->modelWeitereZutatenGaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                bh->sud()->unload();
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
                bh->modelHefegaben()->import(bh->sud()->modelHefegaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                bh->sud()->unload();
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
                bh->modelWeitereZutatenGaben()->import(bh->sud()->modelWeitereZutatenGaben()->mapRowToSource(wdg->row()));
                wdg->updateValues();
            }
            else if (ret == QMessageBox::NoAll)
            {
                return;
            }
            else if (ret == QMessageBox::Cancel)
            {
                bh->sud()->unload();
                return;
            }
        }
    }
}

void TabRezept::updateValues()
{
    double fVal, diff;
    if (!isTabActive())
        return;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    bool enabled = status == Brauhelfer::SudStatus::Rezept || gSettings->ForceEnabled;
    for (auto& lbl : findChildren<LabelGrV*>())
        lbl->setText(gSettings->GravityUnit());
    for (auto& wdg : findChildren<DoubleSpinBoxGrV*>())
        wdg->updateValue();
    for (auto& wdg : findChildren<DoubleSpinBoxSud*>())
        wdg->updateValue();
    for (auto& wdg : findChildren<SpinBoxSud*>())
        wdg->updateValue();

    if (!ui->tbSudname->hasFocus())
    {
        ui->tbSudname->setText(bh->sud()->getSudname());
        ui->tbSudname->setCursorPosition(0);
    }
    if (!ui->cbKategorie->hasFocus())
        ui->cbKategorie->setCurrentIndex(ui->cbKategorie->findText(bh->sud()->getKategorie()));

    // ModuleAusruestung
    if (!ui->cbAnlage->hasFocus())
        ui->cbAnlage->setCurrentIndex(ui->cbAnlage->findText(bh->sud()->getAnlage()));
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
        diff = bh->sud()->getSudhausausbeute() - bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble();
        ui->btnSudhausausbeute->setVisible(enabled && qAbs(diff) > 0.05);
        diff = bh->sud()->getVerdampfungsrate() - bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsrate).toDouble();
        ui->btnVerdampfungsrate->setVisible(enabled && qAbs(diff) > 0.05);
        ui->lblAnlageName->setText(bh->sud()->getAnlage());
        ui->tbAnlageKorrekturSollmenge->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColKorrekturMenge).toDouble());
        ui->wdgAnlageKorrekturSollmenge->setVisible(ui->tbAnlageKorrekturSollmenge->value() > 0);
        ui->tbAnlageSudhausausbeute->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
        ui->tbAnlageVerdampfung->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsrate).toDouble());
        ui->tbAnlageVolumenMaische->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColMaischebottich_MaxFuellvolumen).toDouble());
        ui->tbVolumenMaische->setValue(bh->sud()->geterg_WHauptguss() + BierCalc::MalzVerdraengung * bh->sud()->geterg_S_Gesamt());
        ui->tbVolumenMaische->setError(ui->tbVolumenMaische->value() > ui->tbAnlageVolumenMaische->value());
        ui->tbAnlageVolumenKochen->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_MaxFuellvolumen).toDouble());
        ui->tbVolumenKochen->setValue(BierCalc::volumenWasser(20.0, 100.0, bh->sud()->getMengeSollKochbeginn()));
        ui->tbVolumenKochen->setError(ui->tbVolumenKochen->value() > ui->tbAnlageVolumenKochen->value());
    }

    // ModuleWasseraufbereitung
    if (!ui->cbWasserProfil->hasFocus())
        ui->cbWasserProfil->setCurrentIndex(ui->cbWasserProfil->findText(bh->sud()->getWasserprofil()));
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
        double minVal;
    if (gSettings->GravityName() == "SG") {
        minVal = 1.000;
    } else {
        minVal = 0.0;
    }
    ui->wdgSWMalz->setVisible(ui->tbSWMalz->value() > minVal);
    ui->wdgSWWZMaischen->setVisible(ui->tbSWWZMaischen->value() > minVal);
    ui->wdgSWWZKochen->setVisible(ui->tbSWWZKochen->value() > minVal);
    ui->wdgSWWZGaerung->setVisible(ui->tbSWWZGaerung->value() > minVal);
    ui->wdgSWAnteilHefestarter->setVisible(ui->tbSWAnteilHefestarter->value() > minVal);
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

    Brauhelfer::AnlageTyp anlageTyp = static_cast<Brauhelfer::AnlageTyp>(bh->sud()->getAnlageData(ModelAusruestung::ColTyp).toInt());
    ui->tbFaktorHauptgussEmpfehlung->setValue(BierCalc::hauptgussFaktor(bh->sud()->geterg_Farbe()));
    ui->wdgFaktorHauptguss->setVisible(anlageTyp != Brauhelfer::AnlageTyp::GrainfatherG30 && anlageTyp != Brauhelfer::AnlageTyp::BrauheldPro30);
    diff = bh->sud()->geterg_WHauptguss() - bh->sud()->getWHauptgussEmpfehlung();
    ui->wdgHauptgussEmpfohlen->setVisible(qAbs(diff) > 0.005);
    ui->wdgWasserHGF->setVisible(bh->sud()->gethighGravityFaktor() != 0.0);

    ui->tbKochzeit->setError(ui->tbKochzeit->value() == 0.0);
    Brauhelfer::BerechnungsartHopfen berechnungsArtHopfen = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->sud()->getberechnungsArtHopfen());
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
        ui->layoutRasten->addWidget(new WdgRast(nLayout++, ui->layoutRasten, this));
    while (ui->layoutRasten->count() != nModel)
        delete ui->layoutRasten->itemAt(ui->layoutRasten->count() - 1)->widget();
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
        static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget())->updateValues();
    updateRastenDiagram();
}

void TabRezept::updateRastenDiagram()
{
    ui->diagramRasten->update();
}

void TabRezept::on_btnNeueRast_clicked()
{
    QMap<int, QVariant> values;
    if (bh->sud()->modelRasten()->rowCount() == 0)
    {
        values = {{ModelRasten::ColSudID, bh->sud()->id()},
                  {ModelRasten::ColTyp, static_cast<int>(Brauhelfer::RastTyp::Einmaischen)}};
    }
    else
    {
        values = {{ModelRasten::ColSudID, bh->sud()->id()},
                  {ModelRasten::ColTyp, static_cast<int>(Brauhelfer::RastTyp::Temperatur)}};
    }
    bh->sud()->modelRasten()->append(values);
    ui->scrollAreaRasten->verticalScrollBar()->setValue(ui->scrollAreaRasten->verticalScrollBar()->maximum());
}

void TabRezept::on_btnRastenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Maischplan);
    if (dlg.exec() == QDialog::Accepted)
    {
        bh->sudKopierenModel(bh->modelRasten(),
                             ModelRasten::ColSudID, dlg.sudId(),
                             {{ModelRasten::ColSudID, bh->sud()->id()}});
        bh->sud()->modelRasten()->invalidate();
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
    const int nModel = bh->sud()->modelMalzschuettung()->rowCount();
    int nLayout = ui->layoutMalzGaben->count();
    while (nLayout < nModel)
        ui->layoutMalzGaben->addWidget(new WdgMalzGabe(nLayout++, ui->layoutMalzGaben, this));
    while (ui->layoutMalzGaben->count() != nModel)
        delete ui->layoutMalzGaben->itemAt(ui->layoutMalzGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget())->updateValues();
    updateMalzGaben();
}

void TabRezept::updateMalzGaben()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
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
    ui->diagramMalz->update();
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
        QMap<int, QVariant> values({{ModelMalzschuettung::ColSudID, bh->sud()->id()},
                                    {ModelMalzschuettung::ColName, dlg.name()},
                                    {ModelMalzschuettung::ColProzent, p}});
        bh->sud()->modelMalzschuettung()->append(values);
        ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnMalzGabenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Malz);
    if (dlg.exec() == QDialog::Accepted)
    {
        bh->sudKopierenModel(bh->modelMalzschuettung(),
                             ModelMalzschuettung::ColSudID, dlg.sudId(),
                             {{ModelMalzschuettung::ColSudID, bh->sud()->id()}});
        bh->sud()->modelMalzschuettung()->invalidate();
    }
}

void TabRezept::on_btnNeueZusatzGabeMaischen_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Maischen)}});
        bh->sud()->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnZusazGabenUebernehmenMaischen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::WZutatenMaischen);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = bh->modelWeitereZutatenGaben();
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
        bh->sud()->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::on_btnMalzAusgleichen_clicked()
{
    ProxyModel *model = bh->sud()->modelMalzschuettung();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelMalzschuettung::ColProzent).toDouble();
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
}

void TabRezept::hopfenGaben_modified()
{
    const int nModel = bh->sud()->modelHopfengaben()->rowCount();
    int nLayout = ui->layoutHopfenGaben->count();
    while (nLayout < nModel)
        ui->layoutHopfenGaben->addWidget(new WdgHopfenGabe(nLayout++, ui->layoutHopfenGaben, this));
    while (ui->layoutHopfenGaben->count() != nModel)
        delete ui->layoutHopfenGaben->itemAt(ui->layoutHopfenGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget())->updateValues();
    updateHopfenGaben();
}

void TabRezept::updateHopfenDiagram()
{
    ui->diagramHopfen->update();
}

void TabRezept::updateHopfenGaben()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
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
        QMap<int, QVariant> values({{ModelHopfengaben::ColSudID, bh->sud()->id()},
                                    {ModelHopfengaben::ColName, dlg.name()},
                                    {ModelHopfengaben::ColZeit, bh->sud()->getKochdauer()},
                                    {ModelHopfengaben::ColProzent, p}});
        bh->sud()->modelHopfengaben()->append(values);
        ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnHopfenGabenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Hopfen);
    if (dlg.exec() == QDialog::Accepted)
    {
        bh->sudKopierenModel(bh->modelHopfengaben(),
                             ModelHopfengaben::ColSudID, dlg.sudId(),
                             {{ModelHopfengaben::ColSudID, bh->sud()->id()}});
        bh->sud()->modelHopfengaben()->invalidate();
    }
}

void TabRezept::on_btnNeueZusatzGabeKochen_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen)}});
        bh->sud()->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnZusazGabenUebernehmenKochen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::WZutatenKochen);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = bh->modelWeitereZutatenGaben();
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
        bh->sud()->modelWeitereZutatenGaben()->invalidate();
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

void TabRezept::on_btnHopfenAusgleichen_clicked()
{
    ProxyModel *model = bh->sud()->modelHopfengaben();
    double totalProzent = 0;
    for (int i = 0; i < model->rowCount(); ++i)
        totalProzent += model->data(i, ModelHopfengaben::ColProzent).toDouble();
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
}

void TabRezept::hefeGaben_modified()
{
    const int nModel = bh->sud()->modelHefegaben()->rowCount();
    int nLayout = ui->layoutHefeGaben->count();
    while (nLayout < nModel)
        ui->layoutHefeGaben->addWidget(new WdgHefeGabe(nLayout++, ui->layoutHefeGaben, this));
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
        QMap<int, QVariant> values({{ModelHefegaben::ColSudID, bh->sud()->id()},
                                    {ModelHefegaben::ColName, dlg.name()}});
        bh->sud()->modelHefegaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
        vergaerungsgradUebernehmen(dlg.name());
    }
}

void TabRezept::vergaerungsgradUebernehmen(const QString& hefe)
{
    QString str = bh->modelHefe()->getValueFromSameRow(ModelHefe::ColName, hefe, ModelHefe::ColEVG).toString();
    QRegularExpression regxExp("[-+]?[0-9]*[.,]?[0-9]+");
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
            bh->sud()->setVergaerungsgrad(evg);
    }
}

void TabRezept::on_btnHefeGabenUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Hefe);
    if (dlg.exec() == QDialog::Accepted)
    {
        bh->sudKopierenModel(bh->modelHefegaben(),
                             ModelHefegaben::ColSudID, dlg.sudId(),
                             {{ModelHefegaben::ColSudID, bh->sud()->id()},
                              {ModelHefegaben::ColZugegeben, false}});
        bh->sud()->modelHefegaben()->invalidate();
    }
}

void TabRezept::on_btnNeueHopfenGabeGaerung_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen)},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung)}});
        bh->sud()->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnHopfenGabenUebernehmenGaerung_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::HopfenGaerung);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = bh->modelWeitereZutatenGaben();
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
        bh->sud()->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::on_btnNeueZusatzGabeGaerung_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                    {ModelWeitereZutatenGaben::ColName, dlg.name()},
                                    {ModelWeitereZutatenGaben::ColZeitpunkt, static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung)}});
        bh->sud()->modelWeitereZutatenGaben()->append(values);
        ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnZusazGabenUebernehmenGaerung_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::WZutatenGaerung);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudId = dlg.sudId();
        QMap<int, QVariant> overrideValues = {{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()},
                                              {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}};
        SqlTableModel* model = bh->modelWeitereZutatenGaben();
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
        bh->sud()->modelWeitereZutatenGaben()->invalidate();
    }
}

void TabRezept::weitereZutatenGaben_modified()
{
    int nLayout;
    int nMaischen = 0;
    int nKochen = 0;
    int nGaerung = 0;
    ProxyModel* model = bh->sud()->modelWeitereZutatenGaben();
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
        ui->layoutZusaetzeMaischen->addWidget(new WdgWeitereZutatGabe(Brauhelfer::ZusatzZeitpunkt::Maischen, nLayout++, ui->layoutZusaetzeMaischen, this));
    while (ui->layoutZusaetzeMaischen->count() != nMaischen)
        delete ui->layoutZusaetzeMaischen->itemAt(ui->layoutZusaetzeMaischen->count() - 1)->widget();
    for (int i = 0; i < ui->layoutZusaetzeMaischen->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeMaischen->itemAt(i)->widget())->updateValues();

    nLayout = ui->layoutZusaetzeKochen->count();
    while (nLayout < nKochen)
        ui->layoutZusaetzeKochen->addWidget(new WdgWeitereZutatGabe(Brauhelfer::ZusatzZeitpunkt::Kochen, nLayout++, ui->layoutZusaetzeKochen, this));
    while (ui->layoutZusaetzeKochen->count() != nKochen)
        delete ui->layoutZusaetzeKochen->itemAt(ui->layoutZusaetzeKochen->count() - 1)->widget();
    for (int i = 0; i < ui->layoutZusaetzeKochen->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeKochen->itemAt(i)->widget())->updateValues();

    nLayout = ui->layoutZusaetzeGaerung->count();
    while (nLayout < nGaerung)
        ui->layoutZusaetzeGaerung->addWidget(new WdgWeitereZutatGabe(Brauhelfer::ZusatzZeitpunkt::Gaerung, nLayout++, ui->layoutZusaetzeGaerung, this));
    while (ui->layoutZusaetzeGaerung->count() != nGaerung)
        delete ui->layoutZusaetzeGaerung->itemAt(ui->layoutZusaetzeGaerung->count() - 1)->widget();
    for (int i = 0; i < ui->layoutZusaetzeGaerung->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutZusaetzeGaerung->itemAt(i)->widget())->updateValues();

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
    {
        updateExtrakt();
    }
}

void TabRezept::wasseraufbereitung_modified()
{
    const int nModel = bh->sud()->modelWasseraufbereitung()->rowCount();
    int nLayout = ui->layoutWasseraufbereitung->count();
    while (nLayout < nModel)
        ui->layoutWasseraufbereitung->addWidget(new WdgWasseraufbereitung(nLayout++, ui->layoutWasseraufbereitung, this));
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
        double ra = bh->sud()->getRestalkalitaetSoll() - bh->sud()->getRestalkalitaetIst();
        QMap<int, QVariant> values({{ModelWasseraufbereitung::ColSudID, bh->sud()->id()},
                                    {ModelWasseraufbereitung::ColName, dlg.name()},
                                    {ModelWasseraufbereitung::ColEinheit, dlg.einheit()},
                                    {ModelWasseraufbereitung::ColFaktor, dlg.faktor()},
                                    {ModelWasseraufbereitung::ColRestalkalitaet, ra}});
        bh->sud()->modelWasseraufbereitung()->append(values);
        ui->scrollAreaWasseraufbereitung->verticalScrollBar()->setValue(ui->scrollAreaWasseraufbereitung->verticalScrollBar()->maximum());
    }
}

void TabRezept::on_btnWasseraufbereitungUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Wasseraufbereitung);
    if (dlg.exec() == QDialog::Accepted)
    {
        bh->sudKopierenModel(bh->modelWasseraufbereitung(),
                             ModelWasseraufbereitung::ColSudID, dlg.sudId(),
                             {{ModelWasseraufbereitung::ColSudID, bh->sud()->id()}});
        bh->sud()->modelWasseraufbereitung()->invalidate();
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

void TabRezept::on_cbKategorie_currentTextChanged(const QString &value)
{
    if (ui->cbKategorie->hasFocus())
        bh->sud()->setKategorie(value);
}

void TabRezept::on_btnKategorienVerwalten_clicked()
{
    QList<TableView::ColumnDefinition> colums = {{ModelKategorien::ColName, true, false, 100, new TextDelegate(false, Qt::AlignCenter, this)},
                                                 {ModelKategorien::ColBemerkung, true, false, -1, nullptr}};
    DlgTableView dlg(bh->modelKategorien(), colums, {{ModelKategorien::ColName, tr("Kategorie")}}, ModelKategorien::ColName, this);
    dlg.setWindowTitle(tr("Sudkategorien verwalten"));
    dlg.exec();
}

void TabRezept::on_cbAnlage_currentTextChanged(const QString &value)
{
    if (ui->cbAnlage->hasFocus())
        bh->sud()->setAnlage(value);
}

void TabRezept::on_btnSudhausausbeute_clicked()
{
    bh->sud()->setSudhausausbeute(bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
}

void TabRezept::on_btnVerdampfungsrate_clicked()
{
    bh->sud()->setVerdampfungsrate(bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsrate).toDouble());
}

void TabRezept::on_cbWasserProfil_currentTextChanged(const QString &value)
{
    if (ui->cbWasserProfil->hasFocus())
        bh->sud()->setWasserprofil(value);
}

void TabRezept::on_btnWasserProfil_clicked()
{
    DlgWasserprofile dlg(this);
    dlg.select(bh->sud()->getWasserprofil());
    dlg.exec();
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

void TabRezept::on_btnTagUebernehmen_clicked()
{
    DlgUebernahmeRezept dlg(DlgUebernahmeRezept::Tags);
    if (dlg.exec() == QDialog::Accepted)
    {
        bh->sudKopierenModel(bh->modelTags(),
                             ModelTags::ColSudID, dlg.sudId(),
                             {{ModelTags::ColSudID, bh->sud()->id()}});
        bh->sud()->modelTags()->invalidate();
    }
}

void TabRezept::on_btnTagLoeschen_clicked()
{
    ProxyModel *model = bh->sud()->modelTags();
    QModelIndexList indices = ui->tableTags->selectionModel()->selectedIndexes();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
        model->removeRow(index.row());
}
