#ifndef DLGMODULE_H
#define DLGMODULE_H

#include <QDialog>
#include "settings.h"

namespace Ui {
class DlgModule;
}

class DlgModule : public QDialog
{
    Q_OBJECT

public:
    static DlgModule *Dialog;

public:
    explicit DlgModule(QWidget *parent = nullptr);
    ~DlgModule();

private slots:
    void on_gbModuleGaerverlauf_clicked(bool checked);
    void on_gbModuleZusammenfassung_clicked(bool checked);
    void on_gbModuleEtikette_clicked(bool checked);
    void on_gbModuleBewertung_clicked(bool checked);
    void on_gbModuleBrauuebersicht_clicked(bool checked);
    void on_gbModuleAusruestung_clicked(bool checked);
    void on_gbModuleWasseraufbereitung_clicked(bool checked);
    void on_gbModulePreiskalkulation_clicked(bool checked);
    void on_gbModuleDatenbank_clicked(bool checked);

private:
    Ui::DlgModule *ui;
};

#endif // DLGMODULE_H
