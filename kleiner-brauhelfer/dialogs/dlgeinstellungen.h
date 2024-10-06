#ifndef DLGEINSTELLUNGEN_H
#define DLGEINSTELLUNGEN_H

#include "dlgabstract.h"

namespace Ui {
class DlgEinstellungen;
}

class DlgEinstellungen : public DlgAbstract
{
    Q_OBJECT

public:
    static DlgEinstellungen *Dialog;

    explicit DlgEinstellungen(QWidget *parent = nullptr);
    ~DlgEinstellungen();
    static void restoreView();

signals:
    void restoreViewTriggered();
    void checkUpdateTriggered(bool force);

private slots:
    void on_btnDatabase_clicked();
    void on_cbTheme_currentIndexChanged(int index);
    void on_cbLanguage_currentIndexChanged(int index);
    void on_cbFont_clicked(bool checked);
    void on_cbNumberFormat_clicked(bool checked);
    void on_cbShowTooltips_clicked(bool checked);
    void on_cbShowAnimations_clicked(bool checked);
    void on_cbConfirmClose_clicked(bool checked);
    void on_cbCheckUpdate_toggled(bool checked);
    void on_cbModuleGaerverlauf_clicked(bool checked);
    void on_cbModuleSchnellgaerprobe_clicked(bool checked);
    void on_cbModuleAusdruck_clicked(bool checked);
    void on_cbModuleEtiketten_clicked(bool checked);
    void on_cbModuleBewertungen_clicked(bool checked);
    void on_cbModuleBrauuebersicht_clicked(bool checked);
    void on_cbModuleAnlagen_clicked(bool checked);
    void on_cbModuleLager_clicked(bool checked);
    void on_cbModuleSpeise_clicked(bool checked);
    void on_cbModuleWasseraufbereitung_clicked(bool checked);
    void on_cbModulePreiskalkulation_clicked(bool checked);

private:
    void updateValues();

private:
    Ui::DlgEinstellungen *ui;
};

#endif // DLGEINSTELLUNGEN_H
