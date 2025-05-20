#include "undostack.h"
#include <QApplication>
#include <QWidget>
#include <QAction>
#include "../widgets/widgetdecorator.h"

UndoStack::UndoStack(QObject *parent) :
    QUndoStack(parent),
    mEnabled(true)
{
}

QAction *UndoStack::createUndoAction(QObject *parent, const QString &prefix) const
{
    QAction *result = new QAction(prefix, parent);
    result->setShortcuts(QKeySequence::Undo);
    result->setEnabled(canUndo());
    connect(this, &QUndoStack::canUndoChanged, result, &QAction::setEnabled);
    connect(result, &QAction::triggered, this, &UndoStack::undo);
    return result;
}

QAction *UndoStack::createRedoAction(QObject *parent, const QString &prefix) const
{
    QAction *result = new QAction(prefix, parent);
    result->setShortcuts(QKeySequence::Redo);
    result->setEnabled(canRedo());
    connect(this, &QUndoStack::canRedoChanged, result, &QAction::setEnabled);
    connect(result, &QAction::triggered, this, &UndoStack::redo);
    return result;
}

void UndoStack::undo()
{
    QWidget* wdg = QApplication::focusWidget();
    if (wdg)
        wdg->clearFocus();
    WidgetDecorator::clearValueChanged();
    WidgetDecorator::focusRequired = false;
    QUndoStack::undo();
    WidgetDecorator::focusRequired = true;
}

void UndoStack::redo()
{
    QWidget* wdg = QApplication::focusWidget();
    if (wdg)
        wdg->clearFocus();
    WidgetDecorator::clearValueChanged();
    WidgetDecorator::focusRequired = false;
    QUndoStack::redo();
    WidgetDecorator::focusRequired = true;
}

void UndoStack::push(QUndoCommand *cmd)
{
    if (mEnabled)
        QUndoStack::push(cmd);
    else
        cmd->redo();
}

void UndoStack::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

bool UndoStack::isEnabled() const
{
    return mEnabled;
}
