#ifndef DLGUEBERNAHMEREZEPT_H
#define DLGUEBERNAHMEREZEPT_H

#include "dlgabstract.h"
#include "ui_dlguebernahmerezept.h"

namespace Ui {
class DlgUebernahmeRezept;
}

class DlgUebernahmeRezept : public DlgAbstract
{
    Q_OBJECT
    MAKE_TRANSLATABLE_DLG

public:
    enum Art
    {
        Malz,
        Hopfen,
        Hefe,
        WZutaten,
        Maischplan,
        Wasseraufbereitung,
        WZutatenMaischen,
        WZutatenKochen,
        WZutatenGaerung,
        HopfenGaerung,
        Tags
    };

public:
    explicit DlgUebernahmeRezept(Art art, QWidget *parent = nullptr);
    ~DlgUebernahmeRezept();
    int sudId() const;

private slots:
    void tableViewSud_selectionChanged();
    void on_tableViewSud_doubleClicked(const QModelIndex &index);

private:
    Ui::DlgUebernahmeRezept *ui;
    const Art mArt;
    int mSudId;
};

#endif // DLGUEBERNAHMEREZEPT_H
