#include "dlgsudteilen.h"
#include "ui_dlgsudteilen.h"
#include "settings.h"

extern Settings* gSettings;

DlgSudTeilen::DlgSudTeilen(const QString &name, double amount, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSudTeilen),
    mTotalAmount(amount)
{
    ui->setupUi(this);
    ui->lineEdit_Name1->setText(name + " I");
    ui->lineEdit_Name2->setText(name + " II");
    ui->doubleSpinBox_Amount1->setMaximum(mTotalAmount);
    ui->doubleSpinBox_Amount2->setMaximum(mTotalAmount);
    on_horizontalSlider_Percent_valueChanged(50);
    ui->horizontalSlider_Percent->setFocus();
    adjustSize();
    gSettings->beginGroup(staticMetaObject.className());
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    gSettings->endGroup();
}

DlgSudTeilen::~DlgSudTeilen()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}

double DlgSudTeilen::prozent() const
{
    return ui->horizontalSlider_Percent->value() / 100.0;
}

QString DlgSudTeilen::nameTeil1() const
{
    return ui->lineEdit_Name1->text();
}

QString DlgSudTeilen::nameTeil2() const
{
    return ui->lineEdit_Name2->text();
}

void DlgSudTeilen::on_horizontalSlider_Percent_valueChanged(int position)
{
    if (!ui->spinBox_Percent1->hasFocus())
        ui->spinBox_Percent1->setValue(position);
    if (!ui->spinBox_Percent2->hasFocus())
        ui->spinBox_Percent2->setValue(100 - position);
    if (!ui->doubleSpinBox_Amount1->hasFocus())
        ui->doubleSpinBox_Amount1->setValue((position * mTotalAmount) / 100);
    if (!ui->doubleSpinBox_Amount2->hasFocus())
        ui->doubleSpinBox_Amount2->setValue(((100 - position) * mTotalAmount) / 100);
}

void DlgSudTeilen::on_spinBox_Percent1_valueChanged(int position)
{
    if (ui->spinBox_Percent1->hasFocus())
        ui->horizontalSlider_Percent->setValue(position);
}

void DlgSudTeilen::on_spinBox_Percent2_valueChanged(int position)
{
    if (ui->spinBox_Percent2->hasFocus())
        ui->horizontalSlider_Percent->setValue(100 - position);
}

void DlgSudTeilen::on_doubleSpinBox_Amount1_valueChanged(double amount)
{
    if (ui->doubleSpinBox_Amount1->hasFocus())
        ui->horizontalSlider_Percent->setValue((int)((amount / mTotalAmount) * 100));
}

void DlgSudTeilen::on_doubleSpinBox_Amount2_valueChanged(double amount)
{
    if (ui->doubleSpinBox_Amount2->hasFocus())
        ui->horizontalSlider_Percent->setValue(100 - (int)((amount / mTotalAmount) * 100));
}
