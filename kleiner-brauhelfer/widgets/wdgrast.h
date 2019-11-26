#ifndef WDGRAST_H
#define WDGRAST_H

#include <QWidget>

namespace Ui {
class WdgRast;
}

class WdgRast : public QWidget
{
    Q_OBJECT

public:
    explicit WdgRast(int index, QWidget *parent = nullptr);
    ~WdgRast();
    bool isEnabled() const;
    QString name() const;
    int temperatur() const;
    int dauer() const;
    void remove();

public slots:
    void updateValues(bool full = false);

private slots:
    void on_cbRast_currentTextChanged(const QString &text);
    void on_cbRast_currentIndexChanged(int index);
    void on_tbTemp_valueChanged(int value);
    void on_tbDauer_valueChanged(int value);
    void on_btnLoeschen_clicked();
    void on_btnNachUnten_clicked();
    void on_btnNachOben_clicked();

private:
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);
    void checkEnabled(bool force);

private:
    Ui::WdgRast *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGRAST_H
