#ifndef WDGRAST_H
#define WDGRAST_H

#include <QWidget>

namespace Ui {
class WdgRast;
}

class WdgRast : public QWidget
{
    Q_OBJECT

public:
    explicit WdgRast(int index, QWidget *parent = nullptr);
    ~WdgRast();
    bool isEnabled() const;
    QString name() const;
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);
    void remove();

public slots:
    void updateListe();
    void updateValuesFromListe(int index);
    void updateValues(bool full = false);

private slots:
    void on_cbRast_currentTextChanged(const QString &text);
    void on_cbRast_currentIndexChanged(int index);
    void on_cbTyp_currentIndexChanged(int index);
    void on_tbMengeEinmaischen_valueChanged(double value);
    void on_tbVerhaeltnisEinmaischen_valueChanged(double value);
    void on_tbTempEinmaischen_valueChanged(double value);
    void on_tbDauerEinmaischen_valueChanged(int value);
    void on_tbTempRast_valueChanged(double value);
    void on_tbDauerRast_valueChanged(int value);
    void on_tbMengeInfusion_valueChanged(double value);
    void on_tbVerhaeltnisInfusion_valueChanged(double value);
    void on_tbTempInfusion_valueChanged(double value);
    void on_tbWassertempInfusion_valueChanged(double value);
    void on_tbDauerInfusion_valueChanged(int value);
    void on_tbMengeDekoktion_valueChanged(double value);
    void on_tbVerhaeltnisDekoktion_valueChanged(double value);
    void on_tbRasttempDekoktion_valueChanged(double value);
    void on_tbRastdauerDekoktion_valueChanged(int value);
    void on_tbKochdauerDekoktion_valueChanged(int value);
    void on_tbTeiltempDekoktion_valueChanged(double value);
    void on_tbTempDekoktion_valueChanged(double value);
    void on_tbDauerDekoktion_valueChanged(int value);
    void on_btnLoeschen_clicked();
    void on_btnNachUnten_clicked();
    void on_btnNachOben_clicked();

private:
    void checkEnabled(bool force);

private:
    Ui::WdgRast *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGRAST_H
