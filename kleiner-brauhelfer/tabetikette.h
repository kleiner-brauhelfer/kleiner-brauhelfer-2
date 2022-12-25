#ifndef TABETIKETTE_H
#define TABETIKETTE_H

#include "tababstract.h"
#include "helper/htmlhighlighter.h"

namespace Ui {
class TabEtikette;
}

#ifdef QT_PRINTSUPPORT_LIB
class QPrinter;
#endif

class TabEtikette : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabEtikette(QWidget *parent = nullptr);
    virtual ~TabEtikette() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    bool isPrintable() const Q_DECL_OVERRIDE;
    void printPreview() Q_DECL_OVERRIDE;
    void toPdf() Q_DECL_OVERRIDE;

private slots:
    void updateAll();
    void updateValues();
    void updateTemplateFilePath();
    void updateSvg();
    void updateTags();
    void updateTemplateTags();
    void updateAuswahlListe();
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
    void on_btnToPdf_clicked();
    void on_btnTagNeu_clicked();
    void on_btnTagLoeschen_clicked();

private:
    void onTabActivated() Q_DECL_OVERRIDE;
    bool checkSave();
    QString generateSvg(const QString &svg);
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);
  #ifdef QT_PRINTSUPPORT_LIB
    void loadPageLayout(QPrinter *printer);
    void savePageLayout(const QPrinter *printer);
  #endif

private:
    Ui::TabEtikette *ui;
    QString mTemplateFilePath;
    HtmlHighLighter* mHtmlHightLighter;
    QVariantMap mTemplateTags;
};

#endif // TABETIKETTE_H
