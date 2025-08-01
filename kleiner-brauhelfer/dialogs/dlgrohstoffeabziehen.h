#ifndef DLGROHSTOFFEABZIEHEN_H
#define DLGROHSTOFFEABZIEHEN_H

#include "dlgabstract.h"
#include "brauhelfer.h"

class SudObject;

namespace Ui {
class DlgRohstoffeAbziehen;
}

class DlgRohstoffeAbziehen : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgRohstoffeAbziehen(SudObject* sud, bool abziehen, QWidget *parent = nullptr);
    DlgRohstoffeAbziehen(SudObject* sud, bool abziehen, Brauhelfer::RohstoffTyp typ, const QString& name, double menge, QWidget *parent = nullptr);
    virtual ~DlgRohstoffeAbziehen() Q_DECL_OVERRIDE;
    static void restoreView();

private slots:
    void reject() Q_DECL_OVERRIDE;
    void on_btnAbziehen_clicked();

private:
    void setModels(SudObject* sud, bool alleBrauzutaten, Brauhelfer::RohstoffTyp typ = Brauhelfer::RohstoffTyp::Malz, const QString& name = QString(), double menge = 0.0);

private:
    Ui::DlgRohstoffeAbziehen *ui;
    bool mAbziehen;
    bool mAbgezogen;
};

#endif // DLGROHSTOFFEABZIEHEN_H
