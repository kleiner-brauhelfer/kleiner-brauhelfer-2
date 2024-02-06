#ifndef FILTERBUTTON_H
#define FILTERBUTTON_H

#include "toolbutton.h"
#include "proxymodelsud.h"
#include "dateedit.h"
#include "checkbox.h"

class FilterButton : public ToolButton
{
    Q_OBJECT

public:
    FilterButton(QWidget *parent = nullptr);
    void init(ProxyModelSud* model);

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

#endif // FILTERBUTTON_H
