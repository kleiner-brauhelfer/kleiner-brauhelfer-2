#ifndef TABBRAUUEBERSICHT_H
#define TABBRAUUEBERSICHT_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QItemSelection>

namespace Ui {
class TabBrauUebersicht;
}

class TabBrauUebersicht : public QWidget
{
    Q_OBJECT

public:
    explicit TabBrauUebersicht(QWidget *parent = nullptr);
    ~TabBrauUebersicht();
    void saveSettings();
    void restoreView();
    void setModel(QAbstractItemModel* model);

signals:
    void clicked(int sudId);

private slots:
    void updateDiagram();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void table_selectionChanged(const QItemSelection &selected);
    void diagram_selectionChanged(int sudId);
    void on_cbAuswahlL1_currentIndexChanged(int);
    void on_cbAuswahlL2_currentIndexChanged(int);
    void on_cbAuswahlL3_currentIndexChanged(int);
    void spalteAnzeigen(bool checked);
    void on_tableView_customContextMenuRequested(const QPoint &pos);

private:
    struct AuswahlType
    {
        QString field;
        int precision;
        QString label;
        QString unit;
        double min;
        double max;
    };

private:
    Ui::TabBrauUebersicht *ui;
    QByteArray mDefaultTableState;
    QByteArray mDefaultSplitterState;
    QList<AuswahlType> mAuswahlListe;
};

#endif // TABBRAUUEBERSICHT_H
