#ifndef TABROHSTOFFE_H
#define TABROHSTOFFE_H

#include <QWidget>
#include <QTableView>

namespace Ui {
class TabRohstoffe;
}

class TabRohstoffe : public QWidget
{
    Q_OBJECT

public:
    explicit TabRohstoffe(QWidget *parent = nullptr);
    ~TabRohstoffe();
    void saveSettings();
    void restoreView();

private slots:
    void updateWasser();
    void wasser_selectionChanged(const QItemSelection &selected);
    void on_tableMalz_clicked(const QModelIndex &index);
    void on_tableMalz_customContextMenuRequested(const QPoint &pos);
    void spalteAnzeigenMalz(bool checked);
    void on_tableHopfen_clicked(const QModelIndex &index);
    void on_tableHopfen_customContextMenuRequested(const QPoint &pos);
    void spalteAnzeigenHopfen(bool checked);
    void on_tableHefe_clicked(const QModelIndex &index);
    void on_tableHefe_customContextMenuRequested(const QPoint &pos);
    void spalteAnzeigenHefe(bool checked);
    void on_tableWeitereZutaten_clicked(const QModelIndex &index);
    void on_tableWeitereZutaten_customContextMenuRequested(const QPoint &pos);
    void spalteAnzeigenWeitereZutaten(bool checked);
    void on_buttonAdd_clicked();
    void on_buttonNeuVorlage_clicked();
    void on_buttonDelete_clicked();
    void on_buttonCopy_clicked();
    void on_radioButtonAlle_clicked();
    void on_radioButtonVorhanden_clicked();
    void on_radioButtonInGebrauch_clicked();
    void on_lineEditFilter_textChanged(const QString &pattern);
    void on_toolBoxRohstoffe_currentChanged(int index);
    void on_btnNeuesWasserprofil_clicked();
    void on_btnWasserprofilLoeschen_clicked();
    void on_tbCalciumMg_valueChanged(double value);
    void on_tbCalciumMmol_valueChanged(double value);
    void on_tbMagnesiumMg_valueChanged(double value);
    void on_tbMagnesiumMmol_valueChanged(double value);
    void on_tbSaeurekapazitaet_valueChanged(double value);
    void on_tbCarbonathaerte_valueChanged(double value);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void addEntry(QTableView *table, const QVariantMap &values);
    void replace(int type, const QString &rohstoff);
    QVariant dataWasser(const QString &fieldName) const;
    bool setDataWasser(const QString &fieldName, const QVariant &value);

private:
    Ui::TabRohstoffe *ui;
    int mRowWasser;
    QByteArray mDefaultStateTableMalz;
    QByteArray mDefaultStateTableHopfen;
    QByteArray mDefaultStateTableHefe;
    QByteArray mDefaultStateTableWeitereZutaten;
    QByteArray mDefaultStateTableWasser;
};

#endif // TABROHSTOFFE_H
