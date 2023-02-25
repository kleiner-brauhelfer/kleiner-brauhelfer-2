#include "dlgimportexport.h"
#include "ui_dlgimportexport.h"
#include <QMessageBox>
#include <QFileDialog>
#include "brauhelfer.h"
#include "settings.h"
#include "importexport.h"

#if QT_NETWORK_LIB
#include <QNetworkAccessManager>
#include <QNetworkReply>
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgImportExport::DlgImportExport(bool import, int row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgImportExport),
    mImport(import),
    mRow(row)
{
    ui->setupUi(this);
    ui->gpKbhExport->setVisible(false);
    adjustSize();
    gSettings->beginGroup(staticMetaObject.className());
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    switch (gSettings->value(mImport ? "formatImport" : "formatExport", 0).toInt())
    {
    case 0:
        ui->rbFormatKbh->setChecked(true);
        on_rbFormatKbh_clicked();
        break;
    case 1:
        ui->rbFormatMmum->setChecked(true);
        on_rbFormatMmum_clicked();
        break;
    case 2:
        ui->rbFormatBeerxml->setChecked(true);
        on_rbFormatBeerxml_clicked();
        break;
    case 3:
        ui->rbFormatBrautomat->setChecked(true);
        on_rbFormatBrautomat_clicked();
        break;
    }
    gSettings->endGroup();
    ui->textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->textEdit->setTabStopDistance(QFontMetrics(ui->textEdit->font()).horizontalAdvance("  "));
   #else
    ui->textEdit->setTabStopDistance(2 * QFontMetrics(ui->textEdit->font()).width(' '));
   #endif
  #endif
    setWindowTitle(mImport ? tr("Rezept importieren") : tr("\"%1\" exportieren").arg(bh->modelSud()->data(mRow, ModelSud::ColSudname).toString()));
    ui->rbFormatBrautomat->setVisible(!mImport);
    ui->gpImport->setVisible(mImport);

    connect(ui->cbSud, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbMalzschuettung, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbHopfengaben, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbHefegaben, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbWeitereZutatenGaben, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbRasten, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbWasseraufbereitung, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbSchnellgaerverlauf, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbHauptgaerverlauf, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbNachgaerverlauf, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbBewertungen, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbEtiketten, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbAnhang, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
    connect(ui->cbTags, SIGNAL(clicked()), this, SLOT(on_rbFormatKbh_clicked()));
}

DlgImportExport::~DlgImportExport()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("size", geometry().size());
    if (ui->rbFormatKbh->isChecked())
        gSettings->setValue(mImport ? "formatImport" : "formatExport", 0);
    else if (ui->rbFormatMmum->isChecked())
        gSettings->setValue(mImport ? "formatImport" : "formatExport", 1);
    else if (ui->rbFormatBeerxml->isChecked())
        gSettings->setValue(mImport ? "formatImport" : "formatExport", 2);
    else if (ui->rbFormatBrautomat->isChecked())
        gSettings->setValue(mImport ? "formatImport" : "formatExport", 3);
    gSettings->endGroup();
    delete ui;
}

void DlgImportExport::accept()
{
    if (!ui->textEdit->toPlainText().isEmpty())
    {
        if (mImport)
        {
            if (importieren())
                QDialog::accept();
        }
        else
        {
            if (exportieren())
                QDialog::accept();
        }
    }
}

int DlgImportExport::row() const
{
    return mRow;
}

void DlgImportExport::on_btnOeffnen_clicked()
{
    oeffnen();
}

void DlgImportExport::on_btnDownload_clicked()
{
    if (!ui->tbUrl->text().isEmpty())
        download(ui->tbUrl->text());
}

void DlgImportExport::on_rbFormatKbh_clicked()
{
    if (!mImport)
    {
        QStringList exclude;
        if (!ui->cbSud->isChecked())
            exclude.append("Sud");
        if (!ui->cbMalzschuettung->isChecked())
            exclude.append("Malzschuettung");
        if (!ui->cbHopfengaben->isChecked())
            exclude.append("Hopfengaben");
        if (!ui->cbHefegaben->isChecked())
            exclude.append("Hefegaben");
        if (!ui->cbWeitereZutatenGaben->isChecked())
            exclude.append("WeitereZutatenGaben");
        if (!ui->cbRasten->isChecked())
            exclude.append("Rasten");
        if (!ui->cbWasseraufbereitung->isChecked())
            exclude.append("Wasseraufbereitung");
        if (!ui->cbSchnellgaerverlauf->isChecked())
            exclude.append("Schnellgaerverlauf");
        if (!ui->cbHauptgaerverlauf->isChecked())
            exclude.append("Hauptgaerverlauf");
        if (!ui->cbNachgaerverlauf->isChecked())
            exclude.append("Nachgaerverlauf");
        if (!ui->cbBewertungen->isChecked())
            exclude.append("Bewertungen");
        if (!ui->cbEtiketten->isChecked())
            exclude.append("Etiketten");
        if (!ui->cbAnhang->isChecked())
            exclude.append("Anhang");
        if (!ui->cbTags->isChecked())
            exclude.append("Tags");
        QByteArray content = ImportExport::exportKbh(bh, mRow, exclude);
        ui->textEdit->setPlainText(QString::fromUtf8(content));
        ui->gpKbhExport->setVisible(true);
    }
}

void DlgImportExport::on_rbFormatMmum_clicked()
{
    if (!mImport)
    {
        QByteArray content = ImportExport::exportMaischeMalzundMehr(bh, mRow);
        ui->textEdit->setPlainText(QString::fromUtf8(content));
        ui->gpKbhExport->setVisible(false);
    }
}

void DlgImportExport::on_rbFormatBeerxml_clicked()
{
    if (!mImport)
    {
        QByteArray content = ImportExport::exportBeerXml(bh, mRow);
        ui->textEdit->setPlainText(QString::fromUtf8(content));
        ui->gpKbhExport->setVisible(false);
    }
}

bool DlgImportExport::oeffnen(const QString& filePath_)
{
    bool ret = false;
    gSettings->beginGroup("General");
    QString filePath = filePath_;
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    if (filePath.isEmpty())
    {
        filePath = QFileDialog::getOpenFileName(this, tr("Rezept Import"),
                                                path, "Rezept (*.json *xml);;Alle Dateien (*.*)");
    }
    ui->tbDatei->setText(filePath);
    ui->textEdit->clear();
    if (!filePath.isEmpty())
    {
        QFileInfo fileInfo(filePath);
        gSettings->setValue("exportPath", fileInfo.absolutePath());
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray content = file.readAll();
            file.close();
            ui->textEdit->setPlainText(QString::fromUtf8(content));
            if (fileInfo.suffix() == "xml")
                ui->rbFormatBeerxml->setChecked(true);
            ret = true;
        }
        else
        {
            QMessageBox::warning(this, tr("Rezept Import"), tr("Die Datei konnte nicht geöffnet werden."));
        }
    }
    gSettings->endGroup();
    return ret;
}

bool DlgImportExport::download(const QString &url)
{
    QEventLoop loop;
    QByteArray content;
    QNetworkRequest request(url);
    QNetworkAccessManager mNetManager;
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    mNetManager.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
  #endif
    QNetworkReply *reply = mNetManager.get(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError)
        content = reply->readAll();
    if (content.isEmpty())
    {
        ui->textEdit->clear();
        QMessageBox::critical(nullptr, windowTitle(),
                              tr("Das Rezept konnte nicht heruntergeladen werden.") +
                              "\n\n" + reply->errorString() + ".");
        return false;
    }
    else
    {
        ui->textEdit->setPlainText(QString::fromUtf8(content));
    }
    delete reply;
    return true;
}

bool DlgImportExport::importieren()
{
    QByteArray content = ui->textEdit->toPlainText().toUtf8();

    if (ui->rbFormatKbh->isChecked())
        mRow = ImportExport::importKbh(bh, content);
    else if (ui->rbFormatMmum->isChecked())
        mRow = ImportExport::importMaischeMalzundMehr(bh, content);
    else
        mRow = ImportExport::importBeerXml(bh, content);

    if (mRow >= 0)
    {
        QMessageBox::information(this, tr("Rezept Import"), tr("Das Rezept wurde erfolgreich importiert."));
        return true;
    }
    else
    {
        QMessageBox::warning(this, tr("Rezept Import"), tr("Das Rezept konnte nicht importiert werden."));
        return false;
    }
}

bool DlgImportExport::exportieren()
{
    bool ret = false;
    QByteArray content = ui->textEdit->toPlainText().toUtf8();

    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();

    QString filter;
    if (ui->rbFormatKbh->isChecked())
        filter = "kleiner-brauhelfer (*.json)";
    else if (ui->rbFormatMmum->isChecked())
        filter = "MaischeMalzundMehr (*.json)";
    else if (ui->rbFormatBeerxml->isChecked())
        filter = "BeerXML (*.xml)";
    else if (ui->rbFormatBrautomat->isChecked())
        filter = "kleiner-brauhelfer (*.json)";
    QString sudname = bh->modelSud()->data(mRow, ModelSud::ColSudname).toString();
    QString filePath = QFileDialog::getSaveFileName(this, tr("Sud Export"),
                                     path + "/" + sudname, filter);
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        QFile file(filePath);
        if (file.open(QFile::WriteOnly | QFile::Text))
        {
            file.write(content);
            file.close();
            ret = true;
            QMessageBox::information(this, tr("Sud Export"), tr("Der Sud wurde erfolgreich exportiert."));
        }
        else
        {
            QMessageBox::warning(this, tr("Sud Export"), tr("Die Datei konnte nicht geschrieben werden."));
        }
    }
    gSettings->endGroup();
    return ret;
}

void DlgImportExport::on_rbFormatBrautomat_clicked()
{
    if (!mImport)
    {
        QByteArray content = ImportExport::exportBrautomat(bh, mRow);
        ui->textEdit->setPlainText(QString::fromUtf8(content));
        ui->gpKbhExport->setVisible(false);
    }
}

