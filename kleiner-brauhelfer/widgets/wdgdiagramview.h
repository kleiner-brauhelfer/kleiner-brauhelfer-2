#ifndef WDGDIAGRAMVIEW_H
#define WDGDIAGRAMVIEW_H
//
#include <QWidget>
#include <QDate>
#include <QPainter>
#include <QtGlobal>
#include <QRectF>
//
class WdgDiagramView : public QWidget
{
Q_OBJECT
private:
	void ZeichneText(QPainter * painter, QPoint p, int nummer, int Linie);
	void ZeichneMarkierung(QPainter * painter, QPoint p);
	void zeichneL1();
	void zeichneL2();
	void zeichneL3();
	void zeichneTextMarkiertenPunktL1();
	void zeichneTextMarkiertenPunktL2();
	void zeichneTextMarkiertenPunktL3();
	//Markierter Punkt im Diagramm
	int MarkierterPunkt;
	//Nullpunkt x
	int nullX;
	//Nullpunkt y
	int nullY;
	//Breite der Zeichenfläche
	int zbreite;
	//Höhe der Zeichenfläche
	int zhoehe;
	//Abstand der unterteilungen x achse
	double abstandX;
	//Abstand der unterteilungen y achse
	double abstandY1;
	double abstandY2;
	double abstandY3;
	void zeichneAchsen();
    double maxWertL1;
    double minWertL1;
    double maxWertL2;
    double minWertL2;
    double maxWertL3;
    double minWertL3;
	//Punkte der Linie 1
	QList<QPoint> Linie1;
	//Punkte der Linie 2
	QList<QPoint> Linie2;
	//Punkte der Linie 3
	QList<QPoint> Linie3;
	//true = X Achse ist in Tagen skaliert
	//false = X Achse ist in Monaten skaliert 
	bool day;
	int AnzahlX;
	//Abstände von Widget Zeichenfläche zur Diagrammzeichenfläche
	int AbstandOben;
	int AbstandUnten;
	int AbstandLinks;
	int AbstandRechts;
	int AbstandBeschriftungRechts;
	int AbstandBeschriftungOben;
	//Wert für die Linie die in das Diagramm gezeichnet werden soll
	double WertLinie1;
	double WertLinie2;
	double WertLinie3;
public:
    void MarkierePunkt(int id);
	void MarkierePunkt(QDateTime DateTime);
    int getMarkierterPunkt() const;
	void setWertLinie2(double value);
	void setWertLinie1(double value);
	void setWertLinie3(double value);
	bool WertLinie1Aktiv;
	bool WertLinie2Aktiv;
	bool WertLinie3Aktiv;
	void BildSpeichern(QString Name);
	void DiagrammLeeren();
	QColor colorL1;
	QColor colorL2;
	QColor colorL3;
	//Bezeichnung
	QString BezeichnungL1;
	QString KurzbezeichnungL1;
	QString BezeichnungL2;
	QString KurzbezeichnungL2;
	QString BezeichnungL3;
	QString KurzbezeichnungL3;
    QList<int> Ids;
	//Ausgangsdaten der Linie 1
	QList<QDateTime> L1Datum;
	QList<double> L1Daten;
    int L1Precision;
    double L1Min;
    double L1Max;
	//Ausgangsdaten der Linie 2
	QList<QDateTime> L2Datum;
	QList<double> L2Daten;
    int L2Precision;
    double L2Min;
    double L2Max;
	//Ausgangsdaten der Linie 3
	QList<QDateTime> L3Datum;
    QList<double> L3Daten;
    int L3Precision;
    double L3Min;
    double L3Max;

	void paintEvent(QPaintEvent * event);
	void mousePressEvent ( QMouseEvent * event );
    WdgDiagramView(QWidget * parent = 0,  Qt::WindowFlags f = 0);
signals:
    void sig_selectionChanged(int id);
};
#endif // WDGDIAGRAMVIEW_H
