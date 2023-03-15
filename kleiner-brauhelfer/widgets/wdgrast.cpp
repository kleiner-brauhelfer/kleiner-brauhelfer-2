#include "wdgrast.h"
#include "ui_wdgrast.h"
#include <QLineEdit>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

const QList<WdgRast::Rast> WdgRast::rasten = {
    {tr("Gummirast (Glukanaserast)"),39,20},
    {tr("Maltaserast"),45,30},
    {tr("Weizenrast (Ferulsäurerast)"),45,15},
    {tr("Eiweissrast (Proteaserast)"),54,10},
    {tr("Maltoserast (1. Verzuckerung)"),63,35},
    {tr("Zwischenrast"),67,15},
    {tr("Verzuckerungsrast (2. Verzuckerung)"),71,20},
    {tr("Kombirast"),67,60},
    {tr("Abmaischen"),78,1}
};

WdgRast::WdgRast(int row, QLayout* parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelRasten(), row, parentLayout, parent),
    ui(new Ui::WdgRast),
    mEnabled(true),
    mRastNameManuallyEdited(false)
{
    ui->setupUi(this);
    if (gSettings->theme() == Settings::Theme::Dark)
    {
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QString(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorRast);
    setPalette(pal);
    ui->lblWarnung->setPalette(gSettings->paletteErrorLabel);

    ui->cbRast->setCompleter(nullptr);
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(ui->cbRast->lineEdit(), SIGNAL(textEdited(QString)), this, SLOT(cbRastTextEdited()));

    updateValues();
    updateListe();
    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgRast::~WdgRast()
{
	delete ui;
}

bool WdgRast::isEnabled() const
{
    return mEnabled;
}

QString WdgRast::name() const
{
    return data(ModelRasten::ColName).toString();
}

void WdgRast::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    mEnabled = status == Brauhelfer::SudStatus::Rezept;
    if (gSettings->ForceEnabled)
        mEnabled = true;
}

void WdgRast::updateListe()
{
    ui->cbRast->clear();
    if (mEnabled)
    {
        switch (static_cast<Brauhelfer::RastTyp>(data(ModelRasten::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            ui->cbRast->addItem(tr("Einmaischen"));
            break;
        case Brauhelfer::RastTyp::Temperatur:
            for (const Rast &rast : rasten)
                ui->cbRast->addItem(tr(rast.name.toStdString().c_str()));
            break;
        case Brauhelfer::RastTyp::Infusion:
            ui->cbRast->addItem(tr("Kochendes Wasser"));
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            ui->cbRast->addItem(tr("1/2 Dickmaische"));
            ui->cbRast->addItem(tr("1/3 Dickmaische"));
            ui->cbRast->addItem(tr("1/3 Dünnmaische"));
            ui->cbRast->addItem(tr("1/3 Läutermaische"));
            break;
        }
        if (name().isEmpty())
        {
            setData(ModelRasten::ColName, ui->cbRast->itemText(0));
            updateValuesFromListe(0);
        }
    }
    else
    {
        ui->cbRast->addItem(name());
    }
}

void WdgRast::updateValuesFromListe(int index)
{
    switch (static_cast<Brauhelfer::RastTyp>(data(ModelRasten::ColTyp).toInt()))
    {
    case Brauhelfer::RastTyp::Einmaischen:
        setData(ModelRasten::ColMengenfaktor, 1.0);
        setData(ModelRasten::ColTemp, 57);
        setData(ModelRasten::ColDauer, 5);
        setData(ModelRasten::ColParam3, 18);
        break;
    case Brauhelfer::RastTyp::Temperatur:
        setData(ModelRasten::ColMengenfaktor, 1.0);
        if (index >= 0 && index < rasten.count())
        {
            setData(ModelRasten::ColTemp, rasten[index].temperatur);
            setData(ModelRasten::ColDauer, rasten[index].dauer);
        }
        break;
    case Brauhelfer::RastTyp::Infusion:
        setData(ModelRasten::ColMengenfaktor, 1.0/3);
        setData(ModelRasten::ColParam1, 95);
        setData(ModelRasten::ColDauer, 15);
        break;
    case Brauhelfer::RastTyp::Dekoktion:
        setData(ModelRasten::ColDauer, 15);
        setData(ModelRasten::ColParam1, 95);
        setData(ModelRasten::ColParam2, 15);
        setData(ModelRasten::ColParam3, 0);
        setData(ModelRasten::ColParam4, 0);
        switch (index)
        {
        case 0: // 1/2 Dickmaische
            setData(ModelRasten::ColMengenfaktor, 1.0/2);
            break;
        case 1: // 1/3 Dickmaische
            setData(ModelRasten::ColMengenfaktor, 1.0/3);
            break;
        case 2: // 1/3 Dünnmaische
            setData(ModelRasten::ColMengenfaktor, 1.0/3);
            break;
        case 3: // 1/3 Läutermaische
            setData(ModelRasten::ColMengenfaktor, 1.0/3);
            break;
        }
        break;
    }
}

void WdgRast::updateValues()
{
    checkEnabled();

    ui->cbTyp->setEnabled(mEnabled);
    ui->cbRast->setEditable(mEnabled);
    ui->cbRast->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbMengeEinmaischen->setReadOnly(!mEnabled);
    ui->tbVerhaeltnisEinmaischen->setReadOnly(!mEnabled);
    ui->tbTempEinmaischen->setReadOnly(!mEnabled);
    ui->tbDauerEinmaischen->setReadOnly(!mEnabled);
    ui->tbMalzTempEinmaischen->setReadOnly(!mEnabled);
    ui->tbTempRast->setReadOnly(!mEnabled);
    ui->tbDauerRast->setReadOnly(!mEnabled);
    ui->tbMengeInfusion->setReadOnly(!mEnabled);
    ui->tbVerhaeltnisInfusion->setReadOnly(!mEnabled);
    ui->tbTempInfusion->setReadOnly(!mEnabled);
    ui->tbWassertempInfusion->setReadOnly(!mEnabled);
    ui->tbDauerInfusion->setReadOnly(!mEnabled);
    ui->tbMengeDekoktion->setReadOnly(!mEnabled);
    ui->tbVerhaeltnisDekoktion->setReadOnly(!mEnabled);
    ui->tbRasttempDekoktion->setReadOnly(!mEnabled);
    ui->tbRastdauerDekoktion->setReadOnly(!mEnabled);
    ui->tbKochdauerDekoktion->setReadOnly(!mEnabled);
    ui->tbTeiltempDekoktion->setReadOnly(!mEnabled);
    ui->tbTempDekoktion->setReadOnly(!mEnabled);
    ui->tbDauerDekoktion->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    Brauhelfer::RastTyp typ = static_cast<Brauhelfer::RastTyp>(data(ModelRasten::ColTyp).toInt());
    ui->wdgEinmaischen->setVisible(typ == Brauhelfer::RastTyp::Einmaischen);
    ui->wdgRast->setVisible(typ == Brauhelfer::RastTyp::Temperatur);
    ui->wdgInfusion->setVisible(typ == Brauhelfer::RastTyp::Infusion);
    ui->wdgDekoktion->setVisible(typ == Brauhelfer::RastTyp::Dekoktion);
    if (!ui->cbRast->hasFocus())
        ui->cbRast->setCurrentText(name());
    if (!ui->cbTyp->hasFocus())
        ui->cbTyp->setCurrentIndex(data(ModelRasten::ColTyp).toInt());
    if (!ui->tbMengeEinmaischen->hasFocus())
        ui->tbMengeEinmaischen->setValue(data(ModelRasten::ColMenge).toDouble());
    if (!ui->tbVerhaeltnisEinmaischen->hasFocus())
        ui->tbVerhaeltnisEinmaischen->setValue(data(ModelRasten::ColMengenfaktor).toDouble()*100);
    ui->tbWasserTempEinmaischen->setValue(data(ModelRasten::ColParam1).toDouble());
    if (!ui->tbMalzTempEinmaischen->hasFocus())
        ui->tbMalzTempEinmaischen->setValue(data(ModelRasten::ColParam3).toDouble());
    if (!ui->tbTempEinmaischen->hasFocus())
        ui->tbTempEinmaischen->setValue(data(ModelRasten::ColTemp).toDouble());
    if (!ui->tbDauerEinmaischen->hasFocus())
        ui->tbDauerEinmaischen->setValue(data(ModelRasten::ColDauer).toInt());
    if (!ui->tbTempRast->hasFocus())
        ui->tbTempRast->setValue(data(ModelRasten::ColTemp).toDouble());
    if (!ui->tbDauerRast->hasFocus())
        ui->tbDauerRast->setValue(data(ModelRasten::ColDauer).toInt());
    if (!ui->tbMengeInfusion->hasFocus())
        ui->tbMengeInfusion->setValue(data(ModelRasten::ColMenge).toDouble());
    if (!ui->tbVerhaeltnisInfusion->hasFocus())
        ui->tbVerhaeltnisInfusion->setValue(data(ModelRasten::ColMengenfaktor).toDouble()*100);
    if (!ui->tbTempInfusion->hasFocus())
        ui->tbTempInfusion->setValue(data(ModelRasten::ColTemp).toDouble());
    if (!ui->tbWassertempInfusion->hasFocus())
        ui->tbWassertempInfusion->setValue(data(ModelRasten::ColParam1).toDouble());
    if (!ui->tbDauerInfusion->hasFocus())
        ui->tbDauerInfusion->setValue(data(ModelRasten::ColDauer).toInt());
    if (!ui->tbMengeDekoktion->hasFocus())
        ui->tbMengeDekoktion->setValue(data(ModelRasten::ColMenge).toDouble());
    if (!ui->tbVerhaeltnisDekoktion->hasFocus())
        ui->tbVerhaeltnisDekoktion->setValue(data(ModelRasten::ColMengenfaktor).toDouble()*100);
    if (!ui->tbRasttempDekoktion->hasFocus())
        ui->tbRasttempDekoktion->setValue(data(ModelRasten::ColParam3).toDouble());
    if (!ui->tbRastdauerDekoktion->hasFocus())
        ui->tbRastdauerDekoktion->setValue(data(ModelRasten::ColParam4).toInt());
    if (!ui->tbKochdauerDekoktion->hasFocus())
        ui->tbKochdauerDekoktion->setValue(data(ModelRasten::ColParam2).toInt());
    if (!ui->tbTeiltempDekoktion->hasFocus())
        ui->tbTeiltempDekoktion->setValue(data(ModelRasten::ColParam1).toDouble());
    if (!ui->tbTempDekoktion->hasFocus())
        ui->tbTempDekoktion->setValue(data(ModelRasten::ColTemp).toDouble());
    if (!ui->tbDauerDekoktion->hasFocus())
        ui->tbDauerDekoktion->setValue(data(ModelRasten::ColDauer).toInt());
    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);

    if (mRow == 0 && typ != Brauhelfer::RastTyp::Einmaischen)
    {
        ui->lblWarnung->setText(tr("Der erste Schritt sollte \"Einmaischen\" sein."));
        ui->lblWarnung->setVisible(true);
    }
    else if (mRow > 0 && typ == Brauhelfer::RastTyp::Einmaischen)
    {
        ui->lblWarnung->setText(tr("Nur der erste Schritt sollte \"Einmaischen\" sein."));
        ui->lblWarnung->setVisible(true);
    }
    else
    {
        ui->lblWarnung->setVisible(false);
    }
}

void WdgRast::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now == ui->cbRast)
        mRastNameManuallyEdited = false;
}

void WdgRast::cbRastTextEdited()
{
    mRastNameManuallyEdited = true;
}

void WdgRast::on_cbRast_currentTextChanged(const QString &text)
{
    if (ui->cbRast->hasFocus())
        setData(ModelRasten::ColName, text);
}

void WdgRast::on_cbRast_currentIndexChanged(int index)
{
    if (ui->cbRast->hasFocus() && !mRastNameManuallyEdited)
        updateValuesFromListe(index);
}

void WdgRast::on_cbTyp_currentIndexChanged(int index)
{
    if (ui->cbTyp->hasFocus())
    {
        setData(ModelRasten::ColTyp, index);
        updateListe();
        setData(ModelRasten::ColName, ui->cbRast->itemText(0));
        updateValuesFromListe(0);
    }
}

void WdgRast::on_tbMengeEinmaischen_valueChanged(double value)
{
    if (ui->tbMengeEinmaischen->hasFocus())
        setData(ModelRasten::ColMenge, value);
}

void WdgRast::on_tbVerhaeltnisEinmaischen_valueChanged(double value)
{
    if (ui->tbVerhaeltnisEinmaischen->hasFocus())
        setData(ModelRasten::ColMengenfaktor, value/100);
}

void WdgRast::on_tbMalzTempEinmaischen_valueChanged(double value)
{
    if (ui->tbMalzTempEinmaischen->hasFocus())
        setData(ModelRasten::ColParam3, value);
}

void WdgRast::on_tbTempEinmaischen_valueChanged(double value)
{
    if (ui->tbTempEinmaischen->hasFocus())
        setData(ModelRasten::ColTemp, value);
}

void WdgRast::on_tbDauerEinmaischen_valueChanged(int value)
{
    if (ui->tbDauerEinmaischen->hasFocus())
        setData(ModelRasten::ColDauer, value);
}

void WdgRast::on_tbTempRast_valueChanged(double value)
{
    if (ui->tbTempRast->hasFocus())
        setData(ModelRasten::ColTemp, value);
}

void WdgRast::on_tbDauerRast_valueChanged(int value)
{
    if (ui->tbDauerRast->hasFocus())
        setData(ModelRasten::ColDauer, value);
}

void WdgRast::on_tbMengeInfusion_valueChanged(double value)
{
    if (ui->tbMengeInfusion->hasFocus())
        setData(ModelRasten::ColMenge, value);
}

void WdgRast::on_tbVerhaeltnisInfusion_valueChanged(double value)
{
    if (ui->tbVerhaeltnisInfusion->hasFocus())
        setData(ModelRasten::ColMengenfaktor, value/100);
}

void WdgRast::on_tbTempInfusion_valueChanged(double value)
{
    if (ui->tbTempInfusion->hasFocus())
        setData(ModelRasten::ColTemp, value);
}

void WdgRast::on_tbWassertempInfusion_valueChanged(double value)
{
    if (ui->tbWassertempInfusion->hasFocus())
        setData(ModelRasten::ColParam1, value);
}

void WdgRast::on_tbDauerInfusion_valueChanged(int value)
{
    if (ui->tbDauerInfusion->hasFocus())
        setData(ModelRasten::ColDauer, value);
}

void WdgRast::on_tbMengeDekoktion_valueChanged(double value)
{
    if (ui->tbMengeDekoktion->hasFocus())
        setData(ModelRasten::ColMenge, value);
}

void WdgRast::on_tbVerhaeltnisDekoktion_valueChanged(double value)
{
    if (ui->tbVerhaeltnisDekoktion->hasFocus())
        setData(ModelRasten::ColMengenfaktor, value/100);
}

void WdgRast::on_tbRasttempDekoktion_valueChanged(double value)
{
    if (ui->tbRasttempDekoktion->hasFocus())
        setData(ModelRasten::ColParam3, value);
}

void WdgRast::on_tbRastdauerDekoktion_valueChanged(int value)
{
    if (ui->tbRastdauerDekoktion->hasFocus())
        setData(ModelRasten::ColParam4, value);
}

void WdgRast::on_tbKochdauerDekoktion_valueChanged(int value)
{
    if (ui->tbKochdauerDekoktion->hasFocus())
        setData(ModelRasten::ColParam2, value);
}

void WdgRast::on_tbTeiltempDekoktion_valueChanged(double value)
{
    if (ui->tbTeiltempDekoktion->hasFocus())
        setData(ModelRasten::ColParam1, value);
}

void WdgRast::on_tbTempDekoktion_valueChanged(double value)
{
    if (ui->tbTempDekoktion->hasFocus())
        setData(ModelRasten::ColTemp, value);
}

void WdgRast::on_tbDauerDekoktion_valueChanged(int value)
{
    if (ui->tbDauerDekoktion->hasFocus())
        setData(ModelRasten::ColDauer, value);
}

void WdgRast::on_btnLoeschen_clicked()
{
    remove();
}

void WdgRast::on_btnNachOben_clicked()
{
    if (moveUp())
        bh->modelRasten()->update(bh->sud()->id());
}

void WdgRast::on_btnNachUnten_clicked()
{
    if (moveDown())
        bh->modelRasten()->update(bh->sud()->id());
}
