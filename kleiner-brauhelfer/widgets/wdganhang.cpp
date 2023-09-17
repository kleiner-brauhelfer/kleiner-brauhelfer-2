#include "wdganhang.h"
#include "ui_wdganhang.h"
#include <QFileDialog>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
#include "commands/undostack.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

bool WdgAnhang::isImage(const QString& pfad)
{
    QFileInfo fileInfo(pfad);
    QString ext = fileInfo.suffix().toLower();
    return (ext == QStringLiteral("png") || ext == QStringLiteral("svg")
            || ext == QStringLiteral("gif") || ext == QStringLiteral("jpg")
            || ext == QStringLiteral("jpeg") || ext == QStringLiteral("bmp"));
}

WdgAnhang::WdgAnhang(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgAnhang),
    mIndex(index)
{
    mBasis = QDir(gSettings->databaseDir());
    ui->setupUi(this);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, gSettings->colorAnhang);
    setPalette(pal);
    updateValues();
    connect(bh, &Brauhelfer::discarded, this, &WdgAnhang::updateValues);
    connect(bh->sud()->modelAnhang(), &ProxyModel::modified, this, &WdgAnhang::updateValues);
}

WdgAnhang::~WdgAnhang()
{
    delete ui;
}

QVariant WdgAnhang::data(int col) const
{
    return bh->sud()->modelAnhang()->data(mIndex, col);
}

bool WdgAnhang::setData(int col, const QVariant &value)
{
    gUndoStack->push(new SetModelDataCommand(bh->sud()->modelAnhang(), mIndex, col, value));
    return true;
}

void WdgAnhang::updateValues()
{
    QString pfad = data(ModelAnhang::ColPfad).toString();
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
    return data(ModelAnhang::ColPfad).toString();
}

QString WdgAnhang::getFullPfad() const
{
    QString pfad = getPfad();
    if (QDir::isRelativePath(pfad))
        return QDir::cleanPath(mBasis.filePath(pfad));
    else
        return pfad;
}

void WdgAnhang::remove()
{
    bh->sud()->modelAnhang()->removeRow(mIndex);
}

void WdgAnhang::openDialog()
{
    QString pfad = QFileDialog::getOpenFileName(this, tr("Anhang auswählen"), getFullPfad());
    if (!pfad.isEmpty())
    {
        if (ui->checkBox_Relativ->isChecked())
            setData(ModelAnhang::ColPfad, mBasis.relativeFilePath(pfad));
        else
            setData(ModelAnhang::ColPfad, pfad);
    }
}

void WdgAnhang::on_lineEdit_Pfad_textChanged(const QString &pfad)
{
    if (ui->lineEdit_Pfad->hasFocus())
    {
        if (ui->checkBox_Relativ->isChecked())
            setData(ModelAnhang::ColPfad, mBasis.relativeFilePath(pfad));
        else
            setData(ModelAnhang::ColPfad, pfad);
    }
}

void WdgAnhang::on_checkBox_Relativ_clicked()
{
    QString pfad = getPfad();
    if (ui->checkBox_Relativ->isChecked())
    {
        if (QDir::isRelativePath(pfad))
            setData(ModelAnhang::ColPfad,pfad);
        else
            setData(ModelAnhang::ColPfad, mBasis.relativeFilePath(pfad));
    }
    else
    {
        setData(ModelAnhang::ColPfad, getFullPfad());
    }
}

void WdgAnhang::on_pushButton_Browse_clicked()
{
    openDialog();
}

void WdgAnhang::on_btnOpen_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(getFullPfad()));
}

void WdgAnhang::on_btnLoeschen_clicked()
{
    remove();
}
