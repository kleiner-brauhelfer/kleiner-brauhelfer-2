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
    void printPreview() Q_DECL_OVERRIDE;
    void toPdf() Q_DECL_OVERRIDE;

private slots:
    void updateWebView();
    void on_btnToPdf_clicked();
    void on_btnPrintPreview_clicked();

private:
    void onTabActivated() Q_DECL_OVERRIDE;

private:
    Ui::TabZusammenfassung *ui;
};

#endif // TABZUSAMMENFASSUNG_H
