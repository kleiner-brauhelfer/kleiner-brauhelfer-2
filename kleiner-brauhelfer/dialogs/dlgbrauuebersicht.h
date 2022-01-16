#ifndef DLG_BRAUUEBERSICHT_H
#define DLG_BRAUUEBERSICHT_H

#include "dlgabstract.h"
#include <QAbstractItemModel>
#include <QItemSelection>

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
    void restoreView() Q_DECL_OVERRIDE;
    void setModel(QAbstractItemModel* model);

signals:
    void clicked(int sudId);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private slots:
    void updateDiagram();
    void modelDataChanged(const QModelIndex& index);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void table_selectionChanged(const QItemSelection &selected);
    void diagram_selectionChanged(int sudId);
    void on_cbAuswahlL1_currentIndexChanged(int);
    void on_cbAuswahlL2_currentIndexChanged(int);
    void on_cbAuswahlL3_currentIndexChanged(int);

private:
    struct AuswahlType
    {
        int col;
        int precision;
        QString label;
        QString unit;
        double min;
        double max;
    };

private:
    Ui::DlgBrauUebersicht *ui;
    QByteArray mDefaultSplitterState;
    QList<AuswahlType> mAuswahlListe;
};

#endif // DLG_BRAUUEBERSICHT_H
