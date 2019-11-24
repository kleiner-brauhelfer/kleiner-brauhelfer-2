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

    ui->frameColor->setToolTip(tr("<strong>Hefetyp<br>"
                                  "<font color=\"%1\">Obergärig</font><br>"
                                  "<font color=\"%2\">Untergärig</font><strong>")
            .arg(gSettings->HefeTypOgUgBackgrounds[1].name())
            .arg(gSettings->HefeTypOgUgBackgrounds[2].name()));

    ui->tbMenge->setErrorOnLimit(true);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgHefeGabe::~WdgHefeGabe()
{
    delete ui;
}

bool WdgHefeGabe::isEnabled() const
{
    return mEnabled;
}

QVariant WdgHefeGabe::data(int col) const
{
    return bh->sud()->modelHefegaben()->data(mIndex, col);
}

bool WdgHefeGabe::setData(int col, const QVariant &value)
{
    return bh->sud()->modelHefegaben()->setData(mIndex, col, value);
}

QString WdgHefeGabe::name() const
{
    return data(ModelHefegaben::ColName).toString();
}

int WdgHefeGabe::menge() const
{
    return data(ModelHefegaben::ColMenge).toInt();
}

void WdgHefeGabe::checkEnabled(bool force)
{
    bool enabled = bh->sud()->getStatus() < Sud_Status_Abgefuellt;
    if (data(ModelHefegaben::ColZugegeben).toBool())
        enabled = false;
    if (gSettings->ForceEnabled)
        enabled = true;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        RohstoffAuswahlProxyModel* model = new RohstoffAuswahlProxyModel(ui->cbZutat);
        model->setSourceModel(bh->modelHefe());
        model->setColumnMenge(ModelHefe::ColMenge);
        model->setIndexMengeBenoetigt(bh->sud()->modelHefegaben()->index(mIndex, ModelHefegaben::ColMenge));
        model->sort(ModelHefe::ColBeschreibung, Qt::AscendingOrder);
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(ModelHefe::ColBeschreibung);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbZutat);
        model->setItem(0, 0, new QStandardItem(name()));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(0);
    }
    ui->cbZutat->setEnabled(enabled);
    ui->cbZutat->setCurrentIndex(-1);
    ui->btnLoeschen->setVisible(enabled);
    ui->tbVorhanden->setVisible(enabled);
    ui->btnAufbrauchen->setVisible(enabled);
    ui->lblVorhanden->setVisible(enabled);
    ui->tbMenge->setReadOnly(!enabled);
    ui->tbTage->setReadOnly(!enabled);
    ui->tbDatum->setReadOnly(!enabled);
}

void WdgHefeGabe::updateValues(bool full)
{
    QString hefename = name();

    checkEnabled(full);

    if (!ui->cbZutat->hasFocus())
    {
        ui->cbZutat->setCurrentIndex(-1);
        ui->cbZutat->setCurrentText(hefename);
    }
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(menge());
    if (!ui->tbTage->hasFocus())
        ui->tbTage->setValue(data(ModelHefegaben::ColZugabeNach).toInt());
    QDateTime braudatum = bh->sud()->getBraudatum();
    if (braudatum.isValid())
    {
        ui->tbDatum->setMinimumDateTime(braudatum);
        if (!ui->tbDatum->hasFocus())
            ui->tbDatum->setDate(data(ModelHefegaben::ColZugabeDatum).toDate());
    }
    ui->tbDatum->setVisible(braudatum.isValid());

    int idx = bh->modelHefe()->getValueFromSameRow(ModelHefe::ColBeschreibung, hefename, ModelHefe::ColTypOGUG).toInt();
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

    ui->btnZugeben->setVisible(mEnabled && bh->sud()->getStatus() == Sud_Status_Gebraut);

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelHefe()->getValueFromSameRow(ModelHefe::ColBeschreibung, hefename, ModelHefe::ColMenge).toInt());
        int benoetigt = 0;
        ProxyModel* model = bh->sud()->modelHefegaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, ModelHefegaben::ColName).toString() == hefename)
                benoetigt += model->data(i, ModelHefegaben::ColMenge).toInt();
        }
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());
        ui->btnAufbrauchen->setVisible(ui->tbMenge->value() != ui->tbVorhanden->value());
    }
}

void WdgHefeGabe::remove()
{
    bh->sud()->modelHefegaben()->removeRow(mIndex);
}

void WdgHefeGabe::on_cbZutat_currentIndexChanged(const QString &text)
{
    if (ui->cbZutat->hasFocus())
        setData(ModelHefegaben::ColName, text);
}

void WdgHefeGabe::on_tbMenge_valueChanged(int value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelHefegaben::ColMenge, value);
}

void WdgHefeGabe::on_tbTage_valueChanged(int value)
{
    if (ui->tbTage->hasFocus())
        setData(ModelHefegaben::ColZugabeNach, value);
}

void WdgHefeGabe::on_tbDatum_dateChanged(const QDate &date)
{
    if (ui->tbDatum->hasFocus())
        setData(ModelHefegaben::ColZugabeDatum, date);
}

void WdgHefeGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatum->date();
    setData(ModelHefegaben::ColZugabeDatum, currentDate < date ? currentDate : date);
    setData(ModelHefegaben::ColZugegeben, true);

    if (QMessageBox::question(this, tr("Zutat vom Bestand abziehen"),
                              tr("Soll die Zutat vom Bestand abgezogen werden?")
       ) == QMessageBox::Yes)
        bh->sud()->zutatAbziehen(name(), 1, data(ModelHefegaben::ColMenge).toDouble());
}

void WdgHefeGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgHefeGabe::on_btnAufbrauchen_clicked()
{
    setData(ModelHefegaben::ColMenge, ui->tbVorhanden->value());
}
