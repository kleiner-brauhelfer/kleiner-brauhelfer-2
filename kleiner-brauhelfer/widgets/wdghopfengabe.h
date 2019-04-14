#ifndef WDGHOPFENGABE_H
#define WDGHOPFENGABE_H

#include <QWidget>

namespace Ui {
class WdgHopfenGabe;
}

class WdgHopfenGabe : public QWidget
{
    Q_OBJECT

public:
    explicit WdgHopfenGabe(int index, QWidget *parent = nullptr);
    ~WdgHopfenGabe();
    bool isEnabled() const;
    QVariant data(const QString &fieldName) const;
    bool setData(const QString &fieldName, const QVariant &value);
    QString name() const;
    double prozent() const;
    void setFehlProzent(double value);

public slots:
    void updateValues(bool full = false);

private slots:
    void on_cbZutat_currentIndexChanged(const QString &text);
    void on_tbMengeProzent_valueChanged(double value);
    void on_tbAnteilProzent_valueChanged(double value);
    void on_btnMengeKorrektur_clicked();
    void on_btnAnteilKorrektur_clicked();
    void on_tbMenge_valueChanged(int value);
    void on_tbKochdauer_valueChanged(int dauer);
    void on_cbVwh_clicked(bool checked);
    void on_btnLoeschen_clicked();

private:
    void checkEnabled(bool force);
    bool prozentIbu() const;

private:
    Ui::WdgHopfenGabe *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGHOPFENGABE_H
