#include "wdghopfengabe.h"
#include "ui_wdghopfengabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "mainwindow.h"
#include "dialogs/dlgrohstoffauswahl.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgHopfenGabe::WdgHopfenGabe(int row, QLayout* parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelHopfengaben(), row, parentLayout, parent),
    ui(new Ui::WdgHopfenGabe),
    mEnabled(true),
    mFehlProzent(0)
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
    pal.setColor(QPalette::Window, gSettings->colorHopfen);
    setPalette(pal);

    ui->btnMengeKorrektur->setError(true);
    ui->btnAnteilKorrektur->setError(true);

    ui->tbMenge->setErrorRange(0, ui->tbMenge->maximum());
    ui->tbMengeProLiter->setErrorRange(0, ui->tbMengeProLiter->maximum());

    updateValues();
    connect(bh, &Brauhelfer::modified, this, &WdgHopfenGabe::updateValues);
}

WdgHopfenGabe::~WdgHopfenGabe()
{
    delete ui;
}

bool WdgHopfenGabe::isEnabled() const
{
    return mEnabled;
}

bool WdgHopfenGabe::isValid() const
{
    return mValid;
}

void WdgHopfenGabe::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    mEnabled = status == Brauhelfer::SudStatus::Rezept;
    if (gSettings->ForceEnabled)
        mEnabled = true;
}

void WdgHopfenGabe::updateValues()
{
    QString hopfenname = name();

    checkEnabled();

    ui->btnZutat->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbVorhanden->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->lblVorhanden->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->lblVorhandenEinheit->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    if (!mEnabled)
        ui->btnAufbrauchen->setVisible(false);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbMengeProLiter->setReadOnly(!mEnabled);
    ui->tbMengeProzent->setReadOnly(!mEnabled);
    ui->tbAnteilProzent->setReadOnly(!mEnabled);
    ui->tbKochdauer->setReadOnly(!mEnabled);
    ui->cbZeitpunkt->setEnabled(mEnabled);
    if (!mEnabled)
    {
        ui->btnAnteilKorrektur->setVisible(false);
        ui->btnMengeKorrektur->setVisible(false);
    }
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    int rowRohstoff = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, hopfenname);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(hopfenname);
    ui->btnZutat->setError(!mValid);
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbAnteilProzent->hasFocus())
        ui->tbAnteilProzent->setValue(data(ModelHopfengaben::ColProzent).toDouble());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data(ModelHopfengaben::Colerg_Menge).toDouble());
    if (!ui->tbMengeProLiter->hasFocus())
        ui->tbMengeProLiter->setValue(data(ModelHopfengaben::Colerg_Menge).toDouble() / bh->sud()->getMengeSollAnstellen());
    if (!ui->tbAnteil->hasFocus())
        ui->tbAnteil->setValue(data(ModelHopfengaben::ColIBUAnteil).toDouble());
    ui->tbAlpha->setValue(data(ModelHopfengaben::ColAlpha).toDouble());
    double ausbeute = data(ModelHopfengaben::ColAusbeute).toDouble();
    ui->tbAusbeute->setValue(ausbeute);
    if (!ui->tbKochdauer->hasFocus())
        ui->tbKochdauer->setValue(data(ModelHopfengaben::ColZeit).toInt());
    Brauhelfer::HopfenZeitpunkt zeitpunkt = static_cast<Brauhelfer::HopfenZeitpunkt>(data(ModelHopfengaben::ColZeitpunkt).toInt());
    if (zeitpunkt == Brauhelfer::HopfenZeitpunkt::KochenAlt)
        zeitpunkt = Brauhelfer::HopfenZeitpunkt::Kochen;
    ui->cbZeitpunkt->setCurrentIndex(static_cast<int>(zeitpunkt) - 1);
    int idx = bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColTyp).toInt();
    if (idx >= 0 && idx < gSettings->HopfenTypBackgrounds.count())
    {
        QPalette pal = ui->frameColor->palette();
        pal.setColor(QPalette::Window, gSettings->HopfenTypBackgrounds[idx]);
        ui->frameColor->setPalette(pal);
        ui->frameColor->setToolTip(MainWindow::HopfenTypname[idx]);
    }
    else
    {
        ui->frameColor->setPalette(gSettings->palette);
        ui->frameColor->setToolTip(QStringLiteral(""));
    }

    if (mEnabled)
    {
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            ui->tbVorhanden->setValue(bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColMenge).toDouble());
            double benoetigt = 0;
            for (int i = 0; i < mModel->rowCount(); ++i)
            {
                if (mModel->data(i, ModelHopfengaben::ColName).toString() == hopfenname)
                    benoetigt += mModel->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
            }
            ProxyModel* model = bh->sud()->modelWeitereZutatenGaben();
            for (int i = 0; i < model->rowCount(); ++i)
            {
                Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model->data(i, ModelWeitereZutatenGaben::ColTyp).toInt());
                if (typ == Brauhelfer::ZusatzTyp::Hopfen && model->data(i, ModelWeitereZutatenGaben::ColName).toString() == hopfenname)
                    benoetigt += model->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
            }
            ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);
        }

        switch (zeitpunkt)
        {
        case Brauhelfer::HopfenZeitpunkt::Kochen:
        case Brauhelfer::HopfenZeitpunkt::KochenAlt:
            ui->tbKochdauer->setReadOnly(false);
            ui->tbKochdauer->setMinimum(0);
            ui->tbKochdauer->setMaximum(bh->sud()->getKochdauer());
            break;
        case Brauhelfer::HopfenZeitpunkt::Vorderwuerze:
        case Brauhelfer::HopfenZeitpunkt::Kochbeginn:
        case Brauhelfer::HopfenZeitpunkt::Kochende:
            ui->tbKochdauer->setReadOnly(true);
            break;
        case Brauhelfer::HopfenZeitpunkt::Ausschlagen:
            ui->tbKochdauer->setReadOnly(false);
            ui->tbKochdauer->setMinimum(-bh->sud()->getNachisomerisierungszeit());
            ui->tbKochdauer->setMaximum(0);
        }

        Brauhelfer::BerechnungsartHopfen berechnungsArtHopfen = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->sud()->getberechnungsArtHopfen());
        switch (berechnungsArtHopfen)
        {
        case Brauhelfer::BerechnungsartHopfen::Keine:
            ui->btnMengeKorrektur->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(false);
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(false);
            break;
        case Brauhelfer::BerechnungsartHopfen::Gewicht:
            ui->btnMengeKorrektur->setVisible(mFehlProzent != 0.0);
            ui->tbMengeProzent->setVisible(true);
            ui->tbMengeProzent->setError(ui->tbMengeProzent->value() == 0.0 || mFehlProzent != 0.0);
            ui->lblMengeProzent->setVisible(true);
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(false);
            break;
        case Brauhelfer::BerechnungsartHopfen::IBU:
            ui->btnMengeKorrektur->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(false);
            ui->btnAnteilKorrektur->setVisible(ausbeute > 0.05 && mFehlProzent != 0.0);
            ui->tbAnteilProzent->setVisible(ausbeute > 0.05);
            ui->tbAnteilProzent->setError(ui->tbAnteilProzent->value() == 0.0 || mFehlProzent != 0.0);
            ui->lblAnteilProzent->setVisible(ausbeute > 0.05);
            break;
        }

        if (mRow == 0 && mModel->rowCount() == 1 && berechnungsArtHopfen != Brauhelfer::BerechnungsartHopfen::Keine)
        {
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeProLiter->setReadOnly(true);
            ui->tbMengeProzent->setReadOnly(true);
            ui->tbAnteilProzent->setReadOnly(true);
            ui->btnAufbrauchen->setVisible(false);
        }
        else
        {
            ui->tbMenge->setReadOnly(false);
            ui->tbMengeProLiter->setReadOnly(false);
            ui->tbMengeProzent->setReadOnly(false);
            ui->tbAnteilProzent->setReadOnly(false);
            ui->btnAufbrauchen->setVisible(qAbs(ui->tbVorhanden->value() - ui->tbMenge->value()) > 0.001 && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
        }
    }

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
}

void WdgHopfenGabe::on_btnZutat_clicked()
{
    DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
    dlg.select(name());
    if (dlg.exec() == QDialog::Accepted)
        setData(ModelHopfengaben::ColName, dlg.name());
}

void WdgHopfenGabe::on_tbMengeProzent_valueChanged(double value)
{
    if (ui->tbMengeProzent->hasFocus())
        setData(ModelHopfengaben::ColProzent, value);
}

void WdgHopfenGabe::on_tbAnteilProzent_valueChanged(double value)
{
    if (ui->tbAnteilProzent->hasFocus())
        setData(ModelHopfengaben::ColProzent, value);
}

QString WdgHopfenGabe::name() const
{
    return data(ModelHopfengaben::ColName).toString();
}

double WdgHopfenGabe::prozent() const
{
    return data(ModelHopfengaben::ColProzent).toDouble();
}

void WdgHopfenGabe::setFehlProzent(double value)
{
    if (value < 0.0)
    {
        double p = prozent();
        if (p == 0.0)
            value = 0.0;
        else if (value < -p)
            value = -p;
    }
    mFehlProzent = value;
    QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 2) + " %";
    ui->btnMengeKorrektur->setText(text);
    ui->btnAnteilKorrektur->setText(text);
}

void WdgHopfenGabe::on_btnMengeKorrektur_clicked()
{
    setFocus();
    setData(ModelHopfengaben::ColProzent, prozent() + mFehlProzent);
}

void WdgHopfenGabe::on_btnAnteilKorrektur_clicked()
{
    setFocus();
    setData(ModelHopfengaben::ColProzent, prozent() + mFehlProzent);
}

void WdgHopfenGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelHopfengaben::Colerg_Menge, value);
}

void WdgHopfenGabe::on_tbMengeProLiter_valueChanged(double value)
{
    if (ui->tbMengeProLiter->hasFocus())
        setData(ModelHopfengaben::Colerg_Menge, value * bh->sud()->getMengeSollAnstellen());
}

void WdgHopfenGabe::on_tbKochdauer_valueChanged(int dauer)
{
    if (ui->tbKochdauer->hasFocus())
        setData(ModelHopfengaben::ColZeit, dauer);
}

void WdgHopfenGabe::on_cbZeitpunkt_currentIndexChanged(int index)
{
    if (ui->cbZeitpunkt->hasFocus())
        setData(ModelHopfengaben::ColZeitpunkt, index + 1);
}

void WdgHopfenGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgHopfenGabe::on_btnAufbrauchen_clicked()
{
    setData(ModelHopfengaben::Colerg_Menge, ui->tbVorhanden->value());
}

void WdgHopfenGabe::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgHopfenGabe::on_btnNachUnten_clicked()
{
    moveDown();
}
