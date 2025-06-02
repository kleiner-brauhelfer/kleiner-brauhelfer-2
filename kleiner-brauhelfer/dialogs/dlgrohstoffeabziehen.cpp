#include "dlgrohstoffeabziehen.h"
#include "ui_dlgrohstoffeabziehen.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include "model/doublespinboxdelegate.h"
#include "model/textdelegate.h"
#include "model/spinboxdelegate.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

class RohstoffeAbziehenModel : public QStandardItemModel
{
public:
    explicit RohstoffeAbziehenModel(QObject *parent = nullptr) :
        QStandardItemModel(parent)
    {
    }
    void appendRow(const QString& name, double menge, double vorhanden)
    {
        QStandardItem* itemMenge = new QStandardItem(QString::number(menge));
        itemMenge->setBackground(gSettings->paletteChanged.base());
        QStandardItemModel::appendRow({new QStandardItem(name),
                   itemMenge,
                   new QStandardItem(QString::number(vorhanden))});
    }
};

DlgRohstoffeAbziehen::DlgRohstoffeAbziehen(bool abziehen, QWidget *parent) :
    DlgRohstoffeAbziehen(abziehen, Brauhelfer::RohstoffTyp::Malz, QString(), 0.0, parent)
{
}

DlgRohstoffeAbziehen::DlgRohstoffeAbziehen(bool abziehen, Brauhelfer::RohstoffTyp typ, const QString &name, double menge, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgRohstoffeAbziehen),
    mAbziehen(abziehen),
    mAbgezogen(false)
{
    ui->setupUi(this);
    setWindowTitle(abziehen ? tr("Zutaten vom Lager abziehen") : tr("Zutaten ins Lager zurückgeben"));
    ui->btnAbziehen->setText(abziehen ? tr("Zutaten abziehen") : tr("Zutaten zurückgeben"));
    if (name.isEmpty())
        setModels(true);
    else
        setModels(false, typ, name, menge);
    adjustSize();
}

DlgRohstoffeAbziehen::~DlgRohstoffeAbziehen()
{
    delete ui;
}

void DlgRohstoffeAbziehen::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

void DlgRohstoffeAbziehen::reject()
{
    if (mAbziehen && !mAbgezogen)
    {
        int ret = QMessageBox::warning(this, windowTitle(), tr("Die Zutaten wurden noch nicht abgezogen. Trotzdem schließen?"), QMessageBox::Yes | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
            QDialog::reject();
    }
    else
    {
        QDialog::reject();
    }
}

void DlgRohstoffeAbziehen::setModels(bool alleBrauzutaten, Brauhelfer::RohstoffTyp typ, const QString& name, double menge)
{
    ProxyModel *modelRohstoff;
    QMap<QString, double> list;
    QMap<QString, double> listAbfuellen;
    QMap<QString, double>::const_iterator it;

    list.clear();
    if (alleBrauzutaten)
    {
        modelRohstoff = bh->sud()->modelMalzschuettung();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            QString name = modelRohstoff->data(r, ModelMalzschuettung::ColName).toString();
            list[name] += modelRohstoff->data(r, ModelMalzschuettung::Colerg_Menge).toDouble();
        }
    }
    else if (typ == Brauhelfer::RohstoffTyp::Malz)
    {
        list[name] = menge;
    }
    RohstoffeAbziehenModel* modelMalz = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelMalz->appendRow(it.key(), it.value(),
                             bh->modelMalz()->getValueFromSameRow(ModelMalz::ColName, it.key(), ModelMalz::ColMenge).toDouble());
        it++;
    }
    modelMalz->setHeaderData(0, Qt::Horizontal, tr("Malz"));
    modelMalz->setHeaderData(1, Qt::Horizontal, tr("Menge") + "\n(kg)");
    modelMalz->setHeaderData(2, Qt::Horizontal, tr("Vorhanden") + "\n(kg)");
    ui->tableViewMalz->setModel(modelMalz);
    ui->tableViewMalz->setItemDelegateForColumn(0, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, this));
    ui->tableViewMalz->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, this));
    ui->tableViewMalz->setItemDelegateForColumn(2, new DoubleSpinBoxDelegate(2, this));
    if (modelMalz->rowCount() > 0)
        ui->tableViewMalz->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewMalz->setVisible(false);

    list.clear();
    listAbfuellen.clear();
    if (alleBrauzutaten)
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
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(modelRohstoff->data(r, ModelWeitereZutatenGaben::ColTyp).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen)
            {
                QString name = modelRohstoff->data(r, ModelWeitereZutatenGaben::ColName).toString();
                Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
                if (zeitpunkt != Brauhelfer::ZusatzZeitpunkt::Gaerung || modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZugabeNach).toInt() == 0)
                    list[name] += modelRohstoff->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                else
                    listAbfuellen[name] = 0;
            }
        }
    }
    else if (typ == Brauhelfer::RohstoffTyp::Hopfen)
    {
        list[name] = menge;
    }
    RohstoffeAbziehenModel* modelHopfen = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelHopfen->appendRow(it.key(), it.value(),
                             bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, it.key(), ModelHopfen::ColMenge).toDouble());
        it++;
    }
    it = listAbfuellen.constBegin();
    while (it != listAbfuellen.constEnd())
    {
        modelHopfen->appendRow(it.key(), it.value(),
                             bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, it.key(), ModelHopfen::ColMenge).toDouble());
        it++;
    }
    modelHopfen->setHeaderData(0, Qt::Horizontal, tr("Hopfen"));
    modelHopfen->setHeaderData(1, Qt::Horizontal, tr("Menge") + "\n(g)");
    modelHopfen->setHeaderData(2, Qt::Horizontal, tr("Vorhanden") + "\n(g)");
    ui->tableViewHopfen->setModel(modelHopfen);
    ui->tableViewHopfen->setItemDelegateForColumn(0, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, this));
    ui->tableViewHopfen->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(1, 0, std::numeric_limits<double>::max(), 1, false, this));
    ui->tableViewHopfen->setItemDelegateForColumn(2, new DoubleSpinBoxDelegate(1, this));
    if (modelHopfen->rowCount() > 0)
        ui->tableViewHopfen->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewHopfen->setVisible(false);

    list.clear();
    listAbfuellen.clear();
    if (alleBrauzutaten)
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
    else if (typ == Brauhelfer::RohstoffTyp::Hefe)
    {
        list[name] = menge;
    }
    RohstoffeAbziehenModel* modelHefe = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelHefe->appendRow(it.key(), it.value(),
                             bh->modelHefe()->getValueFromSameRow(ModelHefe::ColName, it.key(), ModelHefe::ColMenge).toDouble());
        it++;
    }
    it = listAbfuellen.constBegin();
    while (it != listAbfuellen.constEnd())
    {
        modelHefe->appendRow(it.key(), it.value(),
                             bh->modelHefe()->getValueFromSameRow(ModelHefe::ColName, it.key(), ModelHefe::ColMenge).toDouble());
        it++;
    }
    modelHefe->setHeaderData(0, Qt::Horizontal, tr("Hefe"));
    modelHefe->setHeaderData(1, Qt::Horizontal, tr("Menge"));
    modelHefe->setHeaderData(2, Qt::Horizontal, tr("Vorhanden"));
    ui->tableViewHefe->setModel(modelHefe);
    ui->tableViewHefe->setItemDelegateForColumn(0, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, this));
    ui->tableViewHefe->setItemDelegateForColumn(1, new SpinBoxDelegate(0, std::numeric_limits<int>::max(), 1, false, this));
    ui->tableViewHefe->setItemDelegateForColumn(2, new SpinBoxDelegate(this));
    if (modelHefe->rowCount() > 0)
        ui->tableViewHefe->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    else
        ui->tableViewHefe->setVisible(false);

    list.clear();
    listAbfuellen.clear();
    if (alleBrauzutaten)
    {
        modelRohstoff = bh->sud()->modelWeitereZutatenGaben();
        for (int r = 0; r < modelRohstoff->rowCount(); ++r)
        {
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(modelRohstoff->data(r, ModelWeitereZutatenGaben::ColTyp).toInt());
            if (typ != Brauhelfer::ZusatzTyp::Hopfen)
            {
                QString name = modelRohstoff->data(r, ModelWeitereZutatenGaben::ColName).toString();
                Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
                if (zeitpunkt != Brauhelfer::ZusatzZeitpunkt::Gaerung || modelRohstoff->data(r, ModelWeitereZutatenGaben::ColZugabeNach).toInt() == 0)
                    list[name] += modelRohstoff->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                else
                    listAbfuellen[name] = 0;
            }
        }
    }
    else if (typ == Brauhelfer::RohstoffTyp::Zusatz)
    {
        list[name] = menge;
    }
    RohstoffeAbziehenModel* modelWz = new RohstoffeAbziehenModel(this);
    it = list.constBegin();
    while (it != list.constEnd())
    {
        modelWz->appendRow(it.key(), it.value(),
                             bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColName, it.key(), ModelWeitereZutaten::ColMengeNormiert).toDouble());
        it++;
    }
    it = listAbfuellen.constBegin();
    while (it != listAbfuellen.constEnd())
    {
        modelWz->appendRow(it.key(), it.value(),
                             bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColName, it.key(), ModelWeitereZutaten::ColMengeNormiert).toDouble());
        it++;
    }
    modelWz->setHeaderData(0, Qt::Horizontal, tr("Zusatz"));
    modelWz->setHeaderData(1, Qt::Horizontal, tr("Menge") + "\n(g)");
    modelWz->setHeaderData(2, Qt::Horizontal, tr("Vorhanden") + "\n(g)");
    ui->tableViewWZ->setModel(modelWz);
    ui->tableViewWZ->setItemDelegateForColumn(0, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, this));
    ui->tableViewWZ->setItemDelegateForColumn(1, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, this));
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
        double menge = model->index(r, 1).data().toDouble();
        if (!mAbziehen)
            menge *= -1;
        bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Malz, name, menge);
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelMalz()->getValueFromSameRow(ModelMalz::ColName, name, ModelMalz::ColMenge));
    }

    model = ui->tableViewHopfen->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        double menge = model->index(r, 1).data().toDouble();
        if (!mAbziehen)
            menge *= -1;
        bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Hopfen, name, menge);
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, name, ModelHopfen::ColMenge));
    }

    model = ui->tableViewHefe->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        double menge = model->index(r, 1).data().toDouble();
        if (!mAbziehen)
            menge *= -1;
        bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Hefe, name, menge);
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelHefe()->getValueFromSameRow(ModelHefe::ColName, name, ModelHefe::ColMenge));
    }

    model = ui->tableViewWZ->model();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        QString name = model->index(r, 0).data().toString();
        double menge = model->index(r, 1).data().toDouble();
        if (!mAbziehen)
            menge *= -1;
        bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Zusatz, name, menge);
        model->setData(model->index(r, 1), 0.0);
        model->setData(model->index(r, 2), bh->modelWeitereZutaten()->getValueFromSameRow(ModelWeitereZutaten::ColName, name, ModelWeitereZutaten::ColMengeNormiert));
    }

    mAbgezogen = true;
}
