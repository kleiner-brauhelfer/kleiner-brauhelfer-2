#ifndef DLGWASSERAUFBEREITUNG_H
#define DLGWASSERAUFBEREITUNG_H

#include <QDialog>

namespace Ui {
class DlgWasseraufbereitung;
}

class DlgWasseraufbereitung : public QDialog
{
    Q_OBJECT

public:
    explicit DlgWasseraufbereitung(QWidget *parent = nullptr);
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

private:
    Ui::DlgWasseraufbereitung *ui;
    QList<Auswahl> mList;
    double mFaktor;
};

#endif // DLGWASSERAUFBEREITUNG_H
