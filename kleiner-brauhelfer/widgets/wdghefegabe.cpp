#include "wdghefegabe.h"
#include "ui_wdghefegabe.h"
#include <QStandardItemModel>
#include <QMessageBox>
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
    connect(bh->sud()->modelHefegaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(updateValues()));
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
    return bh->sud()->modelHefegaben()->data(mIndex, fieldName);
}

bool WdgHefeGabe::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelHefegaben()->setData(mIndex, fieldName, value);
}

void WdgHefeGabe::checkEnabled(bool force)
{
    bool enabled = !bh->sud()->getBierWurdeAbgefuellt();
    if (data("Zugegeben").toBool())
        enabled = false;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        RohstoffAuswahlProxyModel* model = new RohstoffAuswahlProxyModel(ui->cbZutat);
        model->setSourceModel(bh->modelHefe());
        model->setColumnMenge(bh->modelHefe()->fieldIndex("Menge"));
        model->setIndexMengeBenoetigt(bh->sud()->modelHefegaben()->index(mIndex, bh->sud()->modelHefegaben()->fieldIndex("Menge")));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(bh->modelHefe()->fieldIndex("Beschreibung"));
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbZutat);
        model->setItem(0, 0, new QStandardItem(data("Name").toString()));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(0);
    }
    ui->cbZutat->setEnabled(enabled);
    ui->cbZutat->setCurrentIndex(-1);
    ui->btnLoeschen->setVisible(enabled);
    ui->tbVorhanden->setVisible(enabled);
    ui->lblVorhanden->setVisible(enabled);
    ui->tbMenge->setReadOnly(!enabled);
    ui->tbTage->setReadOnly(!enabled);
    ui->tbDatum->setReadOnly(!enabled);
    ui->btnZugeben->setVisible(enabled);
}

void WdgHefeGabe::updateValues()
{
    QString name = data("Name").toString();

    checkEnabled();

    if (!ui->cbZutat->hasFocus())
    {
        ui->cbZutat->setCurrentIndex(-1);
        ui->cbZutat->setCurrentText(name);
    }
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data("Menge").toInt());
    if (!ui->tbTage->hasFocus())
        ui->tbTage->setValue(data("ZugegebenNach").toInt());
    ui->tbDatum->setMinimumDateTime(bh->sud()->getBraudatum());
    if (!ui->tbDatum->hasFocus())
        ui->tbDatum->setDateTime(bh->sud()->getBraudatum().addDays(ui->tbTage->value()));

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
        int benoetigt = 0;
        ProxyModel* model = bh->sud()->modelHefegaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, "Name").toString() == name)
                benoetigt += model->data(i, "Menge").toInt();
        }
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());
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

void WdgHefeGabe::on_tbTage_valueChanged(int value)
{
    if (ui->tbTage->hasFocus())
        setData("ZugegebenNach", value);
}

void WdgHefeGabe::on_tbDatum_dateTimeChanged(const QDateTime &dateTime)
{
    if (ui->tbDatum->hasFocus())
        setData("ZugegebenNach", bh->sud()->getBraudatum().daysTo(dateTime));
}

void WdgHefeGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatum->date();
    ui->tbDatum->setDate(currentDate < date ? currentDate : date);
    setData("Zugegeben", true);

    if (QMessageBox::question(this, tr("Zutat vom Bestand abziehen"),
                              tr("Soll die Zutat vom Bestand abgezogen werden?")
       ) == QMessageBox::Yes)
        bh->sud()->zutatAbziehen(data("Name").toString(), 1, data("Menge").toDouble());
}

void WdgHefeGabe::on_btnLoeschen_clicked()
{
    bh->sud()->modelHefegaben()->removeRow(mIndex);
}
