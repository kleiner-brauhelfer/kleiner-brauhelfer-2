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
#include "dialogs/dlgrohstoffaustausch.h"
#include "dialogs/dlgeinmaischtemp.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabRezept::TabRezept(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabRezept)
{
    ui->setupUi(this);
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

    int col;
    ProxyModel *model = bh->sud()->modelFlaschenlabelTags();
    QTableView *table = ui->tableTags;
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = ModelFlaschenlabelTags::ColTagname;
    model->setHeaderData(col, Qt::Horizontal, tr("Tag"));
    table->setColumnHidden(col, false);
    col = ModelFlaschenlabelTags::ColValue;
    model->setHeaderData(col, Qt::Horizontal, tr("Wert"));
    table->setColumnHidden(col, false);
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    col = ModelFlaschenlabelTags::ColGlobal;
    model->setHeaderData(col, Qt::Horizontal, tr("Global"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new CheckBoxDelegate(table));
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

void TabRezept::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
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

void TabRezept::checkEnabled()
{
    bool gebraut = bh->sud()->getStatus() != Sud_Status_Rezept && !gSettings->ForceEnabled;
    bool abgefuellt = bh->sud()->getStatus() > Sud_Status_Gebraut  && !gSettings->ForceEnabled;
    ui->cbAnlage->setEnabled(!gebraut);
    ui->tbMenge->setReadOnly(gebraut);
    ui->tbSW->setReadOnly(gebraut);
    ui->tbFaktorHauptguss->setReadOnly(gebraut);
    ui->tbHGF->setReadOnly(gebraut);
    ui->tbBittere->setReadOnly(gebraut);
    ui->tbKochzeit->setReadOnly(gebraut);
    ui->tbNachisomerisierungszeit->setReadOnly(gebraut);
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
    QWidget *currentWdg = ui->tabZutaten->currentWidget();

    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data(ModelMalzschuettung::ColName).toString();
        if (name.isEmpty())
        {
            int ret = QMessageBox::question(this, tr("Malzgabe löschen?"),
                                            tr("Es wurde eine ungültige Malzgabe gefunden. Soll diese gelöscht werden?"));
            if (ret == QMessageBox::Yes)
            {
                wdg->remove();
                --i;
                continue;
            }
        }
        int row = bh->modelMalz()->getRowWithValue(ModelMalz::ColBeschreibung, name);
        if (row < 0)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabMalz);
            DlgRohstoffAustausch dlg(DlgRohstoffAustausch::NichtVorhanden, QString(), this);
            dlg.setSud(bh->sud()->getSudname());
            dlg.setModel(bh->modelMalz(), ModelMalz::ColBeschreibung);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    QMap<int, QVariant> values({{ModelMalz::ColBeschreibung, name},
                                                {ModelMalz::ColFarbe, wdg->data(ModelMalzschuettung::ColFarbe)}});
                    bh->modelMalz()->append(values);
                    dlg.setModel(bh->modelMalz(), ModelMalz::ColBeschreibung);
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData(ModelMalzschuettung::ColName, dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data(ModelHopfengaben::ColName).toString();
        if (name.isEmpty())
        {
            int ret = QMessageBox::question(this, tr("Hopfengabe löschen?"),
                                            tr("Es wurde eine ungültige Hopfengabe gefunden. Soll diese gelöscht werden?"));
            if (ret == QMessageBox::Yes)
            {
                wdg->remove();
                --i;
                continue;
            }
        }
        int row = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColBeschreibung, name);
        if (row < 0)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabHopfen);
            DlgRohstoffAustausch dlg(DlgRohstoffAustausch::NichtVorhanden, QString(), this);
            dlg.setSud(bh->sud()->getSudname());
            dlg.setModel(bh->modelHopfen(), ModelHopfen::ColBeschreibung);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    QMap<int, QVariant> values({{ModelHopfen::ColBeschreibung, name},
                                                {ModelHopfen::ColAlpha, wdg->data(ModelHopfengaben::ColAlpha)},
                                                {ModelHopfen::ColPellets, wdg->data(ModelHopfengaben::ColPellets)}});
                    bh->modelHopfen()->append(values);
                    dlg.setModel(bh->modelHopfen(), ModelHopfen::ColBeschreibung);
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData(ModelHopfengaben::ColName, dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
    {
        WdgHefeGabe* wdg = static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data(ModelHefegaben::ColName).toString();
        if (name.isEmpty())
        {
            int ret = QMessageBox::question(this, tr("Hefegabe löschen?"),
                                            tr("Es wurde eine ungültige Hefegabe gefunden. Soll diese gelöscht werden?"));
            if (ret == QMessageBox::Yes)
            {
                wdg->remove();
                --i;
                continue;
            }
        }
        int row = bh->modelHefe()->getRowWithValue(ModelHefe::ColBeschreibung, name);
        if (row < 0)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabHefe);
            DlgRohstoffAustausch dlg(DlgRohstoffAustausch::NichtVorhanden, QString(), this);
            dlg.setSud(bh->sud()->getSudname());
            dlg.setModel(bh->modelHefe(), ModelHefe::ColBeschreibung);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    QMap<int, QVariant> values({{ModelHefe::ColBeschreibung, name}});
                    bh->modelHefe()->append(values);
                    dlg.setModel(bh->modelHefe(), ModelHefe::ColBeschreibung);
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData(ModelHefegaben::ColName, dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    for (int i = 0; i < ui->layoutWeitereZutatenGaben->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutWeitereZutatenGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data(ModelWeitereZutatenGaben::ColName).toString();
        if (name.isEmpty())
        {
            int ret = QMessageBox::question(this, tr("Weitere Zutat löschen?"),
                                            tr("Es wurde eine ungültige weitere Zutat gefunden. Soll diese gelöscht werden?"));
            if (ret == QMessageBox::Yes)
            {
                wdg->remove();
                --i;
                continue;
            }
        }
        int typ = wdg->data(ModelWeitereZutatenGaben::ColTyp).toInt();
        int row;
        if (typ == EWZ_Typ_Hopfen)
            row = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColBeschreibung, name);
        else
            row = bh->modelWeitereZutaten()->getRowWithValue(ModelWeitereZutaten::ColBeschreibung, name);
        if (row < 0)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabWeitereZutaten);
            DlgRohstoffAustausch dlg(DlgRohstoffAustausch::NichtVorhanden, QString(), this);
            dlg.setSud(bh->sud()->getSudname());
            if (typ == EWZ_Typ_Hopfen)
                dlg.setModel(bh->modelHopfen(), ModelHopfen::ColBeschreibung);
            else
                dlg.setModel(bh->modelWeitereZutaten(), ModelWeitereZutaten::ColBeschreibung);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    if (typ == EWZ_Typ_Hopfen)
                    {
                        QMap<int, QVariant> values({{ModelHopfen::ColBeschreibung, name}});
                        bh->modelHopfen()->append(values);
                    }
                    else
                    {
                        QMap<int, QVariant> values({{ModelWeitereZutaten::ColBeschreibung, name},
                                                    {ModelWeitereZutaten::ColEinheiten, wdg->data(ModelWeitereZutatenGaben::ColEinheit)},
                                                    {ModelWeitereZutaten::ColTyp, wdg->data(ModelWeitereZutatenGaben::ColTyp)},
                                                    {ModelWeitereZutaten::ColAusbeute, wdg->data(ModelWeitereZutatenGaben::ColAusbeute)},
                                                    {ModelWeitereZutaten::ColEBC, wdg->data(ModelWeitereZutatenGaben::ColFarbe)}});
                        bh->modelWeitereZutaten()->append(values);
                    }
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData(ModelWeitereZutatenGaben::ColName, dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    ui->tabZutaten->setCurrentWidget(currentWdg);
}

void TabRezept::updateValues()
{
    double restalkalitaetFaktor;
    if (!ui->tbSudnummer->hasFocus())
        ui->tbSudnummer->setValue(bh->sud()->getSudnummer());
    if (!ui->tbSudname->hasFocus())
    {
        ui->tbSudname->setText(bh->sud()->getSudname());
        ui->tbSudname->setCursorPosition(0);
    }
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(bh->sud()->getMenge());
    if (!ui->tbSW->hasFocus())
        ui->tbSW->setValue(bh->sud()->getSW());
    if (!ui->tbCO2->hasFocus())
        ui->tbCO2->setValue(bh->sud()->getCO2());
    if (!ui->tbBittere->hasFocus())
        ui->tbBittere->setValue(bh->sud()->getIBU());
    ui->tbFarbe->setValue((int)bh->sud()->geterg_Farbe());
    ui->tbGesamtschuettung->setValue(bh->sud()->geterg_S_Gesamt());
    ui->tbSWMalz->setValue(bh->sud()->getSW_Malz());
    ui->wdgSWMalz->setVisible(ui->tbSWMalz->value() > 0.0);
    ui->tbSWWZMaischen->setValue(bh->sud()->getSW_WZ_Maischen());
    ui->wdgSWWZMaischen->setVisible(ui->tbSWWZMaischen->value() > 0.0);
    ui->tbSWWZKochen->setValue(bh->sud()->getSW_WZ_Kochen());
    ui->wdgSWWZKochen->setVisible(ui->tbSWWZKochen->value() > 0.0);
    ui->tbSWWZGaerung->setValue(bh->sud()->getSW_WZ_Gaerung());
    ui->wdgSWWZGaerung->setVisible(ui->tbSWWZGaerung->value() > 0.0);
    ui->tbKosten->setValue(bh->sud()->geterg_Preis());
    if (!ui->tbReifezeit->hasFocus())
        ui->tbReifezeit->setValue(bh->sud()->getReifezeit());
    if (!ui->cbAnlage->hasFocus())
        ui->cbAnlage->setCurrentText(bh->sud()->getAnlage());
    ui->cbAnlage->setError(ui->cbAnlage->currentIndex() == -1);
    if (!ui->tbHGF->hasFocus())
        ui->tbHGF->setValue(bh->sud()->gethighGravityFaktor());
    if (!ui->tbFaktorHauptguss->hasFocus())
        ui->tbFaktorHauptguss->setValue(bh->sud()->getFaktorHauptguss());
    ui->tbFaktorHauptgussEmpfehlung->setValue(bh->sud()->getFaktorHauptgussEmpfehlung());
    if (!ui->tbRestalkalitaet->hasFocus())
        ui->tbRestalkalitaet->setValue(bh->sud()->getRestalkalitaetSoll());
    if (!ui->cbWasserProfil->hasFocus())
        ui->cbWasserProfil->setCurrentText(bh->sud()->getWasserprofil());
    ui->cbWasserProfil->setError(ui->cbWasserProfil->currentIndex() == -1);
    ui->tbRestalkalitaetWasser->setValue(bh->sud()->getWasserData(ModelWasser::ColRestalkalitaet).toDouble());
    ui->tbRestalkalitaet->setMaximum(ui->tbRestalkalitaetWasser->value());
    ui->lblWasserprofil->setText(bh->sud()->getWasserprofil());
    restalkalitaetFaktor = bh->sud()->getRestalkalitaetFaktor();
    ui->tbHauptguss->setValue(bh->sud()->geterg_WHauptguss());
    ui->tbMilchsaeureHG->setValue(ui->tbHauptguss->value() * restalkalitaetFaktor);
    ui->tbNachguss->setValue(bh->sud()->geterg_WNachguss());
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
    ui->tbAnlageSudhausausbeute->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudhausausbeute).toDouble());
    ui->tbAnlageVerdampfung->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColVerdampfungsziffer).toDouble());
    ui->tbAnlageVolumenMaische->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColMaischebottich_MaxFuellvolumen).toDouble());
    ui->tbVolumenMaische->setValue(bh->sud()->geterg_WHauptguss() + BierCalc::MalzVerdraengung * bh->sud()->geterg_S_Gesamt());
    ui->tbVolumenMaische->setError(ui->tbVolumenMaische->value() > ui->tbAnlageVolumenMaische->value());
    ui->tbAnlageVolumenKochen->setValue(bh->sud()->getAnlageData(ModelAusruestung::ColSudpfanne_MaxFuellvolumen).toDouble());
    ui->tbVolumenKochen->setValue(BierCalc::volumenWasser(20.0, 100.0, bh->sud()->getMengeSollKochbeginn()));
    ui->tbVolumenKochen->setError(ui->tbVolumenKochen->value() > ui->tbAnlageVolumenKochen->value());
    if (!ui->tbEinmaischtemperatur->hasFocus())
        ui->tbEinmaischtemperatur->setValue(bh->sud()->getEinmaischenTemp());
    if (!ui->tbKochzeit->hasFocus())
        ui->tbKochzeit->setValue(bh->sud()->getKochdauerNachBitterhopfung());
    ui->tbKochzeit->setError(ui->tbKochzeit->value() == 0.0);
    if (!ui->tbNachisomerisierungszeit->hasFocus())
        ui->tbNachisomerisierungszeit->setValue(bh->sud()->getNachisomerisierungszeit());
    if (!ui->cbBerechnungsartHopfen->hasFocus())
        ui->cbBerechnungsartHopfen->setCurrentIndex(bh->sud()->getberechnungsArtHopfen());
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
    if (bh->sud()->getStatus() == Sud_Status_Rezept)
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
    if (bh->sud()->getStatus() == Sud_Status_Rezept)
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
    if (bh->sud()->getStatus() == Sud_Status_Rezept)
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
    double p = 100.0;
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        p -= wdg->prozent();
    }
    if (fabs(p) < 0.1)
        p = 0.0;
    QMap<int, QVariant> values({{ModelMalzschuettung::ColSudID, bh->sud()->id()}});
    int row = bh->sud()->modelMalzschuettung()->append(values);
    bh->sud()->modelMalzschuettung()->setData(row, ModelMalzschuettung::ColProzent, p);
    ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
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
    if (bh->sud()->getStatus() == Sud_Status_Rezept)
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
    double p = 100.0;
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        p -= wdg->prozent();
    }
    if (fabs(p) < 0.1)
        p = 0.0;
    QMap<int, QVariant> values({{ModelHopfengaben::ColSudID, bh->sud()->id()}, {ModelHopfengaben::ColZeit, bh->sud()->getKochdauerNachBitterhopfung()}});
    int row = bh->sud()->modelHopfengaben()->append(values);
    bh->sud()->modelHopfengaben()->setData(row, ModelHopfengaben::ColProzent, p);
    ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
}

void TabRezept::on_cbBerechnungsartHopfen_currentIndexChanged(int index)
{
    if (ui->cbBerechnungsartHopfen->hasFocus())
    {
        bh->sud()->setberechnungsArtHopfen(index);
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
    QMap<int, QVariant> values({{ModelHefegaben::ColSudID, bh->sud()->id()}});
    bh->sud()->modelHefegaben()->append(values);
    ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
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
    QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()}, {ModelWeitereZutatenGaben::ColTyp, EWZ_Typ_Hopfen}});
    bh->sud()->modelWeitereZutatenGaben()->append(values);
    ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->setValue(ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->maximum());
}

void TabRezept::on_btnNeueWeitereZutat_clicked()
{
    QMap<int, QVariant> values({{ModelWeitereZutatenGaben::ColSudID, bh->sud()->id()}});
    bh->sud()->modelWeitereZutatenGaben()->append(values);
    ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->setValue(ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->maximum());
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

void TabRezept::on_tbSudnummer_valueChanged(int value)
{
    if (ui->tbSudnummer->hasFocus())
        bh->sud()->setSudnummer(value);
}

void TabRezept::on_tbSudname_textChanged(const QString &value)
{
    if (ui->tbSudname->hasFocus())
        bh->sud()->setSudname(value);
}

void TabRezept::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        bh->sud()->setMenge(value);
}

void TabRezept::on_tbSW_valueChanged(double value)
{
    if (ui->tbSW->hasFocus())
        bh->sud()->setSW(value);
}

void TabRezept::on_tbCO2_valueChanged(double value)
{
    if (ui->tbCO2->hasFocus())
        bh->sud()->setCO2(value);
}

void TabRezept::on_tbBittere_valueChanged(int value)
{
    if (ui->tbBittere->hasFocus())
        bh->sud()->setIBU(value);
}

void TabRezept::on_tbReifezeit_valueChanged(int value)
{
    if (ui->tbReifezeit->hasFocus())
        bh->sud()->setReifezeit(value);
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

void TabRezept::on_tbHGF_valueChanged(int value)
{
    if (ui->tbHGF->hasFocus())
        bh->sud()->sethighGravityFaktor(value);
}

void TabRezept::on_tbFaktorHauptguss_valueChanged(double value)
{
    if (ui->tbFaktorHauptguss->hasFocus())
        bh->sud()->setFaktorHauptguss(value);
}

void TabRezept::on_cbWasserProfil_currentIndexChanged(const QString &value)
{
    if (ui->cbWasserProfil->hasFocus())
        bh->sud()->setWasserprofil(value);
}

void TabRezept::on_tbRestalkalitaet_valueChanged(double value)
{
    if (ui->tbRestalkalitaet->hasFocus())
        bh->sud()->setRestalkalitaetSoll(value);
}

void TabRezept::on_tbEinmaischtemperatur_valueChanged(int temp)
{
    if (ui->tbEinmaischtemperatur->hasFocus())
        bh->sud()->setEinmaischenTemp(temp);
}

void TabRezept::on_tbKochzeit_valueChanged(int min)
{
    if (ui->tbKochzeit->hasFocus())
        bh->sud()->setKochdauerNachBitterhopfung(min);
}

void TabRezept::on_tbNachisomerisierungszeit_valueChanged(int min)
{
    if (ui->tbNachisomerisierungszeit->hasFocus())
        bh->sud()->setNachisomerisierungszeit(min);
}

void TabRezept::on_btnTagNeu_clicked()
{
    QMap<int, QVariant> values({{ModelFlaschenlabelTags::ColSudID, bh->sud()->id()},
                                {ModelFlaschenlabelTags::ColTagname, tr("Neuer Tag")}});
    ProxyModel *model = bh->sud()->modelFlaschenlabelTags();
    int row = model->append(values);
    if (row >= 0)
    {
        QModelIndex index = model->index(row, ModelFlaschenlabelTags::ColTagname);
        ui->tableTags->setCurrentIndex(index);
        ui->tableTags->scrollTo(index);
        ui->tableTags->edit(index);
    }
}

void TabRezept::on_btnTagLoeschen_clicked()
{
    ProxyModel *model = bh->sud()->modelFlaschenlabelTags();
    QModelIndexList indices = ui->tableTags->selectionModel()->selectedIndexes();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        model->removeRow(index.row());
}
