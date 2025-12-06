#ifndef TABREZEPT_H
#define TABREZEPT_H

#include <QWidget>
#include <QGraphicsItem>
#include "settings.h"
#include "brauhelfer.h"

class SudObject;

namespace Ui {
class TabRezept;
}

class TabRezept : public QWidget
{
    Q_OBJECT

public:
    explicit TabRezept(QWidget *parent = nullptr);
    virtual ~TabRezept() Q_DECL_OVERRIDE;
    void setup(SudObject *sud);
    void saveSettings();
    void restoreView();
    void modulesChanged(Settings::Modules modules);
    void checkEnabled();

private slots:
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void updateValues();

    void rasten_modified();
    void updateMaischplan();
    void updateRastenDiagram();
    void on_btnNeueRast_clicked();
    void on_btnRastenUebernehmen_clicked();
    void on_btnMaischplanAusgleichen_clicked();
    void on_btnMaischplanFaktorAnpassen_clicked();
    void on_btnMaischplanAusgleichenMalz_clicked();

    void updateExtrakt();

    void malzGaben_modified();
    void updateMalzGaben();
    void updateMalzDiagram();
    void on_btnNeueMalzGabe_clicked();
    void on_btnMalzGabenUebernehmen_clicked();
    void on_btnNeueZusatzGabeMaischen_clicked();
    void on_btnZusazGabenUebernehmenMaischen_clicked();
    void on_btnMalzAusgleichen_clicked();
    void on_btnShaAnpassen_clicked();

    void hopfenGaben_modified();
    void updateHopfenGaben();
    void updateHopfenDiagram();
    void on_btnNeueHopfenGabe_clicked();
    void on_btnHopfenGabenUebernehmen_clicked();
    void on_btnNeueZusatzGabeKochen_clicked();
    void on_btnZusazGabenUebernehmenKochen_clicked();
    void on_cbBerechnungsartHopfen_currentIndexChanged(int index);
    void on_btnHopfenAusgleichen_clicked();

    void hefeGaben_modified();
    void on_btnNeueHefeGabe_clicked();
    void on_btnHefeGabenUebernehmen_clicked();
    void on_btnNeueHopfenGabeGaerung_clicked();
    void on_btnHopfenGabenUebernehmenGaerung_clicked();
    void on_btnNeueZusatzGabeGaerung_clicked();
    void on_btnZusazGabenUebernehmenGaerung_clicked();

    void weitereZutatenGaben_modified();

    void wasseraufbereitung_modified();
    void on_btnNeueWasseraufbereitung_clicked();
    void on_btnWasseraufbereitungUebernehmen_clicked();

    void anhaenge_modified();
    void on_btnNeuerAnhang_clicked();

    void on_tbSudname_textChanged(const QString &value);
    void on_cbKategorie_currentTextChanged(const QString &value);
    void on_btnKategorienVerwalten_clicked();
    void on_cbAnlage_currentTextChanged(const QString &value);
    void on_btnSudhausausbeute_clicked();
    void on_btnVerdampfungsrate_clicked();
    void on_cbWasserProfil_currentTextChanged(const QString &value);
    void on_btnWasserProfil_clicked();

    void on_btnTagNeu_clicked();
    void on_btnTagUebernehmen_clicked();
    void on_btnTagLoeschen_clicked();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    int checkRohstoffeDialog(Brauhelfer::RohstoffTyp typ, const QString &name);
    void checkRohstoffe();
    void updateGlas();
    void updateAnlageModel();
    void updateWasserModel();
    void vergaerungsgradUebernehmen(const QString& hefe);

private:
    Ui::TabRezept *ui;
    SudObject *mSud;
    QGraphicsItem *mGlasSvg;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterMaischenState;
    QByteArray mDefaultSplitterKochenState;
    QByteArray mDefaultSplitterGaerungState;
    QByteArray mDefaultSplitterMaischplanState;
    QByteArray mDefaultSplitterWasseraufbereitungState;
};

#endif // TABREZEPT_H
