#ifndef TABREZEPT_H
#define TABREZEPT_H

#include "tababstract.h"
#include <QGraphicsItem>

namespace Ui {
class TabRezept;
}

class TabRezept : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabRezept(QWidget *parent = nullptr);
    virtual ~TabRezept() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView(bool full) Q_DECL_OVERRIDE;
    void checkEnabled();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void updateValues();

    void rasten_modified();
    void updateRastenDiagram();
    void on_btnNeueRast_clicked();
    void on_btnRastenUebernehmen_clicked();

    void malzGaben_modified();
    void updateMalzGaben();
    void updateMalzDiagram();
    void on_btnNeueMalzGabe_clicked();
    void on_btnMalzGabenUebernehmen_clicked();

    void hopfenGaben_modified();
    void updateHopfenGaben();
    void updateHopfenDiagram();
    void on_btnNeueHopfenGabe_clicked();
    void on_btnHopfenGabenUebernehmen_clicked();
    void on_cbBerechnungsartHopfen_currentIndexChanged(int index);

    void hefeGaben_modified();
    void updateHefeDiagram();
    void on_btnNeueHefeGabe_clicked();
    void on_btnHefeGabenUebernehmen_clicked();

    void weitereZutatenGaben_modified();
    void on_btnNeueHopfenstopfenGabe_clicked();
    void on_btnNeueWeitereZutat_clicked();
    void on_btnWeitereZutatUebernehmen_clicked();

    void wasseraufbereitung_modified();
    void on_btnNeueWasseraufbereitung_clicked();
    void on_btnWasseraufbereitungUebernehmen_clicked();

    void anhaenge_modified();
    void on_btnNeuerAnhang_clicked();

    void on_tbSudname_textChanged(const QString &value);
    void on_cbKategorie_currentIndexChanged(const QString &value);
    void on_btnKategorienVerwalten_clicked();
    void on_cbAnlage_currentIndexChanged(const QString &value);
    void on_tbKommentar_textChanged();
    void on_btnSudhausausbeute_clicked();
    void on_btnVerdampfungsrate_clicked();
    void on_cbWasserProfil_currentIndexChanged(const QString &value);
    void on_btnRestalkalitaet_clicked();

    void on_btnTagNeu_clicked();
    void on_btnTagLoeschen_clicked();

private:
    void onTabActivated() Q_DECL_OVERRIDE;
    void checkRohstoffe();
    void updateGlas();
    void updateAnlageModel();
    void updateWasserModel();

private:
    Ui::TabRezept *ui;
    QGraphicsItem *mGlasSvg;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // TABREZEPT_H
