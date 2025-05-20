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
#include <QColorDialog>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"
#include "helper/mustache.h"
#include "model/checkboxdelegate.h"
#include "model/textdelegate.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

TabEtikette::TabEtikette(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabEtikette),
    mTemplateFilePath(QStringLiteral(""))
{
    ui->setupUi(this);

    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTemplate->setTabStopDistance(QFontMetrics(ui->tbTemplate->font()).horizontalAdvance(QStringLiteral("  ")));
   #else
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
   #endif
  #endif
    ui->btnSaveTemplate->setError(true);
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTags->setTabStopDistance(QFontMetrics(ui->tbTags->font()).horizontalAdvance(QStringLiteral("  ")));
   #else
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTags->font()).width(' '));
   #endif
  #endif

    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());

    gSettings->beginGroup("TabEtikette");
    ui->cbSeitenverhaeltnis->setChecked(gSettings->value("Seitenverhaeltnis", true).toBool());
    ui->cbDividingLine->setChecked(gSettings->value("Trennlinie", true).toBool());
    gSettings->endGroup();

    TableView *table = ui->tableTags;
    table->setModel(bh->sud()->modelTags());
    table->appendCol({ModelTags::ColKey, true, false, 0, new TextDelegate(table)});
    table->appendCol({ModelTags::ColValue, true, false, -1, new TextDelegate(table)});
    table->appendCol({ModelTags::ColGlobal, true, false, 0, new CheckBoxDelegate(table)});
    table->build();

    connect(bh, &Brauhelfer::discarded, this, &TabEtikette::updateAll);
    connect(bh->sud(), &SudObject::loadedChanged, this, &TabEtikette::updateAll);
    connect(bh->sud(), &SudObject::modified, this, &TabEtikette::updateTemplateTags);
    connect(bh->sud()->modelTags(), &ProxyModel::modified, this, &TabEtikette::updateTemplateTags);
    connect(bh->sud()->modelEtiketten(), &ProxyModel::modified, this, &TabEtikette::updateValues);
    connect(bh->sud()->modelAnhang(), &ProxyModel::layoutChanged, this, &TabEtikette::updateAuswahlListe);
    connect(bh->sud()->modelAnhang(), &ProxyModel::rowsInserted, this, &TabEtikette::updateAuswahlListe);
    connect(bh->sud()->modelAnhang(), &ProxyModel::rowsRemoved, this, &TabEtikette::updateAuswahlListe);

    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    updateAll();
}

TabEtikette::~TabEtikette()
{
    delete ui;
}

void TabEtikette::saveSettings()
{
    gSettings->beginGroup("TabEtikette");
    gSettings->setValue("Seitenverhaeltnis", ui->cbSeitenverhaeltnis->isChecked());
    gSettings->setValue("Trennlinie", ui->cbDividingLine->isChecked());
    gSettings->endGroup();
}

void TabEtikette::onTabActivated()
{
    updateAll();
}

void TabEtikette::updateAll()
{
    updateAuswahlListe();
    ui->cbAuswahl->setCurrentIndex(0);
    updateValues();
    updateTemplateFilePath();
    updateTemplateTags();
}

void TabEtikette::updateAuswahlListe()
{
    ui->cbAuswahl->clear();
    ui->cbAuswahl->addItem(QStringLiteral(""));
    for (int row = 0; row < bh->sud()->modelAnhang()->rowCount(); ++row)
    {
        QString pfad = bh->sud()->modelAnhang()->index(row, ModelAnhang::ColPfad).data().toString();
        if (pfad.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive))
        {
            QFileInfo fi(pfad);
            ui->cbAuswahl->addItem(fi.fileName(), fi.filePath());
        }
    }

    QDirIterator it(gSettings->dataDir(3), QStringList() << QStringLiteral("*.svg"));
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
    QString output = renderer.render(svg, &context);
    if (renderer.errorPos() != -1)
        qWarning() << "Mustache:" << renderer.error();
    return output;
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
    QColor bgColor = data(ModelEtiketten::ColHintergrundfarbe).toUInt();
    QPalette pal = ui->frameBackgroundColor->palette();
    pal.setColor(QPalette::Window, bgColor);
    ui->frameBackgroundColor->setPalette(pal);
    ui->viewSvg->setViewBackgroundColor(bgColor);

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
    TemplateTags::erstelleTagListe(mTemplateTags, bh->sud()->row());
    mTemplateTags[QStringLiteral("N")] = "N";
    mTemplateTags[QStringLiteral("n")] = "n";
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

    if (index == 0)
    {
        if (bh->sud()->modelEtiketten()->rowCount() > 0)
        {
            QString pfad = data(ModelEtiketten::ColPfad).toString();
            bh->sud()->modelEtiketten()->removeRow(0);
            int row = bh->sud()->modelAnhang()->getRowWithValue(ModelAnhang::ColPfad, pfad);
            if (row >= 0)
                bh->sud()->modelAnhang()->removeRow(row);
        }
    }
    else
    {
        QString pfad = ui->cbAuswahl->itemData(index).toString();
        if (bh->sud()->modelEtiketten()->rowCount() == 0)
        {
            QMap<int, QVariant> values({{ModelEtiketten::ColSudID, bh->sud()->id()},
                                        {ModelEtiketten::ColPfad, pfad}});
            bh->sud()->modelEtiketten()->append(values);
        }
        else
        {
            setData(ModelEtiketten::ColPfad, pfad);
            if (!bh->modelEtiketten()->setValuesFrom(bh->sud()->modelEtiketten()->mapRowToSource(0), pfad))
            {
                setData(ModelEtiketten::ColBreite, 0);
                setData(ModelEtiketten::ColHoehe, 0);
            }
        }
    }

    updateValues();
    updateTemplateFilePath();
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    updateSvg();

    QRectF rect = ui->viewSvg->viewBoxF();
    if (data(ModelEtiketten::ColBreite).toDouble() == 0)
        setData(ModelEtiketten::ColBreite, rect.width());
    if (data(ModelEtiketten::ColHoehe).toDouble() == 0)
        setData(ModelEtiketten::ColHoehe, rect.height());
}

void TabEtikette::on_btnOeffnen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("SVG auswählen"), QStringLiteral(""), tr("SVG (*.svg)"));
    if (!fileName.isEmpty())
    {
        QMap<int, QVariant> values({{ModelAnhang::ColSudID, bh->sud()->id()},
                                    {ModelAnhang::ColPfad, fileName}});
        bh->sud()->modelAnhang()->append(values);
        updateAll();
        on_cbAuswahl_activated(ui->cbAuswahl->findText(QFileInfo(fileName).fileName()));
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
            ui->lblFilePath->setText("<a href=\"" + file.fileName() + "\">" + file.fileName() + "</a>");
            file.close();
        }
        else
        {
            ui->tbTemplate->setPlainText(QStringLiteral(""));
        }
    }
    else
    {
        ui->tbTemplate->setPlainText(QStringLiteral(""));
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

void TabEtikette::on_btnExport_clicked()
{
    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    QString filePath = QFileDialog::getSaveFileName(this, tr("SVG exportieren"),
                                                    path + "/" + bh->sud()->getSudname() + "_" + tr("Etikett") + ".svg", QStringLiteral("SVG (*.svg)"));
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        QFile file(filePath);
        if (file.open(QFile::WriteOnly | QFile::Text))
        {
            QFile fileRead(mTemplateFilePath);
            fileRead.open(QIODevice::ReadOnly | QIODevice::Text);
            QString svg_template = fileRead.readAll();
            fileRead.close();
            if (ui->cbTagsErsetzen->isChecked())
                file.write(generateSvg(svg_template).toUtf8());
            else
                file.write(svg_template.toUtf8());
            file.close();
        }
        else
        {
            QMessageBox::warning(this, tr("SVG Export"), tr("Die Datei konnte nicht geschrieben werden."));
        }
    }
    gSettings->endGroup();
}

#ifdef QT_PRINTSUPPORT_LIB

void TabEtikette::onPrinterPaintRequested(QPrinter *printer)
{
    QWidget* parent = qobject_cast<QWidget*>(sender());
    if(parent)
        parent->setEnabled(false);

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

    QString currentPath = QDir::currentPath();
    QDir::setCurrent(QFileInfo(mTemplateFilePath).absolutePath());

    Mustache::Renderer mustacheRenderer;
    mTemplateTags[QStringLiteral("N")] = ui->tbAnzahl->value();
    bool constSvg = !svg_template.contains(QStringLiteral("{{n}}"));
    if (constSvg)
    {
        Mustache::QtVariantContext context(mTemplateTags);
        QString svg = mustacheRenderer.render(svg_template, &context);
        svgView.load(svg.toUtf8());
    }

    int x = 0, y = 0;
    QPainter painter(printer);
    QPen outline(Qt::darkRed, 0.5*faktorPxPerMM, Qt::DashDotDotLine);
    QColor bgColor = data(ModelEtiketten::ColHintergrundfarbe).toUInt();
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

        if (bgColor.isValid() && bgColor != Qt::transparent)
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(bgColor);
            painter.drawRect(rect);
        }

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
            mTemplateTags[QStringLiteral("n")] = i + 1;
            Mustache::QtVariantContext context(mTemplateTags);
            QString svg = mustacheRenderer.render(svg_template, &context);
            svgView.load(svg.toUtf8());
        }

        if (ui->cbUseImagePainter->isChecked())
        {
            QImage image(labelWidth, labelHeight, QImage::Format_ARGB32_Premultiplied);
            QPainter imagePainter(&image);
            image.fill(Qt::transparent);
            if (svgView.renderer())
                svgView.renderer()->render(&imagePainter);
            painter.drawImage(rect, image);
        }
        else
        {
            if (svgView.renderer())
                svgView.renderer()->render(&painter, rect);
        }

        x += labelWidth + labelDistHor;
    }
    painter.end();

    QDir::setCurrent(currentPath);

    mTemplateTags.remove(QStringLiteral("N"));
    mTemplateTags.remove(QStringLiteral("n"));

    if(parent)
        parent->setEnabled(true);
}

void TabEtikette::loadPageLayout(QPrinter* printer)
{
    QPageLayout layout;
    QMarginsF margins(data(ModelEtiketten::ColRandLinks).toDouble(),
                      data(ModelEtiketten::ColRandOben).toDouble(),
                      data(ModelEtiketten::ColRandRechts).toDouble(),
                      data(ModelEtiketten::ColRandUnten).toDouble());
    layout.setOrientation((QPageLayout::Orientation)data(ModelEtiketten::ColAusrichtung).toInt());
    layout.setUnits(QPageLayout::Millimeter);
    layout.setPageSize(QPageSize((QPageSize::PageSizeId)data(ModelEtiketten::ColPapiergroesse).toInt()), margins);
    printer->setPageLayout(layout);
}

void TabEtikette::savePageLayout(const QPrinter *printer)
{
    QPageLayout layout = printer->pageLayout();
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
}

#endif

bool TabEtikette::isPrintable() const
{
    return true;
}

void TabEtikette::printPreview()
{
  #ifdef QT_PRINTSUPPORT_LIB
    gSettings->beginGroup("General");
    QPrinterInfo printerInfo = QPrinterInfo::printerInfo(gSettings->value("DefaultPrinterEtikette").toString());
    QPrinter printer(printerInfo, QPrinter::HighResolution);
    printer.setColorMode(QPrinter::Color);
    gSettings->endGroup();

    loadPageLayout(&printer);
    printer.setOutputFileName(QStringLiteral(""));
    printer.setOutputFormat(QPrinter::NativeFormat);
    QPrintPreviewDialog dlg(&printer, this);
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &TabEtikette::onPrinterPaintRequested);
    dlg.exec();
    savePageLayout(&printer);
    gSettings->beginGroup("General");
    gSettings->setValue("DefaultPrinterEtikette", printer.printerName());
    gSettings->endGroup();
  #endif
}

void TabEtikette::toPdf()
{
  #ifdef QT_PRINTSUPPORT_LIB
    gSettings->beginGroup("General");
    QPrinterInfo printerInfo = QPrinterInfo::printerInfo(gSettings->value("DefaultPrinterEtikette").toString());
    QPrinter printer(printerInfo, QPrinter::HighResolution);
    printer.setColorMode(QPrinter::Color);
    gSettings->endGroup();

    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                                    path + "/" + bh->sud()->getSudname() + "_" + tr("Etikett") + ".pdf", QStringLiteral("PDF (*.pdf)"));
    if (!fileName.isEmpty())
    {
        loadPageLayout(&printer);
        printer.setOutputFileName(fileName);
        printer.setOutputFormat(QPrinter::PdfFormat);
        onPrinterPaintRequested(&printer);
        savePageLayout(&printer);

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
    ui->scrollArea->setEnabled(ui->cbAuswahl->currentIndex() != 0);
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

void TabEtikette::on_btnBackgroundColor_clicked()
{
    QColorDialog dlg(data(ModelEtiketten::ColHintergrundfarbe).toUInt(), this);
    if (dlg.exec() == QDialog::Accepted) {
        setData(ModelEtiketten::ColHintergrundfarbe, dlg.selectedColor().rgb()&0xffffff);
        updateSvg();
    }
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
    for (const QModelIndex& index : qAsConst(indices))
        model->removeRow(index.row());
}
