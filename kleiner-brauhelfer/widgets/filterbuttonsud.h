#ifndef FILTERBUTTONSUD_H
#define FILTERBUTTONSUD_H

#include "toolbutton.h"
#include <QMouseEvent>

class ProxyModelSud;
class CheckBox;
class ComboBox;
class DateEdit;
class PushButton;

class FilterButtonSud : public ToolButton
{
    Q_OBJECT

public:
    enum Item {
        Clear = 1,
        Status = 2,
        Merkliste = 4,
        Kategorie = 8,
        Anlage = 16,
        Braudatum = 32,
        Alle = 255
    };
    Q_DECLARE_FLAGS(Items, Item)

public:
    FilterButtonSud(QWidget *parent = nullptr);
    ProxyModelSud* model() const;
    void setModel(ProxyModelSud* model, Items items = Item::Alle);

public slots:
    void clear();

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
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    ProxyModelSud* mModel;
    CheckBox* mCheckBoxAlle;
    CheckBox* mCheckBoxRezept;
    CheckBox* mCheckBoxGebraut;
    CheckBox* mCheckBoxAbgefuellt;
    CheckBox* mCheckBoxAusgetrunken;
    CheckBox* mCheckBoxMerkliste;
    ComboBox* mComboBoxKategorie;
    ComboBox* mComboBoxAnlage;
    CheckBox* mCheckBoxDatum;
    DateEdit* mDateEditMin;
    DateEdit* mDateEditMax;
    PushButton* mButtonClear;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FilterButtonSud::Items)

#endif // FILTERBUTTONSUD_H
