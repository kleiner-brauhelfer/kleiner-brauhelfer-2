#ifndef DLGWASSERAUFBEREITUNG_H
#define DLGWASSERAUFBEREITUNG_H

#include "dlgabstract.h"

namespace Ui {
class DlgWasseraufbereitung;
}

class DlgWasseraufbereitung : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgWasseraufbereitung(QWidget *parent = nullptr);
    DlgWasseraufbereitung(const QString& name, int einheit, double faktor, QWidget *parent = nullptr);
    ~DlgWasseraufbereitung();
    QString name() const;
    int einheit() const;
    double faktor() const;

private:
    struct Auswahl
    {
        QString name;
        int einheit;
        double faktor;
        int konzentration;
    };

private slots:
    void on_cbAuswahl_currentIndexChanged(int index);
    void on_tbKonzentration_valueChanged(int value);
    void on_cbEinheit_currentTextChanged(const QString &arg1);

private:
    Ui::DlgWasseraufbereitung *ui;
    QList<Auswahl> mList;
};

#endif // DLGWASSERAUFBEREITUNG_H
