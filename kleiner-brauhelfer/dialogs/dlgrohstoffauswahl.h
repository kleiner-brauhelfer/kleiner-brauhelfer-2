#ifndef DLGROHSTOFFAUSWAHL_H
#define DLGROHSTOFFAUSWAHL_H

#include "ui_dlgrohstoffauswahl.h"
#include <QDialog>

namespace Ui {
class DlgRohstoffAuswahl;
}

class DlgRohstoffAuswahl : public QDialog
{
	Q_OBJECT
	
public:

    enum Rohstoff
    {
        Malz,
        Hopfen,
        Hefe,
        Zusatz
    };

    static void restoreView();

    explicit DlgRohstoffAuswahl(Rohstoff rohstoff, QWidget *parent = nullptr);
    virtual ~DlgRohstoffAuswahl() Q_DECL_OVERRIDE;
    void select(const QString &name);
    QString name() const;

private slots:
    void on_radioButtonAlle_clicked();
    void on_radioButtonVorhanden_clicked();
    void on_lineEditFilter_textChanged(const QString &text);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_buttonBox_accepted();

private:
    Ui::DlgRohstoffAuswahl *ui;
    int mNameCol;
    Rohstoff mRohstoff;
};

#endif // DLGROHSTOFFAUSWAHL_H
