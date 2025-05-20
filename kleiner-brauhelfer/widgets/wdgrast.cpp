#include "wdgrast.h"
#include "ui_wdgrast.h"
#include <QLineEdit>
#include "brauhelfer.h"
#include "settings.h"
#include "dialogs/dlgmaischplanmalz.h"

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

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorRast);
    setPalette(pal);

    ui->cbRast->setCompleter(nullptr);
    connect(qApp, &QApplication::focusChanged, this, &WdgRast::focusChanged);
    connect(ui->cbRast->lineEdit(), &QLineEdit::textEdited, this, &WdgRast::cbRastTextEdited);

    updateValues();
    updateListe();
    connect(bh, &Brauhelfer::modified, this, &WdgRast::updateValues);
    connect(bh->modelMaischplan(), &ModelMaischplan::rowsSwapped, this, &WdgRast::updateListe);
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
    return data(ModelMaischplan::ColAnteilWasser).toDouble();
}

void WdgRast::setFehlProzentWasser(double value)
{
    bool error = value != 0.0;
    ui->tbAnteilWasser->setError(error);
}

double WdgRast::prozentMalz() const
{
    return data(ModelMaischplan::ColAnteilMalz).toDouble();
}

void WdgRast::setFehlProzentMalz(double value)
{
    bool error = value != 0.0;
    ui->tbAnteilMalz->setError(error);
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
        case Brauhelfer::RastTyp::Aufheizen:
            for (const Rast &rast : rasten)
                ui->cbRast->addItem(tr(rast.name.toStdString().c_str()));
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            ui->cbRast->addItem(tr("1/2 Dickmaische"));
            ui->cbRast->addItem(tr("1/3 Dickmaische"));
            ui->cbRast->addItem(tr("1/3 Dünnmaische"));
            ui->cbRast->addItem(tr("1/3 Läutermaische"));
            break;
        default:
            break;
        }
    }
    else
    {
        ui->cbRast->addItem(name());
    }
}

void WdgRast::updateValues()
{
    checkEnabled();

    ui->cbTyp->setEnabled(mEnabled);
    ui->cbRast->setEditable(mEnabled);
    ui->cbRast->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbMengeWasser->setReadOnly(!mEnabled);
    ui->tbAnteilWasser->setReadOnly(!mEnabled);
    ui->tbTempWasser->setReadOnly(!mEnabled);
    ui->tbMengeMalz->setReadOnly(!mEnabled);
    ui->tbAnteilMalz->setReadOnly(!mEnabled);
    ui->tbTempMalz->setReadOnly(!mEnabled);
    ui->tbMengeMaische->setReadOnly(!mEnabled);
    ui->tbAnteilMaische->setReadOnly(!mEnabled);
    ui->tbDauerExtra2->setReadOnly(!mEnabled);
    ui->tbTempExtra2->setReadOnly(!mEnabled);
    ui->tbDauerExtra1->setReadOnly(!mEnabled);
    ui->tbTempExtra1->setReadOnly(!mEnabled);
    ui->tbTempRast->setReadOnly(!mEnabled);
    ui->tbDauerRast->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    if (!ui->cbRast->hasFocus())
        ui->cbRast->setCurrentText(name());
    if (!ui->cbTyp->hasFocus())
        ui->cbTyp->setCurrentIndex(data(ModelMaischplan::ColTyp).toInt());
    if (!ui->tbMengeWasser->hasFocus())
        ui->tbMengeWasser->setValue(data(ModelMaischplan::ColMengeWasser).toDouble());
    if (!ui->tbAnteilWasser->hasFocus())
        ui->tbAnteilWasser->setValue(data(ModelMaischplan::ColAnteilWasser).toDouble());
    if (!ui->tbTempWasser->hasFocus())
        ui->tbTempWasser->setValue(data(ModelMaischplan::ColTempWasser).toDouble());
    if (!ui->tbMengeMalz->hasFocus())
        ui->tbMengeMalz->setValue(data(ModelMaischplan::ColMengeMalz).toDouble());
    if (!ui->tbAnteilMalz->hasFocus())
        ui->tbAnteilMalz->setValue(data(ModelMaischplan::ColAnteilMalz).toDouble());
    if (!ui->tbTempMalz->hasFocus())
        ui->tbTempMalz->setValue(data(ModelMaischplan::ColTempMalz).toDouble());
    if (!ui->tbMengeMaische->hasFocus())
        ui->tbMengeMaische->setValue(data(ModelMaischplan::ColMengeMaische).toDouble());
    if (!ui->tbAnteilMaische->hasFocus())
        ui->tbAnteilMaische->setValue(data(ModelMaischplan::ColAnteilMaische).toDouble());
    if (!ui->tbDauerExtra2->hasFocus())
        ui->tbDauerExtra2->setValue(data(ModelMaischplan::ColDauerExtra2).toDouble());
    if (!ui->tbTempExtra2->hasFocus())
        ui->tbTempExtra2->setValue(data(ModelMaischplan::ColTempExtra2).toDouble());
    if (!ui->tbDauerExtra1->hasFocus())
        ui->tbDauerExtra1->setValue(data(ModelMaischplan::ColDauerExtra1).toDouble());
    if (!ui->tbTempExtra1->hasFocus())
        ui->tbTempExtra1->setValue(data(ModelMaischplan::ColTempExtra1).toDouble());
    if (!ui->tbTempRast->hasFocus())
        ui->tbTempRast->setValue(data(ModelMaischplan::ColTempRast).toDouble());
    if (!ui->tbDauerRast->hasFocus())
        ui->tbDauerRast->setValue(data(ModelMaischplan::ColDauerRast).toDouble());
    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);

    bool wasser = false, malz = false, dekoktion = false;
    switch (static_cast<Brauhelfer::RastTyp>(data(ModelMaischplan::ColTyp).toInt()))
    {
    case Brauhelfer::RastTyp::Einmaischen:
        ui->lblRast->setText(tr("Einmaischen"));
        wasser = true;
        malz = true;
        break;
    case Brauhelfer::RastTyp::Aufheizen:
        ui->lblRast->setText(tr("Rast"));
        break;
    case Brauhelfer::RastTyp::Zubruehen:
        ui->lblRast->setText(tr("Rast"));
        wasser = true;
        break;
    case Brauhelfer::RastTyp::Zuschuetten:
        ui->lblRast->setText(tr("Rast"));
        wasser = true;
        malz = true;
        break;
    case Brauhelfer::RastTyp::Dekoktion:
        ui->lblRast->setText(tr("Absetzen"));
        dekoktion = true;
        break;
    }
    ui->lblMengeWasser->setVisible(wasser);
    ui->tbMengeWasser->setVisible(wasser);
    ui->lblMengeWasserEinheit->setVisible(wasser);
    ui->tbAnteilWasser->setVisible(wasser);
    ui->lblAnteilWasserEinheit->setVisible(wasser);
    ui->tbTempWasser->setVisible(wasser);
    ui->lblTempWasserEinheit->setVisible(wasser);
    ui->lblMengeMalz->setVisible(malz);
    ui->tbMengeMalz->setVisible(malz);
    ui->lblMengeMalzEinheit->setVisible(malz);
    ui->tbAnteilMalz->setVisible(malz);
    ui->lblAnteilMalzEinheit->setVisible(malz);
    ui->btnAnteilMalz->setVisible(mEnabled && malz);
    ui->tbTempMalz->setVisible(malz);
    ui->lblTempMalzEinheit->setVisible(malz);
    ui->lblMengeMaische->setVisible(dekoktion);
    ui->tbMengeMaische->setVisible(dekoktion);
    ui->lblMengeMaischeEinheit->setVisible(dekoktion);
    ui->tbAnteilMaische->setVisible(dekoktion);
    ui->lblAnteilMaischeEinheit->setVisible(dekoktion);
    ui->lblExtra2->setVisible(dekoktion);
    ui->tbDauerExtra2->setVisible(dekoktion);
    ui->lblDauerExtra2Einheit->setVisible(dekoktion);
    ui->tbTempExtra2->setVisible(dekoktion);
    ui->lblTempExtra2Einheit->setVisible(dekoktion);
    ui->lblExtra1->setVisible(dekoktion);
    ui->tbDauerExtra1->setVisible(dekoktion);
    ui->lblDauerExtra1Einheit->setVisible(dekoktion);
    ui->tbTempExtra1->setVisible(dekoktion);
    ui->lblTempExtra1Einheit->setVisible(dekoktion);
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
    {
        switch (static_cast<Brauhelfer::RastTyp>(data(ModelMaischplan::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Aufheizen:
            if (index >= 0 && index < rasten.count())
            {
                setData(ModelMaischplan::ColDauerRast, rasten[index].dauer);
                setData(ModelMaischplan::ColTempRast, rasten[index].temperatur);
            }
            break;
        case Brauhelfer::RastTyp::Dekoktion:
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
        default:
            break;
        }
    }
}

void WdgRast::on_cbTyp_currentIndexChanged(int index)
{
    if (ui->cbTyp->hasFocus())
    {
        setData(ModelMaischplan::ColTyp, index);
        updateListe();
        switch (static_cast<Brauhelfer::RastTyp>(data(ModelMaischplan::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            setData(ModelMaischplan::ColName, tr("Einmaischen"));
            break;
        case Brauhelfer::RastTyp::Aufheizen:
            setData(ModelMaischplan::ColName, ui->cbRast->itemText(0));
            break;
        case Brauhelfer::RastTyp::Zubruehen:
            setData(ModelMaischplan::ColName, tr("Zubrühen"));
                break;
        case Brauhelfer::RastTyp::Zuschuetten:
            setData(ModelMaischplan::ColName, tr("Teilschüttung"));
                break;
        case Brauhelfer::RastTyp::Dekoktion:
            setData(ModelMaischplan::ColName, ui->cbRast->itemText(0));
            break;
        }
    }
}

void WdgRast::on_tbMengeWasser_valueChanged(double value)
{
    if (ui->tbMengeWasser->hasFocus())
        setData(ModelMaischplan::ColMengeWasser, value);
}

void WdgRast::on_tbAnteilWasser_valueChanged(double value)
{
    if (ui->tbAnteilWasser->hasFocus())
        setData(ModelMaischplan::ColAnteilWasser, value);
}

void WdgRast::on_tbTempWasser_valueChanged(double value)
{
    if (ui->tbTempWasser->hasFocus())
        setData(ModelMaischplan::ColTempWasser, value);
}

void WdgRast::on_tbMengeMalz_valueChanged(double value)
{
    if (ui->tbMengeMalz->hasFocus())
        setData(ModelMaischplan::ColMengeMalz, value);
}

void WdgRast::on_tbAnteilMalz_valueChanged(double value)
{
    if (ui->tbAnteilMalz->hasFocus())
        setData(ModelMaischplan::ColAnteilMalz, value);
}

void WdgRast::on_btnAnteilMalz_clicked()
{
    DlgMaischplanMalz dlg(data(ModelMaischplan::ColMengeMalz).toDouble());
    if (dlg.exec() == QDialog::Accepted)
        setData(ModelMaischplan::ColMengeMalz, dlg.value());
}

void WdgRast::on_tbTempMalz_valueChanged(double value)
{
    if (ui->tbTempMalz->hasFocus())
        setData(ModelMaischplan::ColTempMalz, value);
}

void WdgRast::on_tbMengeMaische_valueChanged(double value)
{
    if (ui->tbMengeMaische->hasFocus())
        setData(ModelMaischplan::ColMengeMaische, value);
}

void WdgRast::on_tbAnteilMaische_valueChanged(double value)
{
    if (ui->tbAnteilMaische->hasFocus())
        setData(ModelMaischplan::ColAnteilMaische, value);
}

void WdgRast::on_tbDauerExtra2_valueChanged(int value)
{
    if (ui->tbDauerExtra2->hasFocus())
        setData(ModelMaischplan::ColDauerExtra2, value);
}

void WdgRast::on_tbTempExtra2_valueChanged(double value)
{
    if (ui->tbTempExtra2->hasFocus())
        setData(ModelMaischplan::ColTempExtra2, value);
}

void WdgRast::on_tbDauerExtra1_valueChanged(int value)
{
    if (ui->tbDauerExtra1->hasFocus())
        setData(ModelMaischplan::ColDauerExtra1, value);
}

void WdgRast::on_tbTempExtra1_valueChanged(double value)
{
    if (ui->tbTempExtra1->hasFocus())
        setData(ModelMaischplan::ColTempExtra1, value);
}

void WdgRast::on_tbDauerRast_valueChanged(int value)
{
    if (ui->tbDauerRast->hasFocus())
        setData(ModelMaischplan::ColDauerRast, value);
}

void WdgRast::on_tbTempRast_valueChanged(double value)
{
    if (ui->tbTempRast->hasFocus())
        setData(ModelMaischplan::ColTempRast, value);
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
