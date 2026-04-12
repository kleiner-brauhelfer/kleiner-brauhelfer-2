#ifndef WDGANHANG_H
#define WDGANHANG_H

#include "wdgabstractproxy.h"
#include <QDir>

class SudObject;

namespace Ui {
class WdgAnhang;
}

class WdgAnhang : public WdgAbstractProxy
{
    Q_OBJECT

public:
    static bool isImage(const QString &pfad);
    explicit WdgAnhang(SudObject *sud, int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgAnhang();
    void openDialog();
    QString getPfad() const;
    QString getFullPfad() const;

public slots:
    void updateValues();

private slots:
    void on_lineEdit_Pfad_textChanged(const QString &pfad);
    void on_checkBox_Relativ_clicked();
    void on_pushButton_Browse_clicked();
    void on_btnOpen_clicked();
    void on_btnLoeschen_clicked();

private:
    Ui::WdgAnhang *ui;
    SudObject *mSud;
    QDir mBasis;
};

#endif // WDGANHANG_H
