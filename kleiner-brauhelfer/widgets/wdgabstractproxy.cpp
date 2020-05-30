#include "wdgabstractproxy.h"
#include <QLayout>
#include <QVariantAnimation>
#include <QEventLoop>
#include "settings.h"
#include "proxymodel.h"

extern Settings* gSettings;

WdgAbstractProxy::WdgAbstractProxy(ProxyModel* model, int row, const QLayout* parentLayout, QWidget *parent) :
    QWidget(parent),
    mModel(model),
    mRow(row),
    mParentLayout(parentLayout)
{
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
        connect(animation, SIGNAL(finished()), &loop, SLOT(quit()));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        loop.exec();
    }

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
            w->move(animation->endValue().toPoint() + animation->startValue().toPoint() - p);
            move(p);
        });
        QEventLoop loop;
        connect(animation, SIGNAL(finished()), &loop, SLOT(quit()));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        loop.exec();
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
        animation->setStartValue(pos());
        animation->setEndValue(w->pos());
        connect(animation, &QVariantAnimation::valueChanged, this, [this, w, animation](const QVariant &value)
        {
            QPoint p = value.toPoint();
            move(p);
            w->move(animation->endValue().toPoint() + animation->startValue().toPoint() - p);
        });
        QEventLoop loop;
        connect(animation, SIGNAL(finished()), &loop, SLOT(quit()));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        loop.exec();
    }

    return mModel->swap(mRow, mRow + 1);
}
