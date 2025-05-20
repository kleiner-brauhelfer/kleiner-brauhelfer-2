#ifndef DLGWASSERPROFILE_H
#define DLGWASSERPROFILE_H

#include "dlgabstract.h"
#include <QItemSelection>

namespace Ui {
class DlgWasserprofile;
}

class DlgWasserprofile : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgWasserprofile(QWidget *parent = nullptr);
    ~DlgWasserprofile();
    void saveSettings() Q_DECL_OVERRIDE;
    void loadSettings() Q_DECL_OVERRIDE;
    static void restoreView();
    void select(const QString& profile);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    QVariant dataWasser(int col) const;
    bool setDataWasser(int col, const QVariant &value);

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void updateWasser();
    void wasser_selectionChanged(const QItemSelection &selected);
    void on_btnNeu_clicked();
    void on_btnImport_clicked();
    void on_btnLoeschen_clicked();
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
    void on_tbRestalkalitaetAdd_valueChanged(double value);

private:
    Ui::DlgWasserprofile *ui;
    int mRow;
};

#endif // DLGWASSERPROFILE_H
