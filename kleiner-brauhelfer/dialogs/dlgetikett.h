#ifndef DLGETIKETT_H
#define DLGETIKETT_H

#include <QItemSelection>
#include "dlgabstract.h"
#include "helper/htmlhighlighter.h"

namespace Ui {
class DlgEtikett;
}

#ifdef QT_PRINTSUPPORT_LIB
class QPrinter;
#endif

class SudObject;

class DlgEtikett : public DlgAbstract
{
    Q_OBJECT

public:
    static DlgEtikett *Dialog;

public:
    explicit DlgEtikett(QWidget *parent = nullptr);
    virtual ~DlgEtikett() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void loadSettings() Q_DECL_OVERRIDE;
    static void restoreView();
    void selectSud(int id);
    void printPreview();
    void toPdf();

private slots:
    void updateAll();
    void updateValues();
    void updateTemplateFilePath();
    void updateSvg();
    void updateTags();
    void updateTemplateTags();
    void updateAuswahlListe();
    void onTableSelectionChanged(const QItemSelection &selected);
  #ifdef QT_PRINTSUPPORT_LIB
    void onPrinterPaintRequested(QPrinter *printer);
  #endif
    void on_cbAuswahl_activated(int index);
    void on_btnOeffnen_clicked();
    void on_btnAktualisieren_clicked();
    void on_tbAnzahl_valueChanged(int value);
    void on_tbLabelBreite_valueChanged(double value);
    void on_tbLabelHoehe_valueChanged(double value);
    void on_cbSeitenverhaeltnis_clicked(bool checked);
    void on_btnGroesseAusSvg_clicked();
    void on_btnBackgroundColor_clicked();
    void on_tbAbstandHor_valueChanged(double value);
    void on_tbAbstandVert_valueChanged(double value);
    void on_cbTagsErsetzen_stateChanged();
    void on_cbEditMode_clicked(bool checked);
    void on_tbTemplate_textChanged();
    void on_btnSaveTemplate_clicked();
    void on_btnExport_clicked();
    void on_btnPrint_clicked();
    void on_btnToPdf_clicked();
    void on_btnTagNeu_clicked();
    void on_btnTagLoeschen_clicked();
    void onFilterChanged();
    void on_tbFilter_textChanged(const QString &pattern);

private:
    bool checkSave();
    QString generateSvg(const QString &svg);
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);
  #ifdef QT_PRINTSUPPORT_LIB
    void loadPageLayout(QPrinter *printer);
    void savePageLayout(const QPrinter *printer);
  #endif

private:
    Ui::DlgEtikett *ui;
    SudObject* mSud;
    QString mTemplateFilePath;
    HtmlHighLighter* mHtmlHightLighter;
    QVariantMap mTemplateTags;
};

#endif // DLGETIKETT_H
