#ifndef TABLAGER_H
#define TABLAGER_H

#include <QWidget>

namespace Ui {
class TabLager;
}

class QTableView;

class TabLager : public QWidget
{
    Q_OBJECT

public:
    explicit TabLager(QWidget *parent = nullptr);
    ~TabLager();
    void restoreView();

public slots:
    void add();
    void addVorlage();
    void addObrama();
    void remove();
    void copy();

private slots:
    //void modulesChanged(Settings::Modules modules);
    void on_tableMalz_clicked(const QModelIndex &index);
    void on_tableHopfen_clicked(const QModelIndex &index);
    void on_tableHefe_clicked(const QModelIndex &index);
    void on_tableWeitereZutaten_clicked(const QModelIndex &index);
    void on_tbFilter_textChanged(const QString &pattern);
    void on_toolBoxRohstoffe_currentChanged();
    void updateLabelNumItems();

private:
    void loadSettings();
    void saveSettings();
    void build();
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void addEntry(QTableView *table, const QMap<int, QVariant> &values);

private:
    Ui::TabLager *ui;
};

#endif // TABLAGER_H
