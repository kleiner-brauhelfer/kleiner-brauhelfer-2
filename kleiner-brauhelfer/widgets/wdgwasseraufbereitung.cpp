#include "wdgwasseraufbereitung.h"
#include "ui_wdgwasseraufbereitung.h"
#include "mainwindow.h"
#include "dialogs/dlgwasseraufbereitung.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgWasseraufbereitung::WdgWasseraufbereitung(int row, QLayout *parentLayout, QWidget *parent) :
    WdgAbstractProxy(bh->sud()->modelWasseraufbereitung(), row, parentLayout, parent),
    ui(new Ui::WdgWasseraufbereitung)
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
    pal.setColor(QPalette::Window, gSettings->colorWasser);
    setPalette(pal);

    updateValues();
    connect(bh, &Brauhelfer::modified, this, &WdgWasseraufbereitung::updateValues);
}

WdgWasseraufbereitung::~WdgWasseraufbereitung()
{
    delete ui;
}

void WdgWasseraufbereitung::checkEnabled()
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    mEnabled = status == Brauhelfer::SudStatus::Rezept;
    if (gSettings->ForceEnabled)
        mEnabled = true;
}

void WdgWasseraufbereitung::updateValues()
{
    checkEnabled();

    ui->tbName->setEnabled(mEnabled);
    ui->btnAuswahl->setVisible(mEnabled);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbMenge->setReadOnly(!mEnabled);
    ui->tbRestalkalitaet->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);

    double menge = data(ModelWasseraufbereitung::ColMenge).toDouble();
    if (!ui->tbName->hasFocus())
        ui->tbName->setText(data(ModelWasseraufbereitung::ColName).toString());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(menge);
    ui->tbMengeGesamt->setValue(menge * bh->sud()->geterg_W_Gesamt());
    ui->tbMengeHg->setValue(menge * bh->sud()->geterg_WHauptguss());
    ui->tbMengeNg->setValue(menge * bh->sud()->geterg_WNachguss());
    double w = bh->sud()->getMengeSollHgf();
    ui->tbMengeHgf->setValue(menge * w);
    ui->tbMengeHgf->setVisible(w > 0);
    ui->lblHgf->setVisible(w > 0);
    ui->lblEinheitHgf->setVisible(ui->tbMengeHgf->value() > 0);
    if (!ui->tbRestalkalitaet->hasFocus())
        ui->tbRestalkalitaet->setValue(data(ModelWasseraufbereitung::ColRestalkalitaet).toDouble());
    if (data(ModelWasseraufbereitung::ColFaktor).toDouble() >= 0)
    {
        ui->tbRestalkalitaet->setMinimum(0);
        ui->tbRestalkalitaet->setMaximum(99);
    }
    else
    {
        ui->tbRestalkalitaet->setMinimum(-99);
        ui->tbRestalkalitaet->setMaximum(0);
    }
    QString einheit = MainWindow::Einheiten[data(ModelWasseraufbereitung::ColEinheit).toInt()];
    ui->lblEinheit->setText(einheit + "/l");
    ui->lblEinheitGesamt->setText(einheit);
    ui->lblEinheitHg->setText(einheit);
    ui->lblEinheitNg->setText(einheit);
    ui->lblEinheitHgf->setText(einheit);

    ui->btnNachOben->setEnabled(mRow > 0);
    ui->btnNachUnten->setEnabled(mRow < mModel->rowCount() - 1);
}

void WdgWasseraufbereitung::on_tbName_textChanged(const QString &text)
{
    if (ui->tbName->hasFocus())
        setData(ModelWasseraufbereitung::ColName, text);
}

void WdgWasseraufbereitung::on_btnAuswahl_clicked()
{
    int einheit = data(ModelWasseraufbereitung::ColEinheit).toInt();
    double faktor = data(ModelWasseraufbereitung::ColFaktor).toDouble();
    DlgWasseraufbereitung dlg(ui->tbName->text(), einheit, faktor, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        setData(ModelWasseraufbereitung::ColName, dlg.name());
        setData(ModelWasseraufbereitung::ColEinheit, dlg.einheit());
        setData(ModelWasseraufbereitung::ColFaktor, dlg.faktor());
    }
}

void WdgWasseraufbereitung::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        setData(ModelWasseraufbereitung::ColMenge, value);
}

void WdgWasseraufbereitung::on_tbMengeGesamt_valueChanged(double value)
{
    if (ui->tbMengeGesamt->hasFocus())
        setData(ModelWasseraufbereitung::ColMenge, value / bh->sud()->geterg_W_Gesamt());
}

void WdgWasseraufbereitung::on_tbRestalkalitaet_valueChanged(double value)
{
    if (ui->tbRestalkalitaet->hasFocus())
        setData(ModelWasseraufbereitung::ColRestalkalitaet, value);
}

void WdgWasseraufbereitung::on_btnNachOben_clicked()
{
    moveUp();
}

void WdgWasseraufbereitung::on_btnNachUnten_clicked()
{
    moveDown();
}

void WdgWasseraufbereitung::on_btnLoeschen_clicked()
{
    remove();
}
