#ifndef TABBRAUUEBERSICHT_H
#define TABBRAUUEBERSICHT_H

#include "tababstract.h"
#include <QAbstractItemModel>
#include <QItemSelection>

namespace Ui {
class TabBrauUebersicht;
}

class TabBrauUebersicht : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabBrauUebersicht(QWidget *parent = nullptr);
    virtual ~TabBrauUebersicht() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView(bool full) Q_DECL_OVERRIDE;
    void setModel(QAbstractItemModel* model);

signals:
    void clicked(int sudId);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private slots:
    void updateViews();
    void updateDiagram();
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
    Ui::TabBrauUebersicht *ui;
    QByteArray mDefaultSplitterState;
    QList<AuswahlType> mAuswahlListe;
};

#endif // TABBRAUUEBERSICHT_H
