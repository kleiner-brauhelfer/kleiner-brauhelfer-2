#ifndef WDGBEMERKUNG_H
#define WDGBEMERKUNG_H

#include <QWidget>

namespace Ui {
class WdgBemerkung;
}

class WdgBemerkung : public QWidget
{
    Q_OBJECT

public:
    explicit WdgBemerkung(QWidget *parent = nullptr);
    ~WdgBemerkung();
    void setToolTip(const QString &text);
    void setHtml(const QString& html);
    QString toHtml() const;

signals:
    void changed(const QString& text);

private slots:
    void on_btnEdit_clicked();

private:
    Ui::WdgBemerkung *ui;
};

#endif // WDGBEMERKUNG_H
