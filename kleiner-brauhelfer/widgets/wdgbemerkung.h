#ifndef WDGBEMERKUNG_H
#define WDGBEMERKUNG_H

#include <QWidget>
#include <QTextBrowser>

class SudObject;

namespace Ui {
class WdgBemerkung;
}

class BemerkungTextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    explicit BemerkungTextBrowser(QWidget *parent = nullptr) :
        QTextBrowser(parent)
    {
    }

signals:
    void doubleClicked(QMouseEvent *e);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE
    {
        QTextBrowser::mouseDoubleClickEvent(e);
        emit doubleClicked(e);
    }
};

class WdgBemerkung : public QWidget
{
    Q_OBJECT

public:
    explicit WdgBemerkung(QWidget *parent = nullptr);
    ~WdgBemerkung();
    void setSudObject(SudObject *sud);
    void setToolTip(const QString &text);
    void setPlaceholderText(const QString &text);
    void setHtml(const QString& html);
    QString toHtml() const;

signals:
    void changed(const QString& text);

private slots:
    void on_btnEdit_clicked();

private:
    Ui::WdgBemerkung *ui;
    SudObject *mSud;
};

#endif // WDGBEMERKUNG_H
