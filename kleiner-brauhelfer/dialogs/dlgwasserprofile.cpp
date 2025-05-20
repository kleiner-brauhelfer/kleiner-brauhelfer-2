#include "dlgwasserprofile.h"
#include "ui_dlgwasserprofile.h"
#include <QKeyEvent>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "model/doublespinboxdelegate.h"
#include "model/textdelegate.h"
#include "dialogs/dlgrohstoffvorlage.h"
#include "widgets/widgetdecorator.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgWasserprofile::DlgWasserprofile(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgWasserprofile),
    mRow(0)
{
    WidgetDecorator::suspendValueChanged = true;
    connect(this, &QDialog::finished, [](){WidgetDecorator::suspendValueChanged = false;});

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

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    ProxyModel *proxyModelWasser = new ProxyModel(this);
    TableView *table = ui->tableWasser;
    proxyModelWasser->setSourceModel(bh->modelWasser());
    table->setModel(proxyModelWasser);
    table->appendCol({ModelWasser::ColName, true, false, -1, new TextDelegate(table)});
    table->appendCol({ModelWasser::ColRestalkalitaet, true, false, 120, new DoubleSpinBoxDelegate(2, table)});
    table->build();

    ui->splitter->setSizes({200, 200, 50});

    connect(qApp, &QApplication::focusChanged, this, &DlgWasserprofile::focusChanged);
    connect(ui->tableWasser->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgWasserprofile::wasser_selectionChanged);
    connect(bh->modelWasser(), &ModelWasser::modified, this, &DlgWasserprofile::updateWasser);

    connect(ui->wdgBemerkung, &WdgBemerkung::changed, this, [this](const QString& html){setDataWasser(ModelWasser::ColBemerkung, html);});

    ui->tableWasser->selectRow(0);
}

DlgWasserprofile::~DlgWasserprofile()
{
    delete ui;
}

void DlgWasserprofile::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableState", ui->tableWasser->horizontalHeader()->saveState());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void DlgWasserprofile::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->tableWasser->restoreState(gSettings->value("tableState").toByteArray());
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    gSettings->endGroup();
}

void DlgWasserprofile::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->remove("tableState");
    gSettings->remove("splitterState");
    gSettings->endGroup();
}

void DlgWasserprofile::select(const QString& profile)
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
    int row = model->getRowWithValue(ModelWasser::ColName, profile);
    if (row >= 0)
        ui->tableWasser->setCurrentIndex(model->index(row, ModelWasser::ColName));
}

void DlgWasserprofile::keyPressEvent(QKeyEvent* event)
{
    DlgAbstract::keyPressEvent(event);
    if (ui->tableWasser->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnLoeschen_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnNeu_clicked();
            break;
        }
    }
}

void DlgWasserprofile::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && isAncestorOf(now) && now != ui->tbHelp && !qobject_cast<QSplitter*>(now))
        ui->tbHelp->setHtml(now->toolTip());
}

void DlgWasserprofile::on_btnNeu_clicked()
{
    QMap<int, QVariant> values({{ModelWasser::ColName, tr("Neues Profil")}});
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
    int row = model->append(values);
    if (row >= 0)
    {
        const QModelIndex index = model->index(row, ModelWasser::ColName);
        ui->tableWasser->setCurrentIndex(index);
        ui->tableWasser->scrollTo(ui->tableWasser->currentIndex());
        ui->tableWasser->edit(ui->tableWasser->currentIndex());
    }
}

void DlgWasserprofile::on_btnImport_clicked()
{
    DlgRohstoffVorlage dlg(DlgRohstoffVorlage::Art::Wasserprofil, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
        int row = model->append(dlg.values());
        if (row >= 0)
        {
            const QModelIndex index = model->index(row, ModelWasser::ColName);
            ui->tableWasser->setCurrentIndex(index);
            ui->tableWasser->scrollTo(ui->tableWasser->currentIndex());
            ui->tableWasser->edit(ui->tableWasser->currentIndex());
        }
    }
}

void DlgWasserprofile::on_btnLoeschen_clicked()
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
    QModelIndexList indices = ui->tableWasser->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
    {
        QString name = model->data(index.row(), ModelWasser::ColName).toString();
        int ret = QMessageBox::question(this, tr("Wasserprofil löschen?"),
                                        tr("Soll das Wasserprofil \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
            model->removeRow(index.row());
    }
}

QVariant DlgWasserprofile::dataWasser(int col) const
{
    return bh->modelWasser()->data(mRow, col);
}

bool DlgWasserprofile::setDataWasser(int col, const QVariant &value)
{
    return bh->modelWasser()->setData(mRow, col, value);
}

void DlgWasserprofile::wasser_selectionChanged(const QItemSelection &selected)
{
    if (selected.indexes().count() > 0)
    {
        ProxyModel *proxy = static_cast<ProxyModel*>(ui->tableWasser->model());
        mRow = proxy->mapRowToSource(selected.indexes()[0].row());
        updateWasser();
    }
}

void DlgWasserprofile::updateWasser()
{
    ui->lblWasserprofil->setText(dataWasser(ModelWasser::ColName).toString());
    if (!ui->tbCalciumMg->hasFocus())
        ui->tbCalciumMg->setValue(dataWasser(ModelWasser::ColCalcium).toDouble());
    if (!ui->tbCalciumMmol->hasFocus())
        ui->tbCalciumMmol->setValue(dataWasser(ModelWasser::ColCalciumMmol).toDouble());
    if (!ui->tbCalciumHaerte->hasFocus())
        ui->tbCalciumHaerte->setValue(dataWasser(ModelWasser::ColCalciumHaerte).toDouble());
    if (!ui->tbMagnesiumMg->hasFocus())
        ui->tbMagnesiumMg->setValue(dataWasser(ModelWasser::ColMagnesium).toDouble());
    if (!ui->tbMagnesiumMmol->hasFocus())
        ui->tbMagnesiumMmol->setValue(dataWasser(ModelWasser::ColMagnesiumMmol).toDouble());
    if (!ui->tbMagnesiumHaerte->hasFocus())
        ui->tbMagnesiumHaerte->setValue(dataWasser(ModelWasser::ColMagnesiumHaerte).toDouble());
    if (!ui->tbHydrogencarbonatMg->hasFocus())
        ui->tbHydrogencarbonatMg->setValue(dataWasser(ModelWasser::ColHydrogencarbonat).toDouble());
    if (!ui->tbHydrogencarbonatMmol->hasFocus())
        ui->tbHydrogencarbonatMmol->setValue(dataWasser(ModelWasser::ColHydrogencarbonatMmol).toDouble());
    if (!ui->tbHydrogencarbonatHaerte->hasFocus())
        ui->tbHydrogencarbonatHaerte->setValue(dataWasser(ModelWasser::ColCarbonatHaerte).toDouble());
    if (!ui->tbSulfatMg->hasFocus())
        ui->tbSulfatMg->setValue(dataWasser(ModelWasser::ColSulfat).toDouble());
    if (!ui->tbSulfatMmol->hasFocus())
        ui->tbSulfatMmol->setValue(dataWasser(ModelWasser::ColSulfatMmol).toDouble());
    if (!ui->tbChloridMg->hasFocus())
        ui->tbChloridMg->setValue(dataWasser(ModelWasser::ColChlorid).toDouble());
    if (!ui->tbChloridMmol->hasFocus())
        ui->tbChloridMmol->setValue(dataWasser(ModelWasser::ColChloridMmol).toDouble());
    if (!ui->tbNatriumMg->hasFocus())
        ui->tbNatriumMg->setValue(dataWasser(ModelWasser::ColNatrium).toDouble());
    if (!ui->tbNatriumMmol->hasFocus())
        ui->tbNatriumMmol->setValue(dataWasser(ModelWasser::ColNatriumMmol).toDouble());
    if (!ui->tbRestalkalitaetAdd->hasFocus())
        ui->tbRestalkalitaetAdd->setValue(dataWasser(ModelWasser::ColRestalkalitaetAdd).toDouble());
    ui->tbRestalkalitaet->setValue(dataWasser(ModelWasser::ColRestalkalitaet).toDouble());
    ui->wdgBemerkung->setHtml(dataWasser(ModelWasser::ColBemerkung).toString());
}

void DlgWasserprofile::on_tbCalciumMg_valueChanged(double value)
{
    if (ui->tbCalciumMg->hasFocus())
        setDataWasser(ModelWasser::ColCalcium, value);
}

void DlgWasserprofile::on_tbCalciumMmol_valueChanged(double value)
{
    if (ui->tbCalciumMmol->hasFocus())
        setDataWasser(ModelWasser::ColCalciumMmol, value);
}

void DlgWasserprofile::on_tbCalciumHaerte_valueChanged(double value)
{
    if (ui->tbCalciumHaerte->hasFocus())
        setDataWasser(ModelWasser::ColCalciumHaerte, value);
}

void DlgWasserprofile::on_tbMagnesiumMg_valueChanged(double value)
{
    if (ui->tbMagnesiumMg->hasFocus())
        setDataWasser(ModelWasser::ColMagnesium, value);
}

void DlgWasserprofile::on_tbMagnesiumMmol_valueChanged(double value)
{
    if (ui->tbMagnesiumMmol->hasFocus())
        setDataWasser(ModelWasser::ColMagnesiumMmol, value);
}

void DlgWasserprofile::on_tbMagnesiumHaerte_valueChanged(double value)
{
    if (ui->tbMagnesiumHaerte->hasFocus())
        setDataWasser(ModelWasser::ColMagnesiumHaerte, value);
}

void DlgWasserprofile::on_tbHydrogencarbonatMg_valueChanged(double value)
{
    if (ui->tbHydrogencarbonatMg->hasFocus())
        setDataWasser(ModelWasser::ColHydrogencarbonat, value);
}

void DlgWasserprofile::on_tbHydrogencarbonatMmol_valueChanged(double value)
{
    if (ui->tbHydrogencarbonatMmol->hasFocus())
        setDataWasser(ModelWasser::ColHydrogencarbonatMmol, value);
}

void DlgWasserprofile::on_tbHydrogencarbonatHaerte_valueChanged(double value)
{
    if (ui->tbHydrogencarbonatHaerte->hasFocus())
        setDataWasser(ModelWasser::ColCarbonatHaerte, value);
}

void DlgWasserprofile::on_tbSulfatMg_valueChanged(double value)
{
    if (ui->tbSulfatMg->hasFocus())
        setDataWasser(ModelWasser::ColSulfat, value);
}

void DlgWasserprofile::on_tbSulfatMmol_valueChanged(double value)
{
    if (ui->tbSulfatMmol->hasFocus())
        setDataWasser(ModelWasser::ColSulfatMmol, value);
}

void DlgWasserprofile::on_tbChloridMg_valueChanged(double value)
{
    if (ui->tbChloridMg->hasFocus())
        setDataWasser(ModelWasser::ColChlorid, value);
}

void DlgWasserprofile::on_tbChloridMmol_valueChanged(double value)
{
    if (ui->tbChloridMmol->hasFocus())
        setDataWasser(ModelWasser::ColChloridMmol, value);
}

void DlgWasserprofile::on_tbNatriumMg_valueChanged(double value)
{
    if (ui->tbNatriumMg->hasFocus())
        setDataWasser(ModelWasser::ColNatrium, value);
}

void DlgWasserprofile::on_tbNatriumMmol_valueChanged(double value)
{
    if (ui->tbNatriumMmol->hasFocus())
        setDataWasser(ModelWasser::ColNatriumMmol, value);
}

void DlgWasserprofile::on_tbRestalkalitaetAdd_valueChanged(double value)
{
    if (ui->tbRestalkalitaetAdd->hasFocus())
        setDataWasser(ModelWasser::ColRestalkalitaetAdd, value);
}
