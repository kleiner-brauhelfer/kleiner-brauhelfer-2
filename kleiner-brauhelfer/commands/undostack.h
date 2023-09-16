#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QUndoStack>

class UndoStack : public QUndoStack
{
    Q_OBJECT
public:
    explicit UndoStack(QObject *parent = nullptr);
    QAction *createUndoAction(QObject *parent, const QString &prefix = QString()) const;
    QAction *createRedoAction(QObject *parent, const QString &prefix = QString()) const;
public Q_SLOTS:
    void undo();
    void redo();
};

extern UndoStack* gUndoStack;

#endif // UNDOSTACK_H
