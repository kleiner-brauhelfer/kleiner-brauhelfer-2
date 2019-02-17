#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <QDialog>

namespace Ui {
class DlgAbout;
}

class DlgAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAbout(QWidget *parent = nullptr);
    ~DlgAbout();

private:
    QString generateLink(const QString &url, const QString &name = QString());

private:
    Ui::DlgAbout *ui;
};

#endif // DLGABOUT_H
