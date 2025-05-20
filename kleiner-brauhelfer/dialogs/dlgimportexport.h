#ifndef DLGIMPORTEXPORT_H
#define DLGIMPORTEXPORT_H

#include "dlgabstract.h"

namespace Ui {
class DlgImportExport;
}

class DlgImportExport : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgImportExport(bool import, int row, QWidget *parent = nullptr);
    ~DlgImportExport();
    int row() const;
    bool oeffnen(const QString& filename = QString());
    bool download(const QString& url);

private slots:
    void accept() Q_DECL_OVERRIDE;
    void on_btnOeffnen_clicked();
    void on_btnDownload_clicked();
    void on_rbFormatKbh_clicked();
    void on_rbFormatMmum_clicked();
    void on_rbFormatBeerxml_clicked();
    void on_rbFormatBrautomat_clicked();

private:
    bool importieren();
    bool exportieren();

private:
    Ui::DlgImportExport *ui;
    bool mImport;
    int mRow;
};

#endif // DLGIMPORTEXPORT_H
