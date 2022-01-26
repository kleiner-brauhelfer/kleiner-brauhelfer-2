#ifndef DLGHILFE_H
#define DLGHILFE_H

#include "dlgabstract.h"
#include <QUrl>

namespace Ui {
class DlgHilfe;
}

class DlgHilfe : public DlgAbstract
{
    Q_OBJECT

public:
    static DlgHilfe *Dialog;

public:
    explicit DlgHilfe(QWidget *parent = nullptr);
    ~DlgHilfe();
    static void restoreView();
    void setHomeUrl(const QUrl &url);
    void setUrl(const QUrl &url);

private slots:
    void urlChanged(const QUrl &url);
    void on_btnHome_clicked();

private:
    Ui::DlgHilfe *ui;
    QUrl homeUrl;
};

#endif // DLGHILFE_H
