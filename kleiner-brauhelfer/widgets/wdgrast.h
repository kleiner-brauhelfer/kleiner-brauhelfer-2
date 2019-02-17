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

public slots:
    void updateValues();

private slots:
    void on_cbRast_currentTextChanged(const QString &text);
    void on_cbRast_currentIndexChanged(int index);
    void on_tbTemp_valueChanged(int value);
    void on_tbTemp_editingFinished();
    void on_tbDauer_valueChanged(int value);
    void on_btnLoeschen_clicked();

private:
    QVariant data(const QString &fieldName) const;
    bool setData(const QString &fieldName, const QVariant &value);
    void checkEnabled(bool force);

private:
    Ui::WdgRast *ui;
    int mIndex;
    bool mEnabled;
};

#endif // WDGRAST_H
