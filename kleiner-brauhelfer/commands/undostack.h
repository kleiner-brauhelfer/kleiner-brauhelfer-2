#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QUndoStack>
#include "commands/setmodeldatacommand.h"

class UndoStack : public QUndoStack
{
    Q_OBJECT
public:
    explicit UndoStack(QObject *parent = nullptr);
    QAction *createUndoAction(QObject *parent, const QString &prefix = QString()) const;
    QAction *createRedoAction(QObject *parent, const QString &prefix = QString()) const;
    void push(QUndoCommand *cmd);
    void setEnabled(bool enabled);
    bool isEnabled() const;
public Q_SLOTS:
    void undo();
    void redo();
private:
    bool mEnabled;
};

extern UndoStack* gUndoStack;

#endif // UNDOSTACK_H
