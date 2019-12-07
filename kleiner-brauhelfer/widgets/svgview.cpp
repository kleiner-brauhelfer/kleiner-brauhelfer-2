#include "svgview.h"
#include <QSvgRenderer>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QPaintEvent>
#include <QPrinter>
#include <qmath.h>

SvgView::SvgView(QWidget *parent)
    : QGraphicsView(parent)
    , mSvgItem(nullptr)
    , mBackgroundVisible(false)
    , mOutlineVisible(false)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

    // Prepare background check-board pattern
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);
}

void SvgView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

QRectF SvgView::viewBoxF() const
{
    if (mSvgItem && mSvgItem->renderer())
        return mSvgItem->renderer()->viewBoxF();
    return QRectF();
}

void SvgView::clear()
{
    scene()->clear();
    mSvgItem = nullptr;
}

bool SvgView::openFile(const QString &fileName)
{
    QGraphicsScene *s = scene();
    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(fileName));
    if (!svgItem->renderer()->isValid())
        return false;

    s->clear();
    resetTransform();

    mSvgItem = svgItem.take();
    mSvgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    mSvgItem->setCacheMode(QGraphicsItem::NoCache);
    mSvgItem->setZValue(0);

    if (mBackgroundVisible)
    {
        QGraphicsRectItem *backgroundItem = new QGraphicsRectItem(mSvgItem->boundingRect());
        backgroundItem->setBrush(Qt::white);
        backgroundItem->setPen(Qt::NoPen);
        backgroundItem->setZValue(-1);
        s->addItem(backgroundItem);
    }

    s->addItem(mSvgItem);

    if (mOutlineVisible)
    {
        QGraphicsRectItem *outlineItem = new QGraphicsRectItem(mSvgItem->boundingRect());
        QPen outline(Qt::black, 2, Qt::DashLine);
        outline.setCosmetic(true);
        outline.setColor(QColor(255,0,0));
        outlineItem->setPen(outline);
        outlineItem->setBrush(Qt::NoBrush);
        outlineItem->setZValue(1);
        s->addItem(outlineItem);
    }

    s->setSceneRect(mSvgItem->boundingRect());
    fitInView(mSvgItem, Qt::KeepAspectRatio);
    return true;
}

void SvgView::setViewBackground(bool enable)
{
    mBackgroundVisible = enable;
}

void SvgView::setViewOutline(bool enable)
{
    mOutlineVisible = enable;
}

void SvgView::wheelEvent(QWheelEvent *event)
{
    qreal factor = qPow(1.2, event->delta() / 240.0);
    scale(factor, factor);
    event->accept();
}

void SvgView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (!mSvgItem)
        return;
    fitInView(mSvgItem, Qt::KeepAspectRatio);
}

QSvgRenderer *SvgView::renderer() const
{
    if (mSvgItem)
        return mSvgItem->renderer();
    return nullptr;
}
