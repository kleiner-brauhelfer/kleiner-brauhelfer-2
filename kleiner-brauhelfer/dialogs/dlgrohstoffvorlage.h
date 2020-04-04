#ifndef DLGROHSTOFFVORLAGE_H
#define DLGROHSTOFFVORLAGE_H

#include "ui_dlgrohstoffvorlage.h"
#include <QDialog>

namespace Ui {
class DlgRohstoffVorlage;
}

class DlgRohstoffVorlage : public QDialog
{
	Q_OBJECT
	
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
        WZutatenOBraMa
    };

public:
    explicit DlgRohstoffVorlage(Art art, QWidget *parent = nullptr);
    virtual ~DlgRohstoffVorlage() Q_DECL_OVERRIDE;
    QMap<int, QVariant> values() const;

private slots:
    void slot_save();
    void on_lineEditFilter_textChanged(const QString &txt);
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
    void on_btn_Add_clicked();
    void on_btn_Remove_clicked();
    void on_btn_Import_clicked();
    void on_btn_Export_clicked();
    void on_btn_Restore_clicked();

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
