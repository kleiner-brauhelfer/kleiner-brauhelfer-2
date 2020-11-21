#include "tabetikette.h"
#include "ui_tabetikette.h"
#include <QMessageBox>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#ifdef QT_PRINTSUPPORT_LIB
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#endif
#include <QSvgRenderer>
#include <QDesktopServices>
#include <QJsonDocument>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"
#include "helper/mustache.h"
#include "model/checkboxdelegate.h"

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
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTemplate->setTabStopDistance(QFontMetrics(ui->tbTemplate->font()).horizontalAdvance("  "));
   #else
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
   #endif
  #endif
    ui->btnSaveTemplate->setPalette(gSettings->paletteErrorButton);
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTags->setTabStopDistance(QFontMetrics(ui->tbTags->font()).horizontalAdvance("  "));
   #else
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTags->font()).width(' '));
   #endif
  #endif

    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());

  #ifdef QT_PRINTSUPPORT_LIB
    gSettings->beginGroup("General");
    QPrinterInfo printerInfo = QPrinterInfo::printerInfo(gSettings->value("DefaultPrinterEtikette").toString());
    mPrinter = new QPrinter(printerInfo, QPrinter::HighResolution);
    mPrinter->setColorMode(QPrinter::Color);
    gSettings->endGroup();
  #endif

    TableView *table = ui->tableTags;
    table->setModel(bh->sud()->modelTags());
    table->cols.append({ModelTags::ColKey, true, false, 0, nullptr});
    table->cols.append({ModelTags::ColValue, true, false, -1, nullptr});
    table->cols.append({ModelTags::ColGlobal, true, false, 0, new CheckBoxDelegate(table)});
    table->build();

    connect(bh, SIGNAL(discarded()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateTemplateTags()));
    connect(bh->sud()->modelTags(), SIGNAL(modified()), this, SLOT(updateTemplateTags()));
    connect(bh->sud()->modelEtiketten(), SIGNAL(modified()),this, SLOT(updateValues()));
    connect(bh->sud()->modelAnhang(), SIGNAL(layoutChanged()), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateAuswahlListe()));

    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    updateAll();
}

TabEtikette::~TabEtikette()
{
    delete ui;
  #ifdef QT_PRINTSUPPORT_LIB
    delete mPrinter;
  #endif
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

    for (int row = 0; row < bh->sud()->modelAnhang()->rowCount(); ++row)
    {
        QString pfad = bh->sud()->modelAnhang()->index(row, ModelAnhang::ColPfad).data().toString();
        if (pfad.endsWith(".svg"))
        {
            QFileInfo fi(pfad);
            ui->cbAuswahl->addItem(fi.fileName(), fi.filePath());
        }
    }

    QDirIterator it(gSettings->dataDir(3), QStringList() << "*.svg");
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
    QFileInfo fi(data(ModelEtiketten::ColPfad).toString());
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

    QString currentPath = QDir::currentPath();
    QDir::setCurrent(QFileInfo(mTemplateFilePath).absolutePath());

    if (ui->cbEditMode->isChecked())
    {
        if (ui->cbTagsErsetzen->isChecked())
        {
            QString svg = generateSvg(ui->tbTemplate->toPlainText());
            ui->viewSvg->load(svg.toUtf8());
        }
        else
        {
            ui->viewSvg->load(ui->tbTemplate->toPlainText().toUtf8());
        }
    }
    else
    {
        if (ui->cbTagsErsetzen->isChecked())
        {
            QFile file(mTemplateFilePath);
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QString svg_template = file.readAll();
            file.close();
            QString svg = generateSvg(svg_template);
            ui->viewSvg->load(svg.toUtf8());
        }
        else
        {
            ui->viewSvg->load(mTemplateFilePath);
        }
    }
    ui->viewSvg->setViewOutline(false);

    QDir::setCurrent(currentPath);
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
    TemplateTags::erstelleTagListe(mTemplateTags, TemplateTags::TagAll, bh->sud()->row());
    mTemplateTags["N"] = "N";
    mTemplateTags["n"] = "n";
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

QVariant TabEtikette::data(int col) const
{
    return bh->sud()->modelEtiketten()->data(0, col);
}

bool TabEtikette::setData(int col, const QVariant &value)
{
    return bh->sud()->modelEtiketten()->setData(0, col, value);
}

void TabEtikette::on_cbAuswahl_activated(int index)
{
    if (checkSave())
        return;
    ui->btnSaveTemplate->setVisible(false);

    if (bh->sud()->modelEtiketten()->rowCount() == 0)
    {
        QMap<int, QVariant> values({{ModelEtiketten::ColSudID, bh->sud()->id()},
                                    {ModelEtiketten::ColPfad, ui->cbAuswahl->itemData(index).toString()}});
        bh->sud()->modelEtiketten()->append(values);
    }
    else
    {
        setData(ModelEtiketten::ColPfad, ui->cbAuswahl->itemData(index).toString());
    }

    updateValues();
    updateTemplateFilePath();
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    updateSvg();

    QRectF rect = ui->viewSvg->viewBoxF();
    setData(ModelEtiketten::ColBreite, rect.width());
    setData(ModelEtiketten::ColHoehe, rect.height());
}

void TabEtikette::on_btnOeffnen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("SVG auswählen"), "", tr("SVG (*.svg)"));
    if (!fileName.isEmpty())
    {
        QMap<int, QVariant> values({{ModelAnhang::ColSudID, bh->sud()->id()},
                                    {ModelAnhang::ColPfad, fileName}});
        bh->sud()->modelAnhang()->append(values);
        setData(ModelEtiketten::ColPfad, fileName);
        updateAll();
    }
}

void TabEtikette::on_btnAktualisieren_clicked()
{
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
    ui->lblFilePath->setVisible(checked);
    ui->splitter_1->setHandleWidth(checked ? 5 : 0);

    if (checked)
    {
        QFile file(mTemplateFilePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->tbTemplate->setPlainText(file.readAll());
            ui->lblFilePath->setText(file.fileName());
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

#ifdef QT_PRINTSUPPORT_LIB
void TabEtikette::onPrinterPaintRequested(QPrinter *printer)
{
    qreal faktorPxPerMM = printer->width() / printer->widthMM();
    int labelWidth = static_cast<int>(ui->tbLabelBreite->value() * faktorPxPerMM);
    int labelHeight = static_cast<int>(ui->tbLabelHoehe->value() * faktorPxPerMM);
    int labelDistHor = static_cast<int>(ui->tbAbstandHor->value() * faktorPxPerMM);
    int labelDistVert = static_cast<int>(ui->tbAbstandVert->value() * faktorPxPerMM);

    QFile file(mTemplateFilePath);
    if (!file.exists())
        return;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString svg_template = file.readAll();
    file.close();

    SvgView svgView;
    QImage image(labelWidth, labelHeight, QImage::Format_ARGB32_Premultiplied);
    QPainter imagePainter(&image);
    image.fill(Qt::transparent);

    QString currentPath = QDir::currentPath();
    QDir::setCurrent(QFileInfo(mTemplateFilePath).absolutePath());

    Mustache::Renderer mustacheRenderer;
    mTemplateTags["N"] = ui->tbAnzahl->value();
    bool constSvg = !svg_template.contains("{{n}}");
    if (constSvg)
    {
        Mustache::QtVariantContext context(mTemplateTags);
        QString svg = mustacheRenderer.render(svg_template, &context);
        svgView.load(svg.toUtf8());
    }

    int x = 0, y = 0;
    QPainter painter(printer);
    QPen outline(Qt::darkRed, 0.5*faktorPxPerMM, Qt::DashDotDotLine);
    for (int i = 0; i < ui->tbAnzahl->value(); i++)
    {
        if (x + labelWidth > printer->width())
        {
            x = 0.0;
            y += labelHeight + labelDistVert;
        }

        if (y + labelHeight > printer->height())
        {
            printer->newPage();
            x = 0.0;
            y = 0.0;
        }

        QRect rect = QRect(x, y, labelWidth, labelHeight);

        if (ui->cbDividingLine->isChecked())
        {
            painter.setPen(outline);
            painter.drawLine(0, rect.top() - outline.width(), printer->width(), rect.top() - outline.width());
            painter.drawLine(0, rect.bottom() + outline.width(), printer->width(), rect.bottom() + outline.width());
            painter.drawLine(rect.left() - outline.width(), 0, rect.left() - outline.width(), printer->height());
            painter.drawLine(rect.right() + outline.width(), 0, rect.right() + outline.width(), printer->height());
        }

        if (!constSvg)
        {
            mTemplateTags["n"] = i + 1;
            Mustache::QtVariantContext context(mTemplateTags);
            QString svg = mustacheRenderer.render(svg_template, &context);
            svgView.load(svg.toUtf8());
        }

        if (ui->cbUseImagePainter->isChecked())
        {
            svgView.renderer()->render(&imagePainter);
            painter.drawImage(rect, image);
        }
        else
        {
            svgView.renderer()->render(&painter, rect);
        }

        x += labelWidth + labelDistHor;
    }
    painter.end();

    QDir::setCurrent(currentPath);

    mTemplateTags.remove("N");
    mTemplateTags.remove("n");
}
#endif

void TabEtikette::loadPageLayout()
{
  #ifdef QT_PRINTSUPPORT_LIB
    QPageLayout layout;
    QMarginsF margins(data(ModelEtiketten::ColRandLinks).toDouble(),
                      data(ModelEtiketten::ColRandOben).toDouble(),
                      data(ModelEtiketten::ColRandRechts).toDouble(),
                      data(ModelEtiketten::ColRandUnten).toDouble());
    layout.setOrientation((QPageLayout::Orientation)data(ModelEtiketten::ColAusrichtung).toInt());
    layout.setUnits(QPageLayout::Millimeter);
    layout.setPageSize(QPageSize((QPageSize::PageSizeId)data(ModelEtiketten::ColPapiergroesse).toInt()), margins);
    mPrinter->setPageLayout(layout);
  #endif
}

void TabEtikette::savePageLayout()
{
  #ifdef QT_PRINTSUPPORT_LIB
    QPageLayout layout = mPrinter->pageLayout();
    QPageSize pageSize = layout.pageSize();
    if (pageSize.id() == QPageSize::Custom)
        pageSize = QPageSize(layout.pageSize().sizePoints().transposed());
    if (pageSize.id() == QPageSize::Custom)
        pageSize = QPageSize(layout.pageSize().sizePoints());
    setData(ModelEtiketten::ColPapiergroesse, pageSize.id());
    setData(ModelEtiketten::ColAusrichtung, layout.orientation());
    QMarginsF margins = layout.margins(QPageLayout::Millimeter);
    setData(ModelEtiketten::ColRandLinks, margins.left());
    setData(ModelEtiketten::ColRandOben, margins.top());
    setData(ModelEtiketten::ColRandRechts, margins.right());
    setData(ModelEtiketten::ColRandUnten, margins.bottom());
  #endif
}

void TabEtikette::printPreview()
{
  #ifdef QT_PRINTSUPPORT_LIB
    loadPageLayout();
    mPrinter->setOutputFileName("");
    mPrinter->setOutputFormat(QPrinter::NativeFormat);
    QPrintPreviewDialog dlg(mPrinter, this);
    connect(&dlg, SIGNAL(paintRequested(QPrinter*)), this, SLOT(onPrinterPaintRequested(QPrinter*)));
    dlg.exec();
    savePageLayout();
    gSettings->beginGroup("General");
    gSettings->setValue("DefaultPrinterEtikette", mPrinter->printerName());
    gSettings->endGroup();
  #endif
}

void TabEtikette::toPdf()
{
  #ifdef QT_PRINTSUPPORT_LIB
    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + bh->sud()->getSudname() + "_" + tr("Etikette") +  ".pdf", "PDF (*.pdf)");
    if (!fileName.isEmpty())
    {
        loadPageLayout();
        mPrinter->setOutputFileName(fileName);
        mPrinter->setOutputFormat(QPrinter::PdfFormat);
        onPrinterPaintRequested(mPrinter);
        savePageLayout();

        QFileInfo fi(fileName);
        gSettings->setValue("exportPath", fi.absolutePath());

        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    gSettings->endGroup();
  #endif
}

void TabEtikette::on_btnToPdf_clicked()
{
    printPreview();
}

void TabEtikette::updateValues()
{
    if (!ui->cbAuswahl->hasFocus())
    {
        QVariant auswahl = data(ModelEtiketten::ColPfad);
        for (int i = 0; i < ui->cbAuswahl->count(); ++i)
        {
            if (ui->cbAuswahl->itemData(i) == auswahl)
            {
                ui->cbAuswahl->setCurrentIndex(i);
                break;
            }
        }
    }
    if (!ui->tbAnzahl->hasFocus())
        ui->tbAnzahl->setValue(data(ModelEtiketten::ColAnzahl).toInt());
    if (!ui->tbLabelBreite->hasFocus())
        ui->tbLabelBreite->setValue(data(ModelEtiketten::ColBreite).toDouble());
    if (!ui->tbLabelHoehe->hasFocus())
        ui->tbLabelHoehe->setValue(data(ModelEtiketten::ColHoehe).toDouble());
    if (!ui->tbAbstandHor->hasFocus())
        ui->tbAbstandHor->setValue(data(ModelEtiketten::ColAbstandHor).toDouble());
    if (!ui->tbAbstandVert->hasFocus())
        ui->tbAbstandVert->setValue(data(ModelEtiketten::ColAbstandVert).toDouble());
}

void TabEtikette::on_tbAnzahl_valueChanged(int value)
{
    if (ui->tbAnzahl->hasFocus())
        setData(ModelEtiketten::ColAnzahl, value);
}

void TabEtikette::on_tbLabelBreite_valueChanged(double value)
{
    if (ui->tbLabelBreite->hasFocus())
    {
        setData(ModelEtiketten::ColBreite, value);
        if (ui->cbSeitenverhaeltnis->isChecked())
        {
            QRectF rect = ui->viewSvg->viewBoxF();
            setData(ModelEtiketten::ColHoehe, value * rect.height() / rect.width());
        }
    }
}

void TabEtikette::on_tbLabelHoehe_valueChanged(double value)
{
    if (ui->tbLabelHoehe->hasFocus())
    {
        setData(ModelEtiketten::ColHoehe, value);
        if (ui->cbSeitenverhaeltnis->isChecked())
        {
            QRectF rect = ui->viewSvg->viewBoxF();
            setData(ModelEtiketten::ColBreite, value * rect.width() / rect.height());
        }
    }
}

void TabEtikette::on_cbSeitenverhaeltnis_clicked(bool checked)
{
    if (checked)
    {
        QRectF rect = ui->viewSvg->viewBoxF();
        setData(ModelEtiketten::ColHoehe,  ui->tbLabelBreite->value() * rect.height() / rect.width());
    }
}

void TabEtikette::on_btnGroesseAusSvg_clicked()
{
    QRectF rect = ui->viewSvg->viewBoxF();
    setData(ModelEtiketten::ColBreite, rect.width());
    setData(ModelEtiketten::ColHoehe, rect.height());
}

void TabEtikette::on_tbAbstandHor_valueChanged(double value)
{
    if (ui->tbAbstandHor->hasFocus())
        setData(ModelEtiketten::ColAbstandHor, value);
}

void TabEtikette::on_tbAbstandVert_valueChanged(double value)
{
    if (ui->tbAbstandVert->hasFocus())
        setData(ModelEtiketten::ColAbstandVert, value);
}

void TabEtikette::on_btnLoeschen_clicked()
{
    if (bh->sud()->modelEtiketten()->rowCount() > 0)
    {
        int ret = QMessageBox::question(this, tr("Etikette vom Rezept entfernen?"),
                                        tr("Soll die Etikette vom Rezept entfernt werden?"));
        if (ret == QMessageBox::Yes)
        {
            ui->cbAuswahl->setCurrentIndex(-1);
            bh->sud()->modelEtiketten()->removeRow(0);
            updateAll();
        }
    }
}

void TabEtikette::on_btnTagNeu_clicked()
{
    QMap<int, QVariant> values({{ModelTags::ColSudID, bh->sud()->id()},
                                {ModelTags::ColKey, tr("Neuer Tag")}});
    ProxyModel *model = bh->sud()->modelTags();
    int row = model->append(values);
    if (row >= 0)
    {
        QModelIndex index = model->index(row, ModelTags::ColKey);
        ui->tableTags->setCurrentIndex(index);
        ui->tableTags->scrollTo(index);
        ui->tableTags->edit(index);
    }
}

void TabEtikette::on_btnTagLoeschen_clicked()
{
    ProxyModel *model = bh->sud()->modelTags();
    QModelIndexList indices = ui->tableTags->selectionModel()->selectedIndexes();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        model->removeRow(index.row());
}
