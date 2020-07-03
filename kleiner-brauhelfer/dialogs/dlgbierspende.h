#ifndef DLGBIERSPENDE_H
#define DLGBIERSPENDE_H

#include <QDialog>

namespace Ui {
class DlgBierspende;
}

class DlgBierspende : public QDialog
{
    Q_OBJECT

public:
    explicit DlgBierspende(QWidget *parent = nullptr);
    ~DlgBierspende();

private:
    Ui::DlgBierspende *ui;
};

#endif // DLGBIERSPENDE_H
