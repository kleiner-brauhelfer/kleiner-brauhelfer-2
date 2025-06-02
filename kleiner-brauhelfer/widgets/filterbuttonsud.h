#ifndef FILTERBUTTONSUD_H
#define FILTERBUTTONSUD_H

#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>
#include "proxymodelsud.h"
#include "dateedit.h"

class FilterButtonSud : public QToolButton
{
    Q_OBJECT

public:
    enum Item {
        Status = 1,
        Merkliste = 2,
        Kategorie = 4,
        Anlage = 8,
        Braudatum = 16,
        Alle = 255
    };
    Q_DECLARE_FLAGS(Items, Item)

public:
    FilterButtonSud(QWidget *parent = nullptr);
    ProxyModelSud* model() const;
    void setModel(ProxyModelSud* model, Items items = Item::Alle);

private slots:
    void setStatusAlle(bool value);
    void setStatus();
    void setMerkliste(bool value);
    void setKategorie(const QString& value);
    void setAnlage(const QString& value);
    void setDateAlle(bool value);
    void setMindate(const QDate& date);
    void setMaxdate(const QDate& date);
    void updateWidgets();

private:
    void updateChecked();

private:
    ProxyModelSud* mModel;
    QCheckBox* mCheckBoxAlle;
    QCheckBox* mCheckBoxRezept;
    QCheckBox* mCheckBoxGebraut;
    QCheckBox* mCheckBoxAbgefuellt;
    QCheckBox* mCheckBoxAusgetrunken;
    QCheckBox* mCheckBoxMerkliste;
    QComboBox* mComboBoxKategorie;
    QComboBox* mComboBoxAnlage;
    QCheckBox* mCheckBoxDatum;
    DateEdit* mDateEditMin;
    DateEdit* mDateEditMax;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FilterButtonSud::Items)

#endif // FILTERBUTTONSUD_H
