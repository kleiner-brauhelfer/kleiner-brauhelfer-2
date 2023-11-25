#ifndef SETMODELDATACOMMAND_H
#define SETMODELDATACOMMAND_H

#include <QUndoCommand>
#include <QAbstractItemModel>

class SetModelDataCommand : public QUndoCommand
{
public:
    SetModelDataCommand(QAbstractItemModel *model, int row, int col, const QVariant &value, QUndoCommand *parent = nullptr);
    virtual ~SetModelDataCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE;
    bool mergeWith(const QUndoCommand *other) Q_DECL_OVERRIDE;

private:
    QMetaObject::Connection modelConnection;
    QAbstractItemModel *model;
    int row;
    int col;
    QVariant newValue;
    QVariant prevValue;
};

#endif // SETMODELDATACOMMAND_H
