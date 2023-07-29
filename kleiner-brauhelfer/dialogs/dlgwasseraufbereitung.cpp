#include "dlgwasseraufbereitung.h"
#include "ui_dlgwasseraufbereitung.h"
#include "brauhelfer.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

DlgWasseraufbereitung::DlgWasseraufbereitung(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgWasseraufbereitung)
{
    ui->setupUi(this);
    mList.append({tr("Milchsäure"), static_cast<int>(Brauhelfer::Einheit::ml), -0.0266, 80});
    mList.append({tr("Phosphorsäure"), static_cast<int>(Brauhelfer::Einheit::ml), -0.0332, 10});
    mList.append({tr("Salzsäure"), static_cast<int>(Brauhelfer::Einheit::ml), -0.0124, 10});
    mList.append({tr("Schwefelsäure"), static_cast<int>(Brauhelfer::Einheit::ml), -0.0164, 10});
    mList.append({tr("Sauermalz"), static_cast<int>(Brauhelfer::Einheit::g), -0.0321, 2});
    mList.append({tr("Natriumhydrogenkarbonat (NaHCO3)"), static_cast<int>(Brauhelfer::Einheit::g), 0.03, 100});
    mList.append({tr("Natriumchlorid (NaCl)"), static_cast<int>(Brauhelfer::Einheit::g), 0, 100});
    mList.append({tr("Calciumsulfat-Dihydrat (CaSO4)"), static_cast<int>(Brauhelfer::Einheit::g), -0.1074, 100});
    mList.append({tr("Calciumchlorid-Dihydrat (CaCl2)"), static_cast<int>(Brauhelfer::Einheit::g), -0.0917, 100});
    mList.append({tr("Magnesiumchlorid-Hexahydrat (MgCl2)"), static_cast<int>(Brauhelfer::Einheit::g), -0.251, 100});
    mList.append({tr("Magnesiumsulfat-Heptahydrat (MgSO4)"), static_cast<int>(Brauhelfer::Einheit::g), -0.3077, 100});
    mList.append({tr("<manuell>"), static_cast<int>(Brauhelfer::Einheit::ml), 0, 100});
    for (const auto& it : qAsConst(mList))
        ui->cbAuswahl->addItem(it.name);
    adjustSize();
}

DlgWasseraufbereitung::DlgWasseraufbereitung(const QString& name, int einheit, double faktor, QWidget *parent) :
    DlgWasseraufbereitung(parent)
{
       ui->cbAuswahl->setCurrentIndex(mList.size() - 1);
       ui->tbName->setText(name);
       ui->cbEinheit->setCurrentIndex(einheit);
       ui->tbFaktor->setValue(faktor);
}

DlgWasseraufbereitung::~DlgWasseraufbereitung()
{
    delete ui;
}

QString DlgWasseraufbereitung::name() const
{
    return ui->tbName->text();
}

int DlgWasseraufbereitung::einheit() const
{
    return ui->cbEinheit->currentIndex();
}

double DlgWasseraufbereitung::faktor() const
{
    return ui->tbFaktor->value();
}

void DlgWasseraufbereitung::on_cbAuswahl_currentIndexChanged(int index)
{
    if (index >= 0 && index < mList.size())
    {
        ui->cbEinheit->setCurrentIndex(mList[index].einheit);
        ui->tbKonzentration->setValue(mList[index].konzentration);
        if (index == mList.size() - 1)
        {
            ui->tbName->clear();
            ui->tbKonzentration->setReadOnly(true);
            ui->tbFaktor->setReadOnly(false);
            ui->cbEinheit->setEnabled(true);
        }
        else
        {
            ui->tbName->setText(mList[index].name + QString(" (%1%)").arg(ui->tbKonzentration->value()));
            ui->tbKonzentration->setReadOnly(false);
            ui->tbFaktor->setReadOnly(true);
            ui->cbEinheit->setEnabled(false);
        }
        ui->tbFaktor->setValue(mList[index].faktor / ui->tbKonzentration->value()* 100);
    }
}

void DlgWasseraufbereitung::on_tbKonzentration_valueChanged(int value)
{
    if (ui->tbKonzentration->hasFocus())
    {
        ui->tbName->setText(mList[ui->cbAuswahl->currentIndex()].name + QString(" (%1%)").arg(value));
        ui->tbFaktor->setValue(mList[ui->cbAuswahl->currentIndex()].faktor / value * 100);
    }
}

void DlgWasseraufbereitung::on_cbEinheit_currentTextChanged(const QString &text)
{
    ui->lblEinheitFaktor->setText(text + "/l");
}
