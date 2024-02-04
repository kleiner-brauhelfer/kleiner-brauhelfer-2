#ifndef TABEINSTELLUNGEN_H
#define TABEINSTELLUNGEN_H

#include <QWidget>

namespace Ui {
class TabEinstellungen;
}

class TabEinstellungen : public QWidget
{
    Q_OBJECT

public:
    explicit TabEinstellungen(QWidget *parent = nullptr);
    ~TabEinstellungen();

signals:
    void restoreView();
    void checkUpdate(bool force);

private slots:
    void on_btnDatabase_clicked();
    void on_cbTheme_currentIndexChanged(int index);
    void on_cbLanguage_currentIndexChanged(int index);
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
    Ui::TabEinstellungen *ui;
};

#endif // TABEINSTELLUNGEN_H
