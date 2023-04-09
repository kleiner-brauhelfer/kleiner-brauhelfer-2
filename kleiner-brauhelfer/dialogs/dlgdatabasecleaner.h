#ifndef DLGDATABASECLEANER_H
#define DLGDATABASECLEANER_H

#include "dlgabstract.h"
#include <functional>
#include "sqltablemodel.h"

namespace Ui {
class DlgDatabaseCleaner;
}

class DlgDatabaseCleaner : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgDatabaseCleaner(QWidget* parent = nullptr);
    ~DlgDatabaseCleaner();

private slots:
    void selectionChanged();
    void on_btnDelete_clicked();
    void on_btnWeiter_clicked();

private:
    void next();
    void setTableIds(int type);
    bool testNullField(SqlTableModel* model, const QList<int>& fields, int type);
    bool testInvalidId(SqlTableModel* model, const QList<int>& fields, int type);
    bool testRange(SqlTableModel* model, int min, int max, int col, const QList<int> &visibleFields);
    bool testRange1();
    bool testRange2();
    bool testRange3();
    bool testRange4();
    bool testRange5();
    bool testRange6();
    bool testRange7();

private:
    Ui::DlgDatabaseCleaner *ui;
    QMap<int, QString> mSudIds;
    QMap<int, QString> mAnlagenIds;
    QList<std::function<bool()> > mTestFncs;
    QList<std::function<bool()> >::iterator mItTestFncs;
};

#endif // DLGDATABASECLEANER_H
