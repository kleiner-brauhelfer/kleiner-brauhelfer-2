#ifndef DLG_ROHSTOFFE_H
#define DLG_ROHSTOFFE_H

#include "dlgabstract.h"
#include <QTableView>

namespace Ui {
class DlgRohstoffe;
}

class DlgRohstoffe : public DlgAbstract
{
    Q_OBJECT

public:
    static QStringList HopfenTypname;
    static QStringList HefeTypname;
    static QStringList HefeTypFlTrName;
    static QStringList ZusatzTypname;
    static QStringList Einheiten;
    static QStringList list_tr(const QStringList& list);

    static DlgRohstoffe *Dialog;

public:
    explicit DlgRohstoffe(QWidget *parent = nullptr);
    virtual ~DlgRohstoffe() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView() Q_DECL_OVERRIDE;
    void modulesChanged(Settings::Modules modules) Q_DECL_OVERRIDE;

private slots:
    void updateWasser();
    void wasser_selectionChanged(const QItemSelection &selected);
    void on_tableMalz_clicked(const QModelIndex &index);
    void on_tableHopfen_clicked(const QModelIndex &index);
    void on_tableHefe_clicked(const QModelIndex &index);
    void on_tableWeitereZutaten_clicked(const QModelIndex &index);
    void buttonAdd_clicked();
    void buttonNeuVorlage_clicked();
    void buttonNeuVorlageObrama_clicked();
    void on_buttonDelete_clicked();
    void buttonCopy_clicked();
    void on_radioButtonAlle_clicked();
    void on_radioButtonVorhanden_clicked();
    void on_radioButtonInGebrauch_clicked();
    void on_lineEditFilter_textChanged(const QString &pattern);
    void on_toolBoxRohstoffe_currentChanged(int index);
    void on_tbCalciumMg_editingFinished();
    void on_tbCalciumMmol_editingFinished();
    void on_tbCalciumHaerte_editingFinished();
    void on_tbMagnesiumMg_editingFinished();
    void on_tbMagnesiumMmol_editingFinished();
    void on_tbMagnesiumHaerte_editingFinished();
    void on_tbHydrogencarbonatMg_editingFinished();
    void on_tbHydrogencarbonatMmol_editingFinished();
    void on_tbHydrogencarbonatHaerte_editingFinished();
    void on_tbSulfatMg_editingFinished();
    void on_tbSulfatMmol_editingFinished();
    void on_tbChloridMg_editingFinished();
    void on_tbChloridMmol_editingFinished();
    void on_tbNatriumMg_editingFinished();
    void on_tbNatriumMmol_editingFinished();
    void on_tbRestalkalitaetAdd_editingFinished();

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void addEntry(QTableView *table, const QMap<int, QVariant> &values);
    QVariant dataWasser(int col) const;
    bool setDataWasser(int col, const QVariant &value);
    void updateLabelNumItems();

private:
    Ui::DlgRohstoffe *ui;
    int mRowWasser;
};

#endif // DLG_ROHSTOFFE_H
