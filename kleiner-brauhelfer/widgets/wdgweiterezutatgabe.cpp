#include "wdgweiterezutatgabe.h"
#include "ui_wdgweiterezutatgabe.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "mainwindow.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgWeitereZutatGabe::WdgWeitereZutatGabe(Brauhelfer::ZusatzZeitpunkt zeitpunkt, int row, QLayout* parentLayout, QWidget *parent) :
    WdgAbstractProxy(nullptr, row, parentLayout, parent),
    ui(new Ui::WdgWeitereZutatGabe),
    mEnabled(true),
    mFehlProzentExtrakt(0)
{
    ProxyModel* proxy = new ProxyModel(this);
    proxy->setSourceModel(bh->sud()->modelWeitereZutatenGaben());
    proxy->setFilterKeyColumn(ModelWeitereZutatenGaben::ColZeitpunkt);
    proxy->setFilterRegularExpression(QString::number(static_cast<int>(zeitpunkt)));
    setModel(proxy);

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

    ui->tbMenge->setErrorRange(0, ui->tbMenge->maximum());
    ui->tbMengeTotal->setErrorRange(0, ui->tbMengeTotal->maximum());
    ui->btnKorrekturExtrakt->setError(true);

    updateValues();
    connect(bh, &Brauhelfer::modified, this, &WdgWeitereZutatGabe::updateValues);
}

WdgWeitereZutatGabe::~WdgWeitereZutatGabe()
{
    delete ui;
}

bool WdgWeitereZutatGabe::isEnabled() const
{
    return mEnabled;
}

bool WdgWeitereZutatGabe::isValid() const
{
    return mValid;
}

QString WdgWeitereZutatGabe::name() const
{
    return data(ModelWeitereZutatenGaben::ColName).toString();
}

void WdgWeitereZutatGabe::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    mEnabled = status == Brauhelfer::SudStatus::Rezept;
    Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(data(ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
    if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
        mEnabled = status < Brauhelfer::SudStatus::Abgefuellt;
    if (gSettings->ForceEnabled)
        mEnabled = true;
}

void WdgWeitereZutatGabe::updateValues()
{
    QString zusatzname = name();
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
    Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(data(ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
    Brauhelfer::ZusatzEntnahmeindex entnahmeindex = static_cast<Brauhelfer::ZusatzEntnahmeindex>(data(ModelWeitereZutatenGaben::ColEntnahmeindex).toInt());
    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
    Brauhelfer::ZusatzStatus zugabestatus = static_cast<Brauhelfer::ZusatzStatus>(data(ModelWeitereZutatenGaben::ColZugabestatus).toInt());
    int dauer = data(ModelWeitereZutatenGaben::ColZugabedauer).toInt();

    checkEnabled();

    bool bVal = mEnabled && (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung) && (status == Brauhelfer::SudStatus::Gebraut);
    ui->cbAnstellmenge->setVisible(bVal);
    if (!bVal)
        ui->cbAnstellmenge->setCheckState(Qt::Unchecked);

    ui->btnZutat->setEnabled(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbVorhanden->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->lblVorhanden->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->lblVorhandenEinheit->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->btnAufbrauchen->setVisible(mEnabled && gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->cbEntnahme->setEnabled(mEnabled);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbMengeTotal->setReadOnly(!mEnabled || ui->cbAnstellmenge->isChecked());
    ui->tbExtrakt->setReadOnly(!mEnabled);
    ui->tbExtraktProzent->setReadOnly(!mEnabled);
    ui->btnKorrekturExtrakt->setVisible(mEnabled);
    ui->tbKochdauer->setReadOnly(!mEnabled);
    ui->tbZugabeNach->setReadOnly(!mEnabled);
    ui->tbDatumVon->setReadOnly(!mEnabled);
    ui->tbDauerTage->setReadOnly(!mEnabled);
    ui->tbDatumBis->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    ui->cbEntnahme->setVisible(typ != Brauhelfer::ZusatzTyp::Hopfen);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, typ == Brauhelfer::ZusatzTyp::Hopfen ? gSettings->colorHopfen : gSettings->colorZusatz);
    setPalette(pal);

    int rowRohstoff;
    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
        rowRohstoff = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, zusatzname);
    else
        rowRohstoff = bh->modelWeitereZutaten()->getRowWithValue(ModelWeitereZutaten::ColName, zusatzname);
    mValid = !mEnabled || rowRohstoff >= 0;
    ui->btnZutat->setText(zusatzname);
    ui->btnZutat->setError(!mValid);
    if (!ui->tbMenge->hasFocus())
    {
        if (einheit == Brauhelfer::Einheit::mg)
            ui->tbMenge->setValue(data(ModelWeitereZutatenGaben::ColMenge).toDouble() * 1000);
        else
            ui->tbMenge->setValue(data(ModelWeitereZutatenGaben::ColMenge).toDouble());
    }
    if (!ui->tbMengeTotal->hasFocus())
    {
        QString str = MainWindow::Einheiten[static_cast<int>(einheit)];
        ui->lblEinheit->setText(str);
        ui->lblVorhandenEinheit->setText(str);
        int colMengeTotal = ui->cbAnstellmenge->isChecked() ? ModelWeitereZutatenGaben::Colerg_MengeIst : ModelWeitereZutatenGaben::Colerg_Menge;
        switch (einheit)
        {
        case Brauhelfer::Einheit::Kg:
            ui->lblEinheitProLiter->setText(QStringLiteral("g/L"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(colMengeTotal).toDouble() / 1000);
            break;
        case Brauhelfer::Einheit::g:
            ui->lblEinheitProLiter->setText(QStringLiteral("g/L"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(colMengeTotal).toDouble());
            break;
        case Brauhelfer::Einheit::mg:
            ui->lblEinheitProLiter->setText(QStringLiteral("mg/L"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(colMengeTotal).toDouble() * 1000);
            break;
        case Brauhelfer::Einheit::Stk:
            ui->lblEinheitProLiter->setText(tr("Stk./L"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(0);
            ui->tbMengeTotal->setValue(data(colMengeTotal).toDouble());
            break;
        case Brauhelfer::Einheit::l:
            ui->lblEinheitProLiter->setText(QStringLiteral("mL/L"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(colMengeTotal).toDouble() / 1000);
            break;
        case Brauhelfer::Einheit::ml:
            ui->lblEinheitProLiter->setText(QStringLiteral("mL/L"));
            ui->tbMenge->setDecimals(2);
            ui->tbMengeTotal->setDecimals(2);
            ui->tbVorhanden->setDecimals(2);
            ui->tbMengeTotal->setValue(data(colMengeTotal).toDouble());
            break;
        }
    }
    if (!ui->tbKochdauer->hasFocus())
    {
        ui->tbKochdauer->setMinimum(-bh->sud()->getNachisomerisierungszeit());
        ui->tbKochdauer->setMaximum(bh->sud()->getKochdauer());
        ui->tbKochdauer->setValue(dauer);
    }

    if (!ui->tbExtrakt->hasFocus())
        ui->tbExtrakt->setValue(data(ModelWeitereZutatenGaben::ColExtrakt).toDouble());
    if (!ui->tbExtraktProzent->hasFocus())
        ui->tbExtraktProzent->setValue(data(ModelWeitereZutatenGaben::ColExtraktProzent).toDouble());
    bool visible = data(ModelWeitereZutatenGaben::ColAusbeute).toDouble() > 0;
    ui->tbExtrakt->setVisible(visible);
    ui->tbExtraktProzent->setVisible(visible);
    ui->lblExtrakt->setVisible(visible);
    ui->lblExtraktEinheit->setVisible(visible);
    ui->lblExtraktProzent->setVisible(visible);
    ui->tbExtraktProzent->setError(ui->tbExtraktProzent->value() == 0.0 || mFehlProzentExtrakt != 0.0);
    ui->btnKorrekturExtrakt->setVisible(mEnabled && visible && mFehlProzentExtrakt != 0.0);

    ui->cbEntnahme->setChecked(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
    if (!ui->tbZugabeNach->hasFocus())
        ui->tbZugabeNach->setValue(data(ModelWeitereZutatenGaben::ColZugabeNach).toInt());
    if (!ui->tbDauerTage->hasFocus())
        ui->tbDauerTage->setValue(dauer / 1440);

    QDateTime braudatum = bh->sud()->getBraudatum();
    if (braudatum.isValid())
    {
        ui->tbDatumVon->setMinimumDateTime(braudatum);
        if (!ui->tbDatumVon->hasFocus())
            ui->tbDatumVon->setDate(data(ModelWeitereZutatenGaben::ColZugabeDatum).toDate());
        ui->tbDatumBis->setMinimumDateTime(ui->tbDatumVon->dateTime());
        if (!ui->tbDatumBis->hasFocus())
            ui->tbDatumBis->setDate(data(ModelWeitereZutatenGaben::ColEntnahmeDatum).toDate());
    }
    ui->tbDatumVon->setVisible(braudatum.isValid());

    if (!ui->tbKomentar->hasFocus())
        ui->tbKomentar->setText(data(ModelWeitereZutatenGaben::ColBemerkung).toString());

    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
    {
        int idx = bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColTyp).toInt();
        if (idx >= 0 && idx < gSettings->HopfenTypBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Window, gSettings->HopfenTypBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
            ui->frameColor->setToolTip(tr("Hopfen"));
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
            ui->frameColor->setToolTip(QStringLiteral(""));
        }
    }
    else
    {
        int idx = static_cast<int>(typ);
        if (idx >= 0 && idx < gSettings->WZTypBackgrounds.count())
        {
            QPalette pal = ui->frameColor->palette();
            pal.setColor(QPalette::Window, gSettings->WZTypBackgrounds[idx]);
            ui->frameColor->setPalette(pal);
            ui->frameColor->setToolTip(MainWindow::ZusatzTypname[idx]);
        }
        else
        {
            ui->frameColor->setPalette(gSettings->palette);
            ui->frameColor->setToolTip(QStringLiteral(""));
        }
    }

    if (mEnabled)
    {
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                ui->tbVorhanden->setValue(bh->modelHopfen()->data(rowRohstoff, ModelHopfen::ColMenge).toInt());
            else
                ui->tbVorhanden->setValue(bh->modelWeitereZutaten()->data(rowRohstoff, ModelWeitereZutaten::ColMenge).toDouble());
            if (!ui->cbAnstellmenge->isChecked())
            {
                double benoetigt = 0.0;
                for (int i = 0; i < mModel->rowCount(); ++i)
                {
                    if (mModel->data(i, ModelWeitereZutatenGaben::ColName).toString() == zusatzname)
                        benoetigt += mModel->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                }
                if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                {
                    ProxyModel* model = bh->sud()->modelHopfengaben();
                    for (int i = 0; i < model->rowCount(); ++i)
                    {
                        if (model->data(i, ModelHopfengaben::ColName).toString() == zusatzname)
                            benoetigt += model->data(i, ModelHopfengaben::Colerg_Menge).toDouble();
                    }
                }
                if (einheit == Brauhelfer::Einheit::Kg || einheit == Brauhelfer::Einheit::l)
                    benoetigt /= 1000;
                else if (einheit == Brauhelfer::Einheit::mg)
                    benoetigt *= 1000;
                ui->tbVorhanden->setError(benoetigt - ui->tbVorhanden->value() > 0.001);
            }
            else
            {
                ui->tbVorhanden->setError(false);
            }
        }
        ui->btnEntnehmen->setError(false);
        switch (zugabestatus)
        {
        case Brauhelfer::ZusatzStatus::NichtZugegeben:
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                ui->tbVorhanden->setVisible(true);
                ui->lblVorhanden->setVisible(true);
                ui->lblVorhandenEinheit->setVisible(true);
                ui->btnAufbrauchen->setVisible(qAbs(ui->tbVorhanden->value() - ui->tbMengeTotal->value()) > 0.001 && !ui->cbAnstellmenge->isChecked());
            }
            ui->btnLoeschen->setVisible(true);
            ui->btnNachOben->setVisible(true);
            ui->btnNachUnten->setVisible(true);
            ui->tbZugabeNach->setReadOnly(false);
            ui->tbDatumVon->setReadOnly(false);
            ui->tbDatumBis->setReadOnly(false);
            ui->tbDauerTage->setReadOnly(false);
            ui->cbEntnahme->setEnabled(true);
            ui->tbMenge->setReadOnly(false);
            ui->tbMengeTotal->setReadOnly(ui->cbAnstellmenge->isChecked());
            ui->tbExtrakt->setReadOnly(false);
            ui->tbExtraktProzent->setReadOnly(false);
            ui->btnZutat->setEnabled(true);
            if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                QDate currentDate = QDate::currentDate();
                QDate dateSoll = data(ModelWeitereZutatenGaben::ColZugabeDatum).toDate();
                ui->btnZugeben->setError(currentDate >= dateSoll);
            }
            break;
        case Brauhelfer::ZusatzStatus::Zugegeben:
            ui->tbVorhanden->setVisible(false);
            ui->btnAufbrauchen->setVisible(false);
            ui->lblVorhanden->setVisible(false);
            ui->lblVorhandenEinheit->setVisible(false);
            ui->btnLoeschen->setVisible(false);
            ui->btnNachOben->setVisible(false);
            ui->btnNachUnten->setVisible(false);
            ui->tbZugabeNach->setReadOnly(true);
            ui->tbDatumVon->setReadOnly(true);
            ui->tbDatumBis->setReadOnly(false);
            ui->tbDauerTage->setReadOnly(false);
            ui->cbEntnahme->setEnabled(true);
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeTotal->setReadOnly(true);
            ui->tbExtrakt->setReadOnly(true);
            ui->tbExtraktProzent->setReadOnly(true);
            ui->btnKorrekturExtrakt->setVisible(false);
            ui->btnZutat->setEnabled(false);
            if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                QDate currentDate = QDate::currentDate();
                QDate dateSoll = data(ModelWeitereZutatenGaben::ColZugabeDatum).toDate().addDays(dauer / 1440);
                ui->btnEntnehmen->setError(currentDate >= dateSoll);
            }
            break;
        case Brauhelfer::ZusatzStatus::Entnommen:
            ui->tbVorhanden->setVisible(false);
            ui->btnAufbrauchen->setVisible(false);
            ui->lblVorhanden->setVisible(false);
            ui->lblVorhandenEinheit->setVisible(false);
            ui->btnLoeschen->setVisible(false);
            ui->btnNachOben->setVisible(false);
            ui->btnNachUnten->setVisible(false);
            ui->tbZugabeNach->setReadOnly(true);
            ui->tbDatumVon->setReadOnly(true);
            ui->tbDatumBis->setReadOnly(true);
            ui->tbDauerTage->setReadOnly(true);
            ui->cbEntnahme->setEnabled(false);
            ui->tbMenge->setReadOnly(true);
            ui->tbMengeTotal->setReadOnly(true);
            ui->tbExtrakt->setReadOnly(true);
            ui->tbExtraktProzent->setReadOnly(true);
            ui->btnKorrekturExtrakt->setVisible(false);
            ui->btnZutat->setEnabled(false);
            break;
        }
    }

    if (gSettings->ForceEnabled)
    {
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            ui->tbVorhanden->setVisible(true);
            ui->btnAufbrauchen->setVisible(true);
            ui->lblVorhanden->setVisible(true);
            ui->lblVorhandenEinheit->setVisible(true);
        }
        ui->btnLoeschen->setVisible(true);
        ui->tbZugabeNach->setReadOnly(false);
        ui->tbDatumVon->setReadOnly(false);
        ui->tbDatumBis->setReadOnly(false);
        ui->tbDauerTage->setReadOnly(false);
        ui->cbEntnahme->setEnabled(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbMengeTotal->setReadOnly(false);
        ui->tbExtrakt->setReadOnly(false);
        ui->tbExtraktProzent->setReadOnly(false);
        ui->btnZutat->setEnabled(true);
    }

    ui->wdgKochdauer->setVisible(zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Kochen);
    ui->wdgZugabe->setVisible(zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung);
    ui->lblEntnahme->setVisible(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->tbDauerTage->setVisible(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->lblDauerTage->setVisible(entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->tbDatumBis->setVisible(braudatum.isValid() && entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
    ui->btnZugeben->setVisible(status == Brauhelfer::SudStatus::Gebraut && zugabestatus == Brauhelfer::ZusatzStatus::NichtZugegeben);
    ui->btnEntnehmen->setVisible(status == Brauhelfer::SudStatus::Gebraut && zugabestatus == Brauhelfer::ZusatzStatus::Zugegeben && entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
}

void WdgWeitereZutatGabe::on_btnZutat_clicked()
{
    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
    {
        DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Hopfen, this);
        dlg.select(name());
        if (dlg.exec() == QDialog::Accepted)
            setData(ModelHopfengaben::ColName, dlg.name());
    }
    else
    {
        DlgRohstoffAuswahl dlg(Brauhelfer::RohstoffTyp::Zusatz, this);
        dlg.select(name());
        if (dlg.exec() == QDialog::Accepted)
            setData(ModelWeitereZutatenGaben::ColName, dlg.name());
    }
}

void WdgWeitereZutatGabe::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
    {
        Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
        if (einheit == Brauhelfer::Einheit::mg)
            setData(ModelWeitereZutatenGaben::ColMenge, value / 1000);
        else
            setData(ModelWeitereZutatenGaben::ColMenge, value);
    }
}

void WdgWeitereZutatGabe::on_tbMengeTotal_valueChanged(double value)
{
    if (ui->tbMengeTotal->hasFocus())
    {
        Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
        switch (einheit)
        {
        case Brauhelfer::Einheit::Kg:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
            break;
        case Brauhelfer::Einheit::g:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        case Brauhelfer::Einheit::mg:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value / 1000);
            break;
        case Brauhelfer::Einheit::Stk:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        case Brauhelfer::Einheit::l:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
            break;
        case Brauhelfer::Einheit::ml:
            setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
            break;
        }
    }
}

void WdgWeitereZutatGabe::on_tbKochdauer_valueChanged(int value)
{
    if (ui->tbKochdauer->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabedauer, value);
}

void WdgWeitereZutatGabe::on_tbExtrakt_valueChanged(double value)
{
    if (ui->tbExtrakt->hasFocus())
        setData(ModelWeitereZutatenGaben::ColExtrakt, value);
}

void WdgWeitereZutatGabe::on_tbExtraktProzent_valueChanged(double value)
{
    if (ui->tbExtraktProzent->hasFocus())
        setData(ModelWeitereZutatenGaben::ColExtraktProzent, value);
}

double WdgWeitereZutatGabe::prozentExtrakt() const
{
    return data(ModelWeitereZutatenGaben::ColExtraktProzent).toDouble();
}

double WdgWeitereZutatGabe::fehlProzentExtrakt() const
{
    return mFehlProzentExtrakt;
}

void WdgWeitereZutatGabe::setFehlProzentExtrakt(double value)
{
    if (value < 0.0)
    {
        double p = prozentExtrakt();
        if (p == 0.0)
            value = 0.0;
        else if (value < -p)
            value = -p;
    }
    mFehlProzentExtrakt = value;
    QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 2) + " %";
    ui->btnKorrekturExtrakt->setText(text);
}

void WdgWeitereZutatGabe::on_btnKorrekturExtrakt_clicked()
{
    setFocus();
    setData(ModelWeitereZutatenGaben::ColExtraktProzent, prozentExtrakt() + mFehlProzentExtrakt);
}

void WdgWeitereZutatGabe::on_btnZugeben_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatumVon->date();
    setData(ModelWeitereZutatenGaben::ColZugabeDatum, currentDate < date ? currentDate : date);
    setData(ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Zugegeben));
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        Brauhelfer::ZusatzTyp zusatztyp = static_cast<Brauhelfer::ZusatzTyp>(data(ModelWeitereZutatenGaben::ColTyp).toInt());
        Brauhelfer::RohstoffTyp typ = zusatztyp == Brauhelfer::ZusatzTyp::Hopfen ? Brauhelfer::RohstoffTyp::Hopfen : Brauhelfer::RohstoffTyp::Zusatz;
        int colMengeTotal = ui->cbAnstellmenge->isChecked() ? ModelWeitereZutatenGaben::Colerg_MengeIst : ModelWeitereZutatenGaben::Colerg_Menge;
        DlgRohstoffeAbziehen dlg(true, typ,
                                 data(ModelWeitereZutatenGaben::ColName).toString(),
                                 data(colMengeTotal).toDouble(),
                                 this);
        dlg.exec();
    }
}

void WdgWeitereZutatGabe::on_cbEntnahme_clicked(bool checked)
{
    setData(ModelWeitereZutatenGaben::ColEntnahmeindex, checked);
}

void WdgWeitereZutatGabe::on_tbZugabeNach_valueChanged(int value)
{
    if (ui->tbZugabeNach->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabeNach, value);
}

void WdgWeitereZutatGabe::on_tbDauerTage_valueChanged(int value)
{
    if (ui->tbDauerTage->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabedauer, value * 1440);
}

void WdgWeitereZutatGabe::on_tbDatumVon_dateChanged(const QDate &date)
{
    if (ui->tbDatumVon->hasFocus())
        setData(ModelWeitereZutatenGaben::ColZugabeDatum, date);
}

void WdgWeitereZutatGabe::on_tbDatumBis_dateChanged(const QDate &date)
{
    if (ui->tbDatumBis->hasFocus())
        setData(ModelWeitereZutatenGaben::ColEntnahmeDatum, date);
}

void WdgWeitereZutatGabe::on_btnEntnehmen_clicked()
{
    QDate currentDate = QDate::currentDate();
    QDate date = ui->tbDatumBis->date();
    setData(ModelWeitereZutatenGaben::ColEntnahmeDatum, currentDate < date ? currentDate : date);
    setData(ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Entnommen));
}

void WdgWeitereZutatGabe::on_tbKomentar_textChanged()
{
    if (ui->tbKomentar->hasFocus())
        setData(ModelWeitereZutatenGaben::ColBemerkung, ui->tbKomentar->toPlainText());
}

void WdgWeitereZutatGabe::on_btnLoeschen_clicked()
{
    remove();
}

void WdgWeitereZutatGabe::on_btnAufbrauchen_clicked()
{
    double value = ui->tbVorhanden->value();
    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(ModelWeitereZutatenGaben::ColEinheit).toInt());
    switch (einheit)
    {
    case Brauhelfer::Einheit::Kg:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
        break;
    case Brauhelfer::Einheit::g:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    case Brauhelfer::Einheit::mg:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value / 1000);
        break;
    case Brauhelfer::Einheit::Stk:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    case Brauhelfer::Einheit::l:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value * 1000);
        break;
    case Brauhelfer::Einheit::ml:
        setData(ModelWeitereZutatenGaben::Colerg_Menge, value);
        break;
    }
}

void WdgWeitereZutatGabe::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgWeitereZutatGabe::on_btnNachUnten_clicked()
{
    moveDown();
}

void WdgWeitereZutatGabe::on_cbAnstellmenge_clicked(bool checked)
{
    Q_UNUSED(checked)
    updateValues();
}
