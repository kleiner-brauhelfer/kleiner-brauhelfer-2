#include "svgview.h"
#include <QSvgRenderer>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QPaintEvent>
#include <qmath.h>

SvgView::SvgView(QWidget *parent)
    : QGraphicsView(parent)
    , mSvgItem(nullptr)
    , mOutlineVisible(false)
    , mBackgroundColor(Qt::transparent)
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

template <class T> bool SvgView::load_impl(const T& arg)
{
    try
    {
        QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem());
        if (!svgItem->renderer()->load(arg))
            return false;
        svgItem->setElementId("");

        scene()->clear();
        resetTransform();

        mSvgItem = svgItem.take();
        mSvgItem->setFlags(QGraphicsItem::ItemClipsToShape);
        mSvgItem->setCacheMode(QGraphicsItem::NoCache);
        mSvgItem->setZValue(0);

        if (mBackgroundColor.isValid() && mBackgroundColor != Qt::transparent)
        {
            QGraphicsRectItem *backgroundItem = new QGraphicsRectItem(mSvgItem->boundingRect());
            backgroundItem->setBrush(mBackgroundColor);
            backgroundItem->setPen(Qt::NoPen);
            backgroundItem->setZValue(-1);
            scene()->addItem(backgroundItem);
        }

        scene()->addItem(mSvgItem);

        if (mOutlineVisible)
        {
            QGraphicsRectItem *outlineItem = new QGraphicsRectItem(mSvgItem->boundingRect());
            QPen outline(Qt::red, 2, Qt::DashLine);
            outline.setCosmetic(true);
            outlineItem->setPen(outline);
            outlineItem->setBrush(Qt::NoBrush);
            outlineItem->setZValue(1);
            scene()->addItem(outlineItem);
        }

        scene()->setSceneRect(mSvgItem->boundingRect());
        fitInView(mSvgItem, Qt::KeepAspectRatio);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool SvgView::load(const QString &filename)
{
    return load_impl(filename);
}

bool SvgView::load(const QByteArray &contents)
{
    return load_impl(contents);
}

void SvgView::setViewBackgroundColor(const QColor& color)
{
    mBackgroundColor = color;
}

void SvgView::setViewOutline(bool enable)
{
    mOutlineVisible = enable;
}

void SvgView::wheelEvent(QWheelEvent *event)
{
    qreal factor = qPow(1.2, event->angleDelta().y() / 240.0);
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
