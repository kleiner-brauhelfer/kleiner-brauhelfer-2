#include "wdgdiagramview.h"
#include <QMouseEvent>

WdgDiagramView::WdgDiagramView(QWidget * parent) :
    QWidget(parent),
    L1Precision(6),
    L1Min(0.0),
    L1Max(0.0),
    L2Precision(6),
    L2Min(0.0),
    L2Max(0.0),
    L3Precision(6),
    L3Min(0.0),
    L3Max(0.0)
{
  colorL1 = QColor::fromRgb(116, 30, 166);
  colorL2 = QColor::fromRgb(56, 104, 2);
  colorL3 = QColor::fromRgb(170, 0, 0);
  BezeichnungL1 = "";
  KurzbezeichnungL1 = "";
  BezeichnungL2 = "";
  KurzbezeichnungL2 = "";
  BezeichnungL3 = "";
  KurzbezeichnungL3 = "";
  AbstandOben = 35;
  AbstandUnten = 30;
  AbstandLinks = 20;
  AbstandRechts = 40;
  AbstandBeschriftungRechts = 20;
  AbstandBeschriftungOben = 15;
  WertLinie1Aktiv = false;
  WertLinie2Aktiv = false;
  WertLinie3Aktiv = false;
  //0=Kein Punkt ist angewählt
  MarkierterPunkt = 0;

}
//

void WdgDiagramView::paintEvent(QPaintEvent * )
{
  if ( (L1Daten.count()<2) || (L1Datum.count()<2)){
  }
  else {
    //Gitter Zeichnen
    zeichneAchsen();
    //Linie 1 Zeichnen
    zeichneL1();
    //Linie 2 Zeichnen
    if (!L2Daten.isEmpty()){
      zeichneL2();
    }
    //Linie 3 Zeichnen
    if (!L3Daten.isEmpty()){
      zeichneL3();
    }
    //Zeiche Text Markierten Punkt
    zeichneTextMarkiertenPunktL1();
    //Linie 2 Zeichnen
    if (!L2Daten.isEmpty()){
      zeichneTextMarkiertenPunktL2();
    }
    //Linie 3 Zeichnen
    if (!L3Daten.isEmpty()){
      zeichneTextMarkiertenPunktL3();
    }
  }
}

void WdgDiagramView::mousePressEvent ( QMouseEvent * event )
{
  bool newSelection = false;
  int p1,p2;
  //Punkt finden der Markiert werden soll
  for (int i = 0; i < Linie1.count(); i++){
    //Erster Punkt
    if (i == 0) {
      //p1 = 0;
      p2 = Linie1[i].x() + qRound((Linie1[i+1].x() - Linie1[i].x()) / 2.0);
      if (event -> x() < p2) {
        if (MarkierterPunkt != 1) {
          MarkierterPunkt = 1;
          newSelection = true;
        }
        break;
      }
    }
    //Letzter Punkt
    else if (i == Linie1.count()-1){
      p1 = Linie1[i-1].x() + qRound((Linie1[i].x() - Linie1[i-1].x()) / 2.0);
      if (event -> x() > p1) {
        if (MarkierterPunkt != i + 1) {
          MarkierterPunkt = i + 1;
          newSelection = true;
        }
        break;
      }
    }
    //alle anderen Punkte
    else {
      p1 = Linie1[i-1].x() + qRound((Linie1[i].x() - Linie1[i-1].x()) / 2.0);
      p2 = Linie1[i].x() + qRound((Linie1[i+1].x() - Linie1[i].x()) / 2.0);
      if ((event -> x() > p1) && (event -> x() < p2)) {
         if (MarkierterPunkt != i + 1) {
          MarkierterPunkt = i + 1;
          newSelection = true;
         }
         break;
      }
    }
  }

  if (newSelection) {
    repaint();
    emit sig_selectionChanged(getMarkierterPunkt());
  }
}

void WdgDiagramView::zeichneAchsen()
{
  QPainter painter(this);

  if (L3Daten.isEmpty()){
    AbstandBeschriftungOben = 0;
    AbstandBeschriftungRechts = 0;
    AbstandOben = 20;
    AbstandRechts = 20;
  }
  else {
      AbstandBeschriftungOben = 15;
      AbstandBeschriftungRechts = 20;
      AbstandOben = 35;
      AbstandRechts = 40;
  }

  //Nullpunkt x
  nullX = AbstandLinks;
  //Nullpunkt y
  nullY = height() - AbstandUnten;
  //Breite der Zeichenfläche
  zbreite = width() - nullX - AbstandRechts - 1;
  //Höhe der Zeichenfläche
  zhoehe = nullY - AbstandOben;

  painter.setPen(Qt::black);

  //Achsen
  painter.drawLine(nullX,nullY,nullX + zbreite,nullY);
  painter.drawLine(nullX,nullY,nullX,nullY - zhoehe);

  //Horzizontale Skalierung
  QDateTime startD(L1Datum.first());
  QDateTime endD(L1Datum.last());
  QDateTime td(startD);

  //wenn Diverenz weniger als 31 Tage sind aufteilung in Monat ansonsten in Tage
  if (startD.daysTo(endD) < 32){
    AnzahlX = startD.daysTo(endD)+1;
    day = true;
  }
  else {
    QDate ds,de;
    ds = startD.date();
    de = endD.date();
    ds.setDate(ds.year(),ds.month(),1);
    de.setDate(de.year(),de.month(),de.daysInMonth());
    AnzahlX = qRound(ds.daysTo(de)/30.0);
    day = false;
  }
  abstandX = double(zbreite) / double(AnzahlX);

  painter.setPen(QColor::fromRgb(128, 128, 128));
  //Horizontale skalierung
  for (int i=0; i < AnzahlX; i++){
    painter.drawLine(qRound(nullX + (i + 1) * abstandX), nullY, qRound(nullX + (i + 1) * abstandX), nullY - zhoehe);
    if (day){
      if (abstandX < 40)
        painter.drawText(QRectF(qRound(nullX + i * abstandX),nullY,abstandX,15),Qt::AlignCenter,td.toString("dd"));
      else
        painter.drawText(QRectF(qRound(nullX + i * abstandX),nullY,abstandX,15),Qt::AlignCenter,td.toString("dd.MM"));
      //Anzahl Tage zeichnen
      painter.drawText(QRectF(qRound(nullX + i * abstandX),nullY + 15,abstandX,15),Qt::AlignCenter,QString::number(td.daysTo(startD)*-1));
      td = td.addDays(1);
    }
    else {
      if (abstandX < 35)
        painter.drawText(QRectF(qRound(nullX + i * abstandX),nullY,abstandX,20),Qt::AlignCenter,td.toString("MM"));
      else if (abstandX < 50)
        painter.drawText(QRectF(qRound(nullX + i * abstandX),nullY,abstandX,20),Qt::AlignCenter,td.toString("MM.yy"));
      else
        painter.drawText(QRectF(qRound(nullX + i * abstandX),nullY,abstandX,20),Qt::AlignCenter,td.toString("MM.yyyy"));
      td = td.addMonths(1);
    }
  }

  //Vertikale Skalierung Linie 1
  int skalierung = 10;
  int maxWert = L1Max;
  int minWert = L1Min;
  if (L1Min == 0.0 && L1Max == 0.0)
  {
      //maxwert und minwert ermitteln
      maxWert = L1Daten.first();
      minWert = L1Daten.first();
      for (int i=0; i < L1Daten.count(); i++){
        if (maxWert < L1Daten[i])
          maxWert = L1Daten[i];
        if (minWert > L1Daten[i])
          minWert = L1Daten[i];
      }
      //wenn Eine Linie in das Diagramm gezeichnet werden soll dann überprüfen ob die auch
      //Sichtbereich ist
      if (WertLinie1Aktiv){
        if (WertLinie1 < minWert)
          minWert = WertLinie1;
        else if (WertLinie1 > maxWert)
          maxWert = WertLinie1;
      }
  }

  if (maxWert - minWert < 20)
    skalierung = 1;
  int unterteilungen = (maxWert - minWert) / skalierung + 1;
  abstandY1 = double(zhoehe) / double(unterteilungen);
  if (abstandY1 < 10){
    skalierung = 10;
    unterteilungen = (maxWert - minWert) / skalierung + 1;
    abstandY1 = double(zhoehe) / double(unterteilungen);
  }
  int startwert = minWert / skalierung;
  minWertL1 = minWert;
  maxWertL1 = maxWert + skalierung;
  painter.setPen(colorL1);
  painter.drawText(QRectF(0, nullY - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * startwert));
  if (L2Daten.isEmpty())
    painter.drawText(QRectF(nullX + zbreite, nullY - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * startwert));
  double d;
  int rest;
  QPen pen;
  for (int i=0; i < unterteilungen; i++){
    d = skalierung * (i + startwert + 1);
    rest = qRound(d)%5;
    if (rest == 0){
      pen.setColor(QColor::fromRgb(128, 128, 128));
      pen.setWidth(1);
      pen.setStyle(Qt::SolidLine);
      painter.setPen(pen);
    }
    else{
      pen.setColor(QColor::fromRgb(128, 128, 128));
      pen.setWidth(1);
      pen.setStyle(Qt::DashLine);
      painter.setPen(pen);
    }
    rest = qRound(d)%10;
    if (rest == 0){
      pen.setColor(QColor::fromRgb(0, 0, 0));
      pen.setWidth(1);
      painter.setPen(pen);
    }
    painter.drawLine(nullX+1, qRound(nullY - (i + 1) * abstandY1), nullX + zbreite, qRound(nullY - (i + 1) * abstandY1));
    painter.setPen(colorL1);
    painter.drawText(QRectF(0, qRound(nullY - (i + 1) * abstandY1) - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * (i + startwert + 1)));
    //Wenn Linie 2 nicht aktiv ist rechts auch skalierungstext anzeigen
    if (L2Daten.isEmpty())
      painter.drawText(QRectF(nullX + zbreite, qRound(nullY - (i + 1) * abstandY1) - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * (i + startwert + 1)));
  }


  //Vertikale Skalierung Linie 2
  if (!L2Daten.isEmpty()){
    skalierung = 10;
    maxWert = L2Max;
    minWert = L2Min;
    if (L2Min == 0.0 && L2Max == 0.0)
    {
        //maxwert und minwert ermitteln
        maxWert = L2Daten.first();
        minWert = L2Daten.first();
        for (int i=0; i < L2Daten.count(); i++){
          if (maxWert < L2Daten[i])
            maxWert = L2Daten[i];
          if (minWert > L2Daten[i])
            minWert = L2Daten[i];
        }
        //wenn Eine Linie in das Diagramm gezeichnet werden soll dann überprüfen ob die auch
        //Sichtbereich ist
        if (WertLinie2Aktiv){
          if (WertLinie2 < minWert)
            minWert = WertLinie2;
          else if (WertLinie2 > maxWert)
            maxWert = WertLinie2;
        }
    }
    if (maxWert - minWert < 20)
      skalierung = 1;
    int unterteilungen = (maxWert - minWert) / skalierung + 1;
    abstandY2 = double(zhoehe) / double(unterteilungen);
    if (abstandY2 < 10){
      skalierung = 10;
      unterteilungen = (maxWert - minWert) / skalierung + 1;
      abstandY2 = double(zhoehe) / double(unterteilungen);
    }
    startwert = minWert / skalierung;
    minWertL2 = minWert;
    maxWertL2 = maxWert + skalierung;
    painter.setPen(colorL2);
    painter.drawText(QRectF(nullX + zbreite, nullY - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * startwert));
    for (int i=0; i < unterteilungen; i++){
      //painter.setPen(QColor::fromRgb(128, 128, 128));
      //painter.setPen(Qt::DashLine);
      //painter.drawLine(nullX, qRound(nullY - (i + 1) * abstandY2), nullX + zbreite, qRound(nullY - (i + 1) * abstandY2));
      //painter.setPen(colorL2);
      painter.drawText(QRectF(nullX + zbreite, qRound(nullY - (i + 1) * abstandY2) - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * (i + startwert + 1)));
    }
  }
  //Vertikale Skalierung Linie 3
  if (!L3Daten.isEmpty()){
    skalierung = 10;
    maxWert = L3Max;
    minWert = L3Min;
    if (L3Min == 0.0 && L3Max == 0.0)
    {
        //maxwert und minwert ermitteln
        maxWert = L3Daten.first();
        minWert = L3Daten.first();
        for (int i=0; i < L3Daten.count(); i++){
          if (maxWert < L3Daten[i])
            maxWert = L3Daten[i];
          if (minWert > L3Daten[i])
            minWert = L3Daten[i];
        }
        //wenn Eine Linie in das Diagramm gezeichnet werden soll dann überprüfen ob die auch
        //Sichtbereich ist
        if (WertLinie3Aktiv){
          if (WertLinie3 < minWert)
            minWert = WertLinie3;
          else if (WertLinie3 > maxWert)
            maxWert = WertLinie3;
        }
    }
    if (maxWert - minWert < 20)
      skalierung = 1;
    int unterteilungen = (maxWert - minWert) / skalierung + 1;
    abstandY3 = double(zhoehe) / double(unterteilungen);
    if (abstandY3 < 10){
      skalierung = 10;
      unterteilungen = (maxWert - minWert) / skalierung + 1;
      abstandY3 = double(zhoehe) / double(unterteilungen);
    }
    startwert = minWert / skalierung;
    minWertL3 = minWert;
    maxWertL3 = maxWert + skalierung;
    painter.setPen(colorL3);
    painter.drawText(QRectF(nullX + zbreite + AbstandBeschriftungRechts, nullY - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * startwert));
    for (int i=0; i < unterteilungen; i++){
      //painter.setPen(QColor::fromRgb(128, 128, 128));
      //painter.setPen(Qt::DashLine);
      //painter.drawLine(nullX, qRound(nullY - (i + 1) * abstandY2), nullX + zbreite, qRound(nullY - (i + 1) * abstandY2));
      //painter.setPen(colorL3);
      painter.drawText(QRectF(nullX + zbreite + AbstandBeschriftungRechts, qRound(nullY - (i + 1) * abstandY3) - 5, nullX, 10), Qt::AlignCenter, QString::number(skalierung * (i + startwert + 1)));
    }
  }
}


void WdgDiagramView::zeichneL1()
{
  //Punkte berechnen
  Linie1.clear();
  QDateTime dt;
  double d;
  QPoint po;
  double wby = maxWertL1 - minWertL1;
  QDate dmin = L1Datum.first().date();
  QDate dmax = L1Datum.last().date();
  if (!day){
    dmin.setDate(dmin.year(),dmin.month(),1);
    dmax.setDate(dmax.year(),dmax.month(),dmax.daysInMonth());
  }
  int AnzahlTage = dmin.daysTo(dmax);

  for (int i=0; i < L1Daten.count(); i++) {
    //Y
    dt = L1Datum[i];
    d = L1Daten[i] - minWertL1;
    po.setY(qRound(nullY - (zhoehe / wby * d)));
    //X
    if (day){
      QDateTime dtemp;
      dtemp.setDate(dt.date());
      dtemp.time().setHMS(0,0,0);
      //Zeitoffset berechnen
      int ZeitOffset = qRound(double(abstandX / (24*60*60)) * dtemp.time().secsTo(dt.time()));
      po.setX(qRound(double(nullX+ZeitOffset) + (double(zbreite) / double(AnzahlX) * double(dmin.daysTo(dt.date())))));
    }
    else{
      po.setX(qRound(double(nullX) + (double(zbreite) / double(AnzahlTage) * double(dmin.daysTo(dt.date())))));
    }
    Linie1.append(po);
  }

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen;
  QBrush brush;


  //Wert Linie zeichnen
  if (WertLinie1Aktiv){
    d = WertLinie1 - minWertL1;
    pen.setColor(colorL1);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    brush.setStyle(Qt::SolidPattern);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawLine(nullX+1, qRound(nullY - (zhoehe / wby * d)), nullX + zbreite, qRound(nullY - (zhoehe / wby * d)));
  }

  pen.setStyle(Qt::SolidLine);
  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);
  pen.setColor(colorL1);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(brush);
  //Linie Zeichnen
  for (int i=0; i < Linie1.count(); i++){
    //painter.drawEllipse(Linie1[i],3,3);
    if (i>0)
      painter.drawLine(Linie1[i-1],Linie1[i]);
  }
  //Punkte Zeichnen
  for (int i=0; i < Linie1.count(); i++){
    //Markierung zeichnen
    if (i == MarkierterPunkt - 1){
      ZeichneMarkierung(&painter, Linie1[i]);
      brush.setColor(Qt::red);
      painter.setBrush(brush);
      painter.drawEllipse(Linie1[i],3,3);
      brush.setColor(Qt::white);
      painter.setBrush(brush);
    }
    else
      painter.drawEllipse(Linie1[i],3,3);
  }
  //Bezeichnung
  if (!BezeichnungL1.isEmpty()){
    painter.drawText(QRectF(nullX,AbstandBeschriftungOben,qRound(zbreite / 2.0),AbstandOben),Qt::AlignLeft, BezeichnungL1);
  }
}


void WdgDiagramView::zeichneL2()
{
  //Punkte berechnen
  Linie2.clear();
  QDateTime dt;
  double d;
  QPoint po;
  double wby = maxWertL2 - minWertL2;
  QDate dmin = L2Datum.first().date();
  QDate dmax = L2Datum.last().date();
  if (!day){
    dmin.setDate(dmin.year(),dmin.month(),1);
    dmax.setDate(dmax.year(),dmax.month(),dmax.daysInMonth());
  }
  int AnzahlTage = dmin.daysTo(dmax);

  for (int i=0; i < L2Daten.count(); i++) {
    //Y
    dt = L2Datum[i];
    d = L2Daten[i] - minWertL2;
    po.setY(qRound(nullY - (zhoehe / wby * d)));
    //X
    if (day){
      QDateTime dtemp;
      dtemp.setDate(dt.date());
      dtemp.time().setHMS(0,0,0);
      //Zeitoffset berechnen
      int ZeitOffset = qRound(double(abstandX / (24*60*60)) * dtemp.time().secsTo(dt.time()));
      po.setX(qRound(double(nullX+ZeitOffset) + (double(zbreite) / double(AnzahlX) * double(dmin.daysTo(dt.date())))));
    }
    else{
      po.setX(qRound(double(nullX) + (double(zbreite) / double(AnzahlTage) * double(dmin.daysTo(dt.date())))));
    }
    Linie2.append(po);
  }

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen;
  QBrush brush;

  //Wert Linie zeichnen
  if (WertLinie2Aktiv){
    d = WertLinie2 - minWertL2;
    pen.setColor(colorL2);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    brush.setStyle(Qt::SolidPattern);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawLine(nullX+1, qRound(nullY - (zhoehe / wby * d)), nullX + zbreite, qRound(nullY - (zhoehe / wby * d)));
  }

  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);
  pen.setColor(colorL2);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(brush);
  //Linie Zeichnen
  for (int i=0; i < Linie2.count(); i++){
    //painter.drawEllipse(Linie1[i],3,3);
    if (i>0)
      painter.drawLine(Linie2[i-1],Linie2[i]);
  }
  //Punkte Zeichnen
  for (int i=0; i < Linie2.count(); i++){
    //Markierung zeichnen
    if (i == MarkierterPunkt - 1){
      brush.setColor(Qt::red);
      painter.setBrush(brush);
      painter.drawEllipse(Linie2[i],3,3);
      brush.setColor(Qt::white);
      painter.setBrush(brush);
    }
    else
      painter.drawEllipse(Linie2[i],3,3);

  }
  //Bezeichnung
  if (!BezeichnungL2.isEmpty()){
    painter.drawText(QRectF(qRound(nullX + zbreite / 2.0),AbstandBeschriftungOben,qRound(zbreite / 2.0),AbstandOben),Qt::AlignRight, BezeichnungL2);
  }
}

void WdgDiagramView::zeichneL3()
{
  //Punkte berechnen
  Linie3.clear();
  QDateTime dt;
  double d;
  QPoint po;
  double wby = maxWertL3 - minWertL3;
  QDate dmin = L3Datum.first().date();
  QDate dmax = L3Datum.last().date();
  if (!day){
    dmin.setDate(dmin.year(),dmin.month(),1);
    dmax.setDate(dmax.year(),dmax.month(),dmax.daysInMonth());
  }
  int AnzahlTage = dmin.daysTo(dmax);

  for (int i=0; i < L3Daten.count(); i++) {
    //Y
    dt = L3Datum[i];
    d = L3Daten[i] - minWertL3;
    po.setY(qRound(nullY - (zhoehe / wby * d)));
    //X
    if (day){
      QDateTime dtemp;
      dtemp.setDate(dt.date());
      dtemp.time().setHMS(0,0,0);
      //Zeitoffset berechnen
      int ZeitOffset = qRound(double(abstandX / (24*60*60)) * dtemp.time().secsTo(dt.time()));
      po.setX(qRound(double(nullX+ZeitOffset) + (double(zbreite) / double(AnzahlX) * double(dmin.daysTo(dt.date())))));
    }
    else{
      po.setX(qRound(double(nullX) + (double(zbreite) / double(AnzahlTage) * double(dmin.daysTo(dt.date())))));
    }
    Linie3.append(po);
  }

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen;
  QBrush brush;

  //Wert Linie zeichnen
  if (WertLinie3Aktiv){
    d = WertLinie3 - minWertL3;
    pen.setColor(colorL3);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    brush.setStyle(Qt::SolidPattern);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawLine(nullX+1, qRound(nullY - (zhoehe / wby * d)), nullX + zbreite, qRound(nullY - (zhoehe / wby * d)));
  }

  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);
  pen.setColor(colorL3);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(brush);
  //Linie Zeichnen
  for (int i=0; i < Linie3.count(); i++){
    //painter.drawEllipse(Linie1[i],3,3);
    if (i>0)
      painter.drawLine(Linie3[i-1],Linie3[i]);
  }
  //Punkte Zeichnen
  for (int i=0; i < Linie3.count(); i++){
    //Markierung zeichnen
    if (i == MarkierterPunkt - 1){
      brush.setColor(Qt::red);
      painter.setBrush(brush);
      painter.drawEllipse(Linie3[i],3,3);
      brush.setColor(Qt::white);
      painter.setBrush(brush);
    }
    else
      painter.drawEllipse(Linie3[i],3,3);

  }
  //Bezeichnung
  if (!BezeichnungL3.isEmpty()){
    painter.drawText(QRectF(qRound(nullX + zbreite / 2.0),0,qRound(zbreite / 2.0),AbstandOben),Qt::AlignRight, BezeichnungL3);
  }
}

void WdgDiagramView::zeichneTextMarkiertenPunktL1()
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen;
  QBrush brush;
  pen.setStyle(Qt::SolidLine);
  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);
  pen.setColor(colorL1);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(brush);
  //Punkte durchgehen
  for (int i=0; i < Linie1.count(); i++){
    //Markierung zeichnen
    if (i == MarkierterPunkt - 1){
      ZeichneText(&painter, Linie1[i], i, 1);
    }
  }
}

void WdgDiagramView::zeichneTextMarkiertenPunktL2()
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen;
  QBrush brush;
  pen.setStyle(Qt::SolidLine);
  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);
  pen.setColor(colorL2);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(brush);
  //Punkte durchgehen
  for (int i=0; i < Linie2.count(); i++){
    //Markierung zeichnen
    if (i == MarkierterPunkt - 1){
      ZeichneText(&painter, Linie2[i], i, 2);
    }
  }
}

void WdgDiagramView::zeichneTextMarkiertenPunktL3()
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing,true);
  QPen pen;
  QBrush brush;
  pen.setStyle(Qt::SolidLine);
  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);
  pen.setColor(colorL3);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(brush);
  //Punkte durchgehen
  for (int i=0; i < Linie3.count(); i++){
    //Markierung zeichnen
    if (i == MarkierterPunkt - 1){
      ZeichneText(&painter, Linie3[i], i,3);
    }
  }
}


void WdgDiagramView::DiagrammLeeren()
{
  Ids.clear();
  L1Daten.clear();
  L1Datum.clear();
  L2Daten.clear();
  L2Datum.clear();
  L3Daten.clear();
  L3Datum.clear();
  WertLinie1Aktiv = false;
  WertLinie2Aktiv = false;
  WertLinie3Aktiv = false;
}

void WdgDiagramView::BildSpeichern(QString Name)
{
  repaint();
  QPainter *painter = new QPainter();
  QImage *image = new QImage(QSize(width() - 1, height() - 1), QImage::Format_ARGB32);

  painter -> begin(image);
  this -> render(painter);
  painter -> end();
  image -> save(Name+".png","PNG");
  delete painter;
  delete image;
}

//Sollwertlinie 1
void WdgDiagramView::setWertLinie1(double value)
{
  WertLinie1 = value;
  WertLinie1Aktiv = true;
}


//Sollwertlinie 2
void WdgDiagramView::setWertLinie2(double value)
{
  WertLinie2 = value;
  WertLinie2Aktiv = true;
}

//Sollwertlinie 3
void WdgDiagramView::setWertLinie3(double value)
{
  WertLinie3 = value;
  WertLinie3Aktiv = true;
}

//hebt einen Punkt im Diagramm hervor
void WdgDiagramView::MarkierePunkt(int id)
{
  bool gefunden = false;
  for (int i=0; i < Ids.count(); i++) {
    if (Ids[i] == id) {
      MarkierterPunkt = i + 1;
      gefunden = true;
      break;
    }
  }
  if (!gefunden)
    MarkierterPunkt = 0;

  repaint();
}

int WdgDiagramView::getMarkierterPunkt() const
{
    if (MarkierterPunkt > 0 && MarkierterPunkt <= Ids.count())
        return Ids[MarkierterPunkt - 1];
    return 0;
}

void WdgDiagramView::MarkierePunkt(QDateTime DateTime)
{
  bool gefunden = false;
  for (int i=0; i < L1Datum.count(); i++) {
    QString s1 = DateTime.date().toString(Qt::ISODate);
    QString s2 = L1Datum[i].date().toString(Qt::ISODate);
    if (s1 == s2){
      MarkierterPunkt = i + 1;
      gefunden = true;
      break;
    }
  }
  if (!gefunden)
    MarkierterPunkt = 0;

  repaint();
}


void WdgDiagramView::ZeichneMarkierung(QPainter * painter, QPoint p)
{
  QPen pen, oldpen;
  QBrush brush;
  oldpen = painter -> pen();

  QPoint p1,p2;
  //Farbe setzten
  pen.setColor(Qt::red);
  pen.setWidth(2);
  brush.setStyle(Qt::SolidPattern);
  //nur eine senkrechte Linie zeichnen
  painter -> setPen(pen);
  p1.setX(p.x());
  p2.setX(p1.x());
  p1.setY(nullY);
  p2.setY(AbstandOben);
  painter -> drawLine(p1, p2);

  painter -> setPen(oldpen);
}


void WdgDiagramView::ZeichneText(QPainter * painter, QPoint p, int nummer, int Linie)
{
  QRect rect;
  //Bei 3 Linien alle Linien untereinander
  if (!Linie3.isEmpty()){
    int p1 = zhoehe / 2 - 35;
    if (Linie == 1){
      rect.setRect(p.x()-25, AbstandOben + p1, 60, 20);
    }
    else if (Linie == 2){
      rect.setRect(p.x()-25, AbstandOben + p1 + 25, 60, 20);
    }
    else if (Linie == 3){
      rect.setRect(p.x()-25, AbstandOben + p1 + 25 + 25, 60, 20);
    }
  }
  else {
    //Linienbeschriftung hat unterhalb dem Punkt keinen Platz
    if ((p.y() + 30) > nullY){
      rect.setRect(p.x()-25, p.y()-30, 60, 20);
    }
    //Linienbeschriftung unterhalb
    else {
      //zwei Linien aktiv
      if (!Linie2.isEmpty()){
        //Linie 1 Wenn Linie2 aktiv und der Punkt zu nah dran ist Beschriftung oberhalb zeichnen
        if ((!L2Daten.isEmpty())
            && (p.y() + 60) > Linie2[nummer].y()
            && (Linie == 1)
            && (Linie2[nummer].y() > p.y())){
          rect.setRect(p.x()-25, p.y()-30, 60, 20);
        }
        //Linie 2 Wenn Linie 2 und der Punkt von Linie 1 unterhalb ist und zu nah dran ist
        else if ((Linie == 2)
            && (p.y() < Linie1[nummer].y())
            && (p.y() + 60) > Linie1[nummer].y()){
          rect.setRect(p.x()-25, p.y()-30, 60, 20);
        }
        //Normaler fall
        else {
          rect.setRect(p.x()-25, p.y()+10, 60, 20);
        }
      }
      //nur eine Linie
      else {
        rect.setRect(p.x()-25, p.y()+10, 60, 20);
      }
    }
  }
  painter -> drawRect(rect);
  if (Linie == 1)
    painter -> drawText(rect, Qt::AlignCenter, QString::number(L1Daten[nummer], 'f', L1Precision) + " " + KurzbezeichnungL1);
  else if (Linie == 2)
    painter -> drawText(rect, Qt::AlignCenter, QString::number(L2Daten[nummer], 'f', L2Precision) + " " + KurzbezeichnungL2);
  else
    painter -> drawText(rect, Qt::AlignCenter, QString::number(L3Daten[nummer], 'f', L3Precision) + " " + KurzbezeichnungL3);
}

