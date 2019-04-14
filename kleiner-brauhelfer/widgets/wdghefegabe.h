#ifndef WDGHEFEGABE_H
#define WDGHEFEGABE_H

#include <QWidget>

namespace Ui {
class WdgHefeGabe;
}

class WdgHefeGabe : public QWidget
{
    Q_OBJECT

public:
    explicit WdgHefeGabe(int index, QWidget *parent = nullptr);
    ~WdgHefeGabe();
    bool isEnabled() const;
    QVariant data(const QString &fieldName) const;
    bool setData(const QString &fieldName, const QVariant &value);

public slots:
    void updateValues(bool full = false);

private slots:
    void on_cbZutat_currentIndexChanged(const QString &text);
    void on_tbMenge_valueChanged(int value);
    void on_btnZugeben_clicked();
    void on_btnLoeschen_clicked();

private:
    void checkEnabled(bool force);

private:
    Ui::WdgHefeGabe *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGHEFEGABE_H
