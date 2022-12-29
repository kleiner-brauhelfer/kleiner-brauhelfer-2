#ifndef TABZUSAMMENFASSUNG_H
#define TABZUSAMMENFASSUNG_H

#include "tababstract.h"

namespace Ui {
class TabZusammenfassung;
}

class TabZusammenfassung : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabZusammenfassung(QWidget *parent = nullptr);
    virtual ~TabZusammenfassung() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void modulesChanged(Settings::Modules modules) Q_DECL_OVERRIDE;
    bool isPrintable() const Q_DECL_OVERRIDE;
    void printPreview() Q_DECL_OVERRIDE;
    void toPdf() Q_DECL_OVERRIDE;

private slots:
    void updateAuswahl();
    void updateWebView();
    void on_cbAuswahl_currentTextChanged(const QString &txt);

private:
    void onTabActivated() Q_DECL_OVERRIDE;

private:
    Ui::TabZusammenfassung *ui;
};

#endif // TABZUSAMMENFASSUNG_H
