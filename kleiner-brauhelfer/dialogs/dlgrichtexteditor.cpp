#include "dlgrichtexteditor.h"
#include "ui_dlgrichtexteditor.h"
#include <QTextList>
#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileInfo>
#include "sudobject.h"
#include "modelanhang.h"
#include "settings.h"

extern Settings* gSettings;

DlgRichTextEditor::DlgRichTextEditor(SudObject *sud, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgRichTextEditor),
    mSud(sud)
{
    ui->setupUi(this);
    adjustSize();
    gSettings->beginGroup(staticMetaObject.className());
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    gSettings->endGroup();

    ui->tbRichText->document()->setBaseUrl(QUrl::fromLocalFile(gSettings->databaseDir()));

    ui->tbSource->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbSource->setTabStopDistance(QFontMetrics(ui->tbSource->font()).horizontalAdvance(QStringLiteral("  ")));
   #else
    ui->tbSource->setTabStopDistance(2 * QFontMetrics(ui->tbSource->font()).width(' '));
   #endif
  #endif
    mHtmlHightLighter = new HtmlHighLighter(ui->tbSource->document());

    for(int size : QFontDatabase::standardSizes())
        ui->cbSize->addItem(QString::number(size));

    gSettings->beginGroup(staticMetaObject.className());
    ui->cbFont->setCurrentIndex(gSettings->value("font", ui->cbFont->findText(gSettings->font.family())).toInt());
    ui->cbSize->setCurrentIndex(gSettings->value("fontsize", ui->cbSize->findText(QString::number(gSettings->font.pointSize()))).toInt());
    gSettings->endGroup();

    ui->btnImg->setVisible(sud);

    QTextCharFormat format;
    format.setFont(ui->cbFont->currentFont());
    format.setFontPointSize(ui->cbSize->currentText().toInt());
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

DlgRichTextEditor::~DlgRichTextEditor()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("font", ui->cbFont->currentIndex());
    gSettings->setValue("fontsize", ui->cbSize->currentIndex());
    gSettings->endGroup();
    delete ui;
}

void DlgRichTextEditor::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

QString DlgRichTextEditor::extractBody(const QString& html) const
{
    if (html.isEmpty())
        return html;
    int start = html.indexOf(QStringLiteral("<body"));
    if (start == -1)
        return html;
    start = html.indexOf(QStringLiteral(">"), start) + 2;
    int end = html.indexOf(QStringLiteral("</body>"));
    if (end == -1)
        return html.left(start);
    else
        return html.mid(start, end - start);
}

void DlgRichTextEditor::setHtml(const QString& html)
{
    ui->tbRichText->setHtml(html);
}

QString DlgRichTextEditor::toHtml() const
{
    if (ui->tbRichText->toPlainText().isEmpty())
        return QStringLiteral("");
    return extractBody(ui->tbRichText->toHtml());
}

void DlgRichTextEditor::on_tbRichText_textChanged()
{
    if (!ui->tbSource->hasFocus())
        ui->tbSource->setPlainText(toHtml());
}

void DlgRichTextEditor::on_tbSource_textChanged()
{
    if (ui->tbSource->hasFocus())
        ui->tbRichText->setHtml(ui->tbSource->toPlainText());
}

void DlgRichTextEditor::on_tbRichText_cursorPositionChanged()
{
    QTextCursor cursor = ui->tbRichText->textCursor();

    QTextList *list = cursor.currentList();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;
    if (list)
        style = list->format().style();
    ui->btnListBullet->setChecked(list && style == QTextListFormat::ListDisc);
    ui->btnListOrdered->setChecked(list && style == QTextListFormat::ListDecimal);

    QTextBlockFormat blockFormat = cursor.blockFormat();
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    ui->cbStyle->setCurrentIndex(blockFormat.headingLevel());
  #endif
    ui->btnLeft->setChecked(blockFormat.alignment() & Qt::AlignLeft);
    ui->btnCenter->setChecked(blockFormat.alignment() & Qt::AlignHCenter);
    ui->btnRight->setChecked(blockFormat.alignment() & Qt::AlignRight);
}

void DlgRichTextEditor::on_tbRichText_currentCharFormatChanged(const QTextCharFormat &format)
{
    if (ui->tbRichText->toPlainText().isEmpty())
    {
        QTextCharFormat format;
        format.setFont(ui->cbFont->currentFont());
        format.setFontPointSize(ui->cbSize->currentText().toInt());
        ui->tbRichText->mergeCurrentCharFormat(format);
        return;
    }

    QFont font = format.font();
    ui->cbFont->setCurrentFont(font);
    ui->cbSize->setCurrentIndex(ui->cbSize->findText(QString::number(font.pointSize())));
    ui->btnBold->setChecked(font.bold());
    ui->btnItalic->setChecked(font.italic());
    ui->btnUnderline->setChecked(font.underline());

    QColor fontColor = format.foreground().color();
    QPixmap pix(8, 8);
    pix.fill(fontColor.isValid() ? fontColor : palette().windowText().color());
    ui->btnColorText->setIcon(pix);

    ui->btnLink->setChecked(format.isAnchor());
}

void DlgRichTextEditor::on_cbStyle_currentIndexChanged(int index)
{
    if (!ui->cbStyle->hasFocus())
        return;

    QTextCursor cursor = ui->tbRichText->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::BlockUnderCursor);

    QTextBlockFormat blockFormat = cursor.blockFormat();
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    blockFormat.setHeadingLevel(index);
  #endif
    cursor.setBlockFormat(blockFormat);

    QTextCharFormat format;
    switch (index)
    {
    case 0:
        format.setFontPointSize(10);
        format.setFontWeight(QFont::Normal);
        format.setFontItalic(false);
        break;
    case 1:
        format.setFontPointSize(18);
        format.setFontWeight(QFont::Bold);
        break;
    case 2:
        format.setFontPointSize(16);
        format.setFontWeight(QFont::Bold);
        format.setFontItalic(true);
        break;
    case 3:
        format.setFontPointSize(14);
        format.setFontWeight(QFont::Bold);
        break;
    case 4:
        format.setFontPointSize(12);
        format.setFontWeight(QFont::Bold);
        format.setFontItalic(true);
        break;
    }
    cursor.setCharFormat(format);
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_cbFont_currentFontChanged(const QFont &font)
{
    if (!ui->cbFont->hasFocus())
        return;

    QTextCharFormat format;
    format.setFont(font);
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_cbSize_textActivated(const QString &size)
{
    if (!ui->cbSize->hasFocus())
        return;

    bool ok;
    qreal pointSize = size.toFloat(&ok);
    if (ok)
    {
        QTextCharFormat format;
        format.setFontPointSize(pointSize);
        ui->tbRichText->mergeCurrentCharFormat(format);
    }
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnColorText_clicked()
{
    QColor color = QColorDialog::getColor(ui->tbRichText->textColor(), this);
    if (color.isValid())
    {
        QTextCharFormat format;
        format.setForeground(color);
        ui->tbRichText->mergeCurrentCharFormat(format);
    }
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnBold_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnItalic_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontItalic(checked);
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnUnderline_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontUnderline(checked);
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnLeft_clicked(bool checked)
{
    Q_UNUSED(checked)
    QTextCursor cursor = ui->tbRichText->textCursor();
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(blockFormat);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnCenter_clicked(bool checked)
{
    QTextCursor cursor = ui->tbRichText->textCursor();
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setAlignment(checked ? Qt::AlignHCenter : Qt::AlignLeft);
    cursor.mergeBlockFormat(blockFormat);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnRight_clicked(bool checked)
{
    QTextCursor cursor = ui->tbRichText->textCursor();
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setAlignment(checked ? Qt::AlignRight : Qt::AlignLeft);
    cursor.mergeBlockFormat(blockFormat);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnLink_clicked(bool checked)
{
    QTextCharFormat format;
    QTextCursor cursor = ui->tbRichText->textCursor();
    bool unlink = cursor.charFormat().isAnchor();
    if (checked)
    {
        bool ok = false;
        QString url = QStringLiteral("http://");
        url = QInputDialog::getText(this, tr("Link erstellen"), tr("URL:"), QLineEdit::Normal, url, &ok);
        if (ok && !url.isEmpty())
        {
            format.setAnchor(true);
            format.setAnchorHref(url);
            format.setForeground(palette().color(QPalette::Link));
            format.setFontUnderline(true);
            unlink = false;
        }
    }

    if (unlink)
    {
        format.setAnchor(false);
        format.setForeground(QColor());
        format.setFontUnderline(false);
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::BlockUnderCursor);
        cursor.mergeCharFormat(format);
    }

    ui->btnLink->setChecked(format.isAnchor());
    ui->tbRichText->mergeCurrentCharFormat(format);
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::changeList(bool checked, QTextListFormat::Style style)
{
    QTextCursor cursor = ui->tbRichText->textCursor();
    if (!checked)
    {
        QTextBlockFormat format;
        format.setIndent(cursor.blockFormat().indent());
        cursor.setBlockFormat(format);
    }
    else
    {
        cursor.createList(style);
    }
    ui->tbRichText->setFocus();
}


void DlgRichTextEditor::on_btnListBullet_clicked(bool checked)
{
    changeList(checked, QTextListFormat::ListDisc);
}

void DlgRichTextEditor::on_btnListOrdered_clicked(bool checked)
{
    changeList(checked, QTextListFormat::ListDecimal);
}

void DlgRichTextEditor::changeIndent(int delta)
{
    QTextCursor cursor = ui->tbRichText->textCursor();
    QTextBlockFormat format = cursor.blockFormat();
    int indent = format.indent() + delta;
    if (indent >= 0)
    {
        format.setIndent(indent);
        cursor.setBlockFormat(format);
    }
    ui->tbRichText->setFocus();
}

void DlgRichTextEditor::on_btnIdentInc_clicked()
{
    changeIndent(1);
}

void DlgRichTextEditor::on_btnIndDecr_clicked()
{
    changeIndent(-1);
}

void DlgRichTextEditor::on_btnImg_clicked()
{
    QStringList imgs;
    if (mSud)
    {
        for (int row = 0; row < mSud->modelAnhang()->rowCount(); ++row)
        {
            QString pfad = mSud->modelAnhang()->data(row, ModelAnhang::ColPfad).toString();
            if (pfad.endsWith(QStringLiteral(".png"), Qt::CaseInsensitive)
                || pfad.endsWith(QStringLiteral(".gif"), Qt::CaseInsensitive)
                || pfad.endsWith(QStringLiteral(".jpg"), Qt::CaseInsensitive)
                || pfad.endsWith(QStringLiteral(".jpg"), Qt::CaseInsensitive)
                || pfad.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)
                || pfad.endsWith(QStringLiteral(".bmp"), Qt::CaseInsensitive))
            {
                imgs.append(pfad);
            }
        }
    }

    if (!imgs.isEmpty())
    {
        bool ok = false;
        QString img = QInputDialog::getItem(this, tr("Bild einfügen"), tr("Bild aus Anhang"), imgs, 0, false, &ok);
        if (ok && !img.isEmpty())
        {
            QTextDocumentFragment fragment;
            fragment = QTextDocumentFragment::fromHtml(QStringLiteral("<img src=\"%1\">").arg(img));
            ui->tbRichText->textCursor().insertFragment(fragment);
        }
    }
    else
    {
        QMessageBox::information(this, tr("Bild einfügen"), tr("Bilder müssen zuerst als Anhang eingefügt werden."));
    }
    ui->tbRichText->setFocus();
}
