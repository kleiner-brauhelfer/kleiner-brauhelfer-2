#include "dlgrohstoffeabziehen.h"
#include "ui_dlgrohstoffeabziehen.h"
#include <QStandardItemModel>
#include "model/doublespinboxdelegate.h"
#include "model/readonlydelegate.h"
#include "model/spinboxdelegate.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

class RohstoffeAbziehenModel : public QStandardItemModel
{
public:
    explicit RohstoffeAbziehenModel(QObject *parent = nullptr) :
        QStandardItemModel(parent)
    {
    }
    void appendRow(const QString& name, double menge, double vorhanden)
    {
        QStandardItemModel::appendRow({new QStandardItem(name),
                   new QStandardItem(QString::number(menge)),
                   new QStandardItem(QString::number(vorhanden))});
    }
};

DlgRohstoffeAbziehen::DlgRohstoffeAbziehen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRohstoffeAbziehen)
{
    ui->setupUi(this);
    build();
    adjustSize();
}

DlgRohstoffeAbziehen::DlgRohstoffeAbziehen(int typ, const QString &name, double menge, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRohstoffeAbziehen)
{
    ui->setupUi(this);
    build(typ, name, menge);
    adjustSize();
}

DlgRohstoffeAbziehen::~DlgRohstoffeAbziehen()
{
    delete ui;
}

void DlgRohstoffeAbziehen::build(int typ, const QString& name, double menge)
{
    ProxyModel *modelRohstoff;
    QMap<QString, double> list;
    QMap<QString, double> listAbfuellen;
    QMap<QString, double>::const_iterator it;

    list.clear();
    if (typ == 0)
    {
        list[name] = menge;
    }
    else if (typ == -1)
    {
        modelRohstoff = bh->sud()->modelMalzschuettung();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            QString name = modelRohstoff->data(r, ModelMalzschuettung::ColName).toString();
            list[name] += modelRohstoff->data(r, ModelMalzschuettung::Colerg_Menge).toDouble();
        }
    }
    RohstoffeAbziehenModel* modelMalz = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelMalz->appendRow(it.key(), it.value(),
                             bh->modelMalz()->getValueFromSameRow(ModelMalz::ColBeschreibung, it.key(), ModelMalz::ColMenge).toDouble());
        it++;
    }
    modelMalz->setHeaderData(0, Qt::Horizontal, tr("Malz"));
    modelMalz->setHeaderData(1, Qt::Horizontal, tr("Menge [kg]"));
    modelMalz->setHeaderData(2, Qt::Horizontal, tr("Vorhanden [kg]"));
    ui->tableViewMalz->setModel(modelMalz);
    ui->tableViewMalz->setItemDelegateForColumn(0, new ReadonlyDelegate(this));
    ui->tableViewMalz->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, false, this));
    ui->tableViewMalz->setItemDelegateForColumn(2, new DoubleSpinBoxDelegate(2, this));
    if (modelMalz->rowCount() > 0)
        ui->tableViewMalz->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewMalz->setVisible(false);

    list.clear();
    listAbfuellen.clear();
    if (typ == 1)
    {
        list[name] = menge;
    }
    else if (typ == -1)
    {
        modelRohstoff = bh->sud()->modelHopfengaben();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            QString name = modelRohstoff->data(r, ModelHopfengaben::ColName).toString();
            list[name] += modelRohstoff->data(r, ModelHopfengaben::Colerg_Menge).toDouble();
        }
        modelRohstoff = bh->sud()->modelWeitereZutatenGaben();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            if (modelRohstoff->data(r, ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen)
            {
                QString name = modelRohstoff->data(r, ModelWeitereZutatenGaben::ColName).toString();
                if (modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() != EWZ_Zeitpunkt_Gaerung ||
                    modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZugabeNach).toInt() == 0)
                    list[name] += modelRohstoff->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                else
                    listAbfuellen[name] = 0;
            }
        }
    }
    RohstoffeAbziehenModel* modelHopfen = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelHopfen->appendRow(it.key(), it.value(),
                             bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, it.key(), ModelHopfen::ColMenge).toDouble());
        it++;
    }
    it = listAbfuellen.constBegin();
    while (it != listAbfuellen.constEnd())
    {
        modelHopfen->appendRow(it.key(), it.value(),
                             bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, it.key(), ModelHopfen::ColMenge).toDouble());
        it++;
    }
    modelHopfen->setHeaderData(0, Qt::Horizontal, tr("Hopfen"));
    modelHopfen->setHeaderData(1, Qt::Horizontal, tr("Menge [g]"));
    modelHopfen->setHeaderData(2, Qt::Horizontal, tr("Vorhanden [g]"));
    ui->tableViewHopfen->setModel(modelHopfen);
    ui->tableViewHopfen->setItemDelegateForColumn(0, new ReadonlyDelegate(this));
    ui->tableViewHopfen->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(0, 0, 9999, 1, false, this));
    ui->tableViewHopfen->setItemDelegateForColumn(2, new SpinBoxDelegate(this));
    if (modelHopfen->rowCount() > 0)
        ui->tableViewHopfen->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewHopfen->setVisible(false);

    list.clear();
    listAbfuellen.clear();
    if (typ == 2)
    {
        list[name] = menge;
    }
    else if (typ == -1)
    {
        modelRohstoff = bh->sud()->modelHefegaben();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            QString name = modelRohstoff->data(r, ModelHefegaben::ColName).toString();
            if (modelRohstoff->data(r, ModelHefegaben::ColZugabeNach).toInt() == 0)
                list[name] += modelRohstoff->data(r, ModelHefegaben::ColMenge).toDouble();
            else
                listAbfuellen[name] = 0;
        }
    }
    RohstoffeAbziehenModel* modelHefe = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelHefe->appendRow(it.key(), it.value(),
                             bh->modelHefe()->getValueFromSameRow(ModelHefe::ColBeschreibung, it.key(), ModelHefe::ColMenge).toDouble());
        it++;
    }
    it = listAbfuellen.constBegin();
    while (it != listAbfuellen.constEnd())
    {
        modelHefe->appendRow(it.key(), it.value(),
                             bh->modelHefe()->getValueFromSameRow(ModelHefe::ColBeschreibung, it.key(), ModelHefe::ColMenge).toDouble());
        it++;
    }
    modelHefe->setHeaderData(0, Qt::Horizontal, tr("Hefe"));
    modelHefe->setHeaderData(1, Qt::Horizontal, tr("Menge"));
    modelHefe->setHeaderData(2, Qt::Horizontal, tr("Vorhanden"));
    ui->tableViewHefe->setModel(modelHefe);
    ui->tableViewHefe->setItemDelegateForColumn(0, new ReadonlyDelegate(this));
    ui->tableViewHefe->setItemDelegateForColumn(1, new SpinBoxDelegate(0, 9999, 1, false, this));
    ui->tableViewHefe->setItemDelegateForColumn(2, new SpinBoxDelegate(this));
    if (modelHefe->rowCount() > 0)
        ui->tableViewHefe->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewHefe->setVisible(false);

    list.clear();
    listAbfuellen.clear();
    if (typ == 3)
    {
        list[name] = menge;
    }
    else if (typ == -1)
    {
        modelRohstoff = bh->sud()->modelWeitereZutatenGaben();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            if (modelRohstoff->data(r, ModelWeitereZutatenGaben::ColTyp).toInt() != EWZ_Typ_Hopfen)
            {
                QString name = modelRohstoff->data(r, ModelWeitereZutatenGaben::ColName).toString();
                if (modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() != EWZ_Zeitpunkt_Gaerung ||
                    modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZugabeNach).toInt() == 0)
                    list[name] += modelRohstoff->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                else
                    listAbfuellen[name] = 0;
            }
        }
    }
    RohstoffeAbziehenModel* modelWz = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelWz->appendRow(it.key(), it.value(),
                             bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColBeschreibung, it.key(), ModelWeitereZutaten::ColMengeGramm).toDouble());
        it++;
    }
    it = listAbfuellen.constBegin();
    while (it != listAbfuellen.constEnd())
    {
        modelWz->appendRow(it.key(), it.value(),
                             bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColBeschreibung, it.key(), ModelWeitereZutaten::ColMengeGramm).toDouble());
        it++;
    }
    modelWz->setHeaderData(0, Qt::Horizontal, tr("Weitere Zutat"));
    modelWz->setHeaderData(1, Qt::Horizontal, tr("Menge [g]"));
    modelWz->setHeaderData(2, Qt::Horizontal, tr("Vorhanden [g]"));
    ui->tableViewWZ->setModel(modelWz);
    ui->tableViewWZ->setItemDelegateForColumn(0, new ReadonlyDelegate(this));
    ui->tableViewWZ->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, false, this));
    ui->tableViewWZ->setItemDelegateForColumn(2, new DoubleSpinBoxDelegate(2, this));
    if (modelWz->rowCount() > 0)
        ui->tableViewWZ->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewWZ->setVisible(false);
}

void DlgRohstoffeAbziehen::on_btnAbziehen_clicked()
{
    QAbstractItemModel *model;

    model = ui->tableViewMalz->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        bh->rohstoffAbziehen(0,
                             name,
                             model->index(r, 1).data().toDouble());
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelMalz()->getValueFromSameRow(ModelMalz::ColBeschreibung, name, ModelMalz::ColMenge));
    }

    model = ui->tableViewHopfen->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        bh->rohstoffAbziehen(1,
                             model->index(r, 0).data().toString(),
                             model->index(r, 1).data().toDouble());
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, name, ModelHopfen::ColMenge));
    }

    model = ui->tableViewHefe->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        bh->rohstoffAbziehen(2,
                             model->index(r, 0).data().toString(),
                             model->index(r, 1).data().toDouble());
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelHefe()->getValueFromSameRow(ModelHefe::ColBeschreibung, name, ModelHefe::ColMenge));
    }

    model = ui->tableViewWZ->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        bh->rohstoffAbziehen(3,
                             model->index(r, 0).data().toString(),
                             model->index(r, 1).data().toDouble());
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColBeschreibung, name, ModelWeitereZutaten::ColMengeGramm));
    }
}
