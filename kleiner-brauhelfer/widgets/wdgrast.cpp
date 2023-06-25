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
    WdgAbstractProxy(bh->sud()->modelMaischplan(), row, parentLayout, parent),
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
                icon.addFile(QStringLiteral(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorRast);
    setPalette(pal);
    ui->lblWarnung->setPalette(gSettings->paletteErrorLabel);

    ui->cbRast->setCompleter(nullptr);
    connect(qApp, &QApplication::focusChanged, this, &WdgRast::focusChanged);
    connect(ui->cbRast->lineEdit(), &QLineEdit::textEdited, this, &WdgRast::cbRastTextEdited);

    updateValues();
    updateListe();
    connect(bh, &Brauhelfer::modified, this, &WdgRast::updateValues);
}

WdgRast::~WdgRast()
{
    disconnect(qApp, &QApplication::focusChanged, this, &WdgRast::focusChanged);
	delete ui;
}

bool WdgRast::isEnabled() const
{
    return mEnabled;
}

QString WdgRast::name() const
{
    return data(ModelMaischplan::ColName).toString();
}

double WdgRast::prozentWasser() const
{
    return 100 * data(ModelMaischplan::ColAnteilWasser).toDouble();
}

void WdgRast::setFehlProzentWasser(double value)
{
    bool error = value != 0.0;
    ui->tbVerhaeltnisEinmaischen->setError(error);
    ui->tbVerhaeltnisInfusion->setError(error);
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
        switch (static_cast<Brauhelfer::RastTyp>(data(ModelMaischplan::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            ui->cbRast->addItem(tr("Einmaischen"));
            break;
        case Brauhelfer::RastTyp::Aufheizen:
            for (const Rast &rast : rasten)
                ui->cbRast->addItem(tr(rast.name.toStdString().c_str()));
            break;
        case Brauhelfer::RastTyp::Zubruehen:
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
            setData(ModelMaischplan::ColName, ui->cbRast->itemText(0));
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
    switch (static_cast<Brauhelfer::RastTyp>(data(ModelMaischplan::ColTyp).toInt()))
    {
    case Brauhelfer::RastTyp::Einmaischen:
        setData(ModelMaischplan::ColAnteilMalz, 100);
        setData(ModelMaischplan::ColTempMaische, 57);
        setData(ModelMaischplan::ColDauerMaische, 5);
        setData(ModelMaischplan::ColTempMalz, 18);
        break;
    case Brauhelfer::RastTyp::Aufheizen:
        if (index >= 0 && index < rasten.count())
        {
            setData(ModelMaischplan::ColTempMaische, rasten[index].temperatur);
            setData(ModelMaischplan::ColDauerMaische, rasten[index].dauer);
        }
        break;
    case Brauhelfer::RastTyp::Zubruehen:
        setData(ModelMaischplan::ColAnteilWasser, 33.33);
        setData(ModelMaischplan::ColTempWasser, 95);
        setData(ModelMaischplan::ColDauerMaische, 15);
        break;
    case Brauhelfer::RastTyp::Dekoktion:
        setData(ModelMaischplan::ColDauerMaische, 15);
        setData(ModelMaischplan::ColTempExtra1, 95);
        setData(ModelMaischplan::ColDauerExtra1, 15);
        setData(ModelMaischplan::ColTempExtra2, 0);
        setData(ModelMaischplan::ColDauerExtra2, 0);
        switch (index)
        {
        case 0: // 1/2 Dickmaische
            setData(ModelMaischplan::ColAnteilMaische, 50);
            break;
        case 1: // 1/3 Dickmaische
            setData(ModelMaischplan::ColAnteilMaische, 33.33);
            break;
        case 2: // 1/3 Dünnmaische
            setData(ModelMaischplan::ColAnteilMaische, 33.33);
            break;
        case 3: // 1/3 Läutermaische
            setData(ModelMaischplan::ColAnteilMaische, 33.33);
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

    Brauhelfer::RastTyp typ = static_cast<Brauhelfer::RastTyp>(data(ModelMaischplan::ColTyp).toInt());
    ui->wdgEinmaischen->setVisible(typ == Brauhelfer::RastTyp::Einmaischen);
    ui->wdgRast->setVisible(typ == Brauhelfer::RastTyp::Aufheizen);
    ui->wdgInfusion->setVisible(typ == Brauhelfer::RastTyp::Zubruehen);
    ui->wdgDekoktion->setVisible(typ == Brauhelfer::RastTyp::Dekoktion);
    if (!ui->cbRast->hasFocus())
        ui->cbRast->setCurrentText(name());
    if (!ui->cbTyp->hasFocus())
        ui->cbTyp->setCurrentIndex(data(ModelMaischplan::ColTyp).toInt());
    if (!ui->tbMengeEinmaischen->hasFocus())
        ui->tbMengeEinmaischen->setValue(data(ModelMaischplan::ColMengeWasser).toDouble());
    if (!ui->tbVerhaeltnisEinmaischen->hasFocus())
        ui->tbVerhaeltnisEinmaischen->setValue(data(ModelMaischplan::ColAnteilWasser).toDouble());
    ui->tbWasserTempEinmaischen->setValue(data(ModelMaischplan::ColTempWasser).toDouble());
    if (!ui->tbMalzTempEinmaischen->hasFocus())
        ui->tbMalzTempEinmaischen->setValue(data(ModelMaischplan::ColTempMalz).toDouble());
    if (!ui->tbTempEinmaischen->hasFocus())
        ui->tbTempEinmaischen->setValue(data(ModelMaischplan::ColTempMaische).toDouble());
    if (!ui->tbDauerEinmaischen->hasFocus())
        ui->tbDauerEinmaischen->setValue(data(ModelMaischplan::ColDauerMaische).toInt());
    if (!ui->tbTempRast->hasFocus())
        ui->tbTempRast->setValue(data(ModelMaischplan::ColTempMaische).toDouble());
    if (!ui->tbDauerRast->hasFocus())
        ui->tbDauerRast->setValue(data(ModelMaischplan::ColDauerMaische).toInt());
    if (!ui->tbMengeInfusion->hasFocus())
        ui->tbMengeInfusion->setValue(data(ModelMaischplan::ColMengeWasser).toDouble());
    if (!ui->tbVerhaeltnisInfusion->hasFocus())
        ui->tbVerhaeltnisInfusion->setValue(data(ModelMaischplan::ColAnteilWasser).toDouble());
    if (!ui->tbTempInfusion->hasFocus())
        ui->tbTempInfusion->setValue(data(ModelMaischplan::ColTempMaische).toDouble());
    if (!ui->tbWassertempInfusion->hasFocus())
        ui->tbWassertempInfusion->setValue(data(ModelMaischplan::ColTempWasser).toDouble());
    if (!ui->tbDauerInfusion->hasFocus())
        ui->tbDauerInfusion->setValue(data(ModelMaischplan::ColDauerMaische).toInt());
    if (!ui->tbMengeDekoktion->hasFocus())
        ui->tbMengeDekoktion->setValue(data(ModelMaischplan::ColMengeMaische).toDouble());
    if (!ui->tbVerhaeltnisDekoktion->hasFocus())
        ui->tbVerhaeltnisDekoktion->setValue(data(ModelMaischplan::ColAnteilMaische).toDouble());
    if (!ui->tbRasttempDekoktion->hasFocus())
        ui->tbRasttempDekoktion->setValue(data(ModelMaischplan::ColTempExtra2).toDouble());
    if (!ui->tbRastdauerDekoktion->hasFocus())
        ui->tbRastdauerDekoktion->setValue(data(ModelMaischplan::ColDauerExtra2).toInt());
    if (!ui->tbKochdauerDekoktion->hasFocus())
        ui->tbKochdauerDekoktion->setValue(data(ModelMaischplan::ColDauerExtra1).toInt());
    if (!ui->tbTeiltempDekoktion->hasFocus())
        ui->tbTeiltempDekoktion->setValue(data(ModelMaischplan::ColTempExtra1).toDouble());
    if (!ui->tbTempDekoktion->hasFocus())
        ui->tbTempDekoktion->setValue(data(ModelMaischplan::ColTempMaische).toDouble());
    if (!ui->tbDauerDekoktion->hasFocus())
        ui->tbDauerDekoktion->setValue(data(ModelMaischplan::ColDauerMaische).toInt());
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
        setData(ModelMaischplan::ColName, text);
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
        setData(ModelMaischplan::ColTyp, index);
        updateListe();
        setData(ModelMaischplan::ColName, ui->cbRast->itemText(0));
        updateValuesFromListe(0);
    }
}

void WdgRast::on_tbMengeEinmaischen_valueChanged(double value)
{
    if (ui->tbMengeEinmaischen->hasFocus())
        setData(ModelMaischplan::ColMengeWasser, value);
}

void WdgRast::on_tbVerhaeltnisEinmaischen_valueChanged(double value)
{
    if (ui->tbVerhaeltnisEinmaischen->hasFocus())
        setData(ModelMaischplan::ColAnteilWasser, value);
}

void WdgRast::on_tbMalzTempEinmaischen_valueChanged(double value)
{
    if (ui->tbMalzTempEinmaischen->hasFocus())
        setData(ModelMaischplan::ColTempMalz, value);
}

void WdgRast::on_tbTempEinmaischen_valueChanged(double value)
{
    if (ui->tbTempEinmaischen->hasFocus())
        setData(ModelMaischplan::ColTempMaische, value);
}

void WdgRast::on_tbDauerEinmaischen_valueChanged(int value)
{
    if (ui->tbDauerEinmaischen->hasFocus())
        setData(ModelMaischplan::ColDauerMaische, value);
}

void WdgRast::on_tbTempRast_valueChanged(double value)
{
    if (ui->tbTempRast->hasFocus())
        setData(ModelMaischplan::ColTempMaische, value);
}

void WdgRast::on_tbDauerRast_valueChanged(int value)
{
    if (ui->tbDauerRast->hasFocus())
        setData(ModelMaischplan::ColDauerMaische, value);
}

void WdgRast::on_tbMengeInfusion_valueChanged(double value)
{
    if (ui->tbMengeInfusion->hasFocus())
        setData(ModelMaischplan::ColMengeWasser, value);
}

void WdgRast::on_tbVerhaeltnisInfusion_valueChanged(double value)
{
    if (ui->tbVerhaeltnisInfusion->hasFocus())
        setData(ModelMaischplan::ColAnteilWasser, value);
}

void WdgRast::on_tbTempInfusion_valueChanged(double value)
{
    if (ui->tbTempInfusion->hasFocus())
        setData(ModelMaischplan::ColTempMaische, value);
}

void WdgRast::on_tbWassertempInfusion_valueChanged(double value)
{
    if (ui->tbWassertempInfusion->hasFocus())
        setData(ModelMaischplan::ColTempWasser, value);
}

void WdgRast::on_tbDauerInfusion_valueChanged(int value)
{
    if (ui->tbDauerInfusion->hasFocus())
        setData(ModelMaischplan::ColDauerMaische, value);
}

void WdgRast::on_tbMengeDekoktion_valueChanged(double value)
{
    if (ui->tbMengeDekoktion->hasFocus())
        setData(ModelMaischplan::ColMengeMaische, value);
}

void WdgRast::on_tbVerhaeltnisDekoktion_valueChanged(double value)
{
    if (ui->tbVerhaeltnisDekoktion->hasFocus())
        setData(ModelMaischplan::ColAnteilMaische, value);
}

void WdgRast::on_tbRasttempDekoktion_valueChanged(double value)
{
    if (ui->tbRasttempDekoktion->hasFocus())
        setData(ModelMaischplan::ColTempExtra2, value);
}

void WdgRast::on_tbRastdauerDekoktion_valueChanged(int value)
{
    if (ui->tbRastdauerDekoktion->hasFocus())
        setData(ModelMaischplan::ColDauerExtra2, value);
}

void WdgRast::on_tbKochdauerDekoktion_valueChanged(int value)
{
    if (ui->tbKochdauerDekoktion->hasFocus())
        setData(ModelMaischplan::ColDauerExtra1, value);
}

void WdgRast::on_tbTeiltempDekoktion_valueChanged(double value)
{
    if (ui->tbTeiltempDekoktion->hasFocus())
        setData(ModelMaischplan::ColTempExtra1, value);
}

void WdgRast::on_tbTempDekoktion_valueChanged(double value)
{
    if (ui->tbTempDekoktion->hasFocus())
        setData(ModelMaischplan::ColTempMaische, value);
}

void WdgRast::on_tbDauerDekoktion_valueChanged(int value)
{
    if (ui->tbDauerDekoktion->hasFocus())
        setData(ModelMaischplan::ColDauerMaische, value);
}

void WdgRast::on_btnLoeschen_clicked()
{
    remove();
}

void WdgRast::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgRast::on_btnNachUnten_clicked()
{
    moveDown();
}
