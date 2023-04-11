#include "chart3.h"
#include "settings.h"

extern Settings* gSettings;

Chart3::Chart3(QWidget *parent) :
    ChartBase(parent)
{
    QColor color1 = gSettings->DiagramLinie1;
    QColor color2 = gSettings->DiagramLinie2;
    QColor color3 = gSettings->DiagramLinie3;

    setBackground(Qt::GlobalColor::transparent);

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat(QLocale().dateFormat(QLocale::ShortFormat));
    xAxis->setTicker(dateTicker);
    xAxis->ticker()->setTickCount(8);
    xAxis2->setRange(0, 1);
    yAxis->setLabelColor(color1);
    yAxis->setTickLabelColor(color1);
    yAxis2->setLabelColor(color2);
    yAxis2->setTickLabelColor(color2);
    yAxis3 = axisRect()->addAxis(QCPAxis::atRight);
    yAxis3->setLabelColor(color3);
    yAxis3->setTickLabelColor(color3);

    graph1 = addGraph(xAxis, yAxis);
    graph1->setPen(QPen(color1, 2));
    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color1, Qt::white, 6));
    graph1Limit = addGraph(xAxis2, yAxis);
    graph1Limit->setPen(QPen(color1, 2, Qt::DashLine));
    graph2 = addGraph(xAxis, yAxis2);
    graph2->setPen(QPen(color2, 2));
    graph2->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color2, Qt::white, 6));
    graph3 = addGraph(xAxis, yAxis3);
    graph3->setPen(QPen(color3, 2));
    graph3->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color3, Qt::white, 6));

    yAxisSelection = axisRect()->addAxis(QCPAxis::atLeft);
    yAxisSelection->setVisible(false);
    yAxisSelection->setRange(0, 1);
    graphSelection = addGraph(xAxis, yAxisSelection);
    graphSelection->setPen(QPen(Qt::gray, 2, Qt::DotLine));
    graphSelection->removeFromLegend();
    lblValueX = new QCPItemText(this);
    lblValueX->position->setAxes(xAxis, yAxisSelection);
    lblValueX->setPen(QPen(Qt::black, 2));
    lblValueX->setBrush(QBrush(Qt::white));
    lblValueX->setPadding(QMargins(5, 2, 5, 2));
    lblValueY1 = new QCPItemText(this);
    lblValueY1->position->setAxes(xAxis, yAxisSelection);
    lblValueY1->setPen(QPen(color1, 2));
    lblValueY1->setBrush(QBrush(Qt::white));
    lblValueY1->setPadding(QMargins(5, 2, 5, 2));
    lblValueY2 = new QCPItemText(this);
    lblValueY2->position->setAxes(xAxis, yAxisSelection);
    lblValueY2->setPen(QPen(color2, 2));
    lblValueY2->setBrush(QBrush(Qt::white));
    lblValueY2->setPadding(QMargins(5, 2, 5, 2));
    lblValueY3 = new QCPItemText(this);
    lblValueY3->position->setAxes(xAxis, yAxisSelection);
    lblValueY3->setPen(QPen(color3, 2));
    lblValueY3->setBrush(QBrush(Qt::white));
    lblValueY3->setPadding(QMargins(5, 2, 5, 2));

    clear();

    setInteraction(QCP::iRangeDrag, true);
    axisRect()->setRangeDragAxes({xAxis, yAxis, yAxis2, yAxis3});
    setInteraction(QCP::iRangeZoom, true);
    axisRect()->setRangeZoomAxes({xAxis});

    connect(this, &QCustomPlot::mousePress, this, &Chart3::onMousePress);
    connect(this, &QCustomPlot::mouseDoubleClick, this, &Chart3::onMouseDoubleClick);
}

void Chart3::clear()
{
    setDisabled(true);
    yAxis2->setVisible(false);
    yAxis3->setVisible(false);
    graph1->setVisible(false);
    graph1Limit->setVisible(false);
    graph2->setVisible(false);
    graph3->setVisible(false);
    select(-1);
}

void Chart3::setData1(const QVector<double>& x, const QVector<double>& y, const QString& label, const QString& unit, double precision)
{
    if (x.size() > 1)
    {
        setDisabled(false);
        graph1->setData(x, y, true);
        graph1->setVisible(true);
    }
    unit1 = unit;
    if (unit1.isEmpty())
        yAxis->setLabel(label);
    else
        yAxis->setLabel(label + " (" + unit1 + ")");
    precision1 = precision;
}

void Chart3::setData2(const QVector<double>& x, const QVector<double>& y, const QString& label, const QString& unit, double precision)
{
    if (x.size() > 1)
    {
        graph2->setData(x, y, true);
        unit2 = unit;
        if (unit2.isEmpty())
            yAxis2->setLabel(label);
        else
            yAxis2->setLabel(label + " (" + unit2 + ")");
        precision2 = precision;
        yAxis2->setVisible(true);
        graph2->setVisible(true);
    }
}

void Chart3::setData3(const QVector<double>& x, const QVector<double>& y, const QString& label, const QString& unit, double precision)
{
    if (x.size() > 1)
    {
        graph3->setData(x, y, true);
        unit3 = unit;
        if (unit3.isEmpty())
            yAxis3->setLabel(label);
        else
            yAxis3->setLabel(label + " (" + unit3 + ")");
        precision3 = precision;
        yAxis3->setVisible(true);
        graph3->setVisible(true);
    }
}

void Chart3::setData1Limit(double value)
{
    if (graph1->visible())
    {
        graph1Limit->setData({0, 1}, {value, value}, true);
        graph1Limit->setVisible(true);
    }
}

void Chart3::rescale()
{
    rescaleAxes(true);
    QCPRange range = xAxis->range();
    double expand = 0.01*range.size();
    xAxis->setRange(range.lower-expand, range.upper+expand);
    range = yAxis->range();
    expand = 0.02*range.size();
    yAxis->setRange(range.lower-expand, range.upper+expand);
    range = yAxis2->range();
    expand = 0.02*range.size();
    yAxis2->setRange(range.lower-expand, range.upper+expand);
    range = yAxis3->range();
    expand = 0.02*range.size();
    yAxis3->setRange(range.lower-expand, range.upper+expand);
}

void Chart3::select(int index)
{
    QLocale locale = QLocale();
    bool visible = index >= 0;
    if (visible)
    {
        double x = graph1->data()->at(index)->key;
        double y0 = 0.55;
        double py0 = yAxisSelection->coordToPixel(y0);
        if (graph1->visible())
        {
            graphSelection->setData({x, x}, {0, 1}, true);
            lblValueX->setText(locale.toString(QCPAxisTickerDateTime::keyToDateTime(x).date(), QLocale::ShortFormat));
            lblValueX->position->setCoords(x, 0.55);
            lblValueY1->setText(locale.toString(graph1->data()->at(index)->value, 'f', precision1) + " " + unit1);
            lblValueY1->position->setCoords(x, yAxisSelection->pixelToCoord(py0 + 24));
        }
        if (graph2->visible())
        {
            lblValueY2->setText(locale.toString(graph2->data()->at(index)->value, 'f', precision2) + " " + unit2);
            lblValueY2->position->setCoords(x, yAxisSelection->pixelToCoord(py0 + 2*24));
        }
        if (graph3->visible())
        {
            lblValueY3->setText(locale.toString(graph3->data()->at(index)->value, 'f', precision3) + " " + unit3);
            lblValueY3->position->setCoords(x, yAxisSelection->pixelToCoord(py0 + 3*24));
        }
    }
    graphSelection->setVisible(graph1->visible() && visible);
    lblValueX->setVisible(graph1->visible() && visible);
    lblValueY1->setVisible(graph1->visible() && visible);
    lblValueY2->setVisible(graph2->visible() && visible);
    lblValueY3->setVisible(graph3->visible() && visible);
    replot();
}

void Chart3::onMousePress(QMouseEvent *event)
{
    int newIndex = -1;
    double x = xAxis->pixelToCoord(event->pos().x());
    for (QCPGraphDataContainer::const_iterator it = graph1->data()->constBegin(); it != graph1->data()->constEnd(); it++)
    {
        if (it == graph1->data()->constBegin())
        {
          double p2 = it->key + (((it+1)->key - it->key) / 2.0);
          if (x < p2)
          {
            newIndex = it - graph1->data()->constBegin();
            break;
          }
        }
        else if (it == graph1->data()->constEnd()-1)
        {
          double p1 = (it-1)->key + ((it->key - (it-1)->key) / 2.0);
          if (x > p1)
          {
            newIndex = it - graph1->data()->constBegin();
            break;
          }
        }
        else
        {
          double p1 = (it-1)->key + ((it->key - (it-1)->key) / 2.0);
          double p2 = it->key + (((it+1)->key - it->key) / 2.0);
          if ((x > p1) && (x < p2))
          {
            newIndex = it - graph1->data()->constBegin();
            break;
          }
        }
    }
    if (newIndex != -1)
    {
        select(newIndex);
        emit selectionChanged(newIndex);
    }
}

void Chart3::onMouseDoubleClick(QMouseEvent *event)
{
    Q_UNUSED(event)
    select(-1);
    rescale();
    replot();
}
