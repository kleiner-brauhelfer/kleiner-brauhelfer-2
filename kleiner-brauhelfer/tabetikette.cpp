#include "tabetikette.h"
#include "ui_tabetikette.h"
#include <QMessageBox>
#include <QDirIterator>
#include <QFile>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QPrinter>
#include <QSvgRenderer>
#include <QDesktopServices>
#include <QJsonDocument>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"
#include "helper/mustache.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabEtikette::TabEtikette(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabEtikette),
    mTemplateFilePath("")
{
    ui->setupUi(this);

    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
  #endif
    ui->btnSaveTemplate->setPalette(gSettings->paletteErrorButton);
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
  #endif

    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());

    connect(bh, SIGNAL(discarded()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateTemplateTags()));
    connect(bh->sud()->modelFlaschenlabelTags(), SIGNAL(modified()), this, SLOT(updateTemplateTags()));
    connect(bh->sud()->modelFlaschenlabel(), SIGNAL(modified()),this, SLOT(updateValues()));
    connect(bh->sud()->modelAnhang(), SIGNAL(layoutChanged()), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateAuswahlListe()));

    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    updateAll();
}

TabEtikette::~TabEtikette()
{
    delete ui;
}

void TabEtikette::onTabActivated()
{
    updateAll();
}

void TabEtikette::updateAll()
{
    updateAuswahlListe();
    ui->cbAuswahl->setCurrentIndex(-1);
    updateValues();
    updateTemplateFilePath();
    updateTemplateTags();
}

void TabEtikette::updateAuswahlListe()
{
    ui->cbAuswahl->clear();

    int colPfad = bh->sud()->modelAnhang()->fieldIndex("Pfad");
    for (int row = 0; row < bh->sud()->modelAnhang()->rowCount(); ++row)
    {
        QString pfad = bh->sud()->modelAnhang()->index(row, colPfad).data().toString();
        if (pfad.endsWith(".svg"))
        {
            QFileInfo fi(pfad);
            ui->cbAuswahl->addItem(fi.fileName(), fi.filePath());
        }
    }

    QDirIterator it(gSettings->dataDir(), QStringList() << "*.svg");
    while (it.hasNext())
    {
        it.next();
        ui->cbAuswahl->addItem(it.fileName(), it.filePath());
    }
}

QString TabEtikette::generateSvg(const QString &svg)
{
    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(mTemplateTags);
    return renderer.render(svg, &context);
}

void TabEtikette::updateTemplateFilePath()
{
    QDir baseDir(gSettings->databaseDir());
    QFileInfo fi(data("Auswahl").toString());
    if (!fi.isFile())
        mTemplateFilePath = QString();
    else if (QDir::isRelativePath(fi.filePath()))
        mTemplateFilePath =  QDir::cleanPath(baseDir.filePath(fi.filePath()));
    else
        mTemplateFilePath = fi.filePath();
}

void TabEtikette::updateSvg()
{
    if (mTemplateFilePath.isEmpty())
    {
        ui->viewSvg->clear();
        return;
    }

    if (ui->cbEditMode->isChecked())
    {
        if (ui->cbTagsErsetzen->isChecked())
        {
            QString svg = generateSvg(ui->tbTemplate->toPlainText());
            QTemporaryFile file_svg;
            if (file_svg.open())
                file_svg.write(svg.toUtf8());
            ui->viewSvg->openFile(file_svg.fileName());
            file_svg.close();
        }
        else
        {
            QTemporaryFile file_template;
            file_template.open();
            file_template.write(ui->tbTemplate->toPlainText().toUtf8());
            ui->viewSvg->openFile(file_template.fileName());
            file_template.close();
        }
    }
    else
    {
        if (ui->cbTagsErsetzen->isChecked())
        {
            QFile file(mTemplateFilePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return;
            QString svg_template = file.readAll();
            file.close();

            QString svg = generateSvg(svg_template);
            QTemporaryFile file_svg;
            if (file_svg.open())
                file_svg.write(svg.toUtf8());
            ui->viewSvg->openFile(file_svg.fileName());
            file_svg.close();
        }
        else
        {
            ui->viewSvg->openFile(mTemplateFilePath);
        }
    }
    ui->viewSvg->setViewOutline(false);
}

void TabEtikette::updateTags()
{
    if (ui->tbTags->isVisible())
    {
        QJsonDocument json = QJsonDocument::fromVariant(mTemplateTags);
        ui->tbTags->setPlainText(json.toJson());
    }
}

void TabEtikette::updateTemplateTags()
{
    if (!isTabActive())
        return;
    mTemplateTags.clear();
    TemplateTags::erstelleTagListe(mTemplateTags, TemplateTags::TagRezept | TemplateTags::TagSud, bh->sud()->row());
    updateTags();
    updateSvg();
}

bool TabEtikette::checkSave()
{
    if (ui->btnSaveTemplate->isVisible())
    {
        int ret = QMessageBox::question(this, tr("Änderungen speichern?"),
                                        tr("Sollen die Änderungen gespeichert werden?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
            on_btnSaveTemplate_clicked();
        else if (ret == QMessageBox::Cancel)
            return true;
    }
    return false;
}

QVariant TabEtikette::data(const QString &fieldName) const
{
    return bh->sud()->modelFlaschenlabel()->data(0, fieldName);
}

bool TabEtikette::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelFlaschenlabel()->setData(0, fieldName, value);
}

void TabEtikette::on_cbAuswahl_activated(int index)
{
    if (checkSave())
        return;
    ui->btnSaveTemplate->setVisible(false);
    if (bh->sud()->modelFlaschenlabel()->rowCount() == 0)
    {
        QVariantMap values({{"SudID", bh->sud()->id()},
                            {"BreiteLabel", 185},
                            {"AnzahlLabels", 25},
                            {"AbstandLabels", 0},
                            {"SRandOben", 10},
                            {"SRandLinks", 5},
                            {"SRandRechts", 5},
                            {"SRandUnten", 15}});
        bh->sud()->modelFlaschenlabel()->append(values);
    }
    setData("Auswahl", ui->cbAuswahl->itemData(index).toString());
    updateTemplateFilePath();
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    updateSvg();
}

void TabEtikette::on_cbTagsErsetzen_stateChanged()
{
    updateSvg();
}

void TabEtikette::on_cbEditMode_clicked(bool checked)
{
    if (checkSave())
    {
        ui->cbEditMode->setChecked(true);
        return;
    }

    ui->tbTemplate->setVisible(checked);
    ui->tbTags->setVisible(checked);
    ui->splitter_1->setHandleWidth(checked ? 5 : 0);

    if (checked)
    {
        QFile file(mTemplateFilePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->tbTemplate->setPlainText(file.readAll());
            file.close();
        }
        else
        {
            ui->tbTemplate->setPlainText("");
        }
    }
    else
    {
        ui->tbTemplate->setPlainText("");
    }
    ui->btnSaveTemplate->setVisible(false);
    updateTags();
    updateSvg();
}

void TabEtikette::on_tbTemplate_textChanged()
{
    if (ui->tbTemplate->hasFocus())
    {
        updateSvg();
        ui->btnSaveTemplate->setVisible(true);
    }
}

void TabEtikette::on_btnSaveTemplate_clicked()
{
    QFile file(mTemplateFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write(ui->tbTemplate->toPlainText().toUtf8());
    file.close();
    ui->btnSaveTemplate->setVisible(false);
}

void TabEtikette::on_btnToPdf_clicked()
{
    if (mTemplateFilePath.isEmpty())
        return;

    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + bh->sud()->getSudname() + "_" + tr("Etikette") +  ".pdf", "PDF (*.pdf)");
    if (!fileName.isEmpty())
    {
        // pdf speichern
        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPrinter::A4);
        printer.setOrientation(QPrinter::Portrait);
        printer.setColorMode(QPrinter::Color);
        printer.setPageMargins(
          ui->spinBox_FLabel_RandLinks->value(), ui->spinBox_FLabel_RandOben->value(),
          ui->spinBox_FLabel_RandRechts->value(), ui->spinBox_FLabel_RandUnten->value(),
          QPrinter::Millimeter);
        printer.setFullPage(false);
        printer.setOutputFileName(fileName);
        printer.setOutputFormat(QPrinter::PdfFormat);
        //    printer.setOutputFormat(QPrinter::NativeFormat);

        const QSize sizeSVG = ui->viewSvg->svgSize();

        qreal seitenverhaeltnisSVG = qreal(sizeSVG.width()) / qreal(sizeSVG.height());
        qreal widthPageMM = printer.widthMM();
        qreal heightPageMM = printer.heightMM();
        qreal widthPagePx = printer.width();
        qreal faktorPxPerMM = widthPagePx / widthPageMM;

        qreal breiteMM = ui->spinBox_BreiteLabel->value();
        qreal SVGhoeheMM = (breiteMM / seitenverhaeltnisSVG);
        qreal hoehePx = SVGhoeheMM * faktorPxPerMM;
        qreal abstandMM = ui->spinBox_AbstandLabel->value();
        qreal abstandPx = abstandMM * faktorPxPerMM;

        // Gewünschte Anzahl
        int totalCount = ui->spinBox_AnzahlLabels->value();

        // Anzahl der Streifen pro Seite
        int countPerPage = int(heightPageMM / (SVGhoeheMM + abstandMM));

        QPainter painter(&printer);
        int zaehler = 0;
        // Anzahl Seiten
        int pageCount = int(qRound(double(totalCount) / double(countPerPage) + double(0.5)));

        QImage image(breiteMM * faktorPxPerMM, hoehePx, QImage::Format_ARGB32_Premultiplied);
        QPainter imagePainter(&image);
        image.fill(Qt::transparent);
        ui->viewSvg->renderer()->render(&imagePainter);

        for (int seite = 0; seite < pageCount; seite++)
        {
            for (int i = 0; i < countPerPage; i++)
            {
                if (zaehler >= totalCount)
                {
                    i = countPerPage;
                }
                else
                {
                    QRectF rect = QRectF(0, hoehePx * i + abstandPx * i, breiteMM * faktorPxPerMM, hoehePx);

                    //ui->viewSvg->renderer()->render(&painter, rect); // funktioniert nicht mit allen SVGs
                    painter.drawImage(rect, image);

                    QPen outline(Qt::lightGray, 5, Qt::DashDotDotLine);
                    outline.setCosmetic(true);
                    painter.setPen(outline);
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(rect);

                    zaehler++;
                }
            }
            if (zaehler < totalCount)
            {
                printer.newPage();
            }
        }
        painter.end();

        QFileInfo fi(fileName);
        gSettings->setValue("exportPath", fi.absolutePath());

        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    gSettings->endGroup();
}

void TabEtikette::updateValues()
{
    if (!ui->cbAuswahl->hasFocus())
    {
        QVariant auswahl = data("Auswahl");
        for (int i = 0; i < ui->cbAuswahl->count(); ++i)
        {
            if (ui->cbAuswahl->itemData(i) == auswahl)
            {
                ui->cbAuswahl->setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ui->spinBox_BreiteLabel->hasFocus())
        ui->spinBox_BreiteLabel->setValue(data("BreiteLabel").toInt());
    if (!ui->spinBox_AnzahlLabels->hasFocus())
        ui->spinBox_AnzahlLabels->setValue(data("AnzahlLabels").toInt());
    if (!ui->spinBox_AbstandLabel->hasFocus())
        ui->spinBox_AbstandLabel->setValue(data("AbstandLabels").toInt());
    if (!ui->spinBox_FLabel_RandOben->hasFocus())
        ui->spinBox_FLabel_RandOben->setValue(data("SRandOben").toInt());
    if (!ui->spinBox_FLabel_RandLinks->hasFocus())
        ui->spinBox_FLabel_RandLinks->setValue(data("SRandLinks").toInt());
    if (!ui->spinBox_FLabel_RandRechts->hasFocus())
        ui->spinBox_FLabel_RandRechts->setValue(data("SRandRechts").toInt());
    if (!ui->spinBox_FLabel_RandUnten->hasFocus())
        ui->spinBox_FLabel_RandUnten->setValue(data("SRandUnten").toInt());
}

void TabEtikette::on_spinBox_BreiteLabel_valueChanged(int value)
{
    if (ui->spinBox_BreiteLabel->hasFocus())
        setData("BreiteLabel", value);
}

void TabEtikette::on_spinBox_AnzahlLabels_valueChanged(int value)
{
    if (ui->spinBox_AnzahlLabels->hasFocus())
        setData("AnzahlLabels", value);
}

void TabEtikette::on_spinBox_AbstandLabel_valueChanged(int value)
{
    if (ui->spinBox_AbstandLabel->hasFocus())
        setData("AbstandLabels", value);
}

void TabEtikette::on_spinBox_FLabel_RandOben_valueChanged(int value)
{
    if (ui->spinBox_FLabel_RandOben->hasFocus())
        setData("SRandOben", value);
}

void TabEtikette::on_spinBox_FLabel_RandLinks_valueChanged(int value)
{
    if (ui->spinBox_FLabel_RandLinks->hasFocus())
        setData("SRandLinks", value);
}

void TabEtikette::on_spinBox_FLabel_RandRechts_valueChanged(int value)
{
    if (ui->spinBox_FLabel_RandRechts->hasFocus())
        setData("SRandRechts", value);
}

void TabEtikette::on_spinBox_FLabel_RandUnten_valueChanged(int value)
{
    if (ui->spinBox_FLabel_RandUnten->hasFocus())
        setData("SRandUnten", value);
}

void TabEtikette::on_btnLoeschen_clicked()
{
    if (bh->sud()->modelFlaschenlabel()->rowCount() > 0)
    {
        int ret = QMessageBox::question(this, tr("Etikette löschen?"),
                                        tr("Soll die Etikette gelöscht werden?"));
        if (ret == QMessageBox::Yes)
        {
            ui->cbAuswahl->setCurrentIndex(-1);
            bh->sud()->modelFlaschenlabel()->removeRow(0);
            updateAll();
        }
    }
}
