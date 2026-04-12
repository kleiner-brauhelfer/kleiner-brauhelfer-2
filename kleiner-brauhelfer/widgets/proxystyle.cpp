#include "proxystyle.h"
#include <QStyleOptionSlider>
#include <QPainter>

#define SCROLLBAR_ARROWS_EN 1

static void drawArrow(QPainter* p, QRect rect, Qt::ArrowType arrowDirection, const QBrush& brush, const QPen& pen = Qt::NoPen)
{
    const qreal ArrowBaseRatio = 0.70;
    qreal irx, iry, irw, irh;
    QRectF(rect).getRect(&irx, &iry, &irw, &irh);
    if (irw < 1.0 || irh < 1.0)
        return;
    qreal dw, dh;
    if (arrowDirection == Qt::LeftArrow || arrowDirection == Qt::RightArrow)
    {
        dw = ArrowBaseRatio;
        dh = 1.0;
    }
    else
    {
        dw = 1.0;
        dh = ArrowBaseRatio;
    }
    QSizeF sz = QSizeF(dw, dh).scaled(irw, irh, Qt::KeepAspectRatio);
    qreal aw = sz.width();
    qreal ah = sz.height();
    qreal ax, ay;
    ax = irx + (irw - aw) / 2;
    ay = iry + (irh - ah) / 2;
    QRectF arrowRect(ax, ay, aw, ah);
    QPointF points[3];
    switch (arrowDirection)
    {
    case Qt::DownArrow:
        arrowRect.setTop(std::round(arrowRect.top()));
        points[0] = arrowRect.topLeft();
        points[1] = arrowRect.topRight();
        points[2] = QPointF(arrowRect.center().x(), arrowRect.bottom());
        break;
    case Qt::RightArrow:
        arrowRect.setLeft(std::round(arrowRect.left()));
        points[0] = arrowRect.topLeft();
        points[1] = arrowRect.bottomLeft();
        points[2] = QPointF(arrowRect.right(), arrowRect.center().y());
        break;
    case Qt::LeftArrow:
        arrowRect.setRight(std::round(arrowRect.right()));
        points[0] = arrowRect.topRight();
        points[1] = arrowRect.bottomRight();
        points[2] = QPointF(arrowRect.left(), arrowRect.center().y());
        break;
    case Qt::UpArrow:
        arrowRect.setBottom(std::round(arrowRect.bottom()));
        points[0] = arrowRect.bottomLeft();
        points[1] = arrowRect.bottomRight();
        points[2] = QPointF(arrowRect.center().x(), arrowRect.top());
        break;
    default:
        return;
    }
    p->setPen(pen);
    p->setBrush(brush);
    p->drawConvexPolygon(points, 3);
}

ProxyStyle::ProxyStyle() :
    QProxyStyle(QStringLiteral("Fusion"))
{
}

QRect ProxyStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subcontrol, const QWidget *widget) const
{
    QRect rect = QProxyStyle::subControlRect(control, option, subcontrol, widget);
    switch (control)
    {
    case CC_ScrollBar:
    {
        const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if (scrollBar)
        {
            switch (subcontrol)
            {
            case SC_ScrollBarSlider:
                if (scrollBar->orientation == Qt::Horizontal)
                    return rect.adjusted(0, 3, 0, -3);
                else
                    return rect.adjusted(3, 0, -3, 0);
            case SC_ScrollBarAddLine:
            case SC_ScrollBarSubLine:
              #if SCROLLBAR_ARROWS_EN
                return rect.adjusted(2, 2, -2, -2);
              #else
                return QRect();
              #endif
            default:
                break;
            }
        }
        break;
    }
    default:
        break;
    }
    return rect;
}

void ProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element)
    {
    case PE_PanelScrollAreaCorner:
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(option->palette.brush(QPalette::Window));
        painter->drawRect(option->rect);
        break;
    }
    default:
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}

void ProxyStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    QProxyStyle::drawControl(element, option, painter, widget);
}

void ProxyStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control)
    {
    case CC_ScrollBar:
        {
            const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option);
            if (scrollBar)
            {
                bool isHorizontal = scrollBar->orientation == Qt::Horizontal;
                bool isLeftToRight = option->direction != Qt::RightToLeft;
                bool hover = scrollBar->state & State_MouseOver;
                QBrush brush = hover ? option->palette.brush(QPalette::Highlight) : option->palette.brush(QPalette::Mid);
                QPen pen = hover ? QPen(option->palette.color(QPalette::Mid)) : QPen(Qt::NoPen);
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(pen);
                if (scrollBar->subControls & SC_ScrollBarSlider)
                {
                    QRect rect = subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
                    painter->setBrush(brush);
                    if (isHorizontal)
                        painter->drawRoundedRect(rect, rect.height()/2, rect.height()/2);
                    else
                        painter->drawRoundedRect(rect, rect.width()/2, rect.width()/2);
                }
                if (scrollBar->subControls & SC_ScrollBarSubLine)
                {
                    QRect rect = subControlRect(control, scrollBar, SC_ScrollBarSubLine, widget);
                    Qt::ArrowType arrowType = isHorizontal ? (isLeftToRight ? Qt::LeftArrow : Qt::RightArrow) : (Qt::UpArrow);
                    drawArrow(painter, rect, arrowType, brush, pen);
                }
                if (scrollBar->subControls & SC_ScrollBarAddLine)
                {
                    QRect rect = subControlRect(control, scrollBar, SC_ScrollBarAddLine, widget);
                    Qt::ArrowType arrowType = isHorizontal ? (isLeftToRight ? Qt::RightArrow : Qt::LeftArrow) : (Qt::DownArrow);
                    drawArrow(painter, rect, arrowType, brush, pen);
                }
            }
        }
        break;
    default:
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
}
