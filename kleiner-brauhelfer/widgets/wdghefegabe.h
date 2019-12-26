#ifndef WDGHEFEGABE_H
#define WDGHEFEGABE_H

#include <QWidget>

namespace Ui {
class WdgHefeGabe;
}

class WdgHefeGabe : public QWidget
{
    Q_OBJECT

public:
    explicit WdgHefeGabe(int index, QWidget *parent = nullptr);
    ~WdgHefeGabe();
    bool isEnabled() const;
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);
    QString name() const;
    int menge() const;
    void remove();

public slots:
    void updateValues(bool full = false);

private slots:
    void on_btnZutat_clicked();
    void on_tbMenge_valueChanged(int value);
    void on_tbTage_valueChanged(int value);
    void on_tbDatum_dateChanged(const QDate &date);
    void on_btnZugeben_clicked();
    void on_btnLoeschen_clicked();
    void on_btnAufbrauchen_clicked();

private:
    void checkEnabled(bool force = false);

private:
    Ui::WdgHefeGabe *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGHEFEGABE_H
