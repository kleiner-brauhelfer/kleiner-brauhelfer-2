#include "wdgabstractproxy.h"
#include <QLayout>
#include <QVariantAnimation>
#include <QEventLoop>
#include "settings.h"
#include "proxymodel.h"

extern Settings* gSettings;

WdgAbstractProxy::WdgAbstractProxy(ProxyModel* model, int row, QLayout *parentLayout, QWidget *parent) :
    QWidget(parent),
    mModel(model),
    mRow(row),
    mParentLayout(parentLayout)
{
}

void WdgAbstractProxy::setModel(ProxyModel* model)
{
    mModel = model;
}

int WdgAbstractProxy::row() const
{
    return mRow;
}

QVariant WdgAbstractProxy::data(int col) const
{
    return mModel->data(mRow, col);
}

bool WdgAbstractProxy::setData(int col, const QVariant& value)
{
    return mModel->setData(mRow, col, value);
}

bool WdgAbstractProxy::remove()
{
    if (gSettings->animationsEnabled())
    {        
        QVariantAnimation *animation = new QVariantAnimation();
        animation->setDuration(250);
        animation->setStartValue(pos());
        animation->setEndValue(QPoint(x() - width(), y()));
        connect(animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value)
        {
            move(value.toPoint());
        });
        QEventLoop loop;
        connect(animation, &QAbstractAnimation::finished, &loop, &QEventLoop::quit);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        loop.exec();
    }

    if (ProxyModel* m = qobject_cast<ProxyModel*>(mModel->sourceModel()))
        return m->removeRow(mModel->mapRowToSource(mRow));
    else
        return mModel->removeRow(mRow);
}

bool WdgAbstractProxy::moveUp()
{
    if (mRow <= 0)
        return false;

    if (gSettings->animationsEnabled() && mParentLayout)
    {
        QWidget* w = mParentLayout->itemAt(mRow - 1)->widget();
        QVariantAnimation* animation = new QVariantAnimation();
        animation->setDuration(250);
        animation->setStartValue(pos());
        animation->setEndValue(w->pos());
        connect(animation, &QVariantAnimation::valueChanged, this, [this, w, animation](const QVariant &value)
        {
            QPoint p = value.toPoint();
            QPoint start = animation->startValue().toPoint();
            QPoint end = animation->endValue().toPoint();
            int ax = 0;
            if (end.x() != start.x())
                ax = end.x() + (double)(p.x() - start.x()) / (end.x() - start.x()) * width();
            int ay = 0;
            if (end.y() != start.y())
                ay = end.y() + (double)(p.y() - start.y()) / (end.y() - start.y()) * height();
            w->move(ax, ay);
            move(p);
        });
        QEventLoop loop;
        connect(animation, &QAbstractAnimation::finished, &loop, &QEventLoop::quit);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        loop.exec();
        mParentLayout->update();
    }

    return mModel->swap(mRow, mRow - 1);
}

bool WdgAbstractProxy::moveDown()
{
    if (mRow >= mModel->rowCount() - 1)
        return false;

    if (gSettings->animationsEnabled() && mParentLayout)
    {
        QWidget* w = mParentLayout->itemAt(mRow + 1)->widget();
        QVariantAnimation* animation = new QVariantAnimation();
        animation->setDuration(250);
        animation->setStartValue(w->pos());
        animation->setEndValue(pos());
        connect(animation, &QVariantAnimation::valueChanged, this, [this, w, animation](const QVariant &value)
        {
            QPoint p = value.toPoint();
            QPoint start = animation->startValue().toPoint();
            QPoint end = animation->endValue().toPoint();
            int ax = 0;
            if (end.x() != start.x())
                ax = end.x() + (double)(p.x() - start.x()) / (end.x() - start.x()) * w->width();
            int ay = 0;
            if (end.y() != start.y())
                ay = end.y() + (double)(p.y() - start.y()) / (end.y() - start.y()) * w->height();
            move(ax, ay);
            w->move(p);
        });
        QEventLoop loop;
        connect(animation, &QAbstractAnimation::finished, &loop, &QEventLoop::quit);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        loop.exec();
        mParentLayout->update();
    }

    return mModel->swap(mRow, mRow + 1);
}
