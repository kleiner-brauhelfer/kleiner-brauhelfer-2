#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>

class ProxyStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit ProxyStyle();
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const Q_DECL_OVERRIDE;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const Q_DECL_OVERRIDE;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const Q_DECL_OVERRIDE;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const Q_DECL_OVERRIDE;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const Q_DECL_OVERRIDE;
};

#endif // PROXYSTYLE_H
