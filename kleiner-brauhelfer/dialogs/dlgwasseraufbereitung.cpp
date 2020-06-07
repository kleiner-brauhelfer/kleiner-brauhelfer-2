#include "dlgwasseraufbereitung.h"
#include "ui_dlgwasseraufbereitung.h"
#include "brauhelfer.h"

DlgWasseraufbereitung::DlgWasseraufbereitung(QWidget *parent) :
    QDialog(parent),
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
    mList.append({tr("Calciumsulfat (CaSO4)"), static_cast<int>(Brauhelfer::Einheit::g), -0.1074, 100});
    mList.append({tr("Calciumchlorid (CaCl2)"), static_cast<int>(Brauhelfer::Einheit::g), -0.0917, 100});
    mList.append({tr("Magnesiumchlorid (MgCl2)"), static_cast<int>(Brauhelfer::Einheit::g), -0.251, 100});
    mList.append({tr("Magnesiumsulfat (MgSO4)"), static_cast<int>(Brauhelfer::Einheit::g), -0.3077, 100});
    for (const auto& it : mList)
        ui->cbAuswahl->addItem(it.name);
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
    return mFaktor;
}

void DlgWasseraufbereitung::on_cbAuswahl_currentIndexChanged(int index)
{
    if (index >= 0 && index < mList.size())
    {
        ui->cbEinheit->setCurrentIndex(mList[index].einheit);
        ui->tbKonzentration->setValue(mList[index].konzentration);
        ui->tbName->setText(mList[index].name + tr(" (%1%)").arg(ui->tbKonzentration->value()));
        mFaktor = mList[index].faktor / ui->tbKonzentration->value()* 100;
    }
}

void DlgWasseraufbereitung::on_tbKonzentration_valueChanged(int value)
{
    if (ui->tbKonzentration->hasFocus())
    {
        ui->tbName->setText(mList[ui->cbAuswahl->currentIndex()].name + tr(" (%1%)").arg(value));
        mFaktor = mList[ui->cbAuswahl->currentIndex()].faktor / value * 100;
    }
}
