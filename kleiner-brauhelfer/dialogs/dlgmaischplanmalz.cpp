#include "dlgmaischplanmalz.h"
#include "ui_dlgmaischplanmalz.h"
#include <QStandardItemModel>
#include "model/textdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/spinboxdelegate.h"
#include "widgets/widgetdecorator.h"
#include "sudobject.h"
#include "modelmalzschuettung.h"
#include "settings.h"

extern Settings* gSettings;

DlgMaischplanMalz::DlgMaischplanMalz(SudObject *sud, double value, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgMaischplanMalz),
    mSud(sud)
{
    WidgetDecorator::suspendValueChanged = true;
    connect(this, &QDialog::finished, [](){WidgetDecorator::suspendValueChanged = false;});

    ui->setupUi(this);
    adjustSize();
    ui->tbMenge->setMaximum(mSud->geterg_S_Gesamt());
    ui->tbMenge->setValue(value);

    QStandardItemModel* model = new QStandardItemModel(mSud->modelMalzschuettung()->rowCount(), 3, this);
    model->setHeaderData(0, Qt::Orientation::Horizontal, tr("Malz"));
    model->setHeaderData(1, Qt::Orientation::Horizontal, tr("Gesamtmenge") + "\n(kg)");
    model->setHeaderData(2, Qt::Orientation::Horizontal, tr("Anteil") + "\n(%)");
    for (int row = 0; row < mSud->modelMalzschuettung()->rowCount(); row++)
    {
        model->setData(model->index(row, 0), mSud->modelMalzschuettung()->data(row, ModelMalzschuettung::ColName));
        model->setData(model->index(row, 1), mSud->modelMalzschuettung()->data(row, ModelMalzschuettung::Colerg_Menge));
        model->setData(model->index(row, 2), 0);
        model->setData(model->index(row, 2), gSettings->paletteChanged.base(), Qt::BackgroundRole);
    }

    TableView *table = ui->tableView;
    table->setModel(model);
    table->appendCol({0, true, false, -1, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, table)});
    table->appendCol({1, true, false, 120, new DoubleSpinBoxDelegate(2, table)});
    table->appendCol({2, true, false, 100, new SpinBoxDelegate(0,100,1,false,table)});
    table->build();

    updateAnteile();
    connect(model, &QAbstractItemModel::dataChanged, this, &DlgMaischplanMalz::updateValue);
}

DlgMaischplanMalz::~DlgMaischplanMalz()
{
    delete ui;
}

void DlgMaischplanMalz::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

void DlgMaischplanMalz::updateAnteile()
{
    QAbstractItemModel* model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); row++)
    {
        int anteil = 0;
        if (std::fabs(mSud->geterg_S_Gesamt() - ui->tbMenge->value()) < 0.01)
            anteil = 100;
        else if (std::fabs(mSud->modelMalzschuettung()->data(row, ModelMalzschuettung::Colerg_Menge).toDouble() - ui->tbMenge->value()) < 0.01)
            anteil = 100;
        model->setData(model->index(row, 2), anteil);
    }
}

void DlgMaischplanMalz::updateValue()
{
    if (ui->tbMenge->hasFocus())
        return;
    double sum = 0.0;
    QAbstractItemModel* model = ui->tableView->model();
    for (int row = 0; row < model->rowCount(); row++)
        sum += mSud->modelMalzschuettung()->data(row, ModelMalzschuettung::Colerg_Menge).toDouble() * model->index(row, 2).data().toDouble()/100;
    ui->tbMenge->setValue(sum);
}

double DlgMaischplanMalz::value() const
{
    return ui->tbMenge->value();
}

void DlgMaischplanMalz::on_tbMenge_valueChanged(double)
{
    if (ui->tbMenge->hasFocus())
        updateAnteile();
}
