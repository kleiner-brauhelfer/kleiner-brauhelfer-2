#ifndef TABETIKETTE_H
#define TABETIKETTE_H

#include "tababstract.h"
#include "helper/htmlhighlighter.h"

namespace Ui {
class TabEtikette;
}

class TabEtikette : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabEtikette(QWidget *parent = nullptr);
    virtual ~TabEtikette() Q_DECL_OVERRIDE;

private slots:
    void updateAll();
    void updateValues();
    void updateTemplateFilePath();
    void updateSvg();
    void updateTags();
    void updateTemplateTags();
    void updateAuswahlListe();
    void on_cbAuswahl_activated(int index);
    void on_spinBox_BreiteLabel_valueChanged(int value);
    void on_spinBox_AnzahlLabels_valueChanged(int value);
    void on_spinBox_AbstandLabel_valueChanged(int value);
    void on_spinBox_FLabel_RandOben_valueChanged(int value);
    void on_spinBox_FLabel_RandLinks_valueChanged(int value);
    void on_spinBox_FLabel_RandRechts_valueChanged(int value);
    void on_spinBox_FLabel_RandUnten_valueChanged(int value);
    void on_cbTagsErsetzen_stateChanged();
    void on_cbEditMode_clicked(bool checked);
    void on_tbTemplate_textChanged();
    void on_btnSaveTemplate_clicked();
    void on_btnToPdf_clicked();
    void on_btnLoeschen_clicked();

private:
    void onTabActivated() Q_DECL_OVERRIDE;
    bool checkSave();
    QString generateSvg(const QString &svg);
    QVariant data(const QString &fieldName) const;
    bool setData(const QString &fieldName, const QVariant &value);

private:
    Ui::TabEtikette *ui;
    QString mTemplateFilePath;
    HtmlHighLighter* mHtmlHightLighter;
    QVariantMap mTemplateTags;
};

#endif // TABETIKETTE_H
