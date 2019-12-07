#ifndef SVGVIEW_H
#define SVGVIEW_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QGraphicsSvgItem;
class QSvgRenderer;
class QWheelEvent;
class QPaintEvent;
QT_END_NAMESPACE

class SvgView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SvgView(QWidget *parent = nullptr);

    void clear();
    bool load(const QString &filename);
    bool load(const QByteArray &contents);

    QRectF viewBoxF() const;
    QSvgRenderer *renderer() const;

public slots:
    void setViewBackground(bool enable);
    void setViewOutline(bool enable);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void drawBackground(QPainter *p, const QRectF &rect) Q_DECL_OVERRIDE;

private:
    QGraphicsSvgItem *mSvgItem;
    bool mBackgroundVisible;
    bool mOutlineVisible;
};

#endif // SVGVIEW_H
