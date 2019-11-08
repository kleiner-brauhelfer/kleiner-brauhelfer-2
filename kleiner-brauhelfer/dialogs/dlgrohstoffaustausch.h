#ifndef DLGROHSTOFFAUSTAUSCH_H
#define DLGROHSTOFFAUSTAUSCH_H

#include <QDialog>
#include <QAbstractItemModel>

namespace Ui {
class DlgRohstoffAustausch;
}

class DlgRohstoffAustausch : public QDialog
{
    Q_OBJECT

public:

    enum DlgType
    {
        NichtVorhanden,
        Loeschen
    };

public:
    explicit DlgRohstoffAustausch(DlgType type, const QString &rohstoff, QWidget *parent = nullptr);
    ~DlgRohstoffAustausch();
    void setSud(const QString &sud);
    void setModel(QAbstractItemModel *model, int column);
    QString rohstoff() const;
    void setRohstoff(const QString &rohstoff);
    bool importieren() const;

private slots:
    void on_btnImportieren_clicked();
    void on_btnAustauschen_clicked();
    void on_btnNichtAustauschen_clicked();

private:
    void setNearestIndex();

private:
    Ui::DlgRohstoffAustausch *ui;
    DlgType mType;
    bool mImportieren;
};

#endif // DLGROHSTOFFAUSTAUSCH_H
