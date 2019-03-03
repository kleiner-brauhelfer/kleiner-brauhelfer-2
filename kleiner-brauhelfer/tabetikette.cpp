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
#include "brauhelfer.h"
#include "settings.h"
#include "widgets/webview.h"
#include "helper/mustache.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabEtikette::TabEtikette(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabEtikette),
    mTemplateFilePath("")
{
    ui->setupUi(this);

    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
    ui->btnSaveTemplate->setPalette(gSettings->paletteErrorButton);
    ui->treeViewTemplateTags->setColumnWidth(0, 150);
    ui->treeViewTemplateTags->setColumnWidth(1, 150);

    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());

    connect(bh, SIGNAL(discarded()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateTemplateTags()));
    connect(bh->sud()->modelFlaschenlabelTags(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateTemplateTags()));
    connect(bh->sud()->modelFlaschenlabel(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateValues()));

    connect(bh->sud()->modelAnhang(), SIGNAL(modelReset()), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateAuswahlListe()));
    connect(bh->sud()->modelAnhang(), SIGNAL(sortChanged()), this, SLOT(updateAuswahlListe()));

    gSettings->beginGroup("TabEtikette");

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    gSettings->endGroup();

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
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void TabEtikette::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
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

void TabEtikette::updateTemplateTags()
{
    mTemplateTags.clear();
    WebView::erstelleTagListe(mTemplateTags, bh->sud()->row());

    ui->treeViewTemplateTags->clear();
    for (QVariantMap::const_iterator it = mTemplateTags.begin(); it != mTemplateTags.end(); ++it)
    {
        if (it.value().canConvert<QVariantMap>())
        {
            QVariantMap hash = it.value().toMap();
            QTreeWidgetItem *t = new QTreeWidgetItem(ui->treeViewTemplateTags, {it.key()});
            for (QVariantMap::const_iterator it2 = hash.begin(); it2 != hash.end(); ++it2)
                t->addChild(new QTreeWidgetItem(t, {it2.key(), it2.value().toString()}));
            ui->treeViewTemplateTags->addTopLevelItem(t);
        }
        else
        {
            ui->treeViewTemplateTags->addTopLevelItem(new QTreeWidgetItem(ui->treeViewTemplateTags, {it.key(), it.value().toString()}));
        }
    }

    updateSvg();
}

void TabEtikette::checkSave()
{
    if (ui->btnSaveTemplate->isVisible())
    {
        int ret = QMessageBox::question(this, tr("Änderungen speichern?"),
                                        tr("Sollen die Änderungen gespeichert werden?"));
        if (ret == QMessageBox::Yes)
            on_btnSaveTemplate_clicked();
    }
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
    checkSave();
    ui->btnSaveTemplate->setVisible(false);
    if (bh->sud()->modelFlaschenlabel()->rowCount() == 0)
    {
        QVariantMap values({{"SudID", bh->sud()->id()}});
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
    checkSave();

    ui->tbTemplate->setVisible(checked);
    ui->treeViewTemplateTags->setVisible(checked);
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
        int pageCount = int(round(double(totalCount) / double(countPerPage) + double(0.5)));

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
                    ui->viewSvg->renderer()->render(&painter, QRectF(0, hoehePx * i + abstandPx * i, breiteMM * faktorPxPerMM, hoehePx));
                    QRectF rect = QRectF(0, hoehePx * i + abstandPx * i,breiteMM * faktorPxPerMM, hoehePx);
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
