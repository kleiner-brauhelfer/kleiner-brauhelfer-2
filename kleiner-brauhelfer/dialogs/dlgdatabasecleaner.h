#ifndef DLGDATABASECLEANER_H
#define DLGDATABASECLEANER_H

#include <QDialog>
#include <functional>
#include "sqltablemodel.h"

namespace Ui {
class DlgDatabaseCleaner;
}

class DlgDatabaseCleaner : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDatabaseCleaner(QWidget* parent = nullptr);
    ~DlgDatabaseCleaner();

private slots:
    void on_btnDelete_clicked();
    void on_btnWeiter_clicked();

private:
    void next();
    void setTableIds(const QMap<int, QString>& ids = QMap<int, QString>());
    bool test1(SqlTableModel* model, const QList<int>& fields);
    bool test2(SqlTableModel* model, const QList<int>& fields);
    bool test3(SqlTableModel* model, const QList<int>& fields);

private:
    Ui::DlgDatabaseCleaner *ui;
    QMap<int, QString> mSudIds;
    QList<std::function<bool()> > mTestFncs;
    QList<std::function<bool()> >::const_iterator mItTestFncs;
};

#endif // DLGDATABASECLEANER_H
