#ifndef FILTERBUTTONSUD_H
#define FILTERBUTTONSUD_H

#include "toolbutton.h"
#include "proxymodelsud.h"
#include "dateedit.h"
#include "checkbox.h"

class FilterButtonSud : public ToolButton
{
    Q_OBJECT

public:
    FilterButtonSud(QWidget *parent = nullptr);
    void init(ProxyModelSud* model, bool statusEditable);

private slots:
    void setStatusAlle(bool value);
    void setStatus();
    void setMerkliste(bool value);
    void setDateAlle(bool value);
    void setMindate(const QDate& date);
    void setMaxdate(const QDate& date);

private:
    void updateChecked();

private:
    bool mStatusEditable;
    ProxyModelSud* mModel;
    ProxyModelSud::FilterStatus mStatus;
    CheckBox* mCheckBoxAlle;
    CheckBox* mCheckBoxRezept;
    CheckBox* mCheckBoxGebraut;
    CheckBox* mCheckBoxAbgefuellt;
    CheckBox* mCheckBoxAusgetrunken;
    CheckBox* mCheckBoxMerkliste;
    CheckBox* mCheckBoxDatum;
    DateEdit* mDateEditMin;
    DateEdit* mDateEditMax;
};

#endif // FILTERBUTTONSUD_H
