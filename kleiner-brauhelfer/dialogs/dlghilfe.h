#ifndef DLGHILFE_H
#define DLGHILFE_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class DlgHilfe;
}

class DlgHilfe : public QDialog
{
    Q_OBJECT

public:
    static DlgHilfe *Dialog;

public:
    explicit DlgHilfe(const QUrl &url, QWidget *parent = nullptr);
    ~DlgHilfe();
    void setUrl(const QUrl &url);

private slots:
    void urlChanged(const QUrl &url);
    void on_btnHome_clicked();

private:
    Ui::DlgHilfe *ui;
    const QUrl homeUrl;
};

#endif // DLGHILFE_H
