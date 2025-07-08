#ifndef WDGANHANG_H
#define WDGANHANG_H

#include <QWidget>
#include <QDir>

class SudObject;

namespace Ui {
class WdgAnhang;
}

class WdgAnhang : public QWidget
{
    Q_OBJECT

public:
    static bool isImage(const QString &pfad);
    explicit WdgAnhang(SudObject *sud, int index, QWidget *parent = nullptr);
    ~WdgAnhang();
    void openDialog();
    QString getPfad() const;
    QString getFullPfad() const;
    void remove();

public slots:
    void updateValues();

private slots:
    void on_lineEdit_Pfad_textChanged(const QString &pfad);
    void on_checkBox_Relativ_clicked();
    void on_pushButton_Browse_clicked();
    void on_btnOpen_clicked();
    void on_btnLoeschen_clicked();

private:
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);

private:
    Ui::WdgAnhang *ui;
    SudObject *mSud;
    int mIndex;
    QDir mBasis;
};

#endif // WDGANHANG_H
