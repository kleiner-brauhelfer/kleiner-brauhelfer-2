#ifndef WDGMALZGABE_H
#define WDGMALZGABE_H

#include <QWidget>

namespace Ui {
class WdgMalzGabe;
}

class WdgMalzGabe : public QWidget
{
    Q_OBJECT

public:
    explicit WdgMalzGabe(int index, QWidget *parent = nullptr);
    ~WdgMalzGabe();
    bool isEnabled() const;
    QVariant data(const QString &fieldName) const;
    bool setData(const QString &fieldName, const QVariant &value);
    QString name() const;
    double prozent() const;
    void setFehlProzent(double value);
    void remove();

public slots:
    void updateValues(bool full = false);

private slots:
    void on_tbMengeProzent_valueChanged(double value);
    void on_cbZutat_currentIndexChanged(const QString &text);
    void on_tbMenge_valueChanged(double value);
    void on_btnLoeschen_clicked();
    void on_btnKorrektur_clicked();

private:
    void checkEnabled(bool force);

private:
    Ui::WdgMalzGabe *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGMALZGABE_H
