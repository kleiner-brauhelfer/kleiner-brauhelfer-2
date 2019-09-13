#ifndef DLGISPINDELEINSTELLUNG_H
#define DLGISPINDELEINSTELLUNG_H

#include <QDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTime>
#include <QtSql>
#include <QString>
#include <QColor>
#include "settings.h"
#include "iSpindel/ispindel.h"

namespace Ui {
class dlgispindeleinstellung;
}

class DlgIspindeleinstellung : public QDialog
{
#define cryptMethod QCryptographicHash::Sha1

    Q_OBJECT

public:
    explicit DlgIspindeleinstellung(QWidget *parent = nullptr);
    ~DlgIspindeleinstellung();
    bool useIspindel() const;

private:
    void setSpindelParameterFromUi() const;
    void setUiDatabaseElementsEnable(bool setEnable) const;
    void databaseIsOpen();

private slots:
    void on_btnSaveClose_clicked();
    void on_btnTestConnection_clicked();
    void on_comboBoxChooseSpindel_currentIndexChanged(const QString &text);
    void on_butSaveParameter_clicked();
    void on_butSetResetflag_clicked();

    void on_lineEditX_2_editingFinished();
    void on_lineEditX_1_editingFinished();
    void on_lineEditX_0_editingFinished();

    void on_checkBoxIspindelVerwendung_clicked(bool checked);

private:
    Ui::dlgispindeleinstellung *ui;
    Ispindel *mIspindel;

};

#endif // DLGISPINDELEINSTELLUNG_H
