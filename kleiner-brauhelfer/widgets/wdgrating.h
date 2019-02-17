#ifndef WDGRATING_H
#define WDGRATING_H

#include <QWidget>

namespace Ui {
class WdgRating;
}

class WdgRating : public QWidget
{
    Q_OBJECT

public:
    explicit WdgRating(QWidget *parent = nullptr);
    ~WdgRating();
    void setReadOnly(bool readonly);
    bool isReadOnly() const;
    void setSterne(int sterne);
    int sterne() const;

signals:
    void clicked(int sterne);

private slots:
    void on_btnStar1_clicked();
    void on_btnStar2_clicked();
    void on_btnStar3_clicked();
    void on_btnStar4_clicked();
    void on_btnStar5_clicked();

private:
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;

private:
    Ui::WdgRating *ui;
    bool mReadOnly;
    int mSterne;
    QPixmap mPixmapStar;
    QPixmap mPixmapNoStar;
};

#endif // WDGRATING_H
