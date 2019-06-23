#include "wdgmalzgabe.h"
#include "ui_wdgmalzgabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "model/rohstoffauswahlproxymodel.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgMalzGabe::WdgMalzGabe(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgMalzGabe),
    mIndex(index),
    mEnabled(true)
{
    ui->setupUi(this);
    ui->btnKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnKorrektur->setVisible(false);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgMalzGabe::~WdgMalzGabe()
{
    delete ui;
}

bool WdgMalzGabe::isEnabled() const
{
    return mEnabled;
}

QVariant WdgMalzGabe::data(const QString &fieldName) const
{
    return bh->sud()->modelMalzschuettung()->data(mIndex, fieldName);
}

bool WdgMalzGabe::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelMalzschuettung()->setData(mIndex, fieldName, value);
}

void WdgMalzGabe::checkEnabled(bool force)
{
    bool enabled = bh->sud()->getStatus() == Sud_Status_Rezept;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        RohstoffAuswahlProxyModel* model = new RohstoffAuswahlProxyModel(ui->cbZutat);
        model->setSourceModel(bh->modelMalz());
        model->setColumnMenge(bh->modelMalz()->fieldIndex("Menge"));
        model->setIndexMengeBenoetigt(bh->sud()->modelMalzschuettung()->index(mIndex, bh->sud()->modelMalzschuettung()->fieldIndex("erg_Menge")));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(bh->modelMalz()->fieldIndex("Beschreibung"));
        ui->cbZutat->setEnabled(true);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(true);
        ui->tbVorhanden->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->lblEinheit->setVisible(true);        
        ui->tbMengeProzent->setReadOnly(false);
        ui->tbMenge->setReadOnly(false);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbZutat);
        model->setItem(0, 0, new QStandardItem(name()));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(0);
        ui->cbZutat->setEnabled(false);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->lblEinheit->setVisible(false);
        ui->tbMengeProzent->setReadOnly(true);
        ui->tbMenge->setReadOnly(true);
        ui->btnKorrektur->setVisible(false);
        ui->lblWarnAnteil->setVisible(false);
    }
}

void WdgMalzGabe::updateValues(bool full)
{
    QString malzname = name();

    checkEnabled(full);

    if (!ui->cbZutat->hasFocus())
    {
        ui->cbZutat->setCurrentIndex(-1);
        ui->cbZutat->setCurrentText(malzname);
    }
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data("Prozent").toDouble());
    if (!ui->tbMenge->hasFocus())
    {
        ui->tbMenge->setMaximum(bh->sud()->geterg_S_Gesamt());
        ui->tbMenge->setValue(data("erg_Menge").toDouble());
    }

    QPalette pal = ui->frameColor->palette();
    pal.setColor(QPalette::Background, BierCalc::ebcToColor(data("Farbe").toDouble()));
    ui->frameColor->setPalette(pal);

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelMalz()->getValueFromSameRow("Beschreibung", malzname, "Menge").toDouble());
        double benoetigt = 0.0;
        ProxyModel* model = bh->sud()->modelMalzschuettung();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, "Name").toString() == malzname)
                benoetigt += model->data(i, "erg_Menge").toDouble();
        }
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());
        ui->tbMengeProzent->setError( ui->tbMengeProzent->value() == 0.0);
        ui->tbMenge->setError(ui->tbMenge->value() == 0.0);

        int max = bh->modelMalz()->getValueFromSameRow("Beschreibung", malzname, "MaxProzent").toInt();
        if (ui->tbMengeProzent->value() > max)
        {
            ui->lblWarnAnteil->setVisible(true);
            ui->lblWarnAnteil->setToolTip(tr("Maximal empfohlener Schüttungsanteil (%1%) wurde überschritten.").arg(max));
        }
        else
        {
            ui->lblWarnAnteil->setVisible(false);
        }
    }
}

void WdgMalzGabe::on_cbZutat_currentIndexChanged(const QString &text)
{
    if (ui->cbZutat->hasFocus())
        setData("Name", text);
}

void WdgMalzGabe::on_tbMengeProzent_valueChanged(double value)
{
    if (ui->tbMengeProzent->hasFocus())
        setData("Prozent", value);
}

void WdgMalzGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        setData("erg_Menge", value);
}

QString WdgMalzGabe::name() const
{
    return data("Name").toString();
}

double WdgMalzGabe::prozent() const
{
    return data("Prozent").toDouble();
}

void WdgMalzGabe::setFehlProzent(double value)
{
    if (mEnabled)
    {
        if (value < 0.0)
        {
            double p = prozent();
            if (p == 0.0)
                value = 0.0;
            else if (value < -p)
                value = -p;
        }
        QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 1) + " %";
        ui->btnKorrektur->setText(text);
        ui->btnKorrektur->setProperty("toadd", value);
        ui->btnKorrektur->setVisible(value != 0.0);
    }
    else
    {
        ui->btnKorrektur->setVisible(false);
    }
}

void WdgMalzGabe::remove()
{
    bh->sud()->modelMalzschuettung()->removeRow(mIndex);
}

void WdgMalzGabe::on_btnKorrektur_clicked()
{
    double toadd = ui->btnKorrektur->property("toadd").toDouble();
    setData("Prozent", prozent() + toadd);
}

void WdgMalzGabe::on_btnLoeschen_clicked()
{
    remove();
}
