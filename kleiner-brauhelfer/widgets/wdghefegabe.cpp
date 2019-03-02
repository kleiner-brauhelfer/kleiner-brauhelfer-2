#include "wdghefegabe.h"
#include "ui_wdghefegabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "model/rohstoffauswahlproxymodel.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgHefeGabe::WdgHefeGabe(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgHefeGabe),
    mIndex(index),
    mEnabled(true)
{
    ui->setupUi(this);

    ui->frameColor->setToolTip(tr("<b>Typ<br>"
                                  "<font color=\"%1\">Obergärig</font><br>"
                                  "<font color=\"%2\">Untergärig</font></b>")
            .arg(gSettings->HefeTypOgUgBackgrounds[1].name())
            .arg(gSettings->HefeTypOgUgBackgrounds[2].name()));

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    //connect(bh->sud()->modelHefe(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(updateValues()));
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(updateValues()));
}

WdgHefeGabe::~WdgHefeGabe()
{
    delete ui;
}

bool WdgHefeGabe::isEnabled() const
{
    return mEnabled;
}

QVariant WdgHefeGabe::data(const QString &fieldName) const
{
    // TODO
    if (fieldName == "Name")
        return bh->sud()->getAuswahlHefe();
    if (fieldName == "Menge")
        return bh->sud()->getHefeAnzahlEinheiten();
    return QVariant();
    //return bh->sud()->modelHefeGaben()->data(mIndex, fieldName);
}

bool WdgHefeGabe::setData(const QString &fieldName, const QVariant &value)
{
    // TODO
    if (fieldName == "Name")
        bh->sud()->setAuswahlHefe(value.toString());
    if (fieldName == "Menge")
        bh->sud()->setHefeAnzahlEinheiten(value.toInt());
    return true;
    //return bh->sud()->modelHefeGaben()->setData(mIndex, fieldName, value);
}

void WdgHefeGabe::checkEnabled(bool force)
{
    bool enabled = !bh->sud()->getBierWurdeGebraut();//!bh->sud()->getBierWurdeAbgefuellt();
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        RohstoffAuswahlProxyModel* model = new RohstoffAuswahlProxyModel(ui->cbZutat);
        model->setSourceModel(bh->modelHefe());
        model->setColumnMenge(bh->modelHefe()->fieldIndex("Menge"));
        // TODO
        //model->setIndexMengeBenoetigt(bh->sud()->modelHefeGaben()->index(mIndex, bh->sud()->modelHefeGaben()->fieldIndex("Menge")));
        model->setIndexMengeBenoetigt(bh->modelSud()->index(bh->sud()->row(), bh->modelSud()->fieldIndex("HefeAnzahlEinheiten")));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(bh->modelHefe()->fieldIndex("Beschreibung"));
        ui->cbZutat->setEnabled(true);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(true);
        ui->tbVorhanden->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbTage->setReadOnly(false);
        ui->tbDatum->setReadOnly(false);

        if (!data("Zugegeben").toBool())
        {
            QDate currentDate = QDate::currentDate();
            QDate dateVon = data("Zeitpunkt_von").toDate();
            ui->tbDatum->setDate(currentDate > dateVon ? currentDate : dateVon);
        }
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbZutat);
        model->setItem(0, 0, new QStandardItem(data("Name").toString()));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(0);
        ui->cbZutat->setEnabled(false);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->tbMenge->setReadOnly(true);
        ui->tbTage->setReadOnly(true);
        ui->tbDatum->setReadOnly(true);
    }

    ui->tbDatum->setDate(data("ZugabeDatum").toDate());
}

void WdgHefeGabe::updateValues()
{
    QString name = data("Name").toString();

    checkEnabled(false);

    if (!ui->cbZutat->hasFocus())
    {
        ui->cbZutat->setCurrentIndex(-1);
        ui->cbZutat->setCurrentText(name);
    }
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data("Menge").toInt());

    int idx = bh->modelHefe()->getValueFromSameRow("Beschreibung", name, "TypOGUG").toInt();
    if (idx >= 0 && idx < gSettings->HefeTypOgUgBackgrounds.count())
    {
        QPalette pal = ui->frameColor->palette();
        pal.setColor(QPalette::Background, gSettings->HefeTypOgUgBackgrounds[idx]);
        ui->frameColor->setPalette(pal);
    }
    else
    {
        ui->frameColor->setPalette(gSettings->palette);
    }

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelHefe()->getValueFromSameRow("Beschreibung", name, "Menge").toInt());
        int benoetigt = bh->sud()->getHefeAnzahlEinheiten();
        /*
        ProxyModel* model = bh->sud()->modelHefe();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, "Name").toString() == name)
                benoetigt += model->data(i, "erg_Menge").toDouble();
        }
        */
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());

        if (data("Zugegeben").toBool())
        {
            ui->btnZugeben->setVisible(false);
            ui->tbDatum->setReadOnly(true);
        }
        else
        {
            ui->btnZugeben->setVisible(bh->sud()->getBierWurdeGebraut());
            ui->tbDatum->setReadOnly(false);
        }
    }
}

void WdgHefeGabe::on_cbZutat_currentIndexChanged(const QString &text)
{
    if (ui->cbZutat->hasFocus())
        setData("Name", text);
}

void WdgHefeGabe::on_tbMenge_valueChanged(int value)
{
    if (ui->tbMenge->hasFocus())
        setData("Menge", value);
}

void WdgHefeGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatum->date();
    ui->tbDatum->setDate(currentDate < date ? currentDate : date);
    setData("ZugabeDatum", ui->tbDatum->date());
    setData("Zugegeben", true);
}

void WdgHefeGabe::on_btnLoeschen_clicked()
{
    // TODO
    //bh->sud()->modelHefeGaben()->removeRow(mIndex);
}
