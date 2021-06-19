#ifndef DLGHILFE_H
#define DLGHILFE_H

#include <QDialog>

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

private:
    Ui::DlgHilfe *ui;
};

#endif // DLGHILFE_H
