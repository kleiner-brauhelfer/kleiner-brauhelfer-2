#include "wdganhang.h"
#include "ui_wdganhang.h"
#include <QFileDialog>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

bool WdgAnhang::isImage(const QString pfad)
{
    QFileInfo fileInfo(pfad);
    QString ext = fileInfo.suffix();
    return (ext == "png" || ext == "svg" || ext == "gif" || ext == "jpg" || ext == "jpeg" || ext == "bmp");
}

WdgAnhang::WdgAnhang(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgAnhang),
    mIndex(index)
{
    mBasis = QDir(gSettings->databaseDir());
    ui->setupUi(this);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelAnhang(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgAnhang::~WdgAnhang()
{
    delete ui;
}

QVariant WdgAnhang::data(const QString &fieldName) const
{
    return bh->sud()->modelAnhang()->data(mIndex, fieldName);
}

bool WdgAnhang::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelAnhang()->setData(mIndex, fieldName, value);
}

void WdgAnhang::updateValues()
{
    QString pfad = data("Pfad").toString();
    if (!ui->lineEdit_Pfad->hasFocus())
        ui->lineEdit_Pfad->setText(pfad);
    ui->checkBox_Relativ->setChecked(QDir::isRelativePath(pfad));
    if (isImage(pfad))
    {
        QPixmap pix(getFullPfad());
        if (!pix.isNull())
        {
            ui->bild->setPixmap(pix.scaled(ui->bild->width(), ui->bild->height(), Qt::KeepAspectRatio));
            ui->bild->setVisible(true);
        }
        else
        {
            ui->bild->setVisible(false);
        }
    }
    else
    {
        ui->bild->setVisible(false);
    }
}

QString WdgAnhang::getPfad() const
{
    return data("Pfad").toString();
}

QString WdgAnhang::getFullPfad() const
{
    QString pfad = getPfad();
    if (QDir::isRelativePath(pfad))
        return QDir::cleanPath(mBasis.filePath(pfad));
    else
        return pfad;
}

void WdgAnhang::openDialog()
{
    QString pfad = QFileDialog::getOpenFileName(this, tr("Anhang auswählen"), getFullPfad());
    if (!pfad.isEmpty())
    {
        if (ui->checkBox_Relativ->isChecked())
            setData("Pfad", mBasis.relativeFilePath(pfad));
        else
            setData("Pfad", pfad);
    }
}

void WdgAnhang::on_lineEdit_Pfad_textChanged(const QString &pfad)
{
    if (ui->lineEdit_Pfad->hasFocus())
    {
        if (ui->checkBox_Relativ->isChecked())
            setData("Pfad", mBasis.relativeFilePath(pfad));
        else
            setData("Pfad", pfad);
    }
}

void WdgAnhang::on_checkBox_Relativ_clicked()
{
    QString pfad = getPfad();
    if (ui->checkBox_Relativ->isChecked())
    {
        if (QDir::isRelativePath(pfad))
            setData("Pfad",pfad);
        else
            setData("Pfad", mBasis.relativeFilePath(pfad));
    }
    else
    {
        setData("Pfad", getFullPfad());
    }
}

void WdgAnhang::on_pushButton_Browse_clicked()
{
    openDialog();
}

void WdgAnhang::on_btnLoeschen_clicked()
{
    bh->sud()->modelAnhang()->removeRow(mIndex);
}
