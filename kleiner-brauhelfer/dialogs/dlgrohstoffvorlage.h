#ifndef DLGROHSTOFFVORLAGE_H
#define DLGROHSTOFFVORLAGE_H

#include "dlgabstract.h"
#include "ui_dlgrohstoffvorlage.h"

namespace Ui {
class DlgRohstoffVorlage;
}

class DlgRohstoffVorlage : public DlgAbstract
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
        MalzOBraMa,
        HopfenOBraMa,
        HefeOBraMa,
        WZutatenOBraMa,
        Wasserprofil
    };

public:
    explicit DlgRohstoffVorlage(Art art, QWidget *parent = nullptr);
    virtual ~DlgRohstoffVorlage() Q_DECL_OVERRIDE;
    QMap<int, QVariant> values() const;

private slots:
    void on_lineEditFilter_textChanged(const QString &txt);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    QString getFileName(bool withPath) const;
    void setModel();
    bool isOBraMa() const;

private:
    Ui::DlgRohstoffVorlage *ui;
    Art mRohstoffart;
    QMap<int, QVariant> mValues;
};

#endif // DLGROHSTOFFVORLAGE_H
