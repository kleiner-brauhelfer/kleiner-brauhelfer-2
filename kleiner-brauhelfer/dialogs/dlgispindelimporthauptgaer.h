#ifndef DLGISPINDELIMPORTHAUPTGAER_H
#define DLGISPINDELIMPORTHAUPTGAER_H

#include <QDialog>
#include <QVariantMap>
#include <QDialogButtonBox>
#include <QSqlQueryModel>

namespace Ui {
class DlgIspindelImportHauptgaer;
}

class DlgIspindelImportHauptgaer : public QDialog
{
    Q_OBJECT

public:
    explicit DlgIspindelImportHauptgaer(QWidget *parent = nullptr);
    ~DlgIspindelImportHauptgaer();

    QList<QVariantMap> getValuePlatoDatabase() const;
    int getMultiplikatorForDataImport();

private slots:
    void on_comboBox_chooseIspindel_currentIndexChanged(const QString &text);
    void on_listWidgetTimestampReset_itemSelectionChanged();
    void on_butImport_clicked();
    void on_butShowData_clicked();

signals:
    void sig_datasetToImportReady(QList<QVariantMap>);

private:
    void getChooseDateTime(QDateTime *First, QDateTime *Last);

private:
    Ui::DlgIspindelImportHauptgaer *ui;
    QList<QPair<QDateTime, QStringList> > ResetFlags;
    QSqlQueryModel *mSqlModelView;
    QList<QVariantMap> mValuePlatoDatabase;
};

#endif // DLGISPINDELIMPORTHAUPTGAER_H
