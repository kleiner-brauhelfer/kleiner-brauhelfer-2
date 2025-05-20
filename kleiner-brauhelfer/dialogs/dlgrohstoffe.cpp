#include "dlgrohstoffe.h"
#include "ui_dlgrohstoffe.h"
#include <QTableView>
#include <QMessageBox>
#include <QMenu>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QUrl>
#include "mainwindow.h"
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodelrohstoff.h"
#include "model/textdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ebcdelegate.h"
#include "model/ingredientnamedelegate.h"
#include "model/linklabeldelegate.h"
#include "model/spinboxdelegate.h"
#include "model/phmalzdelegate.h"
#include "dialogs/dlgrohstoffvorlage.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgRohstoffe* DlgRohstoffe::Dialog = nullptr;

DlgRohstoffe::DlgRohstoffe(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DlgRohstoffe)
{
    QPalette pal;

    ui->setupUi(this);
    if (gSettings->theme() == Qt::ColorScheme::Dark)
    {
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QStringLiteral(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionNeu);
    menu->addSeparator();
    menu->addAction(ui->actionNeuVorlage);
    menu->addAction(ui->actionNeuObrama);
    menu->addSeparator();
    menu->addAction(ui->actionNeuKopie);
    ui->buttonAdd->setMenu(menu);
    connect(ui->actionNeu, &QAction::triggered, this, &DlgRohstoffe::buttonAdd_clicked);
    connect(ui->actionNeuVorlage, &QAction::triggered, this, &DlgRohstoffe::buttonNeuVorlage_clicked);
    connect(ui->actionNeuObrama, &QAction::triggered, this, &DlgRohstoffe::buttonNeuVorlageObrama_clicked);
    connect(ui->actionNeuKopie, &QAction::triggered, this, &DlgRohstoffe::buttonCopy_clicked);

    pal = ui->tableMalz->palette();
    pal.setColor(QPalette::Button, gSettings->colorMalz);
    ui->tableMalz->setPalette(pal);

    pal = ui->tableHopfen->palette();
    pal.setColor(QPalette::Button, gSettings->colorHopfen);
    ui->tableHopfen->setPalette(pal);

    pal = ui->tableHefe->palette();
    pal.setColor(QPalette::Button, gSettings->colorHefe);
    ui->tableHefe->setPalette(pal);

    pal = ui->tableWeitereZutaten->palette();
    pal.setColor(QPalette::Button, gSettings->colorZusatz);
    ui->tableWeitereZutaten->setPalette(pal);

    ProxyModelRohstoff *proxyModel;
    TableView *table;

    table = ui->tableMalz;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelMalz());
    table->setModel(proxyModel);

    table = ui->tableHopfen;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelHopfen());
    table->setModel(proxyModel);

    table = ui->tableHefe;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelHefe());
    table->setModel(proxyModel);

    table = ui->tableWeitereZutaten;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelWeitereZutaten());
    table->setModel(proxyModel);

    modulesChanged(Settings::ModuleAlle);
    connect(gSettings, &Settings::modulesChanged, this, &DlgRohstoffe::modulesChanged);
}

DlgRohstoffe::~DlgRohstoffe()
{
    delete ui;
}

void DlgRohstoffe::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableMalzState", ui->tableMalz->horizontalHeader()->saveState());
    gSettings->setValue("tableHopfenState", ui->tableHopfen->horizontalHeader()->saveState());
    gSettings->setValue("tableHefeState", ui->tableHefe->horizontalHeader()->saveState());
    gSettings->setValue("tableWeitereZutatenState", ui->tableWeitereZutaten->horizontalHeader()->saveState());
    int filter = 0;
    if (ui->radioButtonVorhanden->isChecked())
        filter = 1;
    else if (ui->radioButtonInGebrauch->isChecked())
        filter = 2;
    gSettings->setValue("filter", filter);
    gSettings->endGroup();
}

void DlgRohstoffe::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->tableMalz->restoreState(gSettings->value("tableMalzState").toByteArray());
    ui->tableHopfen->restoreState(gSettings->value("tableHopfenState").toByteArray());
    ui->tableHefe->restoreState(gSettings->value("tableHefeState").toByteArray());
    ui->tableWeitereZutaten->restoreState(gSettings->value("tableWeitereZutatenState").toByteArray());
    int filter = gSettings->value("filter", 0).toInt();
    if (filter == 1)
    {
        ui->radioButtonVorhanden->setChecked(true);
        on_radioButtonVorhanden_clicked();
    }
    else if (filter == 2)
    {
        ui->radioButtonInGebrauch->setChecked(true);
        on_radioButtonInGebrauch_clicked();
    }
    else
    {
        ui->radioButtonAlle->setChecked(true);
        on_radioButtonAlle_clicked();
    }
    gSettings->endGroup();
}

void DlgRohstoffe::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->remove("tableMalzState");
    gSettings->remove("tableHopfenState");
    gSettings->remove("tableHefeState");
    gSettings->remove("tableWeitereZutatenState");
    gSettings->endGroup();
}

void DlgRohstoffe::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleLagerverwaltung) ||
        modules.testFlag(Settings::ModuleWasseraufbereitung) ||
        modules.testFlag(Settings::ModulePreiskalkulation))
    {
        build();
    }
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        ui->radioButtonVorhanden->setVisible(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
        ui->radioButtonAlle->setChecked(true);
        on_radioButtonAlle_clicked();
    }
}

void DlgRohstoffe::build()
{
    TableView *table = ui->tableMalz;
    table->clearCols();
    table->appendCol({ModelMalz::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        table->appendCol({ModelMalz::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelMalz::ColFarbe, true, true, 100, new EbcDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
        table->appendCol({ModelMalz::ColpH, true, true, 100, new PhMalzDelegate(table)});
    table->appendCol({ModelMalz::ColMaxProzent, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->appendCol({ModelMalz::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelMalz::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelMalz::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        table->appendCol({ModelMalz::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelMalz::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelMalz::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelMalz::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();

    table = ui->tableHopfen;
    table->clearCols();
    table->appendCol({ModelHopfen::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        table->appendCol({ModelHopfen::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, std::numeric_limits<double>::max(), 1, false, table)});
    table->appendCol({ModelHopfen::ColAlpha, true, true, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, true, table)});
    table->appendCol({ModelHopfen::ColPellets, true, true, 100, new CheckBoxDelegate(table)});
    table->appendCol({ModelHopfen::ColTyp, true, true, 100, new ComboBoxDelegate(MainWindow::HopfenTypname, gSettings->HopfenTypBackgrounds, table)});
    table->appendCol({ModelHopfen::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHopfen::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHopfen::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        table->appendCol({ModelHopfen::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelHopfen::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelHopfen::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelHopfen::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();

    table = ui->tableHefe;
    table->clearCols();
    table->appendCol({ModelHefe::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        table->appendCol({ModelHefe::ColMenge, true, false, 100, new SpinBoxDelegate(0, std::numeric_limits<int>::max(), 1, false, table)});
    table->appendCol({ModelHefe::ColTypOGUG, true, true, 100, new ComboBoxDelegate(MainWindow::HefeTypname, gSettings->HefeTypOgUgBackgrounds, table)});
    table->appendCol({ModelHefe::ColTypTrFl, true, true, 100, new ComboBoxDelegate(MainWindow::HefeTypFlTrName, gSettings->HefeTypTrFlBackgrounds, table)});
    table->appendCol({ModelHefe::ColWuerzemenge, true, true, 100, new DoubleSpinBoxDelegate(1, 0, std::numeric_limits<double>::max(), 1, false, table)});
    table->appendCol({ModelHefe::ColSedimentation, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColEVG, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColTemperatur, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHefe::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHefe::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        table->appendCol({ModelHefe::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelHefe::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelHefe::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelHefe::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();

    table = ui->tableWeitereZutaten;
    table->clearCols();
    table->appendCol({ModelWeitereZutaten::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        table->appendCol({ModelWeitereZutaten::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelWeitereZutaten::ColEinheit, true, false, 100, new ComboBoxDelegate(MainWindow::Einheiten, table)});
    table->appendCol({ModelWeitereZutaten::ColTyp, true, true, 100, new ComboBoxDelegate(MainWindow::ZusatzTypname, gSettings->WZTypBackgrounds, table)});
    table->appendCol({ModelWeitereZutaten::ColAusbeute, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->appendCol({ModelWeitereZutaten::ColUnvergaerbar, true, true, 100, new CheckBoxDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColFarbe, true, true, 100, new EbcDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        table->appendCol({ModelWeitereZutaten::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelWeitereZutaten::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelWeitereZutaten::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelWeitereZutaten::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
}

void DlgRohstoffe::keyPressEvent(QKeyEvent* event)
{
    DlgAbstract::keyPressEvent(event);
    QTableView *table;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        break;
    case 1:
        table = ui->tableHopfen;
        break;
    case 2:
        table = ui->tableHefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        break;
    default:
        return;
    }
    if (table->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_buttonDelete_clicked();
            break;
        case Qt::Key::Key_Insert:
            buttonAdd_clicked();
            break;
        }
    }
}

void DlgRohstoffe::on_tableMalz_clicked(const QModelIndex &index)
{
    if (index.column() == ModelMalz::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::on_tableHopfen_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHopfen::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::on_tableHefe_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHefe::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::on_tableWeitereZutaten_clicked(const QModelIndex &index)
{
    if (index.column() == ModelWeitereZutaten::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::addEntry(QTableView *table, const QMap<int, QVariant> &values)
{
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    ui->radioButtonAlle->setChecked(true);
    on_radioButtonAlle_clicked();
    ui->lineEditFilter->clear();
    int row = model->append(values);
    if (row >= 0)
    {
        table->setCurrentIndex(model->index(row, model->fieldIndex(QStringLiteral("Name"))));
        table->scrollTo(table->currentIndex());
        table->edit(table->currentIndex());
        updateLabelNumItems();
    }
}

void DlgRohstoffe::buttonAdd_clicked()
{
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        addEntry(ui->tableMalz, {{ModelMalz::ColName, tr("Neuer Eintrag")}});
        break;
    case 1:
        addEntry(ui->tableHopfen, {{ModelHopfen::ColName, tr("Neuer Eintrag")}});
        break;
    case 2:
        addEntry(ui->tableHefe, {{ModelHefe::ColName, tr("Neuer Eintrag")}});
        break;
    case 3:
        addEntry(ui->tableWeitereZutaten, {{ModelWeitereZutaten::ColName, tr("Neuer Eintrag")}});
        break;
    }
}

void DlgRohstoffe::buttonNeuVorlage_clicked()
{
    QTableView *table;
    DlgRohstoffVorlage::Art art;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        art = DlgRohstoffVorlage::Art::Malz;
        break;
    case 1:
        table = ui->tableHopfen;
        art = DlgRohstoffVorlage::Art::Hopfen;
        break;
    case 2:
        table = ui->tableHefe;
        art = DlgRohstoffVorlage::Art::Hefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        art = DlgRohstoffVorlage::Art::WZutaten;
        break;
    default:
        return;
    }
    DlgRohstoffVorlage dlg(art, this);
    if (dlg.exec() == QDialog::Accepted)
        addEntry(table, dlg.values());
}

void DlgRohstoffe::buttonNeuVorlageObrama_clicked()
{
    QTableView *table;
    DlgRohstoffVorlage::Art art;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        art = DlgRohstoffVorlage::Art::MalzOBraMa;
        break;
    case 1:
        table = ui->tableHopfen;
        art = DlgRohstoffVorlage::Art::HopfenOBraMa;
        break;
    case 2:
        table = ui->tableHefe;
        art = DlgRohstoffVorlage::Art::HefeOBraMa;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        art = DlgRohstoffVorlage::Art::WZutatenOBraMa;
        break;
    default:
        return;
    }
    DlgRohstoffVorlage dlg(art, this);
    if (dlg.exec() == QDialog::Accepted)
        addEntry(table, dlg.values());
}

void DlgRohstoffe::buttonCopy_clicked()
{
    QTableView *table;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        break;
    case 1:
        table = ui->tableHopfen;
        break;
    case 2:
        table = ui->tableHefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        break;
    default:
        return;
    }
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    SqlTableModel *sourceModel = static_cast<SqlTableModel*>(model->sourceModel());
    for (const QModelIndex& index : table->selectionModel()->selectedRows())
    {
        int row = index.row();
        QMap<int, QVariant> values = sourceModel->copyValues(model->mapRowToSource(row));
        values.insert(model->fieldIndex(QStringLiteral("Name")), model->data(row, model->fieldIndex(QStringLiteral("Name"))).toString() + " " + tr("Kopie"));
        values.remove(model->fieldIndex(QStringLiteral("Menge")));
        values.remove(model->fieldIndex(QStringLiteral("Eingelagert")));
        values.remove(model->fieldIndex(QStringLiteral("Mindesthaltbar")));
        addEntry(table, values);
    }
}

void DlgRohstoffe::on_buttonDelete_clicked()
{
    QTableView *table;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        break;
    case 1:
        table = ui->tableHopfen;
        break;
    case 2:
        table = ui->tableHefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        break;
    default:
        return;
    }
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    QModelIndexList indices = table->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
    {
        bool del = true;
        if (model->data(index.row(), model->fieldIndex(QStringLiteral("InGebrauch"))).toBool())
        {
            QString name = model->data(index.row(), model->fieldIndex(QStringLiteral("Name"))).toString();
            QStringList liste;
            switch (ui->toolBoxRohstoffe->currentIndex())
            {
            case 0:
                liste = bh->modelMalz()->inGebrauchListe(name);
                break;
            case 1:
                liste = bh->modelHopfen()->inGebrauchListe(name);
                break;
            case 2:
                liste = bh->modelHefe()->inGebrauchListe(name);
                break;
            case 3:
                liste = bh->modelWeitereZutaten()->inGebrauchListe(name);
                break;
            }
            QString strListe = "\n\n- " + liste.join(QStringLiteral("\n- "));
            int ret = QMessageBox::question(this, tr("Rohstoff wird verwendet"),
                                            tr("Dieser Rohstoff wird in einem noch nicht gebrauten Sud verwendet. Soll er trotzdem gelöscht werden?") + strListe);
            if (ret != QMessageBox::Yes)
                del = false;
        }
        if (del)
            model->removeRow(index.row());
    }
    updateLabelNumItems();
}

void DlgRohstoffe::on_radioButtonAlle_clicked()
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    updateLabelNumItems();
}

void DlgRohstoffe::on_radioButtonVorhanden_clicked()
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    updateLabelNumItems();
}

void DlgRohstoffe::on_radioButtonInGebrauch_clicked()
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    updateLabelNumItems();
}

void DlgRohstoffe::on_lineEditFilter_textChanged(const QString &pattern)
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilterString(pattern);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilterString(pattern);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilterString(pattern);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilterString(pattern);
    updateLabelNumItems();
}

void DlgRohstoffe::on_toolBoxRohstoffe_currentChanged()
{
    updateLabelNumItems();
}

void DlgRohstoffe::updateLabelNumItems()
{
    QAbstractItemModel* filteredModel;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        filteredModel = ui->tableMalz->model();
        break;
    case 1:
        filteredModel = ui->tableHopfen->model();
        break;
    case 2:
        filteredModel = ui->tableHefe->model();
        break;
    case 3:
        filteredModel = ui->tableWeitereZutaten->model();
        break;
    default:
        ui->lblNumItems->setText(QStringLiteral(""));
        return;
    }
    QAbstractItemModel* sourceModel = static_cast<ProxyModel*>(filteredModel)->sourceModel();
    ProxyModel sourceModelNoDelete;
    sourceModelNoDelete.setSourceModel(sourceModel);
    ui->lblNumItems->setText(QString::number(filteredModel->rowCount()) + " / " + QString::number(sourceModelNoDelete.rowCount()));
}
