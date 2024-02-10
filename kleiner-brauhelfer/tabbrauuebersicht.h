#ifndef TABBRAUUEBERSICHT_H
#define TABBRAUUEBERSICHT_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QItemSelection>
#include "settings.h"

namespace Ui {
class TabBrauuebersicht;
}

class TabBrauuebersicht : public QWidget
{
    Q_OBJECT

public:
    explicit TabBrauuebersicht(QWidget *parent = nullptr);
    ~TabBrauuebersicht();
    void restoreView();

private slots:
    void onModulesChanged(Settings::Modules modules);
    void onLayoutChanged();
    void onDataChanged(const QModelIndex& index);
    void onTableSelectionChanged(const QItemSelection &selected);
    void onDiagramSelectionChanged(int index);
    void on_tbFilter_textChanged(const QString &pattern);
    void on_cbAuswahlL1_currentIndexChanged(int index);
    void on_cbAuswahlL2_currentIndexChanged(int index);
    void on_cbAuswahlL3_currentIndexChanged(int index);

private:
    void saveSettings();
    void build();
    void updateDiagram();
    void updateFilterLabel();

private:
    struct AuswahlType
    {
        int col;
        int precision;
        QString label;
        QString unit;
    };

private:
    Ui::TabBrauuebersicht *ui;
    QList<AuswahlType> mAuswahlListe;
};

#endif // TABBRAUUEBERSICHT_H
