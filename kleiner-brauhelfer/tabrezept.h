#ifndef TABREZEPT_H
#define TABREZEPT_H

#include <QWidget>
#include <QGraphicsItem>

namespace Ui {
class TabRezept;
}

class TabRezept : public QWidget
{
    Q_OBJECT

public:
    explicit TabRezept(QWidget *parent = nullptr);
    ~TabRezept();
    void saveSettings();
    void restoreView();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void updateValues();

    void rasten_modified();
    void on_btnEinmaischtemperatur_clicked();
    void on_btnNeueRast_clicked();

    void malzGaben_modified();
    void malzGaben_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void on_btnNeueMalzGabe_clicked();

    void hopfenGaben_modified();
    void hopfenGaben_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void on_btnNeueHopfenGabe_clicked();
    void on_cbBerechnungsartHopfen_currentIndexChanged(int index);

    void hefeGaben_modified();
    void on_btnNeueHefeGabe_clicked();

    void weitereZutatenGaben_modified();
    void on_btnNeueHopfenstopfenGabe_clicked();
    void on_btnNeueWeitereZutat_clicked();

    void anhaenge_modified();
    void on_btnNeuerAnhang_clicked();

    void on_tbSudnummer_valueChanged(int value);
    void on_tbSudname_textChanged(const QString &value);
    void on_tbMenge_valueChanged(double value);
    void on_tbSW_valueChanged(double value);
    void on_tbCO2_valueChanged(double value);
    void on_tbBittere_valueChanged(int value);
    void on_tbReifezeit_valueChanged(int value);
    void on_cbAnlage_currentIndexChanged(const QString &value);
    void on_tbKommentar_textChanged();
    void on_tbHGF_valueChanged(int value);
    void on_tbFaktorHauptguss_valueChanged(double value);
    void on_cbWasserProfil_currentIndexChanged(const QString &value);
    void on_tbRestalkalitaet_valueChanged(double value);
    void on_tbEinmaischtemperatur_valueChanged(int temp);
    void on_tbKochzeit_valueChanged(int min);
    void on_tbNachisomerisierungszeit_valueChanged(int min);

    void on_btnTagNeu_clicked();
    void on_btnTagLoeschen_clicked();

private:
    void checkEnabled();
    void checkRohstoffe();
    void updateGlas();
    void updateAnlageModel();
    void updateWasserModel();
    void updateMalzGaben();
    void updateHopfenGaben();

private:
    Ui::TabRezept *ui;
    QGraphicsItem *mGlasSvg;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // TABREZEPT_H
