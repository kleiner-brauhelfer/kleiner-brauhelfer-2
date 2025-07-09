#ifndef DLG_BRAUUEBERSICHT_H
#define DLG_BRAUUEBERSICHT_H

#include "dlgabstract.h"
#include <QAbstractItemModel>
#include <QItemSelection>
#include "settings.h"

namespace Ui {
class DlgBrauUebersicht;
}

class DlgBrauUebersicht : public DlgAbstract
{
    Q_OBJECT

public:
    static DlgBrauUebersicht *Dialog;
    
    explicit DlgBrauUebersicht(QWidget *parent = nullptr);
    virtual ~DlgBrauUebersicht() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void loadSettings() Q_DECL_OVERRIDE;
    static void restoreView();
    void select(const QVariant &sudId);

private slots:
    void modulesChanged(Settings::Modules modules);
    void onLayoutChanged();
    void modelDataChanged(const QModelIndex& index);
    void table_selectionChanged(const QItemSelection &selected);
    void diagram_selectionChanged(int index);
    void on_tbFilter_textChanged(const QString &pattern);
    void on_cbAuswahlL1_currentIndexChanged(int);
    void on_cbAuswahlL2_currentIndexChanged(int);
    void on_cbAuswahlL3_currentIndexChanged(int);

private:
    void build();
    void updateDiagram();
    void updateFilterLabel();
    void updateStatistics();

private:
    struct AuswahlType
    {
        int col;
        int precision;
        QString label;
        QString unit;
    };

private:
    Ui::DlgBrauUebersicht *ui;
    QList<AuswahlType> mAuswahlListe;
};

#endif // DLG_BRAUUEBERSICHT_H
