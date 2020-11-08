#ifndef TABROHSTOFFE_H
#define TABROHSTOFFE_H

#include "tababstract.h"
#include <QTableView>

namespace Ui {
class TabRohstoffe;
}

class TabRohstoffe : public TabAbstract
{
    Q_OBJECT

public:
    static QStringList HopfenTypname;
    static QStringList HefeTypname;
    static QStringList HefeTypFlTrName;
    static QStringList ZusatzTypname;
    static QStringList Einheiten;
    static QStringList list_tr(const QStringList& list);

public:
    explicit TabRohstoffe(QWidget *parent = nullptr);
    virtual ~TabRohstoffe() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView(bool full) Q_DECL_OVERRIDE;

private slots:
    void updateWasser();
    void wasser_selectionChanged(const QItemSelection &selected);
    void on_tableMalz_clicked(const QModelIndex &index);
    void on_tableHopfen_clicked(const QModelIndex &index);
    void on_tableHefe_clicked(const QModelIndex &index);
    void on_tableWeitereZutaten_clicked(const QModelIndex &index);
    void on_buttonAdd_clicked();
    void on_buttonNeuVorlage_clicked();
    void on_buttonNeuVorlageObrama_clicked();
    void on_buttonDelete_clicked();
    void on_buttonCopy_clicked();
    void on_radioButtonAlle_clicked();
    void on_radioButtonVorhanden_clicked();
    void on_radioButtonInGebrauch_clicked();
    void on_lineEditFilter_textChanged(const QString &pattern);
    void on_toolBoxRohstoffe_currentChanged(int index);
    void on_tbCalciumMg_valueChanged(double value);
    void on_tbCalciumMmol_valueChanged(double value);
    void on_tbCalciumHaerte_valueChanged(double value);
    void on_tbMagnesiumMg_valueChanged(double value);
    void on_tbMagnesiumMmol_valueChanged(double value);
    void on_tbMagnesiumHaerte_valueChanged(double value);
    void on_tbHydrogencarbonatMg_valueChanged(double value);
    void on_tbHydrogencarbonatMmol_valueChanged(double value);
    void on_tbHydrogencarbonatHaerte_valueChanged(double value);
    void on_tbSulfatMg_valueChanged(double value);
    void on_tbSulfatMmol_valueChanged(double value);
    void on_tbChloridMg_valueChanged(double value);
    void on_tbChloridMmol_valueChanged(double value);
    void on_tbNatriumMg_valueChanged(double value);
    void on_tbNatriumMmol_valueChanged(double value);
    void on_tbRestalkalitaetAdd_valueChanged(double arg1);
    void on_tbBemerkung_textChanged();

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void addEntry(QTableView *table, const QMap<int, QVariant> &values);
    QVariant dataWasser(int col) const;
    bool setDataWasser(int col, const QVariant &value);
    void updateLabelNumItems();

private:
    Ui::TabRohstoffe *ui;
    int mRowWasser;
};

#endif // TABROHSTOFFE_H
