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
    explicit DlgRohstoffVorlage(QWidget *parent = nullptr);
	~DlgRohstoffVorlage();
    void ViewMalzauswahl();
    void ViewHopfenauswahl();
    void ViewHefeauswahl();
    void ViewWeitereZutatenauswahl();
    QVariantMap values() const;

    /*
	QString m_Beschreibung;
    QString m_Verpackungsmenge;
	QString m_Temperatur;
	int m_Typ;
	int m_TypOGUG;
	int m_TypFlTr;
	int m_SED;
	QString m_EVG;
	double m_Alpha;
	double m_Wuerzemenge;
	double m_Farbe;
    double m_Ausbeute;
	int m_MaxProzent;
	QString m_Eigenschaften;
    */

private slots:
    void slot_save();
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
    void on_btn_Add_clicked();
    void on_btn_Remove_clicked();
    void on_btn_Import_clicked();
    void on_btn_Export_clicked();
    void on_btn_Restore_clicked();

private:
    QString getFileName(bool withPath) const;
    void viewImpl(int art);

private:
    Ui::DlgRohstoffVorlage *ui;
    int mRohstoffart;
    QVariantMap mValues;
};

#endif // DLGROHSTOFFVORLAGE_H
