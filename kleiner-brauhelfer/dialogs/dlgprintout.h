#ifndef DLGPRINTOUT_H
#define DLGPRINTOUT_H

#include "dlgabstract.h"

namespace Ui {
class DlgPrintout;
}

class DlgPrintout : public DlgAbstract
{
    Q_OBJECT

public:
    static DlgPrintout *Dialog;

public:
    explicit DlgPrintout(QWidget *parent = nullptr);
    virtual ~DlgPrintout() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void loadSettings() Q_DECL_OVERRIDE;
    static void restoreView();
    void select(const QVariant &sudId);

private slots:
    void updateWebView();
    void onFilterChanged();
    void on_tbFilter_textChanged(const QString &pattern);

private:
    Ui::DlgPrintout *ui;
};

#endif // DLGPRINTOUT_H
