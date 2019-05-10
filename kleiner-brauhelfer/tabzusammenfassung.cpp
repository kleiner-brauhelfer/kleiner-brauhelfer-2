#include "tabzusammenfassung.h"
#include "ui_tabzusammenfassung.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
#include "widgets/wdganhang.h"
#include "templatetags.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabZusammenfassung::TabZusammenfassung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabZusammenfassung)
{
    ui->setupUi(this);

    ui->webview->setHtmlFile("sudinfo.html");

    connect(bh, SIGNAL(modified()), this, SLOT(updateAll()));
    connect(bh, SIGNAL(discarded()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(updateAll()));
}

TabZusammenfassung::~TabZusammenfassung()
{
    delete ui;
}

void TabZusammenfassung::saveSettings()
{
}

void TabZusammenfassung::restoreView()
{
}

void TabZusammenfassung::updateAll()
{
    if (bh->sud()->getStatus() == Sud_Status_Rezept)
        ui->webview->setHtmlFile("zusammenfassung.html");
    else
        ui->webview->setHtmlFile("spickzettel.html");
    updateTemplateTags();
}

void TabZusammenfassung::on_btnToPdf_clicked()
{
    gSettings->beginGroup("General");

    QString path = gSettings->value("exportPath", QDir::homePath()).toString();

    QString fileName = bh->sud()->getSudname();
    QString filePath = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + fileName +  ".pdf", "PDF (*.pdf)");
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        ui->webview->printToPdf(filePath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

    gSettings->endGroup();
}

void TabZusammenfassung::updateTemplateTags()
{
    double f;
    QString s;
    QLocale locale;

    ui->webview->mTemplateTags.clear();
    TemplateTags::erstelleTagListe(ui->webview->mTemplateTags, TemplateTags::TagAll, bh->sud()->row());

    int bewertung = bh->sud()->getBewertungMax();
    if (bewertung > 0)
    {
        if (bewertung > Bewertung_MaxSterne)
            bewertung = Bewertung_MaxSterne;
        s = "";
        for (int i = 0; i < bewertung; i++)
            s += "<img class='star' style='padding:0px;margin:0px;' width='24' border=0>";
        for (int i = bewertung; i < Bewertung_MaxSterne; i++)
            s += "<img class='star_grey' style='padding:0px;margin:0px;' width='24' border=0>";
        ui->webview->mTemplateTags["Sterne"] = s;
    }

    QVariantMap ctxZutaten;

    // Malz
    s = "<table><tbody>";
    for (int i=0; i < bh->sud()->modelMalzschuettung()->rowCount(); i++)
    {
        s += "<tr>";
        s += "<td>" + bh->sud()->modelMalzschuettung()->data(i, "Name").toString() + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(bh->sud()->modelMalzschuettung()->data(i, "erg_Menge").toDouble(), 'f', 2) + "</td>";
        s += "<td>" + tr("kg") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(bh->sud()->modelMalzschuettung()->data(i, "Prozent").toDouble(), 'f', 1) + "</td>";
        s += "<td>" + tr("%") + "</td>";
        s += "</tr>";
    }
    s += "<tr>";
    s += "<td style='font-weight:bold;'>" + tr("Gesamt") + "</td>";
    s += "<td class='value' style='font-weight:bold;' align='right'>" + locale.toString(bh->sud()->geterg_S_Gesamt(), 'f', 2) + "</td>";
    s += "<td>" + tr("kg") + "</td>";
    s += "</tr>";
    s += "</tbody></table>";
    ctxZutaten["Malz"] = s;

    // Hopfen
    s = "<table><tbody>";
    for (int i=0; i < bh->sud()->modelHopfengaben()->rowCount(); i++)
    {
        s += "<tr>";
        QString s2 = QString("%1 (%2%)")
              .arg(bh->sud()->modelHopfengaben()->data(i, "Name").toString())
              .arg(locale.toString(bh->sud()->modelHopfengaben()->data(i, "Alpha").toDouble(), 'f', 1));
        if (bh->sud()->modelHopfengaben()->data(i, "Vorderwuerze").toBool())
            s += "<td>" + tr("VWH ") + s2 + "</td>";
        else
            s += "<td>" + s2 + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelHopfengaben()->data(i, "erg_Menge").toInt()) + "</td>";
        s += "<td>" + tr("g") + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelHopfengaben()->data(i, "Zeit").toInt()) + "</td>";
        s += "<td>" + tr("min") + "</td>";
        s += "</tr>";
    }
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++)
    {
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Hopfen)
      {
        s += "<tr>";
        s += "<td>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</td>";
        s += "<td>" + tr("g") + "</td>";
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
          s += "<td class='value' align='right' colspan='2'>" + tr("Gärung") + "</td>";
        else
          s += "<td class='value' align='right' colspan='2'>" + tr("Anstellen") + "</td>";
        s += "</tr>";
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabestatus").toInt() > EWZ_Zugabestatus_nichtZugegeben) {
          s += "<tr>";
          s += "<td colspan='5' class='kommentar'>";
          s += "<p>Zugegeben am "+ bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt_von").toDate().toString("dd.MM.yyyy")+"</p>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Entnahmeindex").toInt() == EWZ_Entnahmeindex_MitEntnahme) {
            if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabestatus").toInt() == EWZ_Zugabestatus_Entnommen) {
              s += "<p>"+ tr("Entnommen am ")+ bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt_bis").toDate().toString("dd.MM.yyyy")
                  + " (" + tr("Tage: ") +
                  QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()/1440)+")</p>";
            }
          }
          s += "</td>";
          s += "</tr>";
        }
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != "")
        {
          s += "<tr>";
          s += "<td colspan='5' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
          s += "</tr>";
        }
      }
    }
    s += "</tbody></table>";
    ctxZutaten["Hopfen"] = s;

    // Wasser
    f = bh->sud()->getRestalkalitaetFaktor();
    s = "<table><tbody>";
    s += "<tr>";
    s += "<td>" + tr("Hauptguss") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WHauptguss(), 'f', 1) + "</td>";
    s += "<td>" + tr("Liter") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Milchsäure (80%)") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WHauptguss() * f, 'f', 1) + "</td>";
    s += "<td>" + tr("ml") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Nachguss") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WNachguss(), 'f', 1) + "</td>";
    s += "<td>" + tr("Liter") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Milchsäure (80%)") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WNachguss() * f, 'f', 1) + "</td>";
    s += "<td>" + tr("ml") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Gesamt") + "</td>";
    s += "<td class='value' style='font-weight:bold;' align='right'>" + locale.toString(bh->sud()->geterg_S_Gesamt(), 'f', 1) + "</td>";
    s += "<td>" + tr("Liter") + "</td>";
    s += "</tr>";
    s += "</tbody></table>";
    ctxZutaten["Wasser"] = s;

    // Hefe
    s = "<table><tbody>";
    for (int row = 0; row < bh->sud()->modelHefegaben()->rowCount(); ++row)
    {
        s += "<tr>";
        s += "<td>" + bh->sud()->modelHefegaben()->data(row, "Name").toString() + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelHefegaben()->data(row, "Menge").toInt()) + "</td>";

        s += "<td>" + tr("Zugabe nach") + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelHefegaben()->data(row, "ZugegebenNach").toInt()) + "</td>";
        s += "<td>" + tr("Tage") + "</td>";

        s += "</tr>";
    }
    s += "</tbody></table>";
    ctxZutaten["Hefe"] = s;

    //Honig
    bool HonigVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Honig){
        HonigVorhanden = true;
      }
    }
    if (HonigVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Honig){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochbeginn") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Honig"] = s;

    //Zucker
    bool ZuckerVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Zucker){
        ZuckerVorhanden = true;
      }
    }
    if (ZuckerVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Zucker){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochbeginn") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Zucker"] = s;

    //Gewürz
    bool GewuerzVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Gewuerz){
        GewuerzVorhanden = true;
      }
    }
    if (GewuerzVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Gewuerz){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochen") + " (" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()) + "min) </p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Gewuerz"] = s;

    //Frucht
    bool FruchtVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Frucht){
        FruchtVorhanden = true;
      }
    }
    if (FruchtVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Frucht){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochen") + " (" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()) + "min) </p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Frucht"] = s;

    //Sonstiges
    bool SonstigesVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Sonstiges){
        SonstigesVorhanden = true;
      }
    }
    if (SonstigesVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Sonstiges){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochen") + " (" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()) + "min) </p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Sonstiges"] = s;

    ui->webview->mTemplateTags["Zutaten"] = ctxZutaten;

    // Rasten
    if (bh->sud()->modelRasten()->rowCount() > 0)
    {
        s = "<table><tbody>";
        s += "<tr>";
        s += "<td>";
        s += "<p>" + tr("Einmaischen bei") + "</p>";
        s += "</td>";
        s += "<td>";
        s += "<p class='value'>" + QString::number(bh->sud()->getEinmaischenTemp()) + "</p>";
        s += "</td>";
        s += "<td>";
        s += "<p>" + tr("°C") + "</p>";
        s += "</td>";
        s += "</tr>";
        for (int i = 0; i < bh->sud()->modelRasten()->rowCount(); i++)
        {
            s += "<tr>";
            s += "<td>";
            s += "<p>" + bh->sud()->modelRasten()->data(i, "Name").toString() + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p class='value'>" + QString::number(bh->sud()->modelRasten()->data(i, "Temp").toInt()) + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p>" + tr("°C") + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p class='value'>" + QString::number(bh->sud()->modelRasten()->data(i, "Dauer").toInt()) + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p>" + tr("min") + "</p>";
            s += "</td>";
            s += "</tr>";
        }
        s += "</tbody></table>";
        ui->webview->mTemplateTags["Rasten"] = s;
    }

    // Anhang
    QDir databasePath = QDir(gSettings->databaseDir());
    s = "";
    for (int i = 0; i < bh->sud()->modelAnhang()->rowCount(); ++i)
    {
        QString pfad = bh->modelAnhang()->data(i, "Pfad").toString();
        if (QDir::isRelativePath(pfad))
            pfad = QDir::cleanPath(databasePath.filePath(pfad));
        if (WdgAnhang::isImage(pfad))
            s += "<img style=\"max-width:80%;\" src=\"" + pfad + "\" alt=\"" + pfad + "\"></br></br>";
        else
            s += "<a href=\"" + pfad + "\">" + pfad + "</a></br></br>";
    }
    ui->webview->mTemplateTags["Anhang"] = s;

    ui->webview->updateTags();
    ui->webview->updateHtml();
}
